#include "Observer.h"

ping_option ping_options;
bool ping_active = false;
bool lastMobilePingResult = false;
bool lastDesktopPingResult = false;
uint8_t observer_counter = 0;
PingStatus current_ping = DESKTOP;
Ticker pingTicker;
WiFiClient observerClient;
String apiKey = "YDYOL94VS56UMQ78";
const char* server = "api.thingspeak.com";

void pingCallback(void* arg, void *pdata)
{
	struct ping_resp *pingrsp = (struct ping_resp *)pdata;

	if(current_ping == DESKTOP)
		lastDesktopPingResult = pingrsp->bytes > 0;
	else
		lastMobilePingResult = pingrsp->bytes > 0;

	current_ping = current_ping == DESKTOP ? MOBILE : DESKTOP;
	observer_counter++;
	ping_active = false;
}

void initObserver()
{
	ping_options.count = 1;
	ping_options.coarse_time = TIMEOUT_TIME; // Seconds needed until timeout
	ping_regist_recv(&ping_options, pingCallback);
	pingTicker.attach(PING_INTERVAL, pingTick);
}

void pingTick()
{
	if(ping_active || websocketConnectionCount || WebServer.isBusy())
	{
		return;
	}
	else
	{
		ping_active = true;

		// Switch to other ping host
		if(current_ping == DESKTOP)
		{	
			IPAddress remote_addr;
			WiFi.hostByName(Config.desktop_ip.c_str(), remote_addr);
			ping_options.ip = uint32_t(remote_addr);
		}
		else
		{
			IPAddress remote_addr;
			WiFi.hostByName(Config.mobile_ip.c_str(), remote_addr);
			ping_options.ip = uint32_t(remote_addr);
		}
	}

	ping_start(&ping_options);
}

void handleObserver()
{
	static uint8_t observer_counter_limit = 2;
	if(status == RUNNING && effectIndex != 2) // Running and not "Nox"
		observer_counter_limit = 14;
	else
		observer_counter_limit = 2;

	if(observer_counter < observer_counter_limit || ping_active || websocketConnectionCount || WebServer.isBusy())
		return;

	observer_counter = 0;

	if (observerClient.connect(server,80))
	{
		String postStr = apiKey;
		postStr +="&field1=";
		postStr += lastMobilePingResult ? "1" : "0";
		postStr +="&field2=";
		postStr += lastDesktopPingResult ? "1" : "0";
		postStr +="&field3=";
		postStr += Config.alarm_enabled ? (String(Config.alarm_hour) + "." + String(Config.alarm_minute)) : "-1";
		postStr +="&field4=";
		postStr += String(effectIndex);
		postStr += "\r\n\r\n";

		betterShow();
		observerClient.print("POST /update HTTP/1.1\n");
		betterShow();
		observerClient.print("Host: api.thingspeak.com\n");
		betterShow();
		observerClient.print("Connection: close\n");
		betterShow();
		observerClient.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
		betterShow();
		observerClient.print("Content-Type: application/x-www-form-urlencoded\n");
		betterShow();
		observerClient.print("Content-Length: ");
		betterShow();
		observerClient.print(postStr.length());
		betterShow();
		observerClient.print("\n\n");
		betterShow();
		observerClient.print(postStr);
		betterShow();

		betterShow();
		observerClient.print("POST /update HTTP/1.1\nHost: api.thingspeak.com\nConnection: close\nX-THINGSPEAKAPIKEY: " + apiKey + "\nContent-Type: application/x-www-form-urlencoded\nContent-Length: " + String(postStr.length()) + "\n\n" + postStr);
		betterShow();

		Serial.println("Sending");
	}
	else
	{
		Serial.println("Not sending :(");
	}
	observerClient.stop();
}