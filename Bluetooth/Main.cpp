#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "Linux_UART.h"


void* Write_Data(void*);
void* Read_Data(void*);

UART Bluetooth;

int main()
{
	// COM Name:"/dev/ttyUSB0"、鮑率115200、無同位元檢查、資料長度8bits、1停止位元
	if( !Bluetooth.Setup_UART("/dev/ttyUSB0", B115200, ~PARENB, CS8, ~CSTOPB) )
		return 0;
	

	pthread_t Write_ID, Read_ID;

	// 產生讀和寫的執行緒
	if(pthread_create(&Write_ID, NULL, Write_Data, NULL) != 0)
	{
		printf ("Create Write pthread error!\n");
		return 0;
	}
	
	if(pthread_create(&Read_ID, NULL, Read_Data, NULL) != 0)
	{
		pthread_cancel(Write_ID);
		printf ("Create Read pthread error!\n");
		return 0;
	}


	pthread_join(Write_ID, NULL);//等待線程pthread結束
	
	printf("Bluetooth End\n");

	// 釋放資源
	pthread_cancel(Write_ID);
	pthread_cancel(Read_ID);
	
	
	return 1;
}

void* Write_Data(void* arg)
{
	char Write_str[256] = {'\0'};
	
	while(true)
	{
		gets(Write_str);
		
		if(Write_str[0] == '\0')
			break;

		int WriteLength = strlen(Write_str); //get length of string 		
		Write_str[WriteLength] = '\r';			
		Write_str[WriteLength+1] = '\n';		

		Bluetooth.Write(Write_str, WriteLength+2);
	}
	return NULL;
}

void* Read_Data(void* arg)
{
	char Read_str[256] = {'\0'};

	while(true)
	{
		int num = Bluetooth.Read(Read_str, sizeof(Read_str));
		//printf("num = %d %s\n", num, Read_str);
				
		if(num != 0)
		{
			for(int i=0; i < num; i++)	
				printf("%c", Read_str[i]);
		}
		
	}
	return NULL;
}
