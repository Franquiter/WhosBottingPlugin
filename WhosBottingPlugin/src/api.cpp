
#include "api.h"

#include <fstream>
#include <iostream>
#include <json/json.hpp>
#include <vector>

#include "src/framework.h"
using json = nlohmann::json;

#include <cpr/cpr.h>

API::Result API::SendReplayToDetector(const std::vector<uint8_t>& replayBytes) {
	auto response = cpr::Post(
		cpr::Url{"https://whosbotting.com/analyze"},
		cpr::Header{{"Content-Type", "text/plain"}, {"Accept", "application/json"}},
		cpr::Body{std::string(replayBytes.begin(), replayBytes.end())}
	);

	Result result;
	if (response.status_code == 200) {
		json j = json::parse(response.text);

		for (const auto player : j["player_results"]) {
			std::string name = player["name"].get<std::string>();
			int confidence = player["confidence_percent"].get<int>();

			result.playerPercents[name] = confidence;
		}
	} else {
		LOG("Replay analysis failed, server error: " + response.text);
		result.errorMsg = std::format("{}: {}", response.status_code, response.text);
	}

	return result;
}
