#include "BeaconLights.h"

BeaconLights::BeaconLights() : ring(32, LIGHTPIN, NEO_GRB + NEO_KHZ800)
{
  lights_red = ring.Color(255,0,0);
  lights_blue = ring.Color(0,0,255);
  lights_yellow = ring.Color(255,200,10);
}

void BeaconLights::init()
{
  ring.begin();
}

void BeaconLights::set_color(Color color)
{
  ring.setBrightness(255);
  
  switch (color)
  {
    case Color::RED:
      ring.fill(lights_red);
      break;
    case Color::BLUE:
      ring.fill(lights_blue);
      break;
    case Color::YELLOW:
      ring.fill(lights_yellow);
      break;
    default:
      ring.clear();
      break;
  }
  
  ring.show();
}