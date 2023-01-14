#ifndef __BEACON_LIGHTS_H__
#define __BEACON_LIGHTS_H__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "types.h"

#define LIGHTPIN 2

class BeaconLights
{
public:
  BeaconLights();
  
  void init();
  void set_color(Color color);

private:
  Adafruit_NeoPixel ring;
  uint32_t lights_red;
  uint32_t lights_blue;
  uint32_t lights_yellow;
};

#endif