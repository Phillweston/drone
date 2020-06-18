#include "adrc.h"
/**************�ļ�˵��**********************
�Կ��ſ�����
********************************************/

/**********************
��������״̬�۲���
x1'=x2;x2'=b*u+w;
*@y:�����ٵĽ��ٶ�
**********************/
void ADRC_LESO(ADRC_Param *adrc,float y)
{
	float e = y - adrc->SpeEst;
	adrc->SpeEst += (adrc->AccEst + adrc->L1 * e) * T;
	adrc->AccEst +=(adrc->B * adrc->u + adrc->w + adrc->L2 * e) * T;
	adrc->w += adrc->L3 * e * T;
}

/**********************
���Ʋ�������
**********************/
void ADRC_ParamUpdate(ADRC_Param *adrc)
{
	adrc->KpIn = adrc->wc * adrc->wc;
	adrc->KdIn = 2 * adrc->wc;
	adrc->L1 = 3 * adrc->wo;
	adrc->L2 = 3 * adrc->wo * adrc->wo;
	adrc->L3 = adrc->wo * adrc->wo * adrc->wo; 
}

/**********************
״̬��������
**********************/
void ADRC_ParamClear(ADRC_Param *adrc)
{
	adrc->AttOut=0;
	adrc->SpeEst=0;
	adrc->AccEst=0;
	adrc->w=0;
	adrc->u=0;
}
