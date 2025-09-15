
#include "api.h"

#include <fstream>
#include <iostream>
#include <json/json.hpp>
#include <vector>

#include "src/framework.h"
using json = nlohmann::json;

#include <cpr/cpr.h>

std::map<std::string, int> API::SendReplayToDetector(std::string replay_path) {
	std::ifstream replay(replay_path, std::ios::binary);

	std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(replay)), std::istreambuf_iterator<char>());

	auto response = cpr::Post(
		cpr::Url{"https://whosbotting.com/analyze"},
		cpr::Header{{"Content-Type", "text/plain"}, {"Accept", "application/json"}},
		cpr::Body{std::string(buffer.begin(), buffer.end())}
	);
	std::map<std::string, int> results;
	if (response.status_code == 200) {
		json j = json::parse(response.text);
		// ofstream out("analysis.json", std::ios::binary);
		// out << j.dump(4);
		// out.close();
		// cout << j << endl;
		// cout << j["player_results"] << endl;

		for (const auto player : j["player_results"]) {
			std::string name = player["name"].get<std::string>();
			int confidence = player["confidence_percent"].get<int>();

			results[name] = confidence;
			// cout<<"Botting confidence: "<<name<<":"<<confidence<<endl;
		}
	} else {
		LOG(std::string("Error:") + std::to_string(response.status_code) + " " + response.error.message);
	}

	return results;
}
