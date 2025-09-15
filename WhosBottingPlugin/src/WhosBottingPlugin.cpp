#include "WhosBottingPlugin.h"

#include <future>
#include <iostream>
#include <map>

#include "api.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "src/framework.h"

BAKKESMOD_PLUGIN(WhosBottingPlugin, "WhosBottingPlugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> g_GlobalCvarManager;
bool coolEnabled = false;

void WhosBottingPlugin::onLoad() {
	// This line is required for LOG to work and must be before any use of LOG()
	g_GlobalCvarManager = cvarManager;

	// do something when it loads
	LOG("Hello I'm WhosBottingPlugin B)");

	cvarManager->registerCvar("wbp_enabled", "0", "Enable Cool", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) { coolEnabled = cvar.getBoolValue(); });
	cvarManager->registerCvar("whoisbotting_keybind", "", "Keybind name", true, true)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
			std::string newKey = cvar.getStringValue();
			if (!oldValue.empty()) UnbindKey(oldValue);
			if (!newKey.empty()) BindKey(newKey);
		});

	cvarManager->registerNotifier(
		"whoisbotting_k_pressed", [this](std::vector<std::string>) { OnKeybindPress(); }, "Key pressed trigger", PERMISSION_ALL
	);

	{
		std::string key = cvarManager->getCvar("whoisbotting_keybind").getStringValue();
		if (!key.empty()) BindKey(key);
	}

	{ // Hook game ending events like https://github.com/bakkesmodorg/AutoReplayUploader does
		gameWrapper->HookEventWithCaller<ServerWrapper>(
			"Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
			bind(&WhosBottingPlugin::hk_OnGameEnd, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		);

		gameWrapper->HookEventWithCaller<ServerWrapper>(
			"Function TAGame.GameEvent_Soccar_TA.Destroyed",
			bind(&WhosBottingPlugin::hk_OnGameEnd, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		);
		gameWrapper->HookEventWithCaller<ServerWrapper>(
			"Function TAGame.GameEvent_Soccar_TA.PostBeginPlay",
			bind(&WhosBottingPlugin::ClearResults, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		);
	}

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		if (!coolEnabled) return;
		canvas.SetColor(0, 255, 0, 255); // green
		canvas.SetPosition(Vector2{1720, 50});
		canvas.DrawString("Enabled", 2, 2);

		if (lastResults.empty()) return;
		// LOG("draw!");
		int initial_y = 50;
		int initial_x = 50;
		int spacing = 35;

		for (auto result : lastResults) {
			canvas.SetColor(255, 255, 255, 255); // white
			canvas.SetPosition(Vector2{initial_x, initial_y});
			std::string text = std::string(result.first) + ":" + std::to_string(result.second);
			canvas.DrawString(text, 1.7, 1.7);
			initial_y += spacing;
		}
	});
}

void WhosBottingPlugin::onUnload() {
	LOG("I was too cool for this world B'(");
}

void WhosBottingPlugin::hk_OnGameEnd(ServerWrapper server, void* params, std::string event_name) {
	if (!coolEnabled) return;
	// LOG("plugin is enabled and game was finished/destroyed");
	//  Ref:
	//  https://github.com/bakkesmodorg/AutoReplayUploader/blob/master/AutoReplayUploader/AutoReplayUploaderPlugin.cpp#L295

	ReplayDirectorWrapper replay_director = server.GetReplayDirector();
	if (!replay_director) return;

	ReplaySoccarWrapper soccar_replay = replay_director.GetReplay();
	if (!soccar_replay) return;

	LOG("Potential replay save initiated from hk_on_game_end()...");

#if 0
	constexpr int MIN_FRAMES = 100;
	if (soccar_replay.GetNumFrames() < MIN_FRAMES) {
		log(" > Disgarding too-short replay");
		return;
	}

	if (server.GetNumPlayers() < 2) {
		log(" > Disgarding replay with only ", server.GetNumPlayers(), " player(s)");
		return;
	}
#endif

	LOG(" > Completing replay...");
	try {
		constexpr const char* TEMP_EXPORT_PATH = "_wbb_temp_replay_export.replay";
		soccar_replay.StopRecord();
		soccar_replay.ExportReplay(std::filesystem::path(TEMP_EXPORT_PATH));

		auto future = std::async(std::launch::async, API::SendReplayToDetector, std::string(TEMP_EXPORT_PATH));

		lastResults = future.get(); // this makes stutter idk why....
		std::filesystem::remove(TEMP_EXPORT_PATH);

	} catch (std::exception& e) {
		LOG(std::string(" > FAILED to save and submit replay, exception: ") + e.what());
		return;
	}

	LOG(" > Finished processing replay!");
}

void WhosBottingPlugin::ClearResults(ServerWrapper server, void* params, std::string event_name) {
	lastResults.clear();
}

void WhosBottingPlugin::OnKeybindPress() {
	// interpolated google told me to do this i dont understand this :( sorry i didnt
	// know how to call hk_on_game_end outside of an hook :(
	if (!coolEnabled) return;
	if (gameWrapper->IsInOnlineGame()) {
		ServerWrapper sw = gameWrapper->GetOnlineGame();
		if (!sw) {
			LOG("No online game state");
			return;
		}
		hk_OnGameEnd(sw, nullptr, "fake_hook");
	} else {
		ServerWrapper sw = gameWrapper->GetGameEventAsServer();
		if (!sw) {
			LOG("No server game state");
			return;
		}
		hk_OnGameEnd(sw, nullptr, "fake_hook");
	}
}

void WhosBottingPlugin::BindKey(std::string key) {
	if (key.empty()) return;
	cvarManager->executeCommand("bind " + key + " whoisbotting_k_pressed");
}

void WhosBottingPlugin::UnbindKey(std::string key) {
	cvarManager->executeCommand("unbind " + key);
}