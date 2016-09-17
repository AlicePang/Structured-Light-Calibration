// OpenCV
#include <opencv2/opencv.hpp>

// �������ÿ�
#include <vector>
#include <direct.h>

#define VISUAL_DEBUG false
#define CHESS_FRAME_NUMBER 15

// һЩ����
static int PROJECTOR_RESLINE = 1280;
static int PROJECTOR_RESROW = 800;
static int CAMERA_RESLINE = 1280;	// 1280
static int CAMERA_RESROW = 1024;	// 1024
static int PC_BIASLINE = 1366;
static int PC_BIASROW = 0;
static int GRAY_V_NUMDIGIT = 6;
static int GRAY_H_NUMDIGIT = 5;
static int PHASE_NUMDIGIT = 4;
static int SHOW_PICTURE_TIME = 500;		// Ĭ��VisualͼƬ�ĳ���ʱ��

// ��������
int ErrorHandling(std::string message);

// ������ģ�顣ģ��
class CSensor
{
private:
	int m_frameNum;					// �궨�����̸�֡����
	int m_ProPicNum[4];				// ������Pattern���ܼ�ͼ����

	std::string m_chessFilePath;	// �洢�ļ�����·��
	std::string m_chessFileFrameNum;	// ��ǰ�궨��֡��
	std::string m_chessFileCamPath;	// ��������궨���ļ�·��
	std::string m_chessFileCamName;	// ��������궨���ļ���
	std::string m_chessFileProPath[4];	// ����ͶӰ�Ǳ궨���ļ�·��
	std::string m_chessFileProName[4];	// ����ͶӰ�Ǳ궨���ļ���
	std::string m_chessFileSuffix;	// �ļ���׺��

public:
	CSensor();
	~CSensor();
	bool SetChessFrame(int frame);	// �趨��ǰ�궨��֡��	
	cv::Mat GetCamFrame();			// ��ȡ���ͼ��
	int GetPicNum(int patternIdx);	// ��ȡͶӰ��ͼ��������
	cv::Mat GetProFrame(int patternIdx, int picIdx);	// ��ȡͶӰ��ͼ��
};

// ���ݴ洢ģ�顣�洢�м��������浵
class CStorage
{
private:
	std::string m_matFilePath;
	std::string m_matFileName;
	std::string m_matFileSuffix;
	std::string m_storagePath;		// ���ڴ洢������·����debug�á�
public:
	CStorage();
	~CStorage();
	bool Store(cv::Mat *pictures, int num);		// �洢ͼƬ��
	
	bool SetMatFileName(std::string matFilePath,	// �趨�洢·��������
		std::string matFileName,
		std::string matFileSuffix);
};

// ���ӻ�ģ�飬����debug���Զ��������ٴ��ڡ�
class CVisualization
{
private:
	std::string m_winName;		// ��������
public:
	CVisualization(std::string winName);
	~CVisualization();
	int Show(cv::Mat pic, int time, bool norm = false, double zoom = 1.0);
};

// ������������������Ѿ���д�õĸ����롣
// ����Ϊһ��Ҷ�ͼ�����Ϊһ�ŻҶ�ͼ��ÿһ��洢����projector�е����ꡣ
// ʹ��ǰ��Ҫ�ֱ����4��Set�������Ρ�
class CDecode_Gray
{
private:
	int m_numDigit;			// λ��
	int m_grayCodeSize;		// �ܹ��ĸ�������Ŀ
	short * m_gray2bin;		// �����뵽���������ת��
	std::string m_codeFilePath;	// �洢��������ļ�·��
	std::string m_codeFileName;	// �洢��������ļ���
	int resRow;				// ͼ����зֱ���
	int resLine;			// ͼ����зֱ���
	bool m_vertical;		// �趨�����뷽��

	cv::Mat * m_grePicture;	// ����ĻҶ�ͼ
	cv::Mat * m_binPicture;	// �ӹ���Ķ�ֵͼ
	cv::Mat m_result;		// ���

