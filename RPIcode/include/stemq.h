#ifndef __STEMQ_H__
#define __STEMQ_H__

void display_setup(int width, int height);
void display_reset();
void display_update();
void display_clear();

void color_code();
int stemq_print(char* qaa[], int team);

void pos(int x, int y);

#endif
