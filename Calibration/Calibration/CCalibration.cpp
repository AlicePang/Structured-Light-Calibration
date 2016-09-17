#include "head.h"
#include <direct.h>
#include <strstream>
using namespace cv;
using namespace std;

CVisualization myCamera("Camera");
//CVisualization myDebug("Debug");
CVisualization myProjector("Projector");

// ���캯������һЩ��������ֵ
CCalibration::CCalibration()
{
	this->m_sensor = NULL;
	this->m_decodeGray = NULL;
	this->m_decodePS = NULL;
	this->m_grayV = NULL;
	this->m_grayH = NULL;
	this->m_phaseV = NULL;
	this->m_phaseH = NULL;
}

// ����������ȷ�����пռ��Ѿ��ͷš�
CCalibration::~CCalibration()
{
	this->ReleaseSpace();
}

// �ͷſռ䡣��ȫ���ͷ�ָ��Ŀռ䡣
bool CCalibration::ReleaseSpace()
{
	if (this->m_sensor != NULL)
	{
		delete(this->m_sensor);
		this->m_sensor = NULL;
	}
	if (this->m_decodeGray != NULL)
	{
		delete[](this->m_decodeGray);
		this->m_decodeGray = NULL;
	}
	if (this->m_decodePS != NULL)
	{
		delete[](this->m_decodePS);
		this->m_decodePS = NULL;
	}
	if (this->m_grayV != NULL)
	{
		delete[](this->m_grayV);
		this->m_grayV = NULL;
	}
	if (this->m_grayH != NULL)
	{
		delete[](this->m_grayH);
		this->m_grayH = NULL;
	}
	if (this->m_phaseV != NULL)
	{
		delete[](this->m_phaseV);
		this->m_phaseV = NULL;
	}
	if (this->m_phaseH!= NULL)
	{
		delete[](this->m_phaseH);
		this->m_phaseH = NULL;
	}

	return true;
}

// ��ʼ��
bool CCalibration::Init()
{
	if ((this->m_sensor != NULL))
		return false;
	if ((this->m_decodeGray != NULL) || (this->m_decodePS != NULL))
		return false;

	this->m_sensor = new CSensor;
	this->m_decodeGray = new CDecode_Gray;
	this->m_decodePS = new CDecode_Phase;
	this->m_chessLine = 9;
	this->m_chessRow = 6;
	this->m_chessNum = CHESS_FRAME_NUMBER;

	this->m_grayV = new Mat[GRAY_V_NUMDIGIT * 2];
	this->m_grayH = new Mat[GRAY_H_NUMDIGIT * 2];
	this->m_phaseV = new Mat[PHASE_NUMDIGIT];
	this->m_phaseH = new Mat[PHASE_NUMDIGIT];

	return true;
}

