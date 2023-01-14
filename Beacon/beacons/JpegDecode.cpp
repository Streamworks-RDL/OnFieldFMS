#include "JpegDecode.h"
#include <Arduino.h>

Jpeg::Jpeg() : mcus(new MCU[MCU_AMT])
{
  data_len = 0;
  
  osqt = 1.0/sqrt(2);
  osqt2 = 0.5;
  cos1 = cos(PI/16.0);
  cos17 = cos(17.0*PI/16.0);
  
  br_reset();
}

void Jpeg::reset()
{
  data_len = 0;
  br_reset();
}

bool Jpeg::process_incoming_huffman_data(byte last, byte current)
{
  if (current == MARKER) { return true; }
  if (last == MARKER)
  {
    if (current == EOI) { return false; }
    if (current == 0x00)
    {
      data[data_len] = MARKER;
      data_len++;
      return true;
    }
  }
  data[data_len] = current;
  data_len++;
  return true;
}

void Jpeg::process_mcus()
{
  decode_huffman_data();
  dequantize_data();
  idct();
}

void Jpeg::decode_huffman_data()
{
  br_reset();

  int previousDCs[3] = { 0 };
  
  for (uint y = 0; y < MCU_HEIGHT; y++)
  {
    for (uint x = 0; x < MCU_WIDTH; x++)
    {
      decode_MCU_component(
          mcus[y * MCU_WIDTH + x].y,
          0,
          previousDCs[0],
          header.huffmanDCTables[0],//header.colorComponents[0].huffmanDCTableID],
          header.huffmanACTables[0]);//header.colorComponents[0].huffmanACTableID]);
      decode_MCU_component(
          mcus[y * MCU_WIDTH + x].y,
          1,
          previousDCs[0],
          header.huffmanDCTables[0],//header.colorComponents[0].huffmanDCTableID],
          header.huffmanACTables[0]);//header.colorComponents[0].huffmanACTableID]);
      decode_MCU_component(
          mcus[y * MCU_WIDTH + x].cb,
          0,
          previousDCs[1],
          header.huffmanDCTables[1],//header.colorComponents[1].huffmanDCTableID],
          header.huffmanACTables[1]);//header.colorComponents[1].huffmanACTableID]);
      decode_MCU_component(
          mcus[y * MCU_WIDTH + x].cr,
          0,
          previousDCs[2],
          header.huffmanDCTables[1],//header.colorComponents[2].huffmanDCTableID],
          header.huffmanACTables[1]);//header.colorComponents[2].huffmanACTableID]);
    }
  }
}

void Jpeg::decode_MCU_component(int* const component, const uint blockNum, int& previousDC, const HuffmanTable& dcTable, const HuffmanTable& acTable)
{
  byte length = get_next_symbol(dcTable);
  //std::cout << "Reading " << (uint) length << " bits in DC coeff ";
  int coeff = br_read_bits(length);
  //std::cout << "\n";
  if (length != 0 && coeff < (1 << (length - 1)))
  {
    coeff -= (1 << length) - 1;
  }
  component[blockNum] = coeff + previousDC;
  previousDC = component[blockNum];
  
  uint i = 1;
  
  //for 2nd value
  if (blockNum == 0)
  {
    byte symbol = get_next_symbol(acTable);
    
    if (symbol == 0x00) { component[1] = 0; return; }
    
    byte numZeros = symbol >> 4;
    if (symbol == 0xF0) { component[1] = 0; numZeros = 16; }
    i += numZeros;
    
    length = symbol & 0x0F;
    //std::cout << "Reading " << (uint) length << " bits in stored AC coeff ";
    coeff = br_read_bits(length);
    //std::cout << "\n";
    if (coeff < (1 << (length - 1)))
    {
      coeff -= (1 << length) - 1;
    }
    if (i == 1)
    {
      component[1] = coeff;
    }
    else
    {
      component[1] = 0;
    }
    
    i++;
  }
  
  while (i < 64)
  {
    byte symbol = get_next_symbol(acTable);
    
    if (symbol == 0x00) { return; }
    
    byte numZeros = symbol >> 4;
    if (symbol == 0xF0) { numZeros = 16; }
    i += numZeros;
    
    length = symbol & 0x0F;
    //std::cout << "Reading " << (uint) length << " bits in disguarded AC coeff ";
    br_read_bits(length);
    //std::cout << "\n";
    
    i++;
  }
}

byte Jpeg::get_next_symbol(const HuffmanTable& table)
{
  uint current_code = 0;
  //std::cout << "Reading symbol ";
  for (uint i = 0; i < 16; i++)
  {
    int bit = br_read_bit();
    if (bit == -1)
    {
      //std::cout << "\n";
      return -1;
    }
    current_code = (current_code << 1) | bit;
    for (uint j = table.offsets[i]; j < table.offsets[i + 1]; j++)
    {
      if (current_code == table.codes[j])
      {
        //char buf[10];
        //sprintf(buf, "%x", current_code);
        //char buf2[10];
        //sprintf(buf2, "%x", table.symbols[j]);
        //std::cout << " code: " << buf << " symbol: " << buf2 << "\n";
        return table.symbols[j];
      }
    }
  }
  //std::cout << " AAAA";
  //std::cout << "\n";
  return -1;
}

