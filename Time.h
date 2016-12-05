#ifndef _TIME_H
#define _TIME_H

#include <TimeLib.h>
#include <ESPEssentials.h>
#include <ArduinoJson.h>

#include "EffectController.h"
#include "Config.h"

#include "Effects/Wakeup.h"

void initTime();
myTime getSunsetTime();
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
void handleAlarm();

#endif