#pragma once

#include "API.h"
#include "Framework.h"
#include "GuiBase.h"
#include "ReplayInfo.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "version.h"

constexpr auto plugin_version =
	stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class WhosBottingPlugin : public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase, public PluginWindowBase {
private:
	void onLoad() override;

	void RenderWindow() override;

	void TrySendReplay(ServerWrapper server, bool isMidGame);
	void BindKey(std::string key);
	void UnbindKey(std::string key);
	void OnKeybindPress();

	// When a game completes (we get the title screen), we can store the replay ID here
	// This prevents double sends of completed games
	std::optional<ReplayInfo> lastSentCompletedReplayInfo = std::nullopt;

	struct SendReplayState {
		std::future<API::Result> resultFuture;
		std::chrono::time_point<std::chrono::system_clock> sendTime;
	};

	std::optional<SendReplayState> replaySendState = std::nullopt;
	void SendReplayAsync(const std::vector<uint8_t>& replayBytes);
	bool IsReplaySending();

	void hk_OnGameEnd(ServerWrapper server, void* params, std::string eventName);
	void hk_OnGameLeft(ServerWrapper server, void* params, std::string eventName);

	void ShowNotif(std::string title, std::string description, bool isError = false);

	void ShowError(std::string title, std::string description) {
		ShowNotif(title, description, true);
	}
	void PollEvery5S();

	struct LastResult {
		std::map<std::string, int> playerPercents;
		bool isValid = false;
		std::string errorMsg = "";
		bool hasData = false;
	};

	LastResult lastResult;

	void ReturnLast();

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
									// void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
