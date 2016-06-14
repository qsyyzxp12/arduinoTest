#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "/usr/include/Eigen_3.2.7/Eigen/Dense"


#define pi 3.14159265359
#define Deg2Rad(theta) (theta*pi/180) 
#define Rad2Deg(theta) (theta*180/pi)


class Robot_Arm
{
private:
	int Robot_DOF;

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

	// 設定自由度
	Robot_Arm(int DOF);


	// 解構子
	~Robot_Arm();

	int Get_RobotDOF();

	// 設定 Global Frame 轉到 Base Frame 的 TF_Matrix，預設是Identity
	void Set_Base2Globalframe(Eigen::Matrix4d& T);

	// 設定初始角度(deg)
	void Set_Ini_Theta(double *Ini_theta);


	// 產生TransFormation Matrix，單位(mm, deg, mm, deg)
	Eigen::Matrix4d DH2TransforMatrix(double a, double alpha, double d, double theta);

	void DH2TransforMatrix(double a, double alpha, double d, double theta, Eigen::Matrix4d& T);

	
	// 更新 所有自由度中theta值(deg) ==> 更新TF Matrix, P
	void Refreah_TFMatrix(double *New_theta);

	
	// 得到現在末端點相對Base的TF Matrix
	Eigen::Matrix4d T_BaseEnd;

	// 將現在末端點的TF Matrix 等效成 [Position Orientation]，以Base frame表示
	Eigen::VectorXd P;
};