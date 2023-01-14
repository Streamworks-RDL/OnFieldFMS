#ifndef __JPG_H__
#define __JPG_H__

#include "types.h"
#include "HuffmanTables.h"

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
};

#endif