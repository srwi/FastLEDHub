#pragma once

#include <Arduino.h>

class Animation
{
public:
  Animation() = delete;

  /// Constructor
  /// @param name Unique animation name
  Animation(String name);

  /// Get the animation name
  /// @return Animation name
  String getName();

  /// Loop method gets called repeatedly while running
  /// the animation. This method needs to be implemented.
  virtual void loop() = 0;

  /// Setup method gets called when the animation starts.
  /// This method needs to be implemented.
  virtual void reset() = 0;

private:
  String m_name;
};
