#pragma once

// Math 常數
#ifndef e
#define e  2.71828182846
#endif

#ifndef pi
#define pi 3.14159265359
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

// 線性數位濾波器
class Digital_Filter
{
private:
	// 表示線性數位系統的狀態空間表示式
	// X[k+1] = AX[k] + Bu[k]
	// y[k] = CX[k] + Du[k]
	double **A;  // N*N
	double *B;   // N*1
	double *C;   // 1*N
	double D;    // 1*1

	// 系統狀態
	double *X;         // 現在狀態
	double *Next_X;    // 下一時刻狀態
	int Order;         // 系統階數


	// 紀錄是否已設定過系統參數
	bool Set_System_Flag;
	
	
	// 釋放空間
	void Free_Space();

public:
	// 建構子
	Digital_Filter();

	// 解構子
	~Digital_Filter();

	// 給定任意 Single In-Output 線性數位系統A,B,C,D矩陣 ==> X[k+1] = AX[k] + Bu[k], y[k] = CX[k] + Du[k]
	// 由於使用迭代法去計算每一刻X，還必須給定系統初始狀態X(0)
	// 線性數位系統建立，給定A, B, C, D和系統最高階數+初始狀態
	void State_Space(double **_A, double *_B, double *_C, double _D, int _Order, double *_Ini_State);

	// 印出設定的系統參數
	void Show_Parameter();

	
	// 輸入u[k]經過轉移函數後的輸出y[k]
	double Output_y(double Input_u);
};


// 中間值濾波器
class Middle_Filter
{
private:

	// 存放歷史訊號
	int *Value;

	int *Sort_Value;

	// 紀錄陣列大小
	unsigned int Array_Size;

public:
	// 建構子
	Middle_Filter();
	
	// 解構子
	~Middle_Filter();

	// 設定存放歷史訊號的array大小，必須為奇數
	void Set_ArraySize(unsigned int size);

	// 輸入u[k]經過轉移函數後的輸出y[k]
	int Output_y(int Input_u);
};
