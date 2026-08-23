#pragma once

#include <glaze/glaze.hpp>

struct SettingsImpl
{
	std::map<std::string, float> worldSpaces;
};

namespace Settings
{
	static SettingsImpl Runtime;

	inline void Load()
	{
		(void)glz::read_file_json(Runtime, "Data/SKSE/plugins/BakaWorldMapSpeed.json", std::string{});
	}
}
