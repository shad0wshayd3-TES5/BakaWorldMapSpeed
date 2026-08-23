#include "Settings.h"

namespace Hooks
{
	class MenuOpenCloseHandler :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public REX::TSingleton<MenuOpenCloseHandler>
	{
	public:
		virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
		{
			if (a_event && a_event->menuName == "MapMenu")
			{
				if (auto ui = RE::UI::GetSingleton())
				{
					if (auto menu = ui->GetMenu<RE::MapMenu>();
						menu && menu->worldSpace)
					{
						auto map = Settings::Runtime::WorldSpaces.worldSpaces;
						auto eid = menu->worldSpace->editorID.c_str();
						if (map.contains(eid))
						{
							if (a_event->opening)
							{
								const auto value = static_cast<float>(map[eid]);
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

	static void Install()
	{
		if (auto ui = RE::UI::GetSingleton())
		{
			MenuOpenCloseHandler::GetValues();
			ui->AddEventSink<RE::MenuOpenCloseEvent>(MenuOpenCloseHandler::GetSingleton());
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
			Settings::Load();
			break;
		case SKSE::MessagingInterface::kDataLoaded:
			Hooks::Install();
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
