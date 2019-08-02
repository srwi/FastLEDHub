#include "WebUpdate.h"

const char* serverIndex = "<form method='POST' action='/handle_update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void initWebUpdate()
{
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
  WebServer.on("/update", HTTP_GET, []() {
    WebServer.sendHeader("Connection", "close");
    WebServer.sendHeader("Access-Control-Allow-Origin", "*");
    WebServer.send(200, "text/html", serverIndex);
  });
  WebServer.on("/reboot", HTTP_GET, [&]()
  {
    ESP.restart();
  });
  WebServer.on("/stop", HTTP_GET, [&]()
  {
    currentAnimation->stop();
    WebServer.send(200, "text/plain", "Animation stopped.");
  });
  WebServer.on("/pause", HTTP_GET, [&]()
  {
    currentAnimation->pause();
    WebServer.send(200, "text/plain", "Animation paused.");
  });
  WebServer.on("/resume", HTTP_GET, [&]()
  {
    currentAnimation->resume();
    WebServer.send(200, "text/plain", "Animation resumed.");
  });
  WebServer.on("/toggle", HTTP_GET, [&]()
  {
    currentAnimation->toggle();
    WebServer.send(200, "text/plain", "Animation toggled.");
  });
  WebServer.on("/restart", HTTP_GET, [&]()
  {
    currentAnimation->restart();
    WebServer.send(200, "text/plain", "Animation restarted.");
  });
  WebServer.on("/begin", HTTP_GET, [&]()
  {
    if(WebServer.hasArg("animation"))
    {
      String animationName = String(WebServer.arg("animation"));
      WebServer.send(200, "text/plain", "Starting animation '" + animationName + "'...");
      getAnimation(animationName)->begin();
    }
    else
    {
      getAnimation(0)->begin();
    }
  });
}