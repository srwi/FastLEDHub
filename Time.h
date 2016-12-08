#ifndef _TIME_H
#define _TIME_H

#include <ESPEssentials.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Config.h"

struct myTime
{
	int8_t hour;
	int8_t minute;
};

void initTime();
myTime getSunsetTime();
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

#endif