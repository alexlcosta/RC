#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "data_link_layer.h"

// receiver or transmitter
char sender;

int llopen(int port, char *a){
    char buf[SET_UA_BUF_SIZE];

    if(strcmp(a, "RECEIVER")){
        sender = RECEIVER;
    }
    else if(strcmp(a, "TRANSMITTER")){
        sender = TRANSMITTER;
    }
    else {
        printf("Invalid sender!\n");
        return -1;
    }

    //choose port 
    if(port == 0){
        port_name = PORTS0;
    }
    else if(port == 1){
        port_name = PORTS1;
    }
    else{
        printf("Wrong port number!\n"); 
        return -1;
    }

    // open port
    int fd = open(port_name);

    if(fd < 0){
        printf("Couldn't open port!\n'");
        return -1;
    }

    // emissor -> recetor
    buf[0] = FLAG;
    buf[1] = A_EMI;
    buf[2] = C_SET;
    buf[3] = BCC_1;
    buf[4] = FLAG;

    write(fd, buf, SET_UA_BUF_SIZE);

    int counter = 3;
    while(counter > 0){

    }

    return fd;
}

int llwrite(int fd, char * buffer, int length){
    
}

int llread(int fd, char * buffer){

}
/*
TRANSMITTER:
Sends command DISC
Receives DISC command 
Sends UA message 

RECEIVER:
Sends command DISC 
Receives UA message 

em caso de sucesso retorna 0
insucesso -1
*/

int llclose(int fd, unsigned char , unsigned char ){
    if (sender == TRANSMITTER){
        send_commands()
    }
    if (sender == RECEIVER){

    }
    close(fd);
}

/* Enviar mensagem */
void send_msg(int fd, ){
    
    unsigned char buf[SET_UA_BUF_SIZE];   
    buf[0] = FLAG;
    buf[1] = A_EMI;
    buf[2] = C_SET;
    buf[3] = BCC_1;
    buf[4] = FLAG;

    write(fd, buf, SET_UA_BUF_SIZE); 
}

/* 

*/

void send_commands(int sender, , ){
    unsigned char a;
    if (sender == TRANSMITTER){
        a = A_EMI;
    }
    else if (sender == RECEIVER){
        a = A_REC;
    }
    else {
        return -1;
    }

    int res = send_msg(fd,a,C_SET);

}
