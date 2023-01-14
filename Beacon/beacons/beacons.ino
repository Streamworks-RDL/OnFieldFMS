#include "BeaconCamera.h"
#include "BeaconWifi.h"
#include "BeaconLights.h"

//Objects
BeaconCamera camera = BeaconCamera();
BeaconWifi wifi_manager = BeaconWifi();
BeaconLights lights = BeaconLights();

//Setup
#define SW1 3
#define SW2 4
#define SW3 5
#define SW4 6
int field;
int id;
Color light_color = Color::OFF;

void determine_light_color();

void setup()
{
  Serial.begin(921600);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(SW4, INPUT_PULLUP);
  
  delay(1000);
  
  field = !digitalRead(SW1);
  id = (!digitalRead(SW4)) + (!digitalRead(SW3))*2 + (!digitalRead(SW2))*4;
  
  camera.init();
  //wifi_manager.init(field);
  lights.init();

  //attempt to connect to Wifi network:
  lights.set_color(Color::YELLOW);
  //wifi_manager.wait_for_connection();
  lights.set_color(Color::OFF);
  Serial.println("Connected to wifi");
  //wifi_manager.print_status();
  
  /*while (!camera.read_jpeg()) {}
  
  for (int i = 0; i < camera.jpg.data_len; i++)
  {
    char buf[30];
    sprintf(buf, "%02x,", camera.jpg.data[i]);
    Serial.write(buf);
  }
  Serial.println("\nQ");*/
}

void loop()
{
  if (camera.read_jpeg())
  {
    camera.process_jpeg();
    camera.jpg.print_mcus();
    camera.detect_color();
  }
  
  //wifi_manager.read_from_server(id);
  
  //determine_light_color();
  lights.set_color(camera.detected_color);
}

void determine_light_color()
{
  if (camera.detected_color != light_color)
  {
    light_color = camera.detected_color;
    wifi_manager.send_to_server(id, camera.detected_color);
  }
  else if (wifi_manager.server_color != light_color)
  {
    light_color = wifi_manager.server_color;
    camera.detected_color = wifi_manager.server_color;
  }
}