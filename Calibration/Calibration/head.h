// OpenCV
#include <opencv2/opencv.hpp>

// �������ÿ�
#include <vector>
#include <direct.h>

#define VISUAL_DEBUG false
#define CHESS_FRAME_NUMBER 15

// һЩ����
#include "StaticParameters.h"

// ȫ���Ժ���
#include "GlobalFunction.h"

// ������ģ�顣ģ��
#include "CSensor.h"

// ���ݴ洢ģ�顣�洢�м��������浵
#include "CStorage.h"

// ���ӻ�ģ�飬����debug���Զ��������ٴ��ڡ�
#include "CVisualization.h"

// ������������������Ѿ���д�õĸ����롣
// ����Ϊһ��Ҷ�ͼ�����Ϊһ�ŻҶ�ͼ��ÿһ��洢����projector�е����ꡣ
// ʹ��ǰ��Ҫ�ֱ����4��Set�������Ρ�
#include "CDecodeGray.h"

// ���ƽ������������Ѿ���д�õ������롣
// ����Ϊһ��Ҷ�ͼ�����Ϊһ�ŻҶ�ͼ��ÿһ��洢����projector�е�ƫ�����ꡣ
#include "CDecodePhase.h"

// �궨�ࡣ���ڱ궨����ͷ��ͶӰ�ǡ�
// ���л���������ͷ��ͶӰ�ǵĿ��ƣ��������������������н��롣
#include "CCalibration.h"