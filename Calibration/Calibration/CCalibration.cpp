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
}

// ����������ȷ�����пռ��Ѿ��ͷš�
CCalibration::~CCalibration()
{
	this->ReleaseSpace();
}

// �ͷſռ䡣��ȫ���ͷ��ĸ�ָ��Ŀռ䡣
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
	vector<Point3f> objPoint;
	vector<Point3f>().swap(objPoint);
	// �����ά���̸�����
	for (int i = 0; i < this->m_chessLine; i++)
	{
		for (int j = 0; j < this->m_chessRow; j++)
		{
			objPoint.push_back(Point3f(i, j, 0));
		}
	}
	this->m_objPoint.push_back(objPoint);
	return true;
}

// ʶ��Cam�е����̸����
bool CCalibration::RecoChessPointCam(int frameIdx)
{
	bool status = true;

	// ������Ҫ��Storage��Visualization����ʼ��Sensor��
	// ������ʱ��Vector
	strstream ss;
	string IdxtoStr;
	ss << frameIdx;
	ss >> IdxtoStr;
	CStorage recoChessPoint;	// ���ʶ��������̸�
	recoChessPoint.SetMatFileName("RecoChessPoint/", "RecoChessPointCam" + IdxtoStr, ".bmp");
	this->m_sensor->SetChessFrame(frameIdx);
	vector<Point2f> camPoint;
	vector<Point2f>().swap(camPoint);

	// ��ȡ���֡
	Mat camMat;
	camMat = this->m_sensor->GetCamFrame();
	camMat.copyTo(this->m_chessMat);

	// �������̸�
	int maxAttempt = 10;
	int k = 0;
	while (k++ < maxAttempt)
	{
		// ���Ҳ�����
		status = findChessboardCorners(camMat, Size(this->m_chessRow, this->m_chessLine), camPoint, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
		drawChessboardCorners(this->m_chessMat, Size(this->m_chessRow, this->m_chessLine), camPoint, status);
		if (status)
			break;
	}
	myCamera.Show(this->m_chessMat, 300);
	//imwrite("1.bmp", this->m_chessMat);
	if (k >= maxAttempt)
	{
		ErrorHandling("Calibration.RecoChessPoints->findChessboradCorners Error");
		return false;
	}
	
	// ��һ����ȷ
	cornerSubPix(camMat,
		camPoint,
		Size(5, 5),
		Size(-1, -1),
		TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS,
			30,
			0.1));
	
	// �洢���
	this->m_camPoint.push_back(camPoint);
	recoChessPoint.Store(&(this->m_chessMat), 1);

	return true;
}

