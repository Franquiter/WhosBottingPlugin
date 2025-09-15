#include "GuiBase.h"

#include "src/framework.h"

std::string SettingsWindowBase::GetPluginName() {
	return "WhosBottingPlugin";
}

void SettingsWindowBase::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

std::string PluginWindowBase::GetMenuName() {
	return "WhosBottingPlugin";
}

std::string PluginWindowBase::GetMenuTitle() {
	return menuTitle;
}

void PluginWindowBase::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

bool PluginWindowBase::ShouldBlockInput() {
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool PluginWindowBase::IsActiveOverlay() {
	return true;
}

void PluginWindowBase::OnOpen() {
	isWindowOpen = true;
}

void PluginWindowBase::OnClose() {
	isWindowOpen = false;
}

void PluginWindowBase::Render() {
	if (!ImGui::Begin(menuTitle.c_str(), &isWindowOpen, ImGuiWindowFlags_None)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();

	ImGui::End();

	if (!isWindowOpen) {
		g_GlobalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}