#include "state_machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include "macros.h"

unsigned char c = 0x00;

STATE machine(STATE s, SET_UA type, unsigned char input){
    STATE state;     
    
    switch(s){
        case START:
            if(input == FLAG)
                state = FLAG_RCV;
            break;
        case FLAG_RCV:
            if(input == A_EMI)
                state = A_RCV;
            else 
                state = START; 
            break;
        case A_RCV:
            if(type == SET){
                if(input == C_SET)
                    state = C_RCV;
                    break;
            }
            else if(type == UA){
                if(input == C_UA)
                    state = C_RCV;
                    break;
            }
            else if(type == ACK){
                if(input == C_RR0 || input == C_RR1 || input == C_REJ0 || input == C_REJ1){
                    state = C_RCV;
                    c = input;
                    break;
                }
            }
            else if(type == DISC){
                if(input == C_DISC){
                    state = C_RCV;
                    break;
                }
            }
            if(input == FLAG)
                state = FLAG_RCV;
            else
                state = START; 
            break;
        case C_RCV:
            if(type == SET){
                if(input == A_EMI^C_SET){
                    state = BCC_OK;
                    break;
                }
            }
            else if(type == UA){
                if(input == A_EMI^C_UA){
                    state = BCC_OK;
                    break;
                }
            }
            else if(type == ACK){
                if(input == (A_EMI^c)){
                    state = BCC_OK;
                    break;
                }
            }
            else if(type == DISC){
                if(input == (A_EMI^C_DISC)){
                    state = BCC_OK;
                    break;
                }
            }
            if(input == FLAG)
                state = FLAG_RCV;     
            else
                state = START; 
            break;
        case BCC_OK:
            if(input == FLAG)
                state = STOP;
            else 
                state = START; 
            break;
        case STOP:
            state = STOP;
            break;
    }
    
    return state; 
}

STATE infoMachine(STATE s, unsigned char input, unsigned char *frame){
    static int i = 0;

    switch ((s))
    {
    case START:
        i = 0;
        if(input == FLAG){
            s = FLAG_RCV;
            i++;
            frame[i] = FLAG;
        }
        break;
    case FLAG_RCV:
        if(input == A){
            frame[i++] = A;
            s = A_RCV;
        } 
        else if(input == FLAG)
            break;
        else 
            s = START;
        break;
    case A_RCV:
        if(input == C_I0){
            frame[i++] = C_I0;
            c = C_I0;
            s = C_RCV;
            break;
        }
        else if(input == C_I1){
            frame[i++] = C_I1;
            c = C_I1;
            s = C_RCV;
            break;
        }
        else if(input == FLAG)
            s = FLAG_RCV;
        else 
            s = START;
        break;
    case C_RCV:
        if(input == (A_EMI^c)){
            frame[i++] = input;
            s = BCC_OK;
            break;
        }
        else if(input == FLAG)
            s = FLAG_RECEIVED;
        else 
            s = START;
        break;
    case BCC_OK:
        if(input == FLAG){
            state = STOP_STATE;
            frame[i++]=FLAG;
        }
        else 
            frame[i++] = input;
        break;
    case STOP:
        s = START;
        break;
    }

    return state; 
}