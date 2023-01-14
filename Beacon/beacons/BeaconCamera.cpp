#include "BeaconCamera.h"

BeaconCamera::BeaconCamera() : myCAM(OV2640, CS), jpg()
{
  start_capture = true;
  
  detected_color = Color::OFF;
  potential_color = Color::NONE;
  potential_color_start_time = 0;
}

void BeaconCamera::init()
{
  Wire.begin();
  //Serial.begin(921600);
  Serial.println(F("ACK CMD ArduCAM Start! END"));

  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  // initialize SPI:
  SPI.begin();
    //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  byte vid, pid, temp, temp_last;

  while (1)
  {
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      Serial.print(F("ACK CMD SPI interface Error!END "));
      Serial.println(temp);
      delay(1000); continue;
    }
    else
    {
      Serial.println(F("ACK CMD SPI interface OK.END")); break;
    }
  }

  while (1)
  {
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    {
      Serial.println(F("ACK CMD Can't find OV2640 module!"));
      delay(1000); continue;
    }
    else
    {
      Serial.println(F("ACK CMD OV2640 detected.END")); break;
    }
  }
  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_160x120);
  myCAM.OV2640_set_Light_Mode(Manual_A);
  delay(1000);
  myCAM.clear_fifo_flag();
}

bool BeaconCamera::read_jpeg()
{
  if (start_capture)
  {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    start_capture = false;
  }
  if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
  {
    bool is_huffman_stream = false;
    uint length = 0;
    length = myCAM.read_fifo_length();
    if ((length >= MAX_FIFO_SIZE) | (length == 0))
    {
      myCAM.clear_fifo_flag();
      start_capture = true;
      return false;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();//Set fifo burst mode
    byte current_byte = SPI.transfer(0x00);
    byte last_byte;
    length --;
    while ( length-- )
    {
      last_byte = current_byte;
      current_byte = SPI.transfer(0x00);
      if (is_huffman_stream == true)
      {
        if (!jpg.process_incoming_huffman_data(last_byte, current_byte)) //If find the end, break while
        {
          break;
        }
      }
      else if ((last_byte == MARKER) & (current_byte == SOS))
      {
        for (int i = 0; i < 12; i++)
        {
          SPI.transfer(0x00);
          delayMicroseconds(5);
        }
        is_huffman_stream = true;
        jpg.reset();
      }
      delayMicroseconds(15);
    }
    myCAM.CS_HIGH();
    myCAM.clear_fifo_flag();
    start_capture = true;

    return true;
  }
  return false;
}

void BeaconCamera::process_jpeg()
{
  jpg.process_mcus();
}

void BeaconCamera::detect_color()
{
  Color current_color = jpg.detect_color();
  
  if (current_color == Color::NONE)
  {
    potential_color = Color::NONE;
    return;
  }
  
  if (current_color == potential_color)
  {
    if (millis() - potential_color_start_time >= COLOR_TIME_SPAN)
    {
      potential_color_start_time = millis();
      potential_color = Color::NONE;
      
      detected_color = current_color;
    }
  }
  else
  {
    potential_color_start_time = millis();
    potential_color = current_color;
  }
}