#include "adrc.h"
/**************�ļ�˵��**********************
�Կ��ſ�����
********************************************/

/**********************
��������״̬�۲���
x1'=x2;x2'=-a*x2+b*u+w;
*@y:�����ٵĽ��ٶ�
**********************/
void ADRC_LESO(ADRC_Param *adrc,float y)
{
	float e = y - adrc->SpeEst;
	adrc->SpeEst += (adrc->AccEst + 30.0f * e) * T;
	adrc->AccEst +=(adrc->u - adrc->AccEst + adrc->w + 300.0f * e) * T;
	adrc->w += 1000.0f * e * T;
	adrc->AccEst=OneOrder_Filter(adrc->AccEst, adrc->AccDelay);
}

/**********************
��ɢϵͳ���ٿ����ۺϺ���
*@x1:���״��������������
*@x2:���״��������������΢��
*@return:���������
**********************/
#define R  5000.0f 
#define H  0.002f  //>=T
#define D  10.0f   //d=r*h
#define D0 0.02f   //d0=h*d
float ADRC_fhan(float x1, float x2)
{
	float y = x1 + H * x2;
	float a0 = Msqrt(100.0f + 4e4f * ABS(y));  //a0=Msqrt(d*d+8*r*ABS(y));
	float a;
	if (ABS(y) > D0)
		a = x2 + (a0 - D) / 2.0f * SIGN(y);
	else
		a = x2 + y / H;
	if (ABS(a) > D)
		return -R * SIGN(a);
	else
		return -500.0f * a;  //-r*a/d
}

/**********************
����΢����
**********************
void ADRC_TD(ADRC_Param* adrc)
{
	float u = ADRC_fhan(adrc->x1 - adrc->PosOut, adrc->x2);
	adrc->x1 += T * adrc->x2;
	adrc->x2 += T * u;
}*/

/**********************
�����Ժ���fal(e, 0.5, 0.01)
**********************/
float ADRC_fal_5(float x)
{
	float y;
	if(ABS(x)<=0.01)
		y=x*10.0f;
	else
		y=Msqrt(ABS(x))*SIGN(x);
	return y;
}

/**********************
�����Ժ���fal(e, 0.25, 0.01)
**********************/
float ADRC_fal_25(float x)
{
	float y;
	if(ABS(x)<=0.01)
		y=x*31.62278f;
	else
		y=Msqrt(Msqrt(ABS(x)))*SIGN(x);
	return y;
}

/**********************
�����Ժ���fal(e, 1.5, 0.01)
**********************/
float ADRC_fal_1_5(float x)
{
	float y;
	if(ABS(x)<=0.01)
		y=x*0.1f;
	else
	{
		y=ABS(x)*Msqrt(ABS(x))*SIGN(x);
	}
	return y;
}
