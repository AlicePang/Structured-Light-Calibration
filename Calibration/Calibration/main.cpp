#include "head.h"

using namespace cv;
using namespace std;

int main()
{
	// �����ࣺ
	CCalibration myCalibration;		// �궨��
	myCalibration.Init();
	myCalibration.Calibrate();
	myCalibration.Result();
	system("PAUSE");
	return 0;
}