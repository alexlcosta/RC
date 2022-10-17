#include "macros.h"

#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

struct data_ll {
    int port;
    int b_rate;
    int timeout;
    int seq_number;
    int num_transfs;
    int frame_len;
    char frame[MAX_FRAME_SIZE]; // TODO !!!
}

struct data_ll ll;

int llopen(int port, int type);
