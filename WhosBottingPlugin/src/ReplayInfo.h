#pragma once
#include "Framework.h"

struct ReplayInfo {
	std::string id;
	size_t numFrames;

	ReplayInfo(ReplaySoccarWrapper replay) {
		id = replay.GetId().ToString();
		numFrames = replay.GetNumFrames();
	}

	bool operator==(const ReplayInfo& other) const {
		return id == other.id && numFrames == other.numFrames;
	}

	bool operator!=(const ReplayInfo& other) const {
		return !(*this == other);
	}
};