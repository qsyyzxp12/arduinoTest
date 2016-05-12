#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<wiringSerial.h>
#include<errno.h>
#include<fcntl.h>

#define MAX_MES_SIZE 32

char* readSerial(int fd);

int main()
{
	int fd = serialOpen("/dev/ttyACM0", 9600);
	if(fd == -1)
	{
		perror("serialOpen");
		return 1;
	}
	int ret;
	while(!(ret = serialDataAvail(fd)))
	{
		serialPutchar(fd, 'X');
		sleep(1);
	}
	printf("ret = %d\n", ret);
	printf("Serial Data Available!\n");

	while(1)
		printf("%s", readSerial(fd));
	
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
