#ifndef __FMS_H__
#define __FMS_H__

extern char* red_q[5];
extern char* blue_q[5];
extern int question_changed;

void network_setup(char* ip);
void network_close();

void network_get();
void network_post(message msg);

#endif
