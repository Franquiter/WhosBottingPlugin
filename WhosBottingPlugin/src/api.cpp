
#include "src/framework.h"
#include "api.h"
#include <iostream>
#include <fstream>
#include <vector>

#include <json/json.hpp>
using json = nlohmann::json;

#include <cpr/cpr.h>

std::map<std::string, int> API::send_replay_to_detector(std::string replay_path) {
    std::ifstream replay(replay_path, std::ios::binary);

   std::vector<uint8_t> buffer(
        (std::istreambuf_iterator<char>(replay)),
         std::istreambuf_iterator<char>()
    );

    auto r = cpr::Post(
		cpr::Url{ "https://whosbotting.com/analyze" },
		cpr::Header{ {"Content-Type", "text/plain"}, {"Accept", "application/json"} },
		cpr::Body{ std::string(buffer.begin(), buffer.end()) }
    );
	std::map<std::string, int> results;
    if (r.status_code == 200) {
        json j = json::parse(r.text);
        //ofstream out("analysis.json", std::ios::binary);
        //out << j.dump(4);
        //out.close();
        //cout << j << endl;
        //cout << j["player_results"] << endl;


        for (const auto player : j["player_results"]) {
			std::string name = player["name"].get<std::string>();
            int confidence = player["confidence_percent"].get<int>();

            results[name] = confidence;
            //cout<<"Botting confidence: "<<name<<":"<<confidence<<endl;
        }

    }
    else {
        
        LOG(std::string("Error:") + std::to_string(r.status_code) + " " + r.error.message);
    }

    return results;
}

