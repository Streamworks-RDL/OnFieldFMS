#ifndef __JPG_H__
#define __JPG_H__

#include "types.h"
#include "HuffmanTables.h"

//#include <Arduino.h>
#include <iostream>
#include <fstream>

#define MARKER 0xFF

#define SOF0 0xC0 // Baseline DCT
#define SOF1 0xC1 // Extended sequential DCT
#define SOF2 0xC2 // Progressive DCT
#define SOF3 0xC3 // Lossless (sequential)

// Start of Frame markers, differential, Huffman coding
#define SOF5 0xC5 // Differential sequential DCT
#define SOF6 0xC6 // Differential progressive DCT
#define SOF7 0xC7 // Differential lossless (sequential)

// Start of Frame markers, non-differential, arithmetic coding
#define SOF9 0xC9 // Extended sequential DCT
#define SOF10 0xCA // Progressive DCT
#define SOF11 0xCB // Lossless (sequential)

// Start of Frame markers, differential, arithmetic coding
#define SOF13 0xCD // Differential sequential DCT
#define SOF14 0xCE // Differential progressive DCT
#define SOF15 0xCF // Differential lossless (sequential)

// Define Huffman Table(s)
#define DHT 0xC4

// JPEG extensions
#define JPG 0xC8

// Define Arithmetic Coding Conditioning(s)
#define DAC 0xCC

// Restart interval Markers
#define RST0 0xD0
#define RST1 0xD1
#define RST2 0xD2
#define RST3 0xD3
#define RST4 0xD4
#define RST5 0xD5
#define RST6 0xD6
#define RST7 0xD7

// Other Markers
#define SOI 0xD8 // Start of Image
#define EOI 0xD9 // End of Image
#define SOS 0xDA // Start of Scan
#define DQT 0xDB // Define Quantization Table(s)
#define DNL 0xDC // Define Number of Lines
#define DRI 0xDD // Define Restart Interval
#define DHP 0xDE // Define Hierarchical Progression
#define EXP 0xDF // Expand Reference Component(s)

// APPN Markers
#define APP0 0xE0
#define APP1 0xE1
#define APP2 0xE2
#define APP3 0xE3
#define APP4 0xE4
#define APP5 0xE5
#define APP6 0xE6
#define APP7 0xE7
#define APP8 0xE8
#define APP9 0xE9
#define APP10 0xEA
#define APP11 0xEB
#define APP12 0xEC
#define APP13 0xED
#define APP14 0xEE
#define APP15 0xEF

// Misc Markers
#define JPG0 0xF0
#define JPG1 0xF1
#define JPG2 0xF2
#define JPG3 0xF3
#define JPG4 0xF4
#define JPG5 0xF5
#define JPG6 0xF6
#define JPG7 0xF7
#define JPG8 0xF8
#define JPG9 0xF9
#define JPG10 0xFA
#define JPG11 0xFB
#define JPG12 0xFC
#define JPG13 0xFD
#define COM 0xFE
#define TEM 0x01

struct HuffmanTable {
  byte *offsets;
  byte *symbols;
  uint *codes;

  HuffmanTable(uint table_class, uint ID) //table_class = DC [0] / AC [1]
  {
    switch(table_class*2 + ID)
    {
      case 0: //DC table 0
        offsets = DC_table_0_offsets;
        symbols = DC_table_0_symbols;
        codes = DC_table_0_codes;
        break;
      case 1: //DC table 1
        offsets = DC_table_1_offsets;
        symbols = DC_table_1_symbols;
        codes = DC_table_1_codes;
        break;
      case 2: //AC table 0
        offsets = AC_table_0_offsets;
        symbols = AC_table_0_symbols;
        codes = AC_table_0_codes;
        break;
      case 3: //AC table 1
        offsets = AC_table_1_offsets;
        symbols = AC_table_1_symbols;
        codes = AC_table_1_codes;
        break;
    }
  }
};

/*struct ColorComponent {
  byte horizontalSamplingFactor = 1;
  //byte verticalSamplingFactor = 1;
  byte quantizationTableID = 0;
  byte huffmanDCTableID = 0;
  byte huffmanACTableID = 0;

  ColorComponent(byte hsf, *byte vsf,* byte qtID, byte DCID, byte ACID)
  {
    horizontalSamplingFactor = hsf;
    //verticalSamplingFactor = vsf;
    quantizationTableID = qtID;
    huffmanDCTableID = DCID;
    huffmanACTableID = ACID;
  }
};*/

struct MCU {
  int y[2] = { 0 };
  int cb[2] = { 0 };
  int cr[2] = { 0 };
  int* operator[](uint i) {
    switch (i) {
      case 0:
        return y;
      case 1:
        return cb;
      case 2:
        return cr;
      default:
        return nullptr;
    }
  }
};

struct Header {
  /*uint quantizationTables[2][64] =
  {
    {
      //Y quantization [ID 0]
      12,  8,  8, 12, 18, 30, 38, 46,
       9,  9, 11, 14, 20, 44, 45, 41,
      11, 10, 12, 18, 30, 43, 52, 42,
      11, 13, 17, 22, 38, 65, 60, 47,
      14, 17, 28, 42, 51, 82, 77, 58,
      18, 26, 41, 48, 61, 78, 85, 69,
      37, 48, 59, 65, 77, 91, 90, 76,
      54, 69, 71, 74, 84, 75, 77, 74
    },
    {
      //Cb and Cr quantization [ID 1]
      13, 14, 18, 35, 74, 74, 74, 74,
      14, 16, 20, 50, 74, 74, 74, 74,
      18, 20, 42, 74, 74, 74, 74, 74,
      35, 50, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74,
      74, 74, 74, 74, 74, 74, 74, 74
    }
  };*/

  HuffmanTable huffmanDCTables[2] = 
  {
    HuffmanTable(0, 0),
    HuffmanTable(0, 1)
  };
  HuffmanTable huffmanACTables[2] = 
  {
    HuffmanTable(1, 0),
    HuffmanTable(1, 1)
  };

  /*ColorComponent colorComponents[3] =
  {
    //ColorComponent(byte hsf, byte vsf, byte qtID, byte DCID, byte ACID)
    ColorComponent(2, *1,* 0, 0, 0),
    ColorComponent(1, *1,* 1, 1, 1),
    ColorComponent(1, *1,* 1, 1, 1)
  };*/
};

/*const byte zigZagMap[] = {
  0,   1,  8, 16,  9,  2,  3, 10,
  17, 24, 32, 25, 18, 11,  4,  5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13,  6,  7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63
};*/

#endif
