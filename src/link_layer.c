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

    int bytes = read(connectionParameters.port, I_frame, )
    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
