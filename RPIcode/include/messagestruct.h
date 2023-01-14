#ifndef __MESSAGESTRUCT_H__
#define __MESSAGESTRUCT_H__

typedef enum
{
    cmd_none = 0,
    cmd_stemq = 1,
    cmd_beacon = 2,
    cmd_exit = -1
} command;

typedef enum
{
    team_off = 'o',
    team_red = 'r',
    team_blue = 'b'
} color;

struct messageTag;

typedef struct messageTag
{
    command cmd;
    color team;
    int id;
} message;

#endif
