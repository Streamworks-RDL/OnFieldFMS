#ifndef __BEACON_WIFI_H__
#define __BEACON_WIFI_H__

#include <Arduino.h>
#include <WiFiNINA.h>
#include "types.h"
#include "credentials.h"

#define READ_PAGE "Beacons/Display"
#define WRITE_PAGE "Beacons/Update"
#define GET_TIMEBETWEEN 1000

class BeaconWifi
{
public:
  BeaconWifi();
  
  void init(int field);
  void wait_for_connection();
  void print_status();
  void read_from_server(int id);
  void send_to_server(int id, Color color);

  Color server_color;

private:
  char* server;
  int port;
  int status;
  int lastMillis;
  WiFiClient client;
  void process_incoming_data(int id);
};

#endif

