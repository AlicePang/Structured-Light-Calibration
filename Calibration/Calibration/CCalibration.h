#ifndef _CCALIBRATION_H_
#define _CCALIBRATION_H_

#include <opencv2/opencv.hpp>

#include "CSensor.h"
#include "CDecodeGray.h"
#include "CDecodePhase.h"
#include "CVisualization.h"
#include "GlobalFunction.h"
#include "StaticParameters.h"
using namespace cv;
using namespace std;

// �궨�ࡣ���ڱ궨����ͷ��ͶӰ�ǡ�
// ���л���������ͷ��ͶӰ�ǵĿ��ƣ��������������������н��롣
class CCalibration
{
private:
	// ������������
	CSensor * m_sensor;
	string m_patternPath;
	// ������
	CDecodeGray * m_decodeGray;	// ���������
	CDecodePhase * m_decodePS;		// PS����
	// ���̸��X��Y���Լ�ͼƬ��Ŀ
	int m_chessLine;
	int m_chessRow;
	int m_chessNum;
	// �ɼ��������̸�ͼ���ʶ����
	Mat m_chessMat;
	Mat * m_grayV;
	Mat * m_grayH;
	Mat * m_phaseV;
	Mat * m_phaseH;
	Mat * xpro_mats_;
	Mat * ypro_mats_;
	Mat m_chessMatDraw;
	Mat m_proMatDraw;
	//���̸�˳����
	vector<vector<Point2f>> m_camPoint;
	vector<Point2f> m_camPointTmp;
	vector<vector<Point2f>> m_proPoint;
	vector<Point2f> m_proPointTmp;
	vector<vector<Point3f>> m_objPoint;
	vector<Point3f> m_objPointTmp;
	// �궨�ľ���
	Mat m_camMatrix;
	Mat m_camDisCoeffs;
	Mat m_proMatrix;
	Mat m_proDisCoeffs;
	Mat m_R;
	Mat m_T;
	Mat m_E;
	Mat m_F;

	bool ReleaseSpace();			// �ͷſռ�
	bool RecoChessPointObj(int frameIdx);		// ���m_objPoint
	bool RecoChessPointCam(int frameIdx);		// ���m_camPoint
	bool RecoChessPointPro(int frameIdx);		// ���m_proPoint
	bool PushChessPoint(int frameIdx);			// �������̸񣬴洢������
public:
	CCalibration();
	~CCalibration();
	bool Init();				// ��ʼ��
	bool Calibrate();			// �궨��������
	bool Result();
};

#endif