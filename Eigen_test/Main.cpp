#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include  <stdlib.h>

#include "Robot_Arm.h"

int main()
{
	Robot_Arm My_Arm;

	int Raw_Theta[5] = {0, 550, 930, 495, 880};
	
	Eigen::Matrix4d T_Base2Global;

	T_Base2Global << 0, 0, -1, 0,
					 -1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 0, 1;

	My_Arm.Set_Base2Global(T_Base2Global);

	My_Arm.Set_Ini_Theta( My_Arm.RawTheta2Deg(Raw_Theta) );
	
	//My_Arm.Refreah_TFMatrix(theta);

	std::cout << "T = \n" << My_Arm.T_BaseEnd << std::endl;

	std::cout << "P = \n" << My_Arm.P << std::endl;

	printf(My_Arm.Get_TransData());

	

	system("PAUSE");
	return 1;
}
