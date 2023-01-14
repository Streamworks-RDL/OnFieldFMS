//verbose import
#include "verbose_print.h"

//struct imports
#include "messagestruct.h"

//imports
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#define is_color(chr) (chr == team_blue || chr == team_red || chr == team_off)
#define is_team(chr) (chr == team_blue || chr == team_red)

char buff[4] = "   ";

static struct termios oldt, newt;

void termios_setup()
{
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void termios_reset()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

message read_command()
{
    message msg;

    struct pollfd pfd[1];
    pfd[0].fd = STDIN_FILENO;
    pfd[0].events = POLLIN;
    pfd[0].revents = POLLIN;

    if (poll(pfd, 1, 0) > 0)
    {
        buff[0] = buff[1];
        buff[1] = buff[2];
        buff[2] = getchar();

        if (buff[2] == 'e') { msg.cmd = cmd_exit; }
        else if (buff[0] == 's' && buff[1] == 'q' && is_team(buff[2]))
        {
            msg.team = buff[2];
            msg.cmd = cmd_stemq;
        }
        else if (buff[0] == 'l' && buff[1] >= '1' && buff[1] <= '7' && is_color(buff[2]))
        {
            msg.cmd = cmd_beacon;
            msg.team = buff[2];
            msg.id = buff[1] - '0';
        }
        else { msg.cmd = cmd_none; }
    }

    else { msg.cmd = cmd_none; }

    buff[0] = 'p';

    return msg;
}
