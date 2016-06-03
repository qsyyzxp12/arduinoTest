// Linux UART 通訊協定

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>  //Used for UART
#include <unistd.h>	//Used for UART
#include <fcntl.h>	//Used for UART
#include <termios.h>	//Used for UART


// UART 通訊協定類別宣告
class UART
{
	//成員定義
private:
	int  COM_Handle;   // 開啟Com Port的handle
	char COM_NAME[32]; // 紀錄COM Name
		
	int Read_Bytes;  //回傳每次已讀取的byte數
	int Write_Bytes; //回傳每次已寫入的byte數
		
	#pragma region UART COM Port 設定相關函式

	//開啟Com Port
	bool ComPort_Open(char *ComName);

	//設定Com Port
	bool ComPort_Set(unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits);
		
	#pragma endregion
		
public:
		
	//建構子
	UART();
		
	//解構子
	~UART();

	// COM Port開啟並初始化
	bool Setup_UART(char *ComName, unsigned int BaudRate, unsigned int Parity, unsigned int ByteSize, unsigned int StopBits);

	// 使用者自行定義COM開啟參數 (自行定義 struct termios)
	void UserDef_Setup_UART(char *ComName, int com_handle);

	// 取得ComName
	const char* Get_ComName();

	// 取得COM_Handle
	const int Get_Handle();

	//關閉COM Port
	void Close_UART();
		
	//COM Port傳送指令
	bool Write(char *TX_Buffer, int NumberOfBytesToWrite);

	//Com Port讀取指令 (註：讀取Byte數不能大於RX_Buffer的size)，回傳成功讀取Bytes數
	int Read(char *RX_Buffer, int NumberOfBytesToRead);

};
