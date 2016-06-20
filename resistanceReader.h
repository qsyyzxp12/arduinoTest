char* readSerial(int fd);
int handShake(int fd);
void endSerial(int fd);
void signalHandler(int);
void* receivingDataFromSerialPort(void*);
void recvMesHandle(char* recvMes);
void dynamicCalculate();
