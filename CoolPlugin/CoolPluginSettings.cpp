#include "pch.h"
#include "CoolPlugin.h"

void CoolPlugin::RenderSettings() {
    ImGui::TextUnformatted("A really cool plugin");


    CVarWrapper enableCvar = cvarManager->getCvar("cool_enabled");
    if (!enableCvar) { return; }
    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable plugin", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle Cool Plugin");
    }

}