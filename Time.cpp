#include "Time.h"

IPAddress timeServer(129, 6, 15, 28);//(37,34,57,151);
WiFiUDP Udp;
unsigned int localPort = 2390;//8888;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

Ticker alarmTimer;
Ticker wakeupTimer;
Ticker holdTimer;
myTime sunsetTime;

void initTime()
{
	Udp.begin(localPort);
	setSyncProvider(getNtpTime);
	setTime(hour(),minute(),0,1,1,11);
	Serial.println("[Time] Current time: " + String(hour()) + ":" + String(minute()));
	
	sunsetTime = getSunsetTime();
	Serial.println("[Wakeup] Wakeup time will be: " + String(sunsetTime.hour) + ":" + String(sunsetTime.minute));
}

myTime getSunsetTime()
{
	Serial.print("[Wakeup] Getting sunset time...");

	// Request data via GET request
	WiFiClient client;
	const char* host = "api.sunrise-sunset.org";
	String url = "/json?lat=" + String(Config.latitude) + "&lng=" + String(Config.longitude) + "&date=today";
	if (!client.connect(host, 80))
	{
		Serial.println("failed!\nUsing fallback time.");
		return { Config.sunset_hour, Config.sunset_minute };
	}
	client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
	unsigned long timeout = millis();
	while (client.available() == 0)
	{
		if (millis() - timeout > 1500)
		{
			Serial.println("failed!\nUsing fallback time.");
			client.stop();
			return { Config.sunset_hour, Config.sunset_minute };
		}
	}

	// Read result of GET request
	String result;
	while(client.available())
		result += client.readStringUntil('\r');

	// Crop actual JSON data
	result = result.substring(result.indexOf('{'), result.lastIndexOf('}') + 1);
	
	// Get sunset time from JSON data
	StaticJsonBuffer<1000> jsonBuffer; // TODO: optimize size
	JsonObject& root = jsonBuffer.parseObject(result);
	String sunset = root["results"]["sunset"];

	if(sunset == "")
		return { Config.sunset_hour, Config.sunset_minute };

	// Generate wakeup time from JSON and saved settings
	int8_t hour = 0;
	int16_t minute = 0;
	hour = sunset.substring(0, sunset.indexOf(':')).toInt();
	minute = sunset.substring(sunset.indexOf(':') + 1, sunset.lastIndexOf(':')).toInt();
	hour += Config.time_zone;
	hour += Config.summer_time;
	minute += Config.sunset_offset;
	if(sunset.endsWith("PM"))
		hour += 12;
	while(minute >= 60)
	{
		minute -= 60;
		hour++;
	}
	while(minute < 0)
	{
		minute += 60;
		hour--;
	}
	// TODO: use modulo
	while(hour >= 24)
		hour -= 24;
	while(hour < 0)
		hour += 24;

	// Save time as new fallback_time in case there is no wifi connection next time
	Config.sunset_hour = hour;
	Config.sunset_minute = minute;
	Config.save();

	Serial.println("ok!");
	return myTime {hour, minute};
}

time_t getNtpTime()
{
	// Don't get time while websocket connection is open (Could cause connection to drop)
	if(websocketConnectionCount > 0)
	{
		Serial.println("[Time] Won't get time because there are active websocket connections.");
		return now();
	}

	Serial.print("[Time] Getting time from NTP server...");
	time_t time = 0;
	uint32_t beginWait_1 = millis();
	while(time == 0 && millis() - beginWait_1 < 5000)
	{
		// Try to get time
		while (Udp.parsePacket() > 0);
		sendNTPpacket(timeServer);
		uint32_t beginWait_2 = millis();
		while (time == 0 && millis() - beginWait_2 < 1500)
		{
			int size = Udp.parsePacket();
			if (size >= NTP_PACKET_SIZE)
			{
				Udp.read(packetBuffer, NTP_PACKET_SIZE);
				unsigned long secsSince1900;
				secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
				secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
				secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
				secsSince1900 |= (unsigned long)packetBuffer[43];
				time = secsSince1900 - 2208988800UL + ( Config.time_zone + Config.summer_time ) * SECS_PER_HOUR;
			}
		}
	}

	if(time)
		Serial.println("ok!");
	else
		Serial.println("failed!");

	return time;
}

void sendNTPpacket(IPAddress &address)
{
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	packetBuffer[0] = 0b11100011;
	packetBuffer[1] = 0;
	packetBuffer[2] = 6;
	packetBuffer[3] = 0xEC;
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	Udp.beginPacket(address, 123);
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}