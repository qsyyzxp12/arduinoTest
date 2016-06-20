// Linux UART �q�T��w

#include "Linux_UART.h"

#define INVALID_HANDLE_VALUE -1

// UART �q�T��w���O��@
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

		close(COM_Handle); //����COM Port
	}
}


#pragma region UART COM Port �]�w�����禡

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
	
	
	//�}��COM Port, �]���@��Ū���g�J, �D���릡I/O
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
	tcflag_t c_iflag;   //��J�Ҧ� 
    tcflag_t c_oflag;   //��X�Ҧ�
    tcflag_t c_cflag;   //����Ҧ�
    tcflag_t c_lflag;   //�����Ҧ�
    cc_t c_cc[NCCS];	//�S����r�� 
	*************************************/

	// ��o�w�]�]�w
	if(tcgetattr(COM_Handle, &options) < 0)
 	{	
		printf("Get COM Error�G%s termios error\n", COM_NAME);
		return false;
	}

	// �]�w�j�v
	cfsetspeed(&options, BaudRate);


	options.c_iflag = 0;
	
	options.c_oflag = 0;

	
	// Example: �j�v115200�B�L�P�줸�ˬd�B��ƪ���8bits�B1����줸
	// B115200 | ~PARENB | CS8 | ~CSTOPB | CLOCAL | CREAD	
	//options.c_cflag = (BaudRate | ByteSize | CLOCAL | CREAD);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_cflag &= Parity; // ~PARENB
	options.c_cflag &= StopBits; // ~CSTOPB
	
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= ByteSize;
	
	options.c_cflag &= ~CRTSCTS;

	// c_lflag�Ψӱ����C��p��B�z��J�r���C�z�Lc_lflag�]�w��C�𬰥��W�Ҧ��ΫD���W
	options.c_lflag = 0; // �ϥΫD���W
	
	
	// �S����r�� c_cc[NCCS]�Ac_cc �}�C�����Ψөw�q�䴩���S����r���A�Τ@��timeout�ѼơC
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10; // ���(0.1��)(value: unsigned char 0~255)�A�]�w timeout �ɶ�0.1��
	
	tcflush(COM_Handle, TCIFLUSH);

	// �]�w�ǿ�ѼƵ��c�����t��
	if(tcsetattr(COM_Handle, TCSANOW, &options) < 0)
	{
		printf("Set COM Error�G%s termios error\n", COM_NAME);
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

		close(COM_Handle); //����COM Port

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
		tcflush(COM_Handle, TCOFLUSH);//�M��TX buffer
		return false;
	}
	
	return true;
}

int UART::Read(char *RX_Buffer, int NumberOfBytesToRead)
{
	//�P�_�O�_Ū�����\
	Read_Bytes = read(COM_Handle, RX_Buffer, NumberOfBytesToRead);


	if (Read_Bytes < 0)
	{
		//An error occured (will occur if there are no bytes)
		printf("%s RX Error\n", COM_NAME);
		return 0;
	}
	
	return Read_Bytes;
}
