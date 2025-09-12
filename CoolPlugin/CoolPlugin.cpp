#include "pch.h"
#include "CoolPlugin.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include <map> 
#include <iostream>
#include "api.h"
#include "paths.h"



BAKKESMOD_PLUGIN(CoolPlugin, "Cool Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool coolEnabled = false;

void CoolPlugin::onLoad()
{
	// This line is required for LOG to work and must be before any use of LOG()
	_globalCvarManager = cvarManager;
	// do something when it loads
	LOG("Hello I'm CoolPlugin B)");



	cvarManager->registerCvar("cool_enabled", "0", "Enable Cool", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		coolEnabled = cvar.getBoolValue();
			});
	{ // Hook game ending events like https://github.com/bakkesmodorg/AutoReplayUploader does
		if (!coolEnabled) return;
		gameWrapper->HookEventWithCaller<ServerWrapper>(
			"Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
			bind(
				&CoolPlugin::hk_on_game_end,
				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
			)
		);

		gameWrapper->HookEventWithCaller<ServerWrapper>(
			"Function TAGame.GameEvent_Soccar_TA.Destroyed",
			bind(
				&CoolPlugin::hk_on_game_end,
				this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3
			)
		);
	}

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {

		if (!coolEnabled) return;
		if (last_results_.empty()) return;

		for (auto result : last_results_) {
			//auto results = zealan_api(GetDocumentsPath());
			canvas.SetColor(255, 0, 0, 255);            // Red
			canvas.SetPosition(Vector2{ 50, 50 });
			auto path = GetDemosPath(gameWrapper.get());
			canvas.DrawString(path, 2, 2);
		}

		});


}

void CoolPlugin::onUnload() {
	LOG("I was too cool for this world B'(");
}


void CoolPlugin::hk_on_game_end(ServerWrapper server, void* params, std::string event_name) {
	if (!coolEnabled) return;
	// Ref: https://github.com/bakkesmodorg/AutoReplayUploader/blob/master/AutoReplayUploader/AutoReplayUploaderPlugin.cpp#L295

	ReplayDirectorWrapper replay_director = server.GetReplayDirector();
	if (!replay_director)
		return;

	ReplaySoccarWrapper soccar_replay = replay_director.GetReplay();
	if (!soccar_replay)
		return;

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


		

		last_results_ = zealan_api(std::string(TEMP_EXPORT_PATH));
		std::filesystem::remove(TEMP_EXPORT_PATH);


	}
	catch (std::exception& e) {
		LOG(" > FAILED to save and submit replay, exception: ", e.what());
		return;
	}

	LOG(" > Finished processing replay!");
}