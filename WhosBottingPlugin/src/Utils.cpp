#include "Utils.h"

ReplaySoccarWrapper Utils::GetReplayFromServer(ServerWrapper server) {
	ReplayDirectorWrapper replayDirector = server.GetReplayDirector();
	if (!replayDirector) return NULL;

	ReplaySoccarWrapper soccarReplay = replayDirector.GetReplay();
	if (!soccarReplay) return NULL;

	return soccarReplay;
}

std::string Utils::GetReplayID(ReplaySoccarWrapper replay) {
	return replay.GetId().ToString();
}
