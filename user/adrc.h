#ifndef __ADRC_H
#define __ADRC_H

#include "mymath.h"

#define T       0.002f
//�Կ��ſ������Ŀ��Ʋ��������в���
//�Կ��ſ���������ƽ�Ϊ���,ʹ�õĲ������ܻ�Ƶ���Ķ�
typedef struct
{
	//���Ʋ���
	float KpOut;     //�⻷��̬��������
	float KpIn;      //�ڻ����ٶȱ�������
	float KiIn;      //�ڻ����ٶȻ��ֿ���
	float KdIn;      //�ڻ����ٶ�΢�ֿ���
	float Kw;        //�Ŷ���������
	//���в���
	float AttOut;    //�⻷���
	float SpeErr;    //�ڻ����ٶ���������
	float SpeInt;    //�ڻ����ٶ������������
	float SpeEst;    //���ٶȹ���
	float AccEst;    //�Ǽ��ٶȹ���
	float w;         //���Ŷ�
	float u;         //�������������
	//IIR�˲�ʹ��
	float AccDelay;  //�Ǽ��ٶ�
}ADRC_Param;

void ADRC_LESO(ADRC_Param *adrc,float y);
void ADRC_TD(ADRC_Param* adrc);
float ADRC_fal_5(float x);
float ADRC_fal_25(float x);
float ADRC_fal_1_5(float x);

#endif
