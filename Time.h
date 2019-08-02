#ifndef _TIME_H
#define _TIME_H

#include <Arduino.h>
#include <ESPEssentials.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

#include "WebSocket.h"
#include "Animation.h"
#include "Config.h"

void initTime();
void getSunsetTime();
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);

#endif