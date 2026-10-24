#pragma once

#include <Arduino.h>

namespace FMQTT
{
  /// Initialize MQTT
  /// @param projectName Default name used for topics and client ID
  void initialize(const String &projectName);

  /// Handle MQTT client connection and incoming messages.
  /// Should be called regularly in the main loop.
  void handle();

  /// Publish current state (state, brightness, speed, currentAnimation, color)
  void publishStatus();

  /// Publish a custom message under <baseTopic>/<subtopic>
  /// @param subtopic Subtopic name (without leading slash)
  /// @param payload Payload string
  /// @param retain Whether to retain the message
  void publishMessage(const String &subtopic, const String &payload, bool retain = false);

  /// Check if currently connected to the MQTT broker
  /// @return True if connected
  bool isConnected();
}
