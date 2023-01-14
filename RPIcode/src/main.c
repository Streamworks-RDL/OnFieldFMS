//verbose import
#include "verbose_print.h"

//struct imports
#include "messagestruct.h"

//imports
#include <stdlib.h>
#include <stdio.h>

#include "stemq.h"
#include "input.h"
#include "fms.h"

#define RED 0
#define BLUE 1

#define DEFAULT_IP "192.168.1.21:80"

int main(int argc, char* argcv[])
{
    int WIDTH = 80;
    int HEIGHT = 22;
    char* IP = DEFAULT_IP;

    if (argc == 2)
    {
        IP = argcv[1];
    }
    else if (argc == 3)
    {
        WIDTH = atoi(argcv[2]);
        HEIGHT = atoi(argcv[1]);
    }
    else if (argc == 4)
    {
        WIDTH = atoi(argcv[2]);
        HEIGHT = atoi(argcv[1]);
        IP = argcv[3];
    }

    termios_setup();
    display_setup(WIDTH, HEIGHT);
    network_setup(IP);

    while (1)
    {
        network_get();

        if (question_changed) { display_clear(); question_changed = 0; }

        int reroll = stemq_print(red_q, RED);
        reroll = stemq_print(blue_q, BLUE) || reroll;

        color_code();
        display_update();

        //vprint("%d", reroll);

        message msg = read_command();
        if (msg.cmd == cmd_exit) { break; }
        if (msg.cmd == cmd_stemq || msg.cmd == cmd_beacon) { network_post(msg); }
    }

    termios_reset();
    display_reset();
    network_close();

    return 0;
}
