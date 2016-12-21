#include <ESPEssentials.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Time.h"
#include "Hardware.h"
#include "Config.h"
#include "Fade.h"
#include "WebUpdate.h"

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
	}
}

void loop()
{
	if(Wifi.isAP())
	{
		WebServer.handleClient();
	}
	else
	{
		WebServer.handleClient();
		OTA.handle();
		webSocket.loop();
	}

	FastLED.show();
	handleFade();
}