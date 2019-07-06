#include "WebSocket.h"

WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t websocketConnectionCount = 0;
bool liveDataHasChanged = false;
String ipString;

void initWebsocket()
{
  webSocket.begin();
  webSocket.onEvent(websocketEvent);
  if(MDNS.begin("lightstrip"))
  {
    Serial.println("[Wifi] MDNS responder started");
  }
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

void websocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch(type)
  {
    case WStype_DISCONNECTED:
      // Save config if live data (custom color or speed) has changed within websocket connection
      if(liveDataHasChanged)
      {
        Config.save();
        liveDataHasChanged = false;
      }
      websocketConnectionCount--;
      Serial.printf("[Websocket] [%u] Disconnected!\n", num);
    break;
    case WStype_CONNECTED:
    {
      websocketConnectionCount++;
      IPAddress ip = webSocket.remoteIP(num);
      ipString = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
      Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
    }
    break;
    case WStype_TEXT:
      //Serial.printf("[Websocket] [%u] get Text: %s\n", num, payload);
      handleWebsocketText(byteArrayToString(payload), num);
    break;
    case WStype_BIN:
      //Serial.printf("[Websocket] [%u] get Binary: %s\n", num, payload);
      handleWebsocketBinary(payload, num);
    break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    break;
  }
}

void handleWebsocketText(String text, uint8_t num)
{
  char textArray[text.length()];
  text.toCharArray(textArray, text.length());

  if(Config.parseJSON(textArray))
  {
    Config.save();
    return;
  }

  if(text.startsWith("toggle"))
  {
    String effectName = text.substring(7);

    ///////////toggle(effectName);
  }
  else if(text == "requesting_config")
  {
    webSocket.sendTXT(num, Config.getJSON().c_str());
  }
  else
  {
    Serial.println("Command '" + text + "' not found!");
  }
}

String rgbToHex(uint8_t r, uint8_t g, uint8_t b)
{
  String output = "";
  
  if(r == 0)
    output += "00";
  else
    output += (r < 16 ? "0" : "") + String(r, HEX);

  if(g == 0)
    output += "00";
  else
    output += (g < 16 ? "0" : "") + String(g, HEX);

  if(b == 0)
    output += "00";
  else
    output += (b < 16 ? "0" : "") + String(b, HEX);

  return output;
}

void handleWebsocketBinary(uint8_t *binary, uint8_t num)
{
  // switch(binary[0])
  // {
  //  case 0: // Custom Color
  //    customColorNamespace::set(CRGB(binary[1], binary[2], binary[3]));
  //    Config.custom_color = rgbToHex(binary[1], binary[2], binary[3]);
  //    begin(customColor);
  //    liveDataHasChanged = true;
  //    webSocket.sendTXT(num, String("ok").c_str());
  //  break;
  //  case 1: // Speed
  //    setSpeed(binary[1]);
  //    liveDataHasChanged = true;
  //    webSocket.sendTXT(num, String("ok").c_str());
  //  break;
  //  case 4: // Spectroscope data
  //    if(status != STOPPED)
  //      stop();
  //    for(uint16_t i = 0; i < NUM_LEDS; i++)
  //    {
  //      strip[i] = CRGB(binary[1 + i*3], binary[2 + i*3], binary[3 + i*3]);
  //    }
  //  break;
  //  case 5: // Saturation
  //    Config.saturation = binary[1];
  //    liveDataHasChanged = true;
  //    webSocket.sendTXT(num, String("ok").c_str());
  //  break;
  //  case 6: // linearSpectroscope data
  //    linearSpectroscope(binary+1);
  //  break;
  //  case 7: // symmetricalSpectroscope data
  //    symmetricalSpectroscope(binary+1);
  //  break;
  //  case 8: // Custom Color 2
  //    customColor2Namespace::set(CRGB(binary[1], binary[2], binary[3]));
  //    Config.custom_color2 = rgbToHex(binary[1], binary[2], binary[3]);
  //    begin(customColor2);
  //    liveDataHasChanged = true;
  //    webSocket.sendTXT(num, String("ok").c_str());
  //  break;
  // }
}

String byteArrayToString(uint8_t *bytes)
{
  String s = "";
  for(uint16_t i = 0; bytes[i] != '\0'; i++)
  {
    s += char(bytes[i]);
  }
  s += '\0';
  return s;
}

void broadcastStatus()
{
  // Send status as JSON
  // String msg = "{\n  \"status\": " + String((int)status) + ",\n  \"current_effect\": \"" + effectList.get(effectIndex).name + "\"\n}";
  // webSocket.broadcastTXT(msg.c_str());
}