#include "WebUpdate.h"

const char* serverIndex = "<form method='POST' action='/handle_update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void initWebUpdate()
{
  // TODO: Move this to ESPEssentials library
  WebServer.on("/update", HTTP_GET, []() {
    WebServer.sendHeader("Connection", "close");
    WebServer.sendHeader("Access-Control-Allow-Origin", "*");
    WebServer.send(200, "text/html", serverIndex);
  });
  WebServer.on("/handle_update", HTTP_POST, []() {
    WebServer.sendHeader("Connection", "close");
    WebServer.sendHeader("Access-Control-Allow-Origin", "*");
    WebServer.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    ESP.restart();
  },[]() {
    HTTPUpload& upload = WebServer.upload();
    if(upload.status == UPLOAD_FILE_START){
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace))
      {
        Update.printError(Serial);
      }
    }
    else if(upload.status == UPLOAD_FILE_WRITE)
    {
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    }
    else if(upload.status == UPLOAD_FILE_END)
    {
      if(Update.end(true))
      {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      }
      else
      {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });

  // TODO: Move this somewhere else as well
  WebServer.on("/command/remove_wifi_credentials", HTTP_GET, [&]()
  {
    // TODO: Wifi.saveCredentials("", "");
    WebServer.send(200, "text/plain", "Wifi credentials removed. Rebooting...");
    delay(30);
    ESP.restart();
  });
  WebServer.on("/command/reboot", HTTP_GET, [&]()
  {
    ESP.restart();
    WebServer.send(200, "text/plain", "Rebooting...");
  });
  WebServer.on("/command/stop", HTTP_GET, [&]()
  {
    //stop();
    WebServer.send(200, "text/plain", "Stop.");
  });
  WebServer.on("/command/begin", handleEffectCommand);
}

void handleEffectCommand()
{
  if(WebServer.hasArg("effect"))
  {
    WebServer.send(200, "text/plain", "Starting effect #" + String(WebServer.arg("effect")) + "...");
    // TODO: begin(String(WebServer.arg("effect")).toInt());
  }
  else
  {
    WebServer.send(404, "text/plain", "Oops, you forgot something!");
  }
}