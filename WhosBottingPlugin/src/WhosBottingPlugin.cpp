#include "WhosBottingPlugin.h"

#include "Utils.h"
#include "api.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "src/framework.h"

BAKKESMOD_PLUGIN(WhosBottingPlugin, "WhosBottingPlugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> g_GlobalCvarManager; // TODO: Move
bool pluginEnabled = true;								 // TODO: Move

void WhosBottingPlugin::onLoad() {
	// This line is required for LOG to work and must be before any use of LOG()
	g_GlobalCvarManager = cvarManager;

	cvarManager->registerCvar("wbp_enabled", "1", "Enable the plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) { pluginEnabled = cvar.getBoolValue(); });
	cvarManager->registerCvar("whoisbotting_keybind", "F", "Keybind name", true, true)
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
			bind(&WhosBottingPlugin::hk_OnGameLeft, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
		);
	}

	// Here we display toasts for all processed replays
	// TODO: Organize, make replay result a structure
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		if (replaySendState.has_value()) {
			// Cool trick to check if our future is done yet
			if (!IsReplaySending()) {
				auto result = replaySendState->resultFuture.get();
				if (result.IsValid()) {
					std::stringstream stream = {};
					for (auto [player, percent] : result.playerPercents) {
						stream << player << ": " << percent << "%" << std::endl;
					}
					ShowNotif("Who was botting?", stream.str());
				} else {
					ShowError("Replay analysis failed", "Error from server: " + result.errorMsg);
				}

				// Remove optional value
				replaySendState = std::nullopt;
			}
		}
	});
}

void WhosBottingPlugin::TrySendReplay(ServerWrapper server, bool isMidGame) {
	if (!pluginEnabled) return;

	//  Ref:
	//  https://github.com/bakkesmodorg/AutoReplayUploader/blob/master/AutoReplayUploader/AutoReplayUploaderPlugin.cpp#L295

	auto soccarReplay = Utils::GetReplayFromServer(server);
	if (!soccarReplay) return;

	if (soccarReplay.GetbFileCorrupted()) {
		LOG("TrySendReplay(): Skipping corrupted replay");
		return;
	}

	constexpr int MIN_FRAMES = 100;
	if (soccarReplay.GetNumFrames() < MIN_FRAMES) {
		LOG("TrySendReplay(): Skipping replay with only {} frames", soccarReplay.GetNumFrames());
		return;
	}

	if (server.IsPlayingTraining()) {
		LOG("TrySendReplay(): Skipping training replay");
		return;
	}

	if (server.GetNumPlayers() < 2) {
		LOG("TrySendReplay(): Disgarding replay with only {} player(s)", server.GetNumPlayers());
		return;
	}

	// Prevent sending while a request is still pending
	// Otherwise, it will lag them because it will forcibly await the previous request
	if (IsReplaySending()) {
		LOG("TrySendReplay(): A replay is already sending, ignoring...");
		return;
	}

	auto replayInfo = ReplayInfo(soccarReplay);
	if (replayInfo.id == lastSentCompletedReplayInfo->id) {
		LOG("TrySendReplay(): Replay already submitted, ignoring...");
		return;
	}
	try {
		constexpr const char* TEMP_EXPORT_PATH = "___wbp_temp_replay_export.replay";
		soccarReplay.StopRecord();
		soccarReplay.ExportReplay(std::filesystem::path(TEMP_EXPORT_PATH));

		std::ifstream replayFileStream = std::ifstream(TEMP_EXPORT_PATH, std::ios::binary);
		std::vector<uint8_t> replayBytes =
			std::vector<uint8_t>((std::istreambuf_iterator<char>(replayFileStream)), std::istreambuf_iterator<char>());
		replayFileStream.close();
		std::filesystem::remove(TEMP_EXPORT_PATH);

		SendReplayAsync(replayBytes);
		LOG("Replay sent successfully!");
	} catch (std::exception& e) {
		ShowError("Replay analysis failed", std::format("Exception thrown during submission: \"{}\"", e.what()));
		return;
	}

	if (!isMidGame) {
		// Only if the game is completed
		lastSentCompletedReplayInfo = replayInfo;
	}
}

void WhosBottingPlugin::OnKeybindPress() {
	if (!pluginEnabled) return;

	ServerWrapper server = gameWrapper->GetOnlineGame();
	if (server) {
		TrySendReplay(server, true);
	}
}

void WhosBottingPlugin::SendReplayAsync(const std::vector<uint8_t>& replayBytes) {
	replaySendState =
		SendReplayState{std::async(std::launch::async, API::SendReplayToDetector, replayBytes), std::chrono::system_clock::now()};
}

bool WhosBottingPlugin::IsReplaySending() {
	if (replaySendState.has_value()) {
		bool isReady = replaySendState->resultFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
		return !isReady;
	} else {
		return false;
	}
}

void WhosBottingPlugin::hk_OnGameEnd(ServerWrapper server, void* params, std::string eventName) {
	TrySendReplay(server, false);
}

void WhosBottingPlugin::hk_OnGameLeft(ServerWrapper server, void* params, std::string eventName) {
	TrySendReplay(server, false);
}

void WhosBottingPlugin::ShowNotif(std::string title, std::string description, bool isError) {
	// Force-enable toast notifications
	if (auto notifCvar = cvarManager->getCvar("cl_notifications_enabled_beta")) {
		notifCvar.setValue(1);
	}

	constexpr float DURATION = 7.0f;
	gameWrapper->Toast(title, description, "default", DURATION, isError ? ToastType_Error : ToastType_Info);
}

void WhosBottingPlugin::BindKey(std::string key) {
	if (key.empty()) return;
	cvarManager->executeCommand("bind " + key + " whoisbotting_k_pressed");
}

void WhosBottingPlugin::UnbindKey(std::string key) {
	cvarManager->executeCommand("unbind " + key);
}