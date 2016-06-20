#pragma once

// Math �`��
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

// �u�ʼƦ��o�i��
class Digital_Filter
{
private:
	// ��ܽu�ʼƦ�t�Ϊ����A�Ŷ���ܦ�
	// X[k+1] = AX[k] + Bu[k]
	// y[k] = CX[k] + Du[k]
	double **A;  // N*N
	double *B;   // N*1
	double *C;   // 1*N
	double D;    // 1*1

	// �t�Ϊ��A
	double *X;         // �{�b���A
	double *Next_X;    // �U�@�ɨ説�A
	int Order;         // �t�ζ���


	// �����O�_�w�]�w�L�t�ΰѼ�
	bool Set_System_Flag;
	
	
	// ����Ŷ�
	void Free_Space();

public:
	// �غc�l
	Digital_Filter();

	// �Ѻc�l
	~Digital_Filter();

	// ���w���N Single In-Output �u�ʼƦ�t��A,B,C,D�x�} ==> X[k+1] = AX[k] + Bu[k], y[k] = CX[k] + Du[k]
	// �ѩ�ϥέ��N�k�h�p��C�@��X�A�٥������w�t�Ϊ�l���AX(0)
	// �u�ʼƦ�t�ΫإߡA���wA, B, C, D�M�t�γ̰�����+��l���A
	void State_Space(double **_A, double *_B, double *_C, double _D, int _Order, double *_Ini_State);

	// �L�X�]�w���t�ΰѼ�
	void Show_Parameter();

	
	// ��Ju[k]�g�L�ಾ��ƫ᪺��Xy[k]
	double Output_y(double Input_u);
};


// �������o�i��
class Middle_Filter
{
private:

	// �s����v�T��
	int *Value;

	int *Sort_Value;

	// �����}�C�j�p
	unsigned int Array_Size;

public:
	// �غc�l
	Middle_Filter();
	
	// �Ѻc�l
	~Middle_Filter();

	// �]�w�s����v�T����array�j�p�A�������_��
	void Set_ArraySize(unsigned int size);

	// ��Ju[k]�g�L�ಾ��ƫ᪺��Xy[k]
	int Output_y(int Input_u);
};
