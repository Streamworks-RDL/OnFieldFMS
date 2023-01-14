#ifndef __JPEG_DECODE_H__
#define __JPEG_DECODE_H__

#include "jpg.h"

#define PIXEL_HEIGHT 120
#define PIXEL_WIDTH 160

#define MCU_AMT 150
#define MCU_HEIGHT 15
#define MCU_WIDTH 10

#define Y_DC_QUANTIZATION 12
#define CB_CR_DC_QUANTIZATION 13
#define CB_CR_AC_QUANTIZATION 14

class Jpeg
{
public:
  Jpeg();

  void reset();
  bool process_incoming_huffman_data(byte last, byte current);
  void process_mcus();
  MCU* mcus;

  unsigned long val;
  unsigned long val2;

//private:
  void decode_huffman_data();
  void dequantize_data();
  void decode_MCU_component(int* const component, const uint blockNum, int& previousDC, const HuffmanTable& dcTable, const HuffmanTable& acTable);
  byte get_next_symbol(const HuffmanTable& table);
  void idct_component(int* const component);
  void idct();

  byte data[5600];
  uint data_len;
  
  Header header;

  uint __br_next_byte;
  uint __br_next_bit;
  void br_reset();
  int br_read_bit();
  int br_read_bits(const uint length);
};

#endif
