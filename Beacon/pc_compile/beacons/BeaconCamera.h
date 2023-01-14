#ifndef __BEACON_CAMERA_H__
#define __BEACON_CAMERA_H__

#include "JpegDecode.h"

class BeaconCamera
{
public:
  BeaconCamera();
  bool read_jpeg(const std::string filename);
  Jpeg jpg;
};

#endif
