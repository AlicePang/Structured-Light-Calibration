#include "head.h"

using namespace cv;
using namespace std;

int ErrorHandling(std::string message)
{
	cout << "An Error Occurs:" << message << endl;
	system("PAUSE");
	return 0;
}

int main()
{
	// �����ࣺ
	CCalibration myCalibration;		// �궨��
	myCalibration.Init();
	myCalibration.Calibrate();
	myCalibration.Result();
	system("PAUSE");
	return 0;

	/*CCalibration calibration;
	calibration.Init();
	calibration.Calibrate();*/
	/*CCamera Camera;
	bool Running = true;
	if (Running)
	{
		printf("Begin Initalizing Camera...\n");
		Running = Camera.InitCamera();
	}
	if (Running)
	{
		printf("Begin Capture Camera Shot...\n");
		Mat M;
		while (true)
		{
			Running = Camera.getPicture(M);
			if (Running)
			{
				namedWindow("CameraImage");
				imshow("CameraImage", M);
				waitKey(10);
			}
		}
	}
	if (Running)
	{
		printf("Begin Closing Camera...\n");
		Running = Camera.CloseCamera();
	}*/

	system("PAUSE");
	return 0;
}