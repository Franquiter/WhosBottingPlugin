#pragma once
#include "Framework.h"

namespace Utils {
	ReplaySoccarWrapper GetReplayFromServer(ServerWrapper server);
	std::string GetReplayID(ReplaySoccarWrapper replay);
} // namespace Utils