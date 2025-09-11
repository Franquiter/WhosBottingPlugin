#include "pch.h"
#include <windows.h>
#include <shlobj.h>
#include <string>
#include "paths.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include <iostream>
#include <filesystem>

#pragma comment(lib, "shell32.lib")

std::string GetDocumentsPath() {
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

    if (result != S_OK) {
        return ""; // or handle error
    }
    return std::string(my_documents);
}

std::string GetDemosPath(GameWrapper* gw) {
    auto myID = gw->GetUniqueID();

    int platform = myID.GetPlatform();

    std::string path;
    if (platform == 1) { //steam
        path = GetDocumentsPath() + "\\My Games\\Rocket League\\TAGame\\Demos";
    }
    else if (platform == 11) {//epic
        path = GetDocumentsPath() + "\\My Games\\Rocket League\\TAGame\\DemosEpic";
    }
    else {
        return "";//unknown
    }
	return path;
}
std::string GetLatestReplay(GameWrapper* gw) {
	auto path = GetDemosPath(gw);
    if (path.empty()) {
        return "";
    }


    std::string latestReplay;
    std::filesystem::file_time_type latestTime;
    for (const auto& entry : std::filesystem::directory_iterator(path))
        if (entry.is_regular_file() && entry.path().extension() == ".replay") {
            auto ftime = std::filesystem::last_write_time(entry);
            if (latestReplay.empty() || ftime > latestTime) {
                latestTime = ftime;
                latestReplay = entry.path().string();
			}
        }

    return latestReplay;
}
