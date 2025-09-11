#include "pch.h"
#include "CoolPlugin.h"
#include "bakkesmod/wrappers/GameWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include <map> 
#include <iostream>
#include "api.h"
#include "paths.h"



BAKKESMOD_PLUGIN(CoolPlugin, "Cool Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool coolEnabled = false;

void CoolPlugin::onLoad()
{
	// This line is required for LOG to work and must be before any use of LOG()
	_globalCvarManager = cvarManager;
	// do something when it loads
	LOG("Hello I'm CoolPlugin B)");



	cvarManager->registerCvar("cool_enabled", "0", "Enable Cool", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		coolEnabled = cvar.getBoolValue();
			});

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {

		if (!coolEnabled) return;
		//auto results = zealan_api(GetDocumentsPath());
		canvas.SetColor(255, 0, 0, 255);            // Red
		canvas.SetPosition(Vector2{ 50, 50 });
		auto path = GetDemosPath(gameWrapper.get());
		canvas.DrawString(path , 2, 2);
		});


}

void CoolPlugin::onUnload() {
	LOG("I was too cool for this world B'(");
}
