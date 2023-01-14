#include "JpegDecode.h"
#include <cmath>

Jpeg::Jpeg() : mcus(new MCU[MCU_AMT])
{
  data_len = 0;
}

void Jpeg::reset()
{
  data_len = 0;
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

  std::cout << "\n\nPost Huffman:\n";
  for (int i = 0; i < MCU_AMT; i++)
  {
    {
      char buf[30];
      sprintf(buf, "Y values: (%d, %d) | ", mcus[i][0][0], mcus[i][0][1]);
      std::cout << buf;
    }
    {
      char buf[30];
      sprintf(buf, "Cb values: (%d, %d) | ", mcus[i][1][0], mcus[i][1][1]);
      std::cout << buf;
    }
    {
      char buf[30];
      sprintf(buf, "Cr values: (%d, %d)\n", mcus[i][2][0], mcus[i][2][1]);
      std::cout << buf;
    }
  }
  dequantize_data();

  std::cout << "\n\nPost Dequantize:\n";
  for (int i = 0; i < MCU_AMT; i++)
  {
    {
      char buf[30];
      sprintf(buf, "Y values: (%d, %d) | ", mcus[i][0][0], mcus[i][0][1]);
      std::cout << buf;
    }
    {
      char buf[30];
      sprintf(buf, "Cb values: (%d, %d) | ", mcus[i][1][0], mcus[i][1][1]);
      std::cout << buf;
    }
    {
      char buf[30];
      sprintf(buf, "Cr values: (%d, %d)\n", mcus[i][2][0], mcus[i][2][1]);
      std::cout << buf;
    }
  }
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
  std::cout << "Reading " << (uint) length << " bits in DC coeff ";
  int coeff = br_read_bits(length);
  //std::cout << "\n";
  if (length != 0 && coeff < (1 << (length - 1)))
  {
    coeff -= (1 << length) - 1;
  }
  std::cout << " Coeff: " << coeff << "\n";
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
    std::cout << "Reading " << (uint) length << " bits in stored AC coeff ";
    coeff = br_read_bits(length);
    //std::cout << "\n";
    if (coeff < (1 << (length - 1)))
    {
      coeff -= (1 << length) - 1;
    }
    std::cout << " Coeff: " << coeff << "\n";
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
    std::cout << "Reading " << (uint) length << " bits in disguarded AC coeff ";
    br_read_bits(length);
    std::cout << "\n";
    
    i++;
  }
}

byte Jpeg::get_next_symbol(const HuffmanTable& table)
{
  uint current_code = 0;
  std::cout << "Reading symbol ";
  for (uint i = 0; i < 16; i++)
  {
    int bit = br_read_bit();
    if (bit == -1)
    {
      std::cout << "\n";
      return -1;
    }
    current_code = (current_code << 1) | bit;
    for (uint j = table.offsets[i]; j < table.offsets[i + 1]; j++)
    {
      if (current_code == table.codes[j])
      {
        char buf[10];
        sprintf(buf, "%x", current_code);
        char buf2[10];
        sprintf(buf2, "%x", table.symbols[j]);
        std::cout << " code: " << buf << " symbol: " << buf2 << "\n";
        return table.symbols[j];
      }
    }
  }
  std::cout << " AAAA";
  std::cout << "\n";
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
  float osqt = 1.0/std::sqrt(2);
  float osqt2 = 0.5;
  float cos1 = std::cos(M_PI/16.0);
  float cos17 = std::cos(17.0*M_PI/16.0);
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
  }
}

void Jpeg::br_reset()
{
  __br_next_byte = 0;
  __br_next_bit = 0;
  val = 0;
  val2 = 0;
}

int Jpeg::br_read_bit()
{
  val++;
  if (__br_next_byte >= data_len)
  {
    val2++;
    std::cout << " bit error ";
    return -1;
  }
  int bit = (data[__br_next_byte] >> (7 - __br_next_bit)) & 1;
  __br_next_bit++;
  if (__br_next_bit == 8)
  {
    __br_next_bit = 0;    
    __br_next_byte++;
  }

  std::cout << bit;
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
