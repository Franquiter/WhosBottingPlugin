#include "WhosBottingPlugin.h"
#include "keys.h"
#include "src/framework.h"
void WhosBottingPlugin::RenderSettings() {
	ImGui::TextUnformatted("A really cool plugin");
	static int selected = 0;

	CVarWrapper enableCvar = cvarManager->getCvar("wbp_enabled");
	if (!enableCvar) {
		return;
	}
	bool enabled = enableCvar.getBoolValue();
	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Cool Plugin");
	}
	if (ImGui::ListBox("Add Keybind", &selected, POSSIBLE_KEY_NAMES, IM_ARRAYSIZE(POSSIBLE_KEY_NAMES), 4)) {
		cvarManager->getCvar("whoisbotting_keybind").setValue(POSSIBLE_KEY_NAMES[selected]);
	}
}