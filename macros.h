#ifndef MACROS_H
#define MACROS_H

#define FALSE 0
#define TRUE 1 

#define TRANSMITTER 0
#define RECEIVER 1

#define BUF_SIZE 256 
#define SET_UA_BUF_SIZE 5 

#define FLAG 0x7E
#define ESC 0x7D

#define A_EMI 0x03 
#define A_REC 0x01 
#define REC 0x01
#define C_SET 0x03
#define C_UA 0x07
#define BCC_1 A_EMI ^ C_SET
#define BCC_2 A_REC ^ C_UA

#define PORTS0 "dev/ttyS0"
#define PORTS1 "dev/ttyS1"

#endif
