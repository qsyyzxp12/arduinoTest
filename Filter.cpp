#include "Filter.h"

#pragma region 線性數位濾波器

Digital_Filter::Digital_Filter()
{
	A = NULL;
	B = NULL;
	C = NULL;
	D = 0.0;
	X = NULL;
	Next_X = NULL;
	Order = 0;

	Set_System_Flag = false;	
}

Digital_Filter::~Digital_Filter()
{
	// 釋放空間
	if(Set_System_Flag)
		Free_Space();
}

void Digital_Filter::Free_Space()
{
	for(int i=0; i < Order; i++)
		delete[] A[i];
	delete[] A;
	delete[] B;
	delete[] C;
	delete[] X;
	delete[] Next_X;

	Set_System_Flag = false;
}

void Digital_Filter::State_Space(double **_A, double *_B, double *_C, double _D, int _Order, double *_Ini_State)
{
	if(Set_System_Flag)
		Free_Space();

	Order = _Order;

	A = new double*[Order];
	for(int i=0; i < Order; i++)
		A[i] = new double[Order];

	B = new double[Order];
	C = new double[Order];
	X = new double[Order];
	Next_X = new double[Order];

	// 複製矩陣參數
	for(int i=0; i < Order; i++)
	{	
		for(int j=0; j < Order; j++)
			A[i][j] = _A[i][j];
		
		B[i] = _B[i];
		C[i] = _C[i];

		X[i] = _Ini_State[i];
	}

	D = _D;

	Set_System_Flag = true;
}

void Digital_Filter::Show_Parameter()
{
	if(!Set_System_Flag)
	{	
		printf("參數沒有設定\n");
		return;
	}

	// 印出參數
	printf("A =\n");
	for(int i=0; i < Order; i++)
	{
		for(int j=0; j < Order; j++)
			printf("%f\t", A[i][j]);
		printf("\n");
	}

	printf("B =\n");
	for(int i=0; i < Order; i++)
		printf("%f\n", B[i]);

	printf("C =\n");
	for(int i=0; i < Order; i++)
		printf("%f\t", C[i]);
	printf("\n");

	printf("D = %f\n", D);
}

double Digital_Filter::Output_y(double Input_u)
{
	// 表示線性數位系統的狀態空間表示式
	// X[k+1] = AX[k] + Bu[k]
	// y[k] = CX[k] + Du[k]

	double sum = 0.0;
		
	for(int i=0; i < Order; i++)
	{
		for(int j=0; j < Order; j++)
			sum += (A[i][j] * X[j]);

		Next_X[i] = sum + B[i]*Input_u;
			
		sum = 0.0;
	}
	

	double y = 0.0;

	for(int i=0; i < Order; i++)
		y += C[i]*X[i];
	
	y += D*Input_u;

	
	for(int i=0; i < Order; i++)
			X[i] = Next_X[i];

	return y;
}

#pragma endregion


#pragma region 中間值濾波器

Middle_Filter::Middle_Filter()
{
	Value = NULL;
	Sort_Value = NULL;
	Array_Size = 0;
}

Middle_Filter::~Middle_Filter()
{
	delete[] Value;
	delete[] Sort_Value;
}

void Middle_Filter::Set_ArraySize(unsigned int size)
{
	if(Value != NULL || Sort_Value != NULL)
	{	
		delete[] Value;
		delete[] Sort_Value;
	}

	Array_Size = (size%2 == 1)?(size):(size+1);
	Value = new int[Array_Size];
	Sort_Value = new int[Array_Size];


	// 初始值預設為0
	for(int i=0; i < Array_Size; i++)
		Value[i] = 0;
}

int Middle_Filter::Output_y(int Input_u)
{
	//搬移前一刻的值
	for(int i=0; i < (Array_Size-1); i++)
	{	
		Value[i] = Value[i+1];
		Sort_Value[i] = Value[i];
	}
	Value[Array_Size-1] = Input_u;
	Sort_Value[Array_Size-1] = Value[Array_Size-1];

	// 排序
	std::sort(Sort_Value, Sort_Value + Array_Size);
	
	// 回傳中間值
	return Sort_Value[Array_Size/2];
}

#pragma endregion
