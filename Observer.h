#ifndef _OBSERVER_H
#define _OBSERVER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#include "Config.h"

extern "C" {
#include "ping.h"
}
#include "os_type.h"


#define PING_INTERVAL 15 // Seconds
#define TIMEOUT_TIME 1

enum PingStatus
{
	DESKTOP,
	MOBILE
};

void pingCallback(void* arg, void *pdata);
void initObserver();
void pingTick();

#endif