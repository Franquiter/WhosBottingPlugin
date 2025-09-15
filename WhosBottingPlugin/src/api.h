#pragma once
#include <string>
#include <map>

namespace API {
	std::map<std::string, int> send_replay_to_detector(std::string replay_path);
}