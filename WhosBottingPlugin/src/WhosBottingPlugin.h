#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "GuiBase.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class WhosBottingPlugin: public BakkesMod::Plugin::BakkesModPlugin,
	public SettingsWindowBase
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	private: std::map<std::string, int> last_results_;

	void onLoad() override;
	void onUnload() override;
	void hk_on_game_end(ServerWrapper server, void* params, std::string event_name);
	void clear_results(ServerWrapper server, void* params, std::string event_name);
	void BindKey(std::string key);
	void UnbindKey(std::string key);
	void onKeybindPress();
public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
