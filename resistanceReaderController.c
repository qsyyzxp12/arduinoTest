#include<stdio.h>
#include<stdlib.h>
#include<syslog.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>

void callArduinoConnectDaemond();
void killDaemond();

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		printf("Usage: run [start | stop]\n");
		return 0;
	}
	if(!strcmp(argv[1], "start"))
		callArduinoConnectDaemond();
	else if(!strcmp(argv[1], "stop"))
		killDaemond();
	else
		printf("Usage: run [start | stop]\n");
	
	return 0;
}

void killDaemond()
{
	int fd = open("/home/pi/arduinoTest/PID", O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		return;
	}
	char pidStr[16] = {'\0'};
	int readLen = read(fd, pidStr, 16);
	if(readLen < 0)
	{
		perror("read");
		return;
	}

	int pid = atoi(pidStr);
	int ret = kill(pid, SIGTERM);
	if(ret == -1)
		perror("kill");
	else
	{
		int fd = open("/home/pi/arduinoTest/PID", O_TRUNC);
		close(fd);
	}
}

void callArduinoConnectDaemond()
{
	int fd = open("/home/pi/arduinoTest/PID", O_RDONLY);
	int size = lseek(fd, 0, SEEK_END);
	if(size > 0)
	{
		printf("Please stop the pervious process.\nDo \"resistanceReaderController stop\"\n");
		return;
	}
	close(fd);

	int pid = fork();
	if(pid != 0)
		exit(0);
	
	setsid();
	signal(SIGHUP, SIG_IGN);
	pid = fork();
	if(pid != 0)
	{
		fd = open("/home/pi/arduinoTest/PID", O_RDWR | O_CREAT | O_TRUNC, 0777);
		if(fd < 0)
		{
			perror("Open");
			return;
		}
		char str[16] = {'\0'};
		sprintf(str, "%d", pid);
		int ret = write(fd, str, strlen(str));
		if(ret < 0)
			perror("write");
		close(fd);
		exit(0);
	}
	chdir("/");
	umask(0);
/*	
	close(0);
	close(1);
	close(2);
*/	
	execl("/home/pi/arduinoTest/resistanceReader", NULL);
}
