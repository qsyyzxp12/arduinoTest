char* readSerial(int fd);
int handShake(int fd);
void endSerial(int fd);
void signalHandler();
void receivingDataFromSerialPort();
void recvMesHandle(char* recvMes);
