#pragma once
#include <map>
#include <string>

namespace API {
	std::map<std::string, int> SendReplayToDetector(std::string replay_path);
}