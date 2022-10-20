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
            if(input == FLAG)
                state = FLAG_RCV;
            else if(input == A_EMI)
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

