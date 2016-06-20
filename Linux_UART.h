// Linux UART �q�T��w

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>  //Used for UART
#include <unistd.h>	//Used for UART
#include <fcntl.h>	//Used for UART
#include <termios.h>	//Used for UART


// UART �q�T��w���O�ŧi
class UART
{
	//�����w�q
private:
	int  COM_Handle;   // �}��Com Port��handle
	char COM_NAME[32]; // ����COM Name
		
	int Read_Bytes;  //�^�ǨC���wŪ����byte��
	int Write_Bytes; //�^�ǨC���w�g�J��byte��
		
	#pragma region UART COM Port �]�w�����禡

	//�}��Com Port
	bool ComPort_Open(char *ComName);

	//�]�wCom Port
	bool ComPort_Set(unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits);
		
	#pragma endregion
		
public:
		
	//�غc�l
	UART();
		
	//�Ѻc�l
	~UART();

	// COM Port�}�Ҩê�l��
	bool Setup_UART(char *ComName, unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits);

	// �ϥΪ̦ۦ�w�qCOM�}�ҰѼ� (�ۦ�w�q struct termios)
	void UserDef_Setup_UART(char *ComName, int com_handle);

	// ���oComName
	const char* Get_ComName();

	// ���oCOM_Handle
	const int Get_Handle();

	//����COM Port
	void Close_UART();
		
	//COM Port�ǰe���O
	bool Write(char *TX_Buffer, int NumberOfBytesToWrite);

	//Com PortŪ�����O (���GŪ��Byte�Ƥ���j��RX_Buffer��size)�A�^�Ǧ��\Ū��Bytes��
	int Read(char *RX_Buffer, int NumberOfBytesToRead);

};
