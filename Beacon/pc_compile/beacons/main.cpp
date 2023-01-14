#include "BeaconCamera.h"

int main(int argc, char** argv)
{
  if (argc == 2)
  {
    BeaconCamera camera = BeaconCamera();

    for (int runs = 0; runs < 3; runs++)
    {
      if (camera.read_jpeg(argv[1]))
      {
        for (int i = 0; i < camera.jpg.data_len; i++)
        {
            char buf[10];
            sprintf(buf, "0x%02x, ", camera.jpg.data[i]);
            std::cout << buf;
        }
        std::cout << "\n\n";
        camera.jpg.process_mcus();
        std::cout << "Called: " <<camera.jpg.val << " Error: " << camera.jpg.val2 << "\n\nMCUS:\n";
        for (int i = 0; i < MCU_AMT; i++)
        {
          {
            char buf[30];
            sprintf(buf, "Y values: (%d, %d) | ", camera.jpg.mcus[i][0][0], camera.jpg.mcus[i][0][1]);
            std::cout << buf;
          }
          {
            char buf[30];
            sprintf(buf, "Cb values: (%d, %d) | ", camera.jpg.mcus[i][1][0], camera.jpg.mcus[i][1][1]);
            std::cout << buf;
          }
          {
            char buf[30];
            sprintf(buf, "Cr values: (%d, %d)\n", camera.jpg.mcus[i][2][0], camera.jpg.mcus[i][2][1]);
            std::cout << buf;
          }
        }
        for (int i = 0; i < MCU_AMT; i++)
        {
          for (int j = 0; j < 3; j++)
          {
            char buf[30];
            sprintf(buf, "%d|%d|", camera.jpg.mcus[i][j][0], camera.jpg.mcus[i][j][1]);
            std::cout << buf;
          }
          std::cout << ",";
        }
        std::cout << std::endl << std::endl;
      }
      else
      {
        std::cout << "Error: Could not process jpg\n";
        return 1;
      }
      std::cout << "\n\n\nNEXT ONE\n\n\n";
    }
  }
  else
  {
    std::cout << "Error: No jpg given\n";
    return 1;
  }
}