	CVisualization * m_visual;	// ������ʾ�м���

	bool AllocateSpace();		// Ϊ����ľ���short������������ռ�
	bool ReleaseSpace();		// ɾ�����пռ�
	bool Grey2Bin();			// ���Ҷ�ͼ�ӹ�Ϊ��ֵͼ���Ա��һ������
	bool CountResult();			// ���ݶ�ֵͼͳ�ƽ��
	bool Visualize();			// ��ʾ�м���

public:
	CDecode_Gray();
	~CDecode_Gray();

	bool Decode();
	cv::Mat GetResult();

	bool SetMat(int num, cv::Mat pic);						// ������Ӧ�Ҷ�ͼ
	bool SetNumDigit(int numDigit, bool ver);				// ���ø�����λ��
	bool SetMatFileName(std::string codeFilePath,
		std::string codeFileName);			// ���ô洢��������ļ���
};

// ���ƽ������������Ѿ���д�õ������롣
// ����Ϊһ��Ҷ�ͼ�����Ϊһ�ŻҶ�ͼ��ÿһ��洢����projector�е�ƫ�����ꡣ
class CDecode_Phase
{
private:
	int m_numMat;		// ͼƬ��Ŀ��Ĭ��Ϊ2��
	int m_pixPeroid;	// �������ڡ�
	int m_resRow;				// ͼ����зֱ���
	int m_resLine;			// ͼ����зֱ���

	cv::Mat * m_grePicture;	// ����ĻҶ�ͼ
	cv::Mat m_result;		// ���

	CVisualization * m_visual;	// ������ʾ�м���

	bool AllocateSpace();		// Ϊ����ľ�������ռ�
	bool DeleteSpace();			// ɾ�����пռ�
	bool CountResult();			// ���ݹ�һ���Ҷ�ͼͳ�ƽ��
	bool Visualize();			// ��ʾ�м���

public:
	CDecode_Phase();
	~CDecode_Phase();

	bool Decode();
	cv::Mat GetResult();

	bool SetMat(int num, cv::Mat pic);						// ������Ӧ�Ҷ�ͼ
	bool SetNumMat(int numDigit, int pixperiod);			// ���ò���
};

// �궨�ࡣ���ڱ궨����ͷ��ͶӰ�ǡ�
// ���л���������ͷ��ͶӰ�ǵĿ��ƣ��������������������н��롣
class CCalibration
{
private:
	// ������������
	CSensor * m_sensor;
	// ������
	CDecode_Gray * m_decodeGray;	// ���������
	CDecode_Phase * m_decodePS;		// PS����
	// ���̸��X��Y���Լ�ͼƬ��Ŀ
	int m_chessLine;
	int m_chessRow;
	int m_chessNum;
	// �ɼ��������̸�ͼ���ʶ����
	cv::Mat m_chessMat;
	cv::Mat * m_grayV;
	cv::Mat * m_grayH;
	cv::Mat * m_phaseV;
	cv::Mat * m_phaseH;
	cv::Mat m_chessMatDraw;
	cv::Mat m_proMatDraw;
	//���̸�˳����
	std::vector<std::vector<cv::Point2f>> m_camPoint;
	std::vector<cv::Point2f> m_camPointTmp;
	std::vector<std::vector<cv::Point2f>> m_proPoint;
	std::vector<cv::Point2f> m_proPointTmp;
	std::vector<std::vector<cv::Point3f>> m_objPoint;
	std::vector<cv::Point3f> m_objPointTmp;
	// �궨�ľ���
	cv::Mat m_camMatrix;
	cv::Mat m_camDisCoeffs;
	cv::Mat m_proMatrix;
	cv::Mat m_proDisCoeffs;
	cv::Mat m_R;
	cv::Mat m_T;
	cv::Mat m_E;
	cv::Mat m_F;

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