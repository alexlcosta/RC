#include "macros.h"
#include "aux.h"

//byte stuffing
int byteStuffing(int size, const unsigned char* data, unsigned char* frame){
    int length = 4;
    unsigned char bcc2 = 0x00;

    for(int i = 0; i < size; i++){
        bcc2 = bcc2 ^ data[i];
        if(data[i]==ESC){
            frame[length++] = ESC;
            frame[length++] = ESC ^ 0x20; 
        }
        else if(data[i]==FLAG){
            frame[length++] = ESC;
            frame[length++] = FLAG^0x20;
        }
        else{
            frame[length++] = data[i];
        }
    }

    if(bcc2==ESC){
            frame[length++] = ESC;
            frame[length++] = ESC ^ 0x20; 
        }
    else if(bcc2==FLAG){
        frame[length++] = ESC;
        frame[length++] = FLAG^0x20;
    }

    return length;
}

//byte destuffing

//createFrameInfo
int createIFrame(int size, const unsigned char *data, int iFrameType, unsigned char *frame){
    frame[0] = FLAG;
    frame[1] = A_EMI;
    if(iFrameType == 0)
        frame[2] = C_I0;
    else
        frame[2] = C_I1;

    frame[3] = frame[2] ^ A_EMI;
    
    int length = byteStuffing(size, data, frame);
    frame[length] = FLAG;

    return length++;
}

//createSupFrame    
void createSupFrame(FRAME type, unsigned char *frame){
    frame[0] = FLAG;
    frame[1] = A_EMI;
    switch (type)
    {
    case SET:
        frame[2] = C_SET;
        frame[3] = A_EMI ^ C_SET;
        break;
    case UA:
        frame[2] = C_UA;
        frame[3] = A_EMI ^ C_UA;
        break;
    case RR0:
        frame[2] = C_RR0;
        frame[3] = C_RR0 ^ A_EMI;
        break;
    case RR1:
        frame[2] = C_RR1;
        frame[3] = C_RR1 ^ A_EMI;
        break;
    case REJ0: 
        frame[2] = C_REJ0;
        frame[3] = C_REJ0 ^ A_EMI;
        break;
    case REJ1:
        frame[2] = C_REJ1;
        frame[3] = C_REJ1 ^ A_EMI;
    case DISC:
        frame[1] = A_REC;
        frame[2] = C_DISC;
        frame[3] = A_REC ^ C_DISC;
    }

    frame[4] = FLAG;
}

//receive SUP frame

void receiveSupFrame(int fd, unsigned char *frame, int type){
    unsigned char byte;
    int length = 0;
    int byte = 0;
    STATE state = START; 

    do{
        byte = read(fd, &byte, 1);
        if(byte == 0)
            continue;   
        state = machine(state, UA, byte);
        length++;
        frame[length-1] = byte;
    }while(state==STOP);
}