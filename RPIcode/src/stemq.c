//verbose import
#include "verbose_print.h"

//imports
#include <stdio.h>
#include <string.h>

//macros
#define max(a, b) ((a > b) ? a : b)
#define min(a, b) ((a > b) ? b : a)

#define SETUP "\e[?5h\e[2J"
#define CLEAR "\ec"
#define COPY  "\e7"
#define PASTE "\e8"

#define RED_SQUARE  "\e[31m \e[39m"
#define BLUE_SQUARE "\e[34m \e[39m"


//terminal size
int WIDTH;
int HEIGHT;

//Save and restore cursor location
void sv_pos() { printf(COPY); }
void rs_pos() { printf(PASTE); }

//Move cursor to location (x, y) where (0, 0) is the first character in the top left of the screen
void pos(int x, int y) { printf("\e[%d;%dH", y+1, x+1); }

//Clear screen, set colors, and set screen size
void display_setup(int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    printf(SETUP);
}

//Clear screen
void display_clear() { printf(SETUP); }

//Reset VT100
void display_reset() { printf(CLEAR); }

//Move the blinking cursor (and any console logs) off screen and flush the output
void display_update() { pos(0, HEIGHT/2 + 1); fflush(stdout); }

//Draws a small square in the corner of each display for identification purposes
void color_code()
{
    sv_pos();
    pos(WIDTH/2 -1, HEIGHT/2);
    printf(RED_SQUARE);
    pos(WIDTH -1, HEIGHT/2);
    printf(BLUE_SQUARE);
    rs_pos();
}

int calc_rows(char* qaa[], int width)
{
    unsigned int rows = 0;
    for (int i = 1; i < 5; i++)
    {
        unsigned int row_length, position = 0, total = strlen(qaa[i]);

        while (1)
        {
            if (total - position < width)
            {
                rows++;
                break;
            }

            for (row_length = min(width, total - position); row_length > 0; row_length--)
            {
                if (qaa[i][position + row_length] == ' ')
                {
                    position += row_length + 1;
                    rows++;
                    break;
                }
            }
            if (row_length == 0) { return -1; }
        }
    }
    return rows;
}

//Prints text to the screen with word wrapping
int word_wrap(char* text, int x, int y, int width, int center)
{
    unsigned int row_length, rows = 0, position = 0, total = strlen(text);

    while (1)
    {
        if (total - position < width)
        {
            pos(x + (center ? (width-total + position)/2 : 0), y+rows);
            printf("%s", text+position);
            rows++;
            break;
        }

        for (row_length = min(width, total - position); row_length > 0; row_length--)
        {
            if (text[position + row_length] == ' ')
            {
                pos(x + (center ? (width-row_length)/2 : 0), y+rows);
                printf("%.*s", row_length, text+position);
                position += row_length + 1;
                rows++;
                break;
            }
        }
        if (row_length == 0) { return 1000; }
    }
    return rows;
}

int stemq_print(char* qaa[], int team)
{
    int q_height = word_wrap(qaa[0], team ? WIDTH/2 + 1 : 1, 1, (WIDTH)/2-2, 1) + 1;

    int rows = calc_rows(qaa, (WIDTH)/2-5);

    if (rows >= 0)
    {
        int h = HEIGHT/2 - rows;
        pos(team ? WIDTH/2 + 1 : 1, h);
        printf("a)");
        h += word_wrap(qaa[1], team ? WIDTH/2 + 4 : 4, h, (WIDTH)/2-5, 0);

        pos(team ? WIDTH/2 + 1 : 1, h);
        printf("b)");
        h += word_wrap(qaa[2], team ? WIDTH/2 + 4 : 4, h, (WIDTH)/2-5, 0);

        pos(team ? WIDTH/2 + 1 : 1, h);
        printf("c)");
        h += word_wrap(qaa[3], team ? WIDTH/2 + 4 : 4, h, (WIDTH)/2-5, 0);

        pos(team ? WIDTH/2 + 1 : 1, h);
        printf("d)");
        h += word_wrap(qaa[4], team ? WIDTH/2 + 4 : 4, h, (WIDTH)/2-5, 0);
    }

    return q_height + rows > HEIGHT/2;
}
