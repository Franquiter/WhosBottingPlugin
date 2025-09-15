
#include "src/framework.h"
#include "api.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
#include "json.hpp"
using json = nlohmann::json;
#include <cpr/cpr.h>
using namespace cpr;

map<string, int> zealan_api(string replay_path) {
    ifstream replay(replay_path, ios::binary);

    vector<uint8_t> buffer(
        (istreambuf_iterator<char>(replay)),
        istreambuf_iterator<char>()
    );

    auto r = Post(
        Url{ "https://whosbotting.com/analyze" },
        Header{ {"Content-Type", "text/plain"}, {"Accept", "application/json"} },
        Body{ string(buffer.begin(), buffer.end()) }
    );
    map<string, int> results;
    if (r.status_code == 200) {
        json j = json::parse(r.text);
        //ofstream out("analysis.json", std::ios::binary);
        //out << j.dump(4);
        //out.close();
        //cout << j << endl;
        //cout << j["player_results"] << endl;


        for (const auto player : j["player_results"]) {
            string name = player["name"].get<string>();
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

