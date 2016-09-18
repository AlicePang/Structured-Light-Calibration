#ifndef _CSENSOR_H_
#define _CSENSOR_H_

#include "head.h"

#include <string>
#include <opencv2/opencv.hpp>
#include <strstream>

using namespace std;
using namespace cv;

// ������ģ�顣ģ��
class CSensor
{
private:
	int m_frameNum;					// �궨�����̸�֡����
	int m_ProPicNum[4];				// ������Pattern���ܼ�ͼ����

	string m_chessFilePath;	// �洢�ļ�����·��
	string m_chessFileFrameNum;	// ��ǰ�궨��֡��
	string m_chessFileCamPath;	// ��������궨���ļ�·��
	string m_chessFileCamName;	// ��������궨���ļ���
	string m_chessFileProPath[4];	// ����ͶӰ�Ǳ궨���ļ�·��
	string m_chessFileProName[4];	// ����ͶӰ�Ǳ궨���ļ���
	string m_chessFileSuffix;	// �ļ���׺��

public:
	CSensor();
	~CSensor();
	bool SetChessFrame(int frame);	// �趨��ǰ�궨��֡��	
	Mat GetCamFrame();			// ��ȡ���ͼ��
	int GetPicNum(int patternIdx);	// ��ȡͶӰ��ͼ��������
	Mat GetProFrame(int patternIdx, int picIdx);	// ��ȡͶӰ��ͼ��
};

#endif