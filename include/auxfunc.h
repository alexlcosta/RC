#ifndef AUX_H
#define AUX_H

#include "state_machine.h"
#include <stdio.h>

typedef enum{
    SET,
    UA,
    RR1,
    REJ1,
    RR0,
    REJ0,
    DISC
} FRAME;

int byteStuffing(int size, const unsigned char* data, unsigned char* frame);
int createIFrame(int size, const unsigned char *data, int iFrameType, unsigned char *frame);
void createSupFrame(FRAME type, unsigned char *frame);
void receiveSupFrame(int fd, unsigned char *frame, int type);
int receiveIFrame(int fd, unsigned char *frame);


#endif
