#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>
#include<signal.h>

#define MAX_MES_SIZE 32

char* errMes;
int fd;

char* readSerial(int fd);
int handShake(int fd);
void endSerial(int fd);
void signalHandler();

int main()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	errMes = malloc(sizeof(char)*64);
	bzero(errMes, sizeof(char)*64);

	fd = serialOpen("/dev/ttyACM0", 9600);
	if(fd == -1)
	{
		perror("serialOpen");
		return 1;
	}

	int ret = handShake(fd);
	if(!ret)
	{
		printf("HandShake Error\n");
		endSerial(fd);
		return 1;
	}
	printf("HandShake success\n");
	while(1)
		printf("%s", readSerial(fd));

	endSerial(fd);
	return 0;
}

void signalHandler()
{
	printf("signal catched\n");
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
		int ret;
		while(!(ret = serialDataAvail(fd)))
		{
			int i=0;
			for(i=0; i<strlen(reqMes); i++)
				serialPutchar(fd, reqMes[i]);
			sleep(1);
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
