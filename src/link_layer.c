// Link layer protocol implementation

#include "link_layer.h"

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int frame_num = 0;
////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    int fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);

    connectionParameters.port = fd;

    if (fd < 0)
    {
        perror(connectionParameters.serialPort);
        return -1;
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        return -1;
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 1;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        return -1; 
    }

    if(connectionParameters.role == LlTx){
        unsigned char *SET_frame = (unsigned char*)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
        unsigned char *UA_frame = (unsigned char*)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));

        createSupFrame(SET, SET_frame);
        write(fd, SET_frame, SUP_FRAME_SIZE);

        sleep(1);
     
        receiveSupFrame(fd, UA_frame, UA);
    }
    if(connectionParameters.role == LlRx){
        unsigned char *SET_frame = (unsigned char*)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
        unsigned char *UA_frame = (unsigned char*)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));

        receiveSupFrame(fd, SET_frame, SET);

        createSupFrame(UA, UA_frame);
        write(fd, UA_frame, SUP_FRAME_SIZE);
    }

    return 1;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize, LinkLayer connectionParameters)
{   
    unsigned char *I_frame = (unsigned char*)malloc((bufSize*2+6) * sizeof(unsigned char));
    unsigned char *ACK_frame = (unsigned char*)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
    int length = createIFrame(bufSize, buf, frame_num, *I_frame);

    write(connectionParameters.port, I_frame, length);

    sleep(1);

    receiveSupFrame(connectionParameters.port, ACK_frame, ACK);

    if(ACK_frame[2] == C_RR1)
        frame_num = 1;
    else if(ACK_frame[2] == C_RR0)
        frame_num = 0;
    else{
        printf("oh no\n");
        return -1;
    }

    return bufSize;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet, LinkLayer connectionParameters)
{
    unsigned char *I_frame = (unsigned char*)malloc((MAX_PAYLOAD_SIZE*2+6) * sizeof(unsigned char));

    int bytes = receiveIFrame(connectionParameters.port, I_frame);
    if(bytes == -1){
        printf("error iframe\n");
        return -1;
    }

    unsigned char *frame = (unsigned char *)malloc(SUP_FRAME_SIZE*sizeof(unsigned char));

    if(I_frame[2] == C_I0 && connectionParameters.sequence_number == 1){
        printf("Resend I1\n");
        createSupFrame(RR1, frame);
    }
    else if (I_frame[2] == C_I1 && connectionParameters.sequence_number == 0){
        printf("Resend I0\n");
        createSupFrame(RR0, frame);
    }
    else if (I_frame[2] == C_I1){
        createSupFrame(RR0, frame);
    }
    else if (I_frame[2] == C_I0){
        createSupFrame(RR1, frame);
    }
    else{
        return -1;
    } 

    write(connectionParameters.port, frame, SUP_FRAME_SIZE);

    for(int i = 0; i < bytes; i++){
        packet[i] = I_frame[i+4];
    }

    sleep(1);

    return bytes;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(LinkLayer connectionParameters)
{
    if(connectionParameters.role == LlTx){
        if(createTDisc(connectionParameters)){
            printf("Fail\n");
            return -1;
        }
    }
    else if(connectionParameters.role == LlRx){
        if(createRDisc(connectionParameters)){
            printf("Fail\n");
            return -1;
        }
    }

   if (tcsetattr(connectionParameters.port, TCSANOW, &oldtio) == -1){
        perror("Couldn't close port\n");
        return -1;
    }

    return close(connectionParameters.port);
}

int createTDisc(LinkLayer connectionParameters){
    unsigned char *frame = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
    unsigned char *RCVframe = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
    unsigned char *UAframe = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));

    createSupFrame(DISC, frame);

    write(ll.port, frame, SUP_FRAME_SIZE);

    sleep(1);

    if(receiveSupFrame(connectionParameters.port, RCVframe, DISC) == -1){
        printf("Fail\n");
        return -1;
    }

    createSupFrame(UA, UAframe);

    write(connectionParameters.port, UAframe, SUP_FRAME_SIZE);

    return 1;
}

int createRDisc(LinkLayer connectionParameters) {
    unsigned char *frame = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
    unsigned char *RCVframe = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));
    unsigned char *UAframe = (unsigned char *)malloc(SUP_FRAME_SIZE * sizeof(unsigned char));

    if(receiveSupFrame(connectionParameters.port, frame, DISC) == -1){
        printf("Fail\n");
        return -1;
    }

    createSupFrame(DISC, RCVframe);

    write(connectionParameters.port, RCVframe, SUP_FRAME_SIZE);

    if(receiveSupFrame(connectionParameters.port, UAframe, UA) == -1){
        printf("Fail\n");
        return -1;
    }

    return 0;
}