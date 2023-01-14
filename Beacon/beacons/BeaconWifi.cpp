#include "BeaconWifi.h"

BeaconWifi::BeaconWifi()
{
  port = 80;
  status = WL_IDLE_STATUS;
  lastMillis = -GET_TIMEBETWEEN;
}

void BeaconWifi::init(int field)
{
  if (field == 0)
  {
    server = "192.168.1.21";
  }
  else
  {
    server = "192.168.1.21"; //replace for 2nd field router
  }
  
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
}

void BeaconWifi::wait_for_connection()
{
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    status = WiFi.begin(SECRET_SSID, SECRET_PSK);
  }
}

void BeaconWifi::send_to_server(int id, Color color)
{
  if (client.connect(server, port))
  {
    //Serial.println("connected to server");
    // Make a HTTP request:
    client.print("POST /");
    client.print(WRITE_PAGE);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: 12");
    client.println();
    client.print("Id=");
    client.print(id);
    client.print("&state=");
    client.print((char) color);
  }
}

void BeaconWifi::read_from_server(int id)
{
  if (millis() - lastMillis >= GET_TIMEBETWEEN)
  {
    lastMillis = millis();
    
    if (client.connect(server, port))
    {
      //Serial.println("connected to server");
      // Make a HTTP request:
      client.print("GET /");
      client.println(READ_PAGE);
      client.println("Connection: keep-alive");
      client.println();
    }
  }
  
  process_incoming_data(id);
}

void BeaconWifi::process_incoming_data(int id)
{
  int line = 1;
  int state = 0; //0 = control, 1 = read, 2 = start
  int offset = 0;

  while (client.available())
  {
    char c = client.read();
    if (state == 1)
    {
      if (c == '<' || c == '>' || c == '\n') {
        line++;
      }
    }

    if (c == '<' || c == '>' || c == '\n') { state = c == '<' ? 0 : 2; continue; }

    if (state == 2)
    {
      if (c > ' ' )
      {
        if (line == id)
        {
          //Serial.print(line);
          //Serial.println(c);
          server_color = (Color) c;
        }
        state = 1;
      }
    }
  }
}

void BeaconWifi::print_status() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}