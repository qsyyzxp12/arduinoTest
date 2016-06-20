#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <vector>
#include "/usr/include/Eigen_3.2.7/Eigen/Dense"


#define pi 3.14159265359
#define Deg2Rad(theta) (theta*pi/180) 
#define Rad2Deg(theta) (theta*180/pi)


class Robot_Arm
{
private:
	int Robot_DOF;

	// 存放可變電阻轉換的值，單位deg
	double Theta[5];

	// 存放藍芽傳輸的資料
	char TransData[31];

	// 從Global Frame 轉到 Base Frame ==> 預設是 Identity Matrix
	Eigen::Matrix4d T_Base2Global;
	
	bool Set_Ini_Theta_Flag;

	// 儲存初始狀態末端點的[Position Orientation]，以Base frame表示
	Eigen::VectorXd Ini_P;

	std::vector<Eigen::Matrix4d> TransFormation_Matrix;

	// 從Rotation Matrix中獲得手臂的Orientation
	Eigen::Vector3d Get_Orientation(Eigen::Matrix3d& R);

	// 從Transformation Matrix中獲得手臂的[Position Orientation]
	void Get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & _P);

public:
	// 建構子
	Robot_Arm();

	// 解構子
	~Robot_Arm();

	// 獲得 Robot 自由度
	int Get_RobotDOF();

	// 設定 Global Frame 轉到 Base Frame 的 TF_Matrix，預設是Identity
	void Set_Base2Global(Eigen::Matrix4d& T);

	// 設定初始角度(deg)
	void Set_Ini_Theta(double *Ini_theta);


	// 產生TransFormation Matrix，單位(mm, deg, mm, deg)
	Eigen::Matrix4d DH2TransforMatrix(double a, double alpha, double d, double theta);

	// 更新TransFormation Matrix，單位(mm, deg, mm, deg)
	void DH2TransforMatrix(double a, double alpha, double d, double theta, Eigen::Matrix4d& T);

	// 更新 所有自由度中theta值(deg) ==> 更新TF Matrix, P
	void Refresh_TFMatrix(double *New_theta);

	// 轉換可變電阻值到定義的角度(deg)
	double* RawTheta2Deg(int *Raw_Theta);

	// 獲得藍芽傳輸的資料，+XXXX+YYYY+ZZZZ+xxxx+yyyy+zzzz'\n' => 共31Bytes
	char* Get_TransData();
	
	// 得到現在末端點相對Base的TF Matrix
	Eigen::Matrix4d T_BaseEnd;

	// 將現在末端點的TF Matrix 等效成 [Position Orientation]，以Base frame表示
	Eigen::VectorXd P;
};



#pragma region Transformation Matrix 與 P 之間互轉

namespace Quaternion
{

	static Eigen::Vector3d get_Orientation(Eigen::Matrix3d& R)
	{
		Eigen::Vector3d k;

		double theta = acos( (R(0,0)+R(1,1)+R(2,2)-1)/2 );

		// theta = 0 (deg) ==> singular
		if(abs(theta) <= 0.0000001)
		{
			k << 0, 0, 0;
		}
		else if(abs(theta-pi) <= 0.0000001) // theta = 180 (deg) ==> singular
		{
			Eigen::Matrix3d temp = (R + Eigen::Matrix3d::Identity()) / 2;
		
			// 令 kx >= 0
			k(0) = sqrt(temp(0,0));

			if(k(0) == 0)
			{
				if(temp(2,1) > 0) // ky*kz > 0
				{
					k(1) = sqrt(temp(1,1)); //ky
					k(2) = sqrt(temp(2,2)); //kZ
				}
				else
				{
					k(1) = sqrt(temp(1,1)); //ky
					k(2) = -sqrt(temp(2,2)); //kZ
				}
			}
			else  // kx > 0
			{
				if(temp(1,0) > 0) // kx*ky > 0
					k(1) = sqrt(temp(1,1)); //ky
				else
					k(1) = -sqrt(temp(1,1)); //ky


				if(temp(2,0) > 0) // kx*kz > 0
					k(2) = sqrt(temp(2,2)); //kz
				else
					k(2) = -sqrt(temp(2,2)); //kz
			}
		}
		else
		{
			k << R(2,1)-R(1,2), R(0,2)-R(2,0), R(1,0)-R(0,1);
			k = k / (2*sin(theta));
		}

		return theta*k;
	};


	// 從Transformation Matrix 獲得 P = [Position, Orientation]
	static void get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & P)
	{
		Eigen::Matrix3d R = T.topLeftCorner(3,3);

		P << T(0,3), T(1,3), T(2,3), get_Orientation(R);
	};

	// 從 P = [Position, Orientation] 轉換成 Transformation Matrix
	static void get_Robot_TFMatrix(Eigen::VectorXd & P, Eigen::Matrix4d& T)
	{
		Eigen::Vector3d k = P.bottomLeftCorner(3,1);
		
		double theta = k.norm();
		
		k = k / theta; //變單位向量

		Eigen::Matrix3d temp;
		temp << 0, -k(2), k(1),
				k(2), 0, -k(0),
				-k(1), k(0), 0;

		Eigen::Matrix3d R = cos(theta)*Eigen::Matrix3d::Identity() + (1-cos(theta))*k*k.transpose() + sin(theta)*temp;


		T << R, P.topLeftCorner(3,1),
			0, 0, 0, 1;
	};

};
#pragma endregion
