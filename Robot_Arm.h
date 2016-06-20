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

	// �s��i�ܹq���ഫ���ȡA���deg
	double Theta[5];

	// �s���Ū޶ǿ骺���
	char TransData[31];

	// �qGlobal Frame ��� Base Frame ==> �w�]�O Identity Matrix
	Eigen::Matrix4d T_Base2Global;
	
	bool Set_Ini_Theta_Flag;

	// �x�s��l���A�����I��[Position Orientation]�A�HBase frame���
	Eigen::VectorXd Ini_P;

	std::vector<Eigen::Matrix4d> TransFormation_Matrix;

	// �qRotation Matrix����o���u��Orientation
	Eigen::Vector3d Get_Orientation(Eigen::Matrix3d& R);

	// �qTransformation Matrix����o���u��[Position Orientation]
	void Get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & _P);

public:
	// �غc�l
	Robot_Arm();

	// �Ѻc�l
	~Robot_Arm();

	// ��o Robot �ۥѫ�
	int Get_RobotDOF();

	// �]�w Global Frame ��� Base Frame �� TF_Matrix�A�w�]�OIdentity
	void Set_Base2Global(Eigen::Matrix4d& T);

	// �]�w��l����(deg)
	void Set_Ini_Theta(double *Ini_theta);


	// ����TransFormation Matrix�A���(mm, deg, mm, deg)
	Eigen::Matrix4d DH2TransforMatrix(double a, double alpha, double d, double theta);

	// ��sTransFormation Matrix�A���(mm, deg, mm, deg)
	void DH2TransforMatrix(double a, double alpha, double d, double theta, Eigen::Matrix4d& T);

	// ��s �Ҧ��ۥѫפ�theta��(deg) ==> ��sTF Matrix, P
	void Refresh_TFMatrix(double *New_theta);

	// �ഫ�i�ܹq���Ȩ�w�q������(deg)
	double* RawTheta2Deg(int *Raw_Theta);

	// ��o�Ū޶ǿ骺��ơA+XXXX+YYYY+ZZZZ+xxxx+yyyy+zzzz'\n' => �@31Bytes
	char* Get_TransData();
	
	// �o��{�b�����I�۹�Base��TF Matrix
	Eigen::Matrix4d T_BaseEnd;

	// �N�{�b�����I��TF Matrix ���Ħ� [Position Orientation]�A�HBase frame���
	Eigen::VectorXd P;
};



#pragma region Transformation Matrix �P P ��������

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
		
			// �O kx >= 0
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


	// �qTransformation Matrix ��o P = [Position, Orientation]
	static void get_Robot_PosOri(Eigen::Matrix4d& T, Eigen::VectorXd & P)
	{
		Eigen::Matrix3d R = T.topLeftCorner(3,3);

		P << T(0,3), T(1,3), T(2,3), get_Orientation(R);
	};

	// �q P = [Position, Orientation] �ഫ�� Transformation Matrix
	static void get_Robot_TFMatrix(Eigen::VectorXd & P, Eigen::Matrix4d& T)
	{
		Eigen::Vector3d k = P.bottomLeftCorner(3,1);
		
		double theta = k.norm();
		
		k = k / theta; //�ܳ��V�q

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
