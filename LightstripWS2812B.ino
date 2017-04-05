#include <Arduino.h>
#include <ESPEssentials.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Time.h"
#include "Hardware.h"
#include "Config.h"
#include "Fade.h"
#include "WebUpdate.h"
#include "Observer.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("");

	Config.init();
	initController();
	initHardware();
	begin("Nox");

	Wifi.autoConnect("Lightstrip AP (192.168.4.1)");
	WebServer.init();

	initTime();
	if(!Wifi.isAP())
	{
		OTA.init("Lightstrip");
		initWebsocket();
		initWebUpdate();
		initObserver();
	}
}

void loop()
{
	if(!Wifi.isAP())
	{
		OTA.handle();
		webSocket.loop();
	}
	WebServer.handleClient();
	
	handleFade();
	handleObserver();
	betterShow();
}