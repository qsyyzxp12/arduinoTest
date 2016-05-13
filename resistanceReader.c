#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>
#include<signal.h>

#define MAX_MES_SIZE 	32
#define BAUD_RATE		9600
#define SERIAL_PORT		"/dev/ttyACM0"

char* errMes;
int fd;

char* readSerial(int fd);
int handShake(int fd);
void endSerial(int fd);
void signalHandler();

int main()
{
	//register signal callback
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	//init errMes
	errMes = malloc(sizeof(char)*64);
	bzero(errMes, sizeof(char)*64);

	//init serial port file descriptor
	fd = serialOpen(SERIAL_PORT, BAUD_RATE);
	if(fd == -1)
	{
		perror("serialOpen");
		return 1;
	}
	serialFlush(fd);

	int ret = handShake(fd);
	if(!ret)
	{
		printf("HandShake Error\n");
		endSerial(fd);
		return 1;
	}
	printf("HandShake success\n");
	printf("Begin receive resistance value\n");
	while(1)
	{
		if(!serialDataAvail(fd))
			continue;
		char* recvMes = readSerial(fd);
		printf("%s", recvMes);
	}

	endSerial(fd);
	return 0;
}

void signalHandler()
{
	endSerial(fd);
	exit(1);
}

void endSerial(int fd)
{
	char reqMes[] = "End";
	serialPuts(fd, reqMes);
	serialClose(fd);
}

int handShake(int fd)
{
	char reqMes[] = "Arduino?";
	int errCount = 0;
	while(errCount < 5)
	{
		while(1)
		{
			int i=0;
			for(i=0; i<strlen(reqMes); i++)
				serialPutchar(fd, reqMes[i]);
			if(!serialDataAvail(fd))
				sleep(1);
			else
				break;
		}

		printf("Serial Data Available!\n");
		char* recvMes = readSerial(fd);
		if(!strcmp(recvMes, "YES\r\n"))
			return 1;
		else if(!strcmp(recvMes, "NO\r\n"))
		{
			printf("Receive mes \"NO\"");
			errCount++;
			continue;
		}
		else
		{
			printf("Receive upexpected message - %s", recvMes);
			errCount++;
			sleep(5);
			continue;
		}
	}
	return 0;
}

char* readSerial(int fd)
{
	char* mes = malloc(sizeof(char)*MAX_MES_SIZE);
	bzero(mes, sizeof(char)*MAX_MES_SIZE);
	int top = 0;

	while(1)
	{
		char c = (char)serialGetchar(fd);
		mes[top++] = c;
		if(top > 1)
			if(mes[top-1] == '\n' && mes[top-2] == '\r')
				break;
	}
	return mes;
}
