// Linux UART 通訊協定

#include "Linux_UART.h"

#define INVALID_HANDLE_VALUE -1

// UART 通訊協定類別實作
UART::UART()
{
	COM_Handle = INVALID_HANDLE_VALUE;
	memset(COM_NAME, '\0', sizeof(COM_NAME));
}

UART::~UART()
{
	if (COM_Handle != INVALID_HANDLE_VALUE)
	{	
		// Clear the DTR line
		int DTR_flag = TIOCM_DTR;
		ioctl(COM_Handle, TIOCMBIC, &DTR_flag);

		// Flush any information in I/O buffer
		tcflush(COM_Handle, TCIOFLUSH);

		close(COM_Handle); //關閉COM Port
	}
}


#pragma region UART COM Port 設定相關函式

bool UART::ComPort_Open(char *ComName)
{
	// OPEN THE UART
	//  The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//	O_RDONLY - Open for reading only.
	//	O_RDWR - Open for reading and writing.
	//	O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//	if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//	immediately with a failure status if the output can't be written immediately.
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	
	
	//開啟COM Port, 設為一般讀取寫入, 非阻塞式I/O
	COM_Handle = open(ComName, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);

	if (COM_Handle < 0)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART at %s.\nEnsure it is not in use by another application\n", COM_NAME);
		return false;
	}
	
	fcntl(COM_Handle, F_SETFL, 0); //Reads will be blocking

	return true;
}

bool UART::ComPort_Set(unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits)
{
	//CONFIGURE THE UART
	//  The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem control lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)

	struct termios options;

	/************************************
	The flags parameter - see http://blog.xuite.net/uwlib_mud/twblog/108242774-Linux+RS-232+%E7%A8%8B%E5%BC%8F%E8%A8%AD%E8%A8%88
	tcflag_t c_iflag;   //輸入模式 
    tcflag_t c_oflag;   //輸出模式
    tcflag_t c_cflag;   //控制模式
    tcflag_t c_lflag;   //局部模式
    cc_t c_cc[NCCS];	//特殊控制字元 
	*************************************/

	// 獲得預設設定
	if(tcgetattr(COM_Handle, &options) < 0)
 	{	
		printf("Get COM Error：%s termios error\n", COM_NAME);
		return false;
	}

	// 設定鮑率
	cfsetspeed(&options, BaudRate);


	options.c_iflag = 0;
	
	options.c_oflag = 0;

	
	// Example: 鮑率115200、無同位元檢查、資料長度8bits、1停止位元
	// B115200 | ~PARENB | CS8 | ~CSTOPB | CLOCAL | CREAD	
	//options.c_cflag = (BaudRate | ByteSize | CLOCAL | CREAD);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= Parity; // ~PARENB
	options.c_cflag &= StopBits; // ~CSTOPB
	
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= ByteSize;
	
	options.c_cflag &= ~CRTSCTS;

	// c_lflag用來控制串列埠如何處理輸入字元。透過c_lflag設定串列埠為正規模式或非正規
	options.c_lflag = 0; // 使用非正規
	
	
	// 特殊控制字元 c_cc[NCCS]，c_cc 陣列成員用來定義支援的特殊控制字元，及一些timeout參數。
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10; // 單位(0.1秒)(value: unsigned char 0~255)，設定 timeout 時間0.1秒
	
	tcflush(COM_Handle, TCIFLUSH);

	// 設定傳輸參數結構的內含值
	if(tcsetattr(COM_Handle, TCSANOW, &options) < 0)
	{
		printf("Set COM Error：%s termios error\n", COM_NAME);
		return false;
	}

	return true;
}

#pragma endregion


bool UART::Setup_UART(char *ComName, unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits)
{
	strcpy(COM_NAME, ComName);
	

	if( !ComPort_Open(COM_NAME) )
		return false;
	
	if( !ComPort_Set(BaudRate, Parity, ByteSize, StopBits) )
		return false;

	
	// Set the Data Terminal Ready line
	int DTR_flag = TIOCM_DTR;
	ioctl(COM_Handle, TIOCMBIS, &DTR_flag);


	// Flush any information in buffer
	tcflush(COM_Handle, TCIOFLUSH);

	return true;
}

void UART::UserDef_Setup_UART(char *ComName, int com_handle)
{
	strcpy(COM_NAME, ComName);
	COM_Handle = com_handle;
}

const char* UART::Get_ComName()
{
	return COM_NAME;
}

const int UART::Get_Handle()
{
	return COM_Handle;
}

void UART::Close_UART()
{
	if (COM_Handle != INVALID_HANDLE_VALUE)
	{	
		// Clear the DTR line
		int DTR_flag = TIOCM_DTR;
		ioctl(COM_Handle, TIOCMBIC, &DTR_flag);

		// Flush any information in I/O buffer
		tcflush(COM_Handle, TCIOFLUSH);

		close(COM_Handle); //關閉COM Port

		COM_Handle = INVALID_HANDLE_VALUE;
	}
}

bool UART::Write(char *TX_Buffer, int NumberOfBytesToWrite)
{
	// Send Data	
	Write_Bytes = write(COM_Handle, TX_Buffer, NumberOfBytesToWrite);
	
	if(Write_Bytes < 0)
	{
		printf("%s TX Error\n", COM_NAME);
		return false;
	}
	else if(Write_Bytes != NumberOfBytesToWrite)
	{
		printf("%s TX not successful\n", COM_NAME);
		tcflush(COM_Handle, TCOFLUSH);//清空TX buffer
		return false;
	}
	
	return true;
}

int UART::Read(char *RX_Buffer, int NumberOfBytesToRead)
{
	//判斷是否讀取成功
	Read_Bytes = read(COM_Handle, RX_Buffer, NumberOfBytesToRead);


	if (Read_Bytes < 0)
	{
		//An error occured (will occur if there are no bytes)
		printf("%s RX Error\n", COM_NAME);
		return 0;
	}
	
	return Read_Bytes;
}
