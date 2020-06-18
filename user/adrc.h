#ifndef __ADRC_H
#define __ADRC_H

#include "mymath.h"

#define T       0.002f
//�Կ��ſ������Ŀ��Ʋ���������״̬����
//�Կ��ſ���������ƽ�Ϊ���,ʹ�õĲ������ܻ�Ƶ���Ķ�
typedef struct
{
	//���Ʋ���(ֱ�ӵ���)
	float wc;        //����������
	float wo;        //�۲�������
	float B;         //�Ŷ���������
	float KpOut;     //�⻷��̬��������
	//���Ʋ���(��ӵ���)
	float KpIn;      //�ڻ����ٶȱ�������
	float KdIn;      //�ڻ����ٶ�΢�ֿ���
	float L1,L2,L3;  //ESO�۲�����
	//״̬����
	float AttOut;    //�⻷���
	float SpeEst;    //���ٶȹ���
	float AccEst;    //�Ǽ��ٶȹ���
	float w;         //���Ŷ�
	float u;         //�������������
}ADRC_Param;

void ADRC_LESO(ADRC_Param *adrc,float y);
void ADRC_ParamUpdate(ADRC_Param *adrc);
void ADRC_ParamClear(ADRC_Param *adrc);

#endif
