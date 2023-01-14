#ifndef __BEACON_CAMERA_H__
#define __BEACON_CAMERA_H__

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <Arduino.h>
#include "JpegDecode.h"

#define CS 7

#define COLOR_TIME_SPAN 100

class BeaconCamera
{
public:
  BeaconCamera();
  
  void init();
  bool read_jpeg();
  void process_jpeg();
  void detect_color();
  
  Color detected_color;

//private:
  ArduCAM myCAM;
  Jpeg jpg;
  bool start_capture;
  
  Color potential_color;
  int potential_color_start_time;
};

#endif
