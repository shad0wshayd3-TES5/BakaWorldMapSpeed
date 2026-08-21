namespace REX::JSON
{
	template <class T, class U, class Store = SettingStore>
	using Map = Setting<std::map<T, U>, Store>;

	template <class Store = SettingStore>
	using MapStrF32 = Map<std::string, float, Store>;

	template void SettingLoad<std::map<std::string, float>>(void*, path_t, std::map<std::string, float>&, std::map<std::string, float>&);
	template void SettingSave<std::map<std::string, float>>(void*, path_t, std::map<std::string, float>&);
}

namespace JSON
{
	static REX::JSON::MapStrF32 WorldSpaces{ "worldSpaces", {} };

	static void Init()
	{
		const auto json = REX::JSON::SettingStore::GetSingleton();
		json->Init(
			"Data/SKSE/plugins/BakaWorldMapSpeed.json",
			"Data/SKSE/plugins/BakaWorldMapSpeedCustom.json");
		json->Load();
	}
}

namespace HOOK
{
	class MenuOpenCloseHandler :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		static MenuOpenCloseHandler* GetSingleton()
		{
			static MenuOpenCloseHandler singleton;
			return std::addressof(singleton);
		}

		virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
		{
			if (a_event && a_event->menuName == "MapMenu")
			{
				if (auto UI = RE::UI::GetSingleton())
				{
					if (auto MapMenu = UI->GetMenu<RE::MapMenu>();
						MapMenu && MapMenu->worldSpace)
					{
						auto map = JSON::WorldSpaces.GetValue();
						auto eid = MapMenu->worldSpace->editorID.c_str();
						if (map.contains(eid))
						{
							if (a_event->opening)
							{
								const auto value = map[eid];
								SetValues(value, value);
							}
							else
							{
								SetValues(MinPanSpeed, MaxPanSpeed);
							}
						}
					}
				}
			}

			return RE::BSEventNotifyControl::kContinue;
		}

		static void SetValues(float a_min, float a_max)
		{
			if (auto ini = RE::INISettingCollection::GetSingleton())
			{
				if (auto setting = ini->GetSetting("fMapWorldMinPanSpeed:MapMenu"sv))
					setting->data.f = a_min;
				if (auto setting = ini->GetSetting("fMapWorldMaxPanSpeed:MapMenu"sv))
					setting->data.f = a_max;
			}
		}

		static void GetValues()
		{
			if (auto ini = RE::INISettingCollection::GetSingleton())
			{
				if (auto setting = ini->GetSetting("fMapWorldMinPanSpeed:MapMenu"sv))
					MinPanSpeed = setting->GetFloat();
				if (auto setting = ini->GetSetting("fMapWorldMaxPanSpeed:MapMenu"sv))
					MaxPanSpeed = setting->GetFloat();
			}
		}

		inline static auto MinPanSpeed{ 60000.0f };
		inline static auto MaxPanSpeed{ 75000.0f };
	};

	static void Init()
	{
		MenuOpenCloseHandler::GetValues();

		if (auto UI = RE::UI::GetSingleton())
		{
			UI->AddEventSink<RE::MenuOpenCloseEvent>(MenuOpenCloseHandler::GetSingleton());
		}
	}
}

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type)
		{
		case SKSE::MessagingInterface::kPostLoad:
			JSON::Init();
			break;
		case SKSE::MessagingInterface::kDataLoaded:
			HOOK::Init();
			break;
		default:
			break;
		}
	}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
	return true;
}
