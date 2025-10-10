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
	return false;
}

void PluginWindowBase::OnOpen() {
	isWindowOpen = true;
}

void PluginWindowBase::OnClose() {
	isWindowOpen = false;
}

void PluginWindowBase::Render() {
	ImGuiIO& io = ImGui::GetIO(); //get the viewport information
	ImGui::SetNextWindowPos(ImVec2(0, 0)); //this is top left corner origin of display 

	ImGui::SetNextWindowSize(io.DisplaySize); //i now set this to be size of the display so i can later draw text in arbitrary position
	
	if (!ImGui::Begin(
			menuTitle.c_str(),
			&isWindowOpen,
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoInputs
		 ))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	RenderWindow();
	ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	ImGui::End();

}