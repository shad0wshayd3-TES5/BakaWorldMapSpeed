#pragma once

#include <glaze/glaze.hpp>

struct SettingsImpl
{
	std::map<std::string, float> worldSpaces;
};

namespace Settings
{
	namespace Runtime
	{
		static SettingsImpl WorldSpaces;
	}

	inline void Load()
	{
		(void)glz::read_file_json(Runtime::WorldSpaces, "Data/SKSE/plugins/BakaWorldMapSpeed.json", std::string{});
	}
}
