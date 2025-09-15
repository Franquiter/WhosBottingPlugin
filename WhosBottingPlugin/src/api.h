#pragma once
#include <map>
#include <string>
#include <vector>

namespace API {
	// Result of a replay analysis
	struct Result {
		std::map<std::string, int> playerPercents;
		std::string errorMsg;

		bool IsValid() const {
			return !playerPercents.empty();
		}
	};

	Result SendReplayToDetector(const std::vector<uint8_t>& replayBytes);
} // namespace API