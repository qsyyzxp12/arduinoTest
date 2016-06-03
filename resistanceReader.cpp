#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>
#include<csignal>
//#include<pthread.h>
#include"resistanceReader.h"
#include"Linux_UART.h"

#define MAX_MES_SIZE 	32
#define INPUT_PORT		"/dev/ttyACM0"
#define INPUT_BAUD_RATE	9600
#define OUTPUT_PORT		"/dev/ttyUSB0"
#define OUTPU_BAUD_RATE B115200	

int fd;
int resistanceVals[5];

int main()
{
	//register signal callback
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

/*	pthread_t tid;
	int ret = pthread_create(&tid, NULL, receivingDataFromSerialPort, NULL);
	if(ret)
	{
		perror("pthread_create:");
		return 1;
	}
*/
	dynamicCalculate();

//	pthread_join(tid, NULL);
	return 0;
}

void dynamicCalculate()
{
//	if(!Bluetooth.Setup_UART(OUTPUT_PORT, OUTPUT_BAUD_RATE, ~PARENB, ~CSTOPB))	
//		return 0;
}

void* receivingDataFromSerialPort(void*)
{
	//init serial port file descriptor
	fd = serialOpen(INPUT_PORT, INPUT_BAUD_RATE);
	if(fd == -1)
	{
		perror("serialOpen");
		return NULL;
	}
	serialFlush(fd);

	int ret = handShake(fd);
	if(!ret)
	{
		printf("HandShake Error\n");
		endSerial(fd);
		return NULL;
	}
	printf("HandShake success\n");
	printf("Begin receive resistance value\n");
	while(1)
	{
		if(!serialDataAvail(fd))
			continue;
		char* recvMes = readSerial(fd);
		recvMesHandle(recvMes);
	}
	endSerial(fd);
	return NULL;
}

void recvMesHandle(char* recvMes)
{
	int top = 0;
	int vals[5] = {0};
	char* valStr = strtok(recvMes, ",");
	while(valStr)
	{
		resistanceVals[top++] = atoi(valStr);
		valStr = strtok(NULL, ",");
	}
	free(recvMes);	

	for(top = 0; top < 5; top++)
	{
		printf("%d,", vals[top]);
	}
	printf("\n");
}

void signalHandler(int input)
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
	char* mes = (char*)malloc(sizeof(char)*MAX_MES_SIZE);
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
