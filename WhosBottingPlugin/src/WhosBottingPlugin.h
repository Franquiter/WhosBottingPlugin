#pragma once

#include "API.h"
#include "Framework.h"
#include "GuiBase.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"

constexpr auto plugin_version =
	stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class WhosBottingPlugin : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase {
private:
	void onLoad() override;
	void onUnload() override;

	void hk_OnGameEnd(ServerWrapper server, void* params, std::string event_name);
	void BindKey(std::string key);
	void UnbindKey(std::string key);
	void OnKeybindPress();

	std::optional<std::future<API::Result>> replayResultFuture;
	void SendReplayAsync(const std::vector<uint8_t>& replayBytes);

	void ShowNotif(std::string title, std::string description, bool isError = false);

	void ShowError(std::string title, std::string description) {
		ShowNotif(title, description, true);
	}

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
									// void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