// �궨������
bool CCalibration::Calibrate()
{
	bool status = true;

	// �жϲ����Ƿ�Ϸ�
	if ((this->m_sensor == NULL))
		return false;
	if ((this->m_decodeGray == NULL) || (this->m_decodePS == NULL))
		return false;

	// ��ʼ�����������Vector
	std::vector<std::vector<cv::Point2f>>().swap(this->m_camPoint);
	std::vector<std::vector<cv::Point2f>>().swap(this->m_proPoint);
	std::vector<std::vector<cv::Point3f>>().swap(this->m_objPoint);
	//CVisualization visual("Result", 100);

	// ʶ�����̸񣬼������������Vector
	for (int i = 0; i < this->m_chessNum; i++)
	{
		// ���ObjPoint��
		status = this->RecoChessPointObj(i);
		printf("For %dth picture: ObjPoint finished.\n", i+1);

		// ���CamPoint��
		status = this->RecoChessPointCam(i);
		printf("For %dth picture: CamPoint finished.\n", i+1);

		// ���ProPoint��
		status = this->RecoChessPointPro(i);
		printf("For %dth picture: ProPoint finished.\n", i+1);

		// �ж�ʶ�����Ƿ���ȷ
		int key = 0;
		key = myCamera.Show(this->m_chessMatDraw, 100, false, 0.5);
		key = myProjector.Show(this->m_proMatDraw, 0, false, 0.5);
		if (key == 'c')
		{
			status = this->PushChessPoint(i);
		}
		else
		{
			i = i - 1;
			continue;
		}

		//visual.Show(this->m_chessMat);
		printf("Finish %dth picture.\n", i+1);
	}

	// �궨����
	printf("Begin Calibrating.\n");
	calibrateCamera(this->m_objPoint,
		this->m_camPoint,
		Size(CAMERA_RESLINE, CAMERA_RESROW),
		this->m_camMatrix,
		this->m_camDisCoeffs,
		noArray(),
		noArray(),
		CALIB_FIX_K3 + CALIB_FIX_PRINCIPAL_POINT,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish Camera.\n");
	calibrateCamera(this->m_objPoint,
		this->m_proPoint,
		Size(PROJECTOR_RESLINE, PROJECTOR_RESROW),
		this->m_proMatrix,
		this->m_proDisCoeffs,
		noArray(),
		noArray(),
		CALIB_FIX_K3,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish Projector.\n");
	stereoCalibrate(this->m_objPoint,
		this->m_camPoint,
		this->m_proPoint,
		this->m_camMatrix,
		this->m_camDisCoeffs,
		this->m_proMatrix,
		this->m_proDisCoeffs,
		Size(CAMERA_RESLINE, CAMERA_RESROW),
		this->m_R,
		this->m_T,
		this->m_E,
		this->m_F,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish stereo Calibration.\n");

	return true;
}

// �����������
bool CCalibration::Result()
{
	FileStorage fs("Result.yml", FileStorage::WRITE);
	cout << "CamMat" << endl;
	fs << "CamMat" << this->m_camMatrix;
	cout << this->m_camMatrix << endl;
	
	cout << "ProMat" << endl;
	fs << "ProMat" << this->m_proMatrix;
	cout << this->m_proMatrix << endl;

	cout << "R,T" << endl;
	fs << "R" << this->m_R;
	fs << "T" << this->m_T;
	cout << this->m_R << endl;
	cout << this->m_T << endl;
	fs.release();
	return true;
}

// ʶ��Obj�е����̸����
bool CCalibration::RecoChessPointObj(int frameIdx)
{
	vector<Point3f>().swap(this->m_objPointTmp);
	// �����ά���̸�����
	for (int i = 0; i < this->m_chessLine; i++)
	{
		for (int j = 0; j < this->m_chessRow; j++)
		{
			this->m_objPointTmp.push_back(Point3f(i, j, 0));
		}
	}
	
	return true;
}

// ʶ��Cam�е����̸����
bool CCalibration::RecoChessPointCam(int frameIdx)
{
	bool status = true;
	vector<Point2f>().swap(this->m_camPointTmp);
	this->m_sensor->SetChessFrame(frameIdx);

	// ����ʶ��ǵ�
	while (true)
	{
		// ��ȡ��ǰ���֡
		Mat camMat;
		camMat = this->m_sensor->GetCamFrame();
		camMat.copyTo(this->m_chessMat);
		camMat.copyTo(this->m_chessMatDraw);

		// Ѱ�����̸�
		int maxAttempt = 10;
		int k = 0;
		int found = 0;
		while (k++ < maxAttempt)
		{
			found = findChessboardCorners(this->m_chessMat, Size(this->m_chessRow, this->m_chessLine), this->m_camPointTmp, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
			drawChessboardCorners(this->m_chessMatDraw, Size(this->m_chessRow, this->m_chessLine), this->m_camPointTmp, found);
			if (found)
			{
				break;
			}
		}

		// ����ɹ�Ѱ�ң���������
		if (k < maxAttempt)
		{
			cornerSubPix(camMat,
				this->m_camPointTmp,
				Size(5, 5),
				Size(-1, -1),
				TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS,
				30,
				0.1));
			break;
		}
	}

	return status;
}

// ʶ��Pro�е����̸����
bool CCalibration::RecoChessPointPro(int frameIdx)
{
	bool status = true;
	vector<Point2f>().swap(this->m_proPointTmp);
	this->m_sensor->SetChessFrame(frameIdx);

	// ���ݽṹ����������Ұ��ÿ�����ͶӰ������
	Mat tempMat;
	Mat vGrayMat;
	Mat hGrayMat;
	Mat vPhaseMat;
	Mat hPhaseMat;
	Mat vProjectorMat;
	Mat hProjectorMat;
	// Gray_v
	this->m_decodeGray->SetNumDigit(GRAY_V_NUMDIGIT, true);
	this->m_decodeGray->SetMatFileName("Projector/Gray_v/", "GrayCode.txt");
	for (int i = 0; i < GRAY_V_NUMDIGIT * 2; i++)		// Projector
	{
		tempMat = this->m_sensor->GetProFrame(0, i);//Gray_v��ͼƬ���Ϊ0
		this->m_decodeGray->SetMat(i, tempMat);
	}
	this->m_decodeGray->Decode();
	vGrayMat = this->m_decodeGray->GetResult();

	// Gray_h
	this->m_decodeGray->SetNumDigit(GRAY_H_NUMDIGIT, false);
	this->m_decodeGray->SetMatFileName("Projector/Gray_h/", "GrayCode.txt");
	for (int i = 0; i < GRAY_H_NUMDIGIT * 2; i++)		// Projector
	{
		tempMat = this->m_sensor->GetProFrame(1, i);//Gray_h��ͼƬ���Ϊ1
		this->m_decodeGray->SetMat(i, tempMat);
	}
	this->m_decodeGray->Decode();
	hGrayMat = this->m_decodeGray->GetResult();

	// Phase_v
	int v_pixPeriod = PROJECTOR_RESLINE / (1 << (GRAY_V_NUMDIGIT - 1));
	this->m_decodePS->SetNumMat(PHASE_NUMDIGIT, v_pixPeriod);
	tempMat = this->m_sensor->GetCamFrame();
	for (int i = 0; i < PHASE_NUMDIGIT; i++)
	{
		tempMat = this->m_sensor->GetProFrame(2, i);//Phase_v��ͼ����Ϊ2
		this->m_decodePS->SetMat(i, tempMat);
	}
	this->m_decodePS->Decode();
	vPhaseMat = this->m_decodePS->GetResult();

	// Phase_h
	int h_pixPeriod = PROJECTOR_RESROW / (1 << (GRAY_H_NUMDIGIT - 1));
	this->m_decodePS->SetNumMat(PHASE_NUMDIGIT, h_pixPeriod);
	tempMat = this->m_sensor->GetCamFrame();
	for (int i = 0; i < PHASE_NUMDIGIT; i++)
	{
		tempMat = this->m_sensor->GetProFrame(3, i);//Phase_h��ͼ����Ϊ3
		this->m_decodePS->SetMat(i, tempMat);
	}
	this->m_decodePS->Decode();
	hPhaseMat = this->m_decodePS->GetResult();

	// �ϲ�


	//vProjectorMat = vGrayMat + vPhaseMat;
	//hProjectorMat = hGrayMat + hPhaseMat;
	//myDebug.Show(vProjectorMat, 00, true);
	//myDebug.Show(hProjectorMat, 00, true);

	// ������������ת��ΪͶӰ�Ǹ������
	vector<Point2f>::iterator i;
	for (i = this->m_camPointTmp.begin(); i != this->m_camPointTmp.end(); ++i)
	{
		Point2f cam = *i;
		Point2f pro;
		int X = cam.x;
		int Y = cam.y;
		pro.x = vProjectorMat.at<ushort>(Y, X);
		pro.y = hProjectorMat.at<ushort>(Y, X);
		this->m_proPointTmp.push_back(pro);
		//cout << "(" << X << "," << Y << ") -> (" << pro.x << "," << pro.y << ")" << endl;
	}

	// ����ͶӰ��ת����ĸ������
	this->m_proMatDraw.create(800, 1280, CV_8UC1);
	this->m_proMatDraw.setTo(0);
	drawChessboardCorners(this->m_proMatDraw, Size(this->m_chessRow, this->m_chessLine), this->m_proPointTmp, true);

	return status;
}

// �������̸񣬴洢������
bool CCalibration::PushChessPoint(int frameIdx)
{
	bool status = true;

	// �������̸�
	this->m_objPoint.push_back(this->m_objPointTmp);
	this->m_camPoint.push_back(this->m_camPointTmp);
	this->m_proPoint.push_back(this->m_proPointTmp);

	// �洢����ͼ��
	strstream ss;
	string IdxtoStr;
	ss << frameIdx;
	ss >> IdxtoStr;

	// ���ͼ��
	CStorage camChessMat;
	camChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "CameraMat", ".bmp");
	camChessMat.Store(&(this->m_chessMat), 1);
	camChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "CameraMatDraw", ".bmp");
	camChessMat.Store(&(this->m_chessMatDraw), 1);

	// ͶӰ��ͼ��
	CStorage proChessMat;
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "vGray", ".bmp");
	proChessMat.Store(this->m_grayV, GRAY_V_NUMDIGIT * 2);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "hGray", ".bmp");
	proChessMat.Store(this->m_grayV, GRAY_H_NUMDIGIT * 2);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "vPhase", ".bmp");
	proChessMat.Store(this->m_grayV, PHASE_NUMDIGIT);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "hPhase", ".bmp");
	proChessMat.Store(this->m_grayV, PHASE_NUMDIGIT);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "ProMatDraw", ".bmp");
	proChessMat.Store(&(this->m_proMatDraw), 1);

	return status;
}