#include <stdio.h>
#include  <stdlib.h>

#include "Robot_Arm.h"

int main()
{
	Robot_Arm My_Arm(5);

	double theta[5] = {-90, 0, 90, -90, -90};

	My_Arm.Set_Ini_Theta(theta);
	
	My_Arm.Refresh_TFMatrix(theta);

	std::cout << "T = \n" << My_Arm.T_BaseEnd << std::endl;

	std::cout << "P = \n" << My_Arm.P << std::endl;

	system("PAUSE");
	return 1;
}
