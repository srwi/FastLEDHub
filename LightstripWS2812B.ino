#include <Arduino.h>
#include <ESPEssentials.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Hardware.h"
#include "Config.h"
#include "WebUpdate.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("");

	Config.init();
	initController();
	initHardware();
	begin(Config.startup_effect);

	initESPEssentials("Lightstrip");
	WebServer.init();

	OTA.init("Lightstrip");
	initWebsocket();
	initWebUpdate();
}

void loop()
{
	handleESPEssentials();
	webSocket.loop();
	betterShow();
}