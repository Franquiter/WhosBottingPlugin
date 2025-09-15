#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"
constexpr auto plugin_version =
	stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class WhosBottingPlugin : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {
private:
	std::map<std::string, int> lastResults;

	void onLoad() override;
	void onUnload() override;
	void hk_OnGameEnd(ServerWrapper server, void* params, std::string event_name);
	void ClearResults(ServerWrapper server, void* params, std::string event_name);
	void BindKey(std::string key);
	void UnbindKey(std::string key);
	void OnKeybindPress();

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
									// void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
