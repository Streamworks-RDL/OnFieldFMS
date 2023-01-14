#include "BeaconCamera.h"

BeaconCamera::BeaconCamera() : jpg() { }

bool BeaconCamera::read_jpeg(const std::string filename)
{
  std::ifstream inFile = std::ifstream(filename, std::ios::in | std::ios::binary);
    if (!inFile.is_open())
    {
        std::cout << "Error - Error opening input file\n";
        return false;
  }

  bool is_huffman_stream = false;
  byte current_byte = inFile.get();
  byte last_byte;
  while (true)
  {
    last_byte = current_byte;
    current_byte = inFile.get();
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
        inFile.get();
      }
      is_huffman_stream = true;
      jpg.reset();
    }
  }
  return true;
}
