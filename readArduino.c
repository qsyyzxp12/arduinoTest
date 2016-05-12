#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>

#define MAX_MES_SIZE 32

char* errMes;

char* readSerial(int fd);
int handShake(int fd);

int main()
{
	errMes = malloc(sizeof(char)*64);
	bzero(errMes, sizeof(char)*64);
	int fd = serialOpen("/dev/ttyACM0", 9600);
	if(fd == -1)
	{
		perror("serialOpen");
		return 1;
	}

	int ret = handShake(fd);
	if(!ret)
	{
		printf("HandShake: %s\n", errMes);
		return 1;
	}
	printf("HandShake success\n");
	return 0;
}

int handShake(int fd)
{
	int ret;
	char reqMes[] = "Arduino?";
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
		bzero(errMes, sizeof(char)*64);
		strcat(errMes, "Receive mes \"NO\"");
		return 0;
	}
	else
	{
		bzero(errMes, sizeof(char)*64);
		errMes = strcat("Receive unexpected message - ", recvMes);
		strcat(errMes, recvMes);
		return 0;
	}
	
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
