#include "Settings.h"

namespace Hooks
{
	class MenuOpenCloseHandler :
		public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
		public REX::TSingleton<MenuOpenCloseHandler>
	{
	public:
		static void Install()
		{
			GetValues();
			if (auto ui = RE::UI::GetSingleton())
				ui->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());
		}

		virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override
		{
			if (a_event && a_event->menuName == "MapMenu"sv)
			{
				if (auto ui = RE::UI::GetSingleton())
				{
					if (auto mapMenu = ui->GetMenu<RE::MapMenu>();
						mapMenu && mapMenu->worldSpace)
					{
						auto iter = Settings::Runtime.worldSpaces.find(mapMenu->worldSpace->editorID.c_str());
						if (iter != Settings::Runtime.worldSpaces.end())
						{
							if (a_event->opening)
							{
								SetValues(iter->second, iter->second);
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

	private:
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
			Hooks::MenuOpenCloseHandler::Install();
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