// dequantize all MCUs
void Jpeg::dequantize_data() {
  for (uint i = 0; i < MCU_AMT; i++)
  {
      mcus[i].y[0] *= Y_DC_QUANTIZATION;
      mcus[i].y[1] *= Y_DC_QUANTIZATION;
      
      mcus[i].cb[0] *= CB_CR_DC_QUANTIZATION;
      mcus[i].cb[1] *= CB_CR_AC_QUANTIZATION;
      
      mcus[i].cr[0] *= CB_CR_DC_QUANTIZATION;
      mcus[i].cr[1] *= CB_CR_AC_QUANTIZATION;
  }
}

// perform IDCT on all MCUs
void Jpeg::idct() {
  float temp;
  
  for (uint i = 0; i < MCU_AMT; i++)
  {
    //Y values
    mcus[i].y[0] = osqt2*mcus[i].y[0]/4.0;
    mcus[i].y[1] = osqt2*mcus[i].y[1]/4.0;

    //Cb values
    temp = (osqt2*mcus[i].cb[0] + osqt*mcus[i].cb[1]*cos1)/4.0;
    mcus[i].cb[1] = (osqt2*mcus[i].cb[0] + osqt*mcus[i].cb[1]*cos17)/4.0;
    mcus[i].cb[0] = temp;

    //Cr values
    temp = (osqt2*mcus[i].cr[0] + osqt*mcus[i].cr[1]*cos1)/4.0;
    mcus[i].cr[1] = (osqt2*mcus[i].cr[0] + osqt*mcus[i].cr[1]*cos17)/4.0;
    mcus[i].cr[0] = temp;

    /*
    //CbCr value 0 (x=0, y=0)
    sum = osqt2*val[0] //(i=0, j=0)
    sum += osqt*val[1]*cos1 //(i=0, j=1)
    sum/4.0->val[0]

    //CbCr value 1 (x=8, y=0)
    sum = osqt2*val[0] //(i=0, j=0)
    sum += osqt*val[1]*cos17 //(i=0, j=1)
    sum/4.0->val[1]

    //Y value (x=0, y=0)
    sum = osqt2*val[blockNum] //(i=0, j=0)
    sum/4.0->val[blockNum]
    
    //sum = ci*cj*coeff[i,j]*cos((2x+1)*j*pi/16)
    //sum += ci*cj*coeff[i,j]*cos((2x+1)*j*pi/16)
    */
  }
}

void Jpeg::print_mcus()
{
  for (int i = 0; i < MCU_AMT; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      char buf[30];
      sprintf(buf, "%d|%d|", mcus[i][j][0], mcus[i][j][1]);
      Serial.print(buf);
    }
    Serial.print(",");
  }
  Serial.println();
}

Color Jpeg::detect_color()
{
  int red_count = 0;
  int blue_count = 0;
  for (int i = 0; i < MCU_AMT; i++)
  {
    if (match(mcus[i].y[0], mcus[i].cb[0], mcus[i].cr[0], Color::BLUE))
      blue_count++;
    if (match(mcus[i].y[1], mcus[i].cb[1], mcus[i].cr[1], Color::BLUE))
      blue_count++;

    if (match(mcus[i].y[0], mcus[i].cb[0], mcus[i].cr[0], Color::RED))
      red_count++;
    if (match(mcus[i].y[1], mcus[i].cb[1], mcus[i].cr[1], Color::RED))
      red_count++;
  }
  if (red_count >= blue_count & red_count > 30)
  {
    return Color::RED;
  }
  if (blue_count >= red_count & blue_count > 30)
  {
    return Color::BLUE;
  }
  return Color::NONE;
}

bool Jpeg::match(int y, int cb, int cr, Color color)
{
  if (color == Color::RED)
  {
      if (DRED_Y_MIN < y & y < DRED_Y_MAX &
        DRED_CB_MIN < cb & cb < DRED_CB_MAX &
        DRED_CR_MIN < cr & cr < DRED_CR_MAX)
        { return true; }
  }
  if (color == Color::BLUE)
  {
      if (DBLUE_Y_MIN < y & y < DBLUE_Y_MAX &
        DBLUE_CB_MIN < cb & cb < DBLUE_CB_MAX &
        DBLUE_CR_MIN < cr & cr < DBLUE_CR_MAX)
        { return true; }
  }
  return false;
}

void Jpeg::br_reset()
{
  __br_next_byte = 0;
  __br_next_bit = 0;
}

int Jpeg::br_read_bit()
{
  if (__br_next_byte >= data_len) { return -1; }
  int bit = (data[__br_next_byte] >> (7 - __br_next_bit)) & 1;
  __br_next_bit++;
  if (__br_next_bit == 8)
  {
    __br_next_bit = 0;
    __br_next_byte++;
  }
  return bit;
}
int Jpeg::br_read_bits(const uint length)
{
  int bits = 0;
  for (uint i = 0; i < length; i++)
  {
    int bit = br_read_bit();
    if (bit == -1)
    {
      bits = -1;
      break;
    }
    bits = (bits << 1) | bit;
  }
  
  return bits;
}