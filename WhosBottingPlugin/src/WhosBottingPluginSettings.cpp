#include "src/framework.h"
#include "WhosBottingPlugin.h"
#include "keys.h"
void WhosBottingPlugin::RenderSettings() {
    ImGui::TextUnformatted("A really cool plugin");
    static int selected = 0;

    CVarWrapper enableCvar = cvarManager->getCvar("wbp_enabled");
    if (!enableCvar) { return; }
    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable plugin", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle Cool Plugin");
    }
    if (ImGui::ListBox("Add Keybind", &selected, possibleKeyNames, IM_ARRAYSIZE(possibleKeyNames), 4)) {
        cvarManager->getCvar("whoisbotting_keybind").setValue(possibleKeyNames[selected]);
    }
}