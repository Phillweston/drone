#ifndef __ADRC_H
#define __ADRC_H

#include "mymath.h"

//�Կ��ſ������Ŀ��Ʋ��������в���
//�Կ��ſ���������ƽ�Ϊ���,ʹ�õĲ������ܻ�Ƶ���Ķ�
typedef struct
{
	//���Ʋ���
	float KpOut;     //�⻷λ�ñ�������
	float KpIn;      //�ڻ��ٶȱ�������
	float KdIn;      //�ڻ����ٶȱ�������
	float Kw;        //����״̬�۲���������С
	//���в���
	float PosOut;    //λ��������
	float SpeEst;    //���ٶȵ�״̬����
	float AccEst;    //�Ǽ��ٶȵ�״̬����
	float w;         //���Ŷ�
	float u;         //�������������
}ADRC_Param;

typedef struct
{
	float depth;
	float theta;
	float delay[3];
}BSFilter;

float ADRC_fhan(float x1,float x2);
float ADRC_TD(float r,float *derivative);
float ADRC_fal(float x);
float ADRC_ESO(float u,float y,float b);
void ADRC_LESO(ADRC_Param *adrc,float y);

#endif
