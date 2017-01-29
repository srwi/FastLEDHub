#include "Observer.h"

ping_option ping_options;
bool ping_active = false;
PingStatus current_ping = DESKTOP;
Ticker pingTicker;

void pingCallback(void* arg, void *pdata)
{
	struct ping_resp *pingrsp = (struct ping_resp *)pdata;

	if (pingrsp->bytes > 0)
	{
		Serial.println("Success: " + String((int)current_ping));
	}
	else
	{
		Serial.println("Timeout: " + String((int)current_ping));
	}

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
			current_ping = MOBILE;
		}
		else
		{
			IPAddress remote_addr;
			WiFi.hostByName(Config.mobile_ip.c_str(), remote_addr);
			ping_options.ip = uint32_t(remote_addr);
			current_ping = DESKTOP;
		}
	}

	ping_start(&ping_options);
}