#include <ESPEssentials.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Time.h"
#include "Hardware.h"
#include "Config.h"
#include "Fade.h"

void setup()
{
	Serial.begin(115200);
	Serial.println("");
	
	Wifi.autoConnect("Lightstrip AP (192.168.4.1)");
	WebServer.init();
	if(Wifi.isAP())
		return;
	OTA.init("Lightstrip");
	
	Config.init();
	initHardware();
	initController();
	begin(Config.last_effect);
	initWebsocket();
	initTime();
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
		handleFade();

		// TODO: only show() if using dither and while fade is active? Or maybe if no websocket connection is open. Or maybe dont show() when using Custom Color effect.
		// Or maybe if brightness != 255
		FastLED.show();
	}
}