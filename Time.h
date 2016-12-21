#ifndef _TIME_H
#define _TIME_H

#include <ESPEssentials.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

#include "WebSocket.h"
#include "EffectController.h"
#include "Config.h"

void initTime();
void getSunsetTime();
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

#endif