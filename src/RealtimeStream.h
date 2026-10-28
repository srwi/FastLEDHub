#pragma once

#include <Arduino.h>

#define REALTIME_UDP_PORT 21324
#define REALTIME_DDP_PORT 4048
#define REALTIME_HYPERION_PORT 19446

namespace RealtimeStream
{
  /// Initialize UDP sockets for realtime streaming
  void initialize();

  /// Handle incoming realtime packets and check timeouts.
  /// Should be called regularly in FastLEDHubClass::handle().
  void handle();

  /// Check if realtime live streaming is currently active
  /// @return True if streaming
  bool isActive();

  /// Manually stop/exit realtime streaming
  void stop();
}
