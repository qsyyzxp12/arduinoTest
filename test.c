#include<stdio.h>
#include<stdlib.h>     /*標準函數庫定義*/
#include<unistd.h>     /*Unix 標準函數定義*/
#include<sys/types.h> 
#include<sys/stat.h>  
#include<fcntl.h>      /*檔控制定義*/
#include<termios.h>    /*PPSIX 終端控制定義*/
#include<errno.h>  

#define BAUD_RATE B115200

/*
	struct termios
	{
		tcflag_t c_iflag; // input mode flags
		tcflag_t c_oflag; // output mode flags
		tcflag_t c_cflag; // control mode flags
		tcflag_t c_lflag; // local mode flags
		cc_t c_line;      // line discipline
		cc_t c_cc[NCCS];  // control characters
	};
*/

void read_Serial_Port(const char* DEVICE_PORT);
int readMessage(int file);

int main()
{
	read_Serial_Port("/dev/ttyACM0");
	return 0;
}

void read_Serial_Port(const char* DEVICE_PORT)
{
    int file;
    struct termios options;
    char message[100];
    int b;

    file = open(DEVICE_PORT, O_RDONLY | O_NOCTTY | O_NDELAY);

    if(file == -1)
	{
		perror("Unable to open the serial port\n");
	}
    printf("Serial port open successful\n");

    tcgetattr(file, &options);          
    cfsetispeed(&options, BAUD_RATE);                   
    cfsetospeed(&options, BAUD_RATE);
    options.c_cflag |= (CLOCAL | CREAD);              
    options.c_cflag |= PARENB;                      //No parity                 
    options.c_cflag |= PARODD;                      
    options.c_cflag &= ~CSTOPB;                     
    options.c_cflag &= ~CSIZE;                      
    options.c_cflag |= CS8;                         //8 bits    
    tcsetattr(file, TCSANOW, &options);          
//    fcntl(file, F_SETFL, FNDELAY);

    printf("Reading serial port ...\n\n"); 
    b = readMessage(file);
    if (b == 0)
		printf("Error while reading serial port\n");
    else 
		printf("Serial port read successful\n");
    close(file);
    printf("Serial port closed\n");
}

int readMessage(int file)
{
    if (file != -1)
    {
        char data[100] = {'\0'};
		int readLen = 0;
        while (1)
        {
        	readLen = read(file, data, 1);
			if(readLen == 0)
				continue;
            else if (readLen == -1)
            {
				if(errno == 11)
					continue;
                printf("reception error\n");
                printf("code errno = %d\n", errno);
				perror("");
                return 0;
            }
			printf("readLen = %d\n", readLen);
			printf("data = %s\n", data);
        }
    }
}

