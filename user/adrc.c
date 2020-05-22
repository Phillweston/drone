#include "adrc.h"
/**************�ļ�˵��**********************
�Կ��ſ�������غ���,��һ�����еĺ�����������
���ϵ���ǰ
********************************************/

/**********************
��������״̬�۲���
x1'=x2;x2'=-x2+u+w;
*@y:������ٶ�
**********************/
#define T       0.002
void ADRC_LESO(ADRC_Param *adrc,float y)
{
	float e=y-adrc->SpeEst;
	adrc->SpeEst+=(adrc->AccEst+15.0f*e)*T;
	adrc->AccEst+=(adrc->u-adrc->AccEst+adrc->w +75.0f*e)*T;
	adrc->w+=125.0f*e*T;
}

//���º�����ʱδ����

/**********************
��ɢϵͳ���ٿ����ۺϺ���
*@x1:���׻����������
*@x2:���׻����������΢��
*@return:���������
**********************/
#define adrcR   4
#define adrcH   0.02  //>=T
#define adrcD   0.0016  //H*H*R
float ADRC_fhan(float x1, float x2)
{
	float y1 = x1 + adrcH*x2;
	float y2 = x1 + 2 * adrcH*x2;
	if ((y1 >= -adrcD) && (y1 <= adrcD) && (y2 >= -adrcD) && (y2 <= adrcD))
		return -y2 / adrcH / adrcH;
	float u = (1 + Msqrt(1 + 8 * ABS(y1) / adrcD)) / 2 + 0.0001f;
	short k = (short)u;
	if (k > 2)
		u = (1 - k / 2.0f)*adrcR*SIGN(y1) - (x1 + k*adrcH*x2) / ((k - 1)*adrcH*adrcH);
	else if (k == 2)
		u = -adrcR / 2.0f*SIGN(y1) - (x1 + 3 * adrcH*x2) / 2 * adrcH*adrcH;
	else if (k == 1)
		u = -adrcR*SIGN(x2);
	return LIMIT(u, -adrcR, adrcR);
}

/**********************
����΢����
*@u:����
*@derivative:ԭ�źŵ�΢�����
*@return:ԭ�źŵ��˲����
**********************/
float ADRC_TD(float r,float *derivative)
{
	static float x1=0,x2=0;
	float u=ADRC_fhan(x1-r,x2);
	x1+=x2;
	x2+=u;
	*derivative=x2;
	return x1;
}

/**********************
����������
**********************/
float ADRC_fal(float x)
{
	float y;
	if(ABS(x)<=0.02)
		y=x*0.1414213562;
	else
		y=Msqrt(ABS(x))*SIGN(x);
	return y;
}

/**********************
����״̬�۲���(bug)
**********************/
float ADRC_ESO(float u,float y,float b)
{
	static float z1=0,z2=0;
	float e=z1-y;
	z1+=b*u+z2-50*e;
	z2-=220*ADRC_fal(e);
	float w=z2/b;
	return w;
}