// ʶ��Pro�е����̸����
bool CCalibration::RecoChessPointPro(int frameIdx)
{
	bool status = true;

	// ������Ҫ��Storage
	strstream ss;
	string IdxtoStr;
	ss << frameIdx;
	ss >> IdxtoStr;
	CStorage vGrayStore;
	CStorage hGrayStore;
	CStorage vPhaseStore;
	CStorage hPhaseStore;
	CStorage vProjectorStore;
	CStorage hProjectorStore;
	vGrayStore.SetMatFileName("Decode/" + IdxtoStr + "/", "vGray", ".bmp");		// ���phaseʶ����
	hGrayStore.SetMatFileName("Decode/" + IdxtoStr + "/", "hGray", ".bmp");
	vPhaseStore.SetMatFileName("Decode/" + IdxtoStr + "/", "vPhase", ".bmp");
	hPhaseStore.SetMatFileName("Decode/" + IdxtoStr + "/", "hPhase", ".bmp");
	vProjectorStore.SetMatFileName("Decode/" + IdxtoStr + "/", "vProjector", ".bmp");
	hProjectorStore.SetMatFileName("Decode/" + IdxtoStr + "/", "hProjector", ".bmp");
	
	// ���ô����������Ӧ֡����Ϣ
	this->m_sensor->SetChessFrame(frameIdx);
	
	// ������ʱ��Vector
	vector<Point2f> proPoint;
	vector<Point2f>().swap(proPoint);

	// ��ȡ����������
	vector<Point2f> camPoint = this->m_camPoint.back();
	Mat tempMat;
	Mat vGrayMat;
	Mat hGrayMat;
	Mat vPhaseMat;
	Mat hPhaseMat;
	Mat vProjectorMat;
	Mat hProjectorMat;

	// ���ݽṹ����������Ұ��ÿ�����ͶӰ������
	// Gray_v
	this->m_decodeGray->SetNumDigit(GRAY_V_NUMDIGIT, true);
	this->m_decodeGray->SetMatFileName("Projector/Gray_v/", "GrayCode.txt");
	tempMat = this->m_sensor->GetCamFrame();	// Camera
	this->m_decodeGray->SetCamMat(tempMat);
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
	tempMat = this->m_sensor->GetCamFrame();	// Camera
	this->m_decodeGray->SetCamMat(tempMat);
	for (int i = 0; i < GRAY_H_NUMDIGIT * 2; i++)		// Projector
	{
		tempMat = this->m_sensor->GetProFrame(1, i);//Gray_h��ͼƬ���Ϊ1
		this->m_decodeGray->SetMat(i, tempMat);
	}
	this->m_decodeGray->Decode();
	hGrayMat = this->m_decodeGray->GetResult();

	// Phase_v
	int v_pixPeriod = PROJECTOR_RESLINE / (1 << GRAY_V_NUMDIGIT);
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
	int h_pixPeriod = PROJECTOR_RESROW / (1 << GRAY_H_NUMDIGIT);
	this->m_decodePS->SetNumMat(PHASE_NUMDIGIT, h_pixPeriod);
	tempMat = this->m_sensor->GetCamFrame();
	for (int i = 0; i < PHASE_NUMDIGIT; i++)
	{
		tempMat = this->m_sensor->GetProFrame(3, i);//Phase_h��ͼ����Ϊ2
		this->m_decodePS->SetMat(i, tempMat);
	}
	this->m_decodePS->Decode();
	hPhaseMat = this->m_decodePS->GetResult();

	// �ϲ�
	vProjectorMat = vGrayMat + vPhaseMat;
	hProjectorMat = hGrayMat + hPhaseMat;
	//myDebug.Show(vProjectorMat, 00, true);
	//myDebug.Show(hProjectorMat, 00, true);

	// ������������ת��ΪͶӰ�Ǹ������
	vector<Point2f>::iterator i;
	for (i = camPoint.begin(); i != camPoint.end(); ++i)
	{
		Point2f cam = *i;
		Point2f pro;
		int X = cam.x;
		int Y = cam.y;
		pro.x = vProjectorMat.at<ushort>(Y, X);
		pro.y = hProjectorMat.at<ushort>(Y, X);
		proPoint.push_back(pro);
		//cout << "(" << X << "," << Y << ") -> (" << pro.x << "," << pro.y << ")" << endl;
	}
	Mat tempPro;
	tempPro.create(800, 1280, CV_8UC1);
	tempPro.setTo(0);
	drawChessboardCorners(tempPro, Size(this->m_chessRow, this->m_chessLine), proPoint, true);
	Mat tempShow;
	resize(tempPro, tempShow, Size(CAMERA_RESLINE, CAMERA_RESROW));
	myProjector.Show(tempShow, 00);
	//imwrite("2.bmp", tempShow);
	this->m_proPoint.push_back(proPoint);

	// �洢
	vGrayStore.Store(&vGrayMat, 1);
	hGrayStore.Store(&hGrayMat, 1);
	vPhaseStore.Store(&vPhaseMat, 1);
	hPhaseStore.Store(&hPhaseMat, 1);
	vProjectorStore.Store(&vProjectorMat, 1);
	hProjectorStore.Store(&hProjectorMat, 1);

	return true;
}