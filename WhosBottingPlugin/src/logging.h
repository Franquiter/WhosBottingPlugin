// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once
#include <format>
#include <memory>
#include <source_location>
#include <string>

#include "bakkesmod/wrappers/cvarmanagerwrapper.h"

extern std::shared_ptr<CVarManagerWrapper> g_GlobalCvarManager;

template <typename... Args>
void LOG(std::string str, Args&&... args) {
	if (!g_GlobalCvarManager) return;

	std::string formatted = std::vformat(str, std::make_format_args(args...));
	g_GlobalCvarManager->log(formatted);
}