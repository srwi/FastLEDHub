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


#define PING_INTERVAL 8 //s
#define TIMEOUT_TIME 7 //s

enum PingStatus
{
	DESKTOP,
	MOBILE
};

void pingCallback(void* arg, void *pdata);
void initObserver();
void pingTick();
void handleObserver();

#endif