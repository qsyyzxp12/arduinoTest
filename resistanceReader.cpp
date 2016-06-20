#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>
#include<csignal>
#include<pthread.h>
#include"resistanceReader.h"

#include"Linux_UART.h"
#include"Robot_Arm.h"
#include"Filter.h"

#define MAX_MES_SIZE 	32
#define INPUT_PORT		"/dev/ttyACM0"
#define INPUT_BAUD_RATE	9600
#define OUTPUT_PORT		"/dev/ttyUSB0"
#define OUTPUT_BAUD_RATE B115200	

int fd = 0;
static int resistanceVals[5] = {0};
Middle_Filter filter[5];

UART Bluetooth;

int data_count = 0;

int main()
{
	//register signal callback
	//signal(SIGINT, signalHandler);
	//signal(SIGTERM, signalHandler);

	if(!Bluetooth.Setup_UART(OUTPUT_PORT, OUTPUT_BAUD_RATE, ~PARENB, CS8, ~CSTOPB))	
	{
		printf("Bluetooth setup Error\n");
		return 1;
	}

	pthread_t tid;
	
	int ret = pthread_create(&tid, NULL, receivingDataFromSerialPort, NULL);
	
	if(ret)
	{
		perror("pthread_create:");
		return 1;
	}
	
	dynamicCalculate();

	pthread_join(tid, NULL);
	return 0;
}

void dynamicCalculate()
{	
	Robot_Arm My_Arm;
	Eigen::Matrix4d T_Base2Global;
	T_Base2Global << 0, 0, -1, 0,
					 -1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 0, 1;
	My_Arm.Set_Base2Global(T_Base2Global);

	while(data_count < 50)
		printf("Wait... Data_Count = %d\n", data_count);
	
	My_Arm.Set_Ini_Theta(My_Arm.RawTheta2Deg(resistanceVals));
	
	while(1)
	{
//		for(int i=0; i<5; i++)
//			printf("%d,", resistanceVals[i]);
//		printf("\n");

		My_Arm.Refresh_TFMatrix(My_Arm.RawTheta2Deg(resistanceVals));
		char* output = My_Arm.Get_TransData();
		
		for(int i=0; i<31; i++)
			printf("%c", output[i]);
		
		Bluetooth.Write(output, 31);
		usleep(100000);
	}
}

void* receivingDataFromSerialPort(void*)
{
	for(int i=0; i<5; i++)
		filter[i].Set_ArraySize(30);

	//init serial port file descriptor
	fd = serialOpen(INPUT_PORT, INPUT_BAUD_RATE);
	
	if(fd == -1)
	{
		perror("Input Serial Port Open");
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
		{
			usleep(10000);
			continue;
		}
		char* recvMes = readSerial(fd);
		recvMesHandle(recvMes);
		data_count++;
	}
	endSerial(fd);
	return NULL;
}

void recvMesHandle(char* recvMes)
{
/*	int top = 0;
	printf("recvMes = %s", recvMes);
	char* valStr = strtok(recvMes, ",");
	printf("recvMes2 = %s", recvMes);
	while(valStr)
	{
		printf("top = %d", top);
	//	resistanceVals[top++] = atoi(valStr);
		resistanceVals[top] = filter[top].Output_y(atoi(valStr));
		top++;
		valStr = strtok(NULL, ",");
	}
*/

	int Raw_ResistanceVals[5] = {0};

	sscanf(recvMes, "%d,%d,%d,%d,%d,\r\n", &Raw_ResistanceVals[0], &Raw_ResistanceVals[1], &Raw_ResistanceVals[2], &Raw_ResistanceVals[3], &Raw_ResistanceVals[4]);
	
	for(int i=0; i<5; i++)
		resistanceVals[i] = filter[i].Output_y(Raw_ResistanceVals[i]);

	//free(recvMes);	
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

char mes[MAX_MES_SIZE]={0};

char* readSerial(int fd)
{
	//char* mes = (char*)malloc(sizeof(char)*MAX_MES_SIZE);
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
