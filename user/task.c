#include "task.h"
/**************�ļ�˵��**********************
���˷��������ƺ���֮��Ķ�ʱ�������ֱ�Ϊ��
Lock_And_Unlock();             ����������
RC_Prepare();                  �Խ��ջ����źŽ���Ԥ����
IMU_Processing();              ��̬������£�MPU6050����У׼
********************************************/

u8 ReqMsg1=0,ReqMsg2=0,ReqMsg3=0;  //��λ��ָ��
u8 ErrCnt=0;  //δ�յ�ң�����źŵĴ���
AxisInt acc;  //������ٶ�У׼������
//���²���Ϊ��������ʹ��
AxisInt gyro;  //������ٶ�У׼������
float roll,pitch,yaw;  //��������̬
short RCdata[4];  //ң������������
ADRC_Param adrR,adrP;  //�Կ��ſ���������
float Kyaw,YawOut;  //yaw�������������������
float RolBias,PitBias,YawBias;  //�̶�ƫ��,���ڵ����Ŷ�
float throttle=0;  //ƽ��λ�ô����͵�ʵ�ʲο��������
short PwmOut[4];  //�������,��ֵ������ʱ��,���PWM

/***********************
��̬�������,MPU6050����У׼
*@period:2ms
**********************/
void IMU_Processing(void)
{
	static float IIRax[3],IIRay[3],IIRaz[3];
	static float IIRgx[3],IIRgy[3],IIRgz[3];
	AxisInt oacc,ogyro;
	MPU_Get_Accelerometer(&acc.x,&acc.y,&acc.z);
	MPU_Get_Gyroscope(&gyro.x,&gyro.y,&gyro.z);
	oacc=acc;ogyro=gyro;
	acc.x=IIR_LowPassFilter(oacc.x,IIRax);
	acc.y=IIR_LowPassFilter(oacc.y,IIRay);
	acc.z=IIR_LowPassFilter(oacc.z,IIRaz);
	gyro.x=IIR_LowPassFilter(ogyro.x,IIRgx);
	gyro.y=IIR_LowPassFilter(ogyro.y,IIRgy);
	gyro.z=IIR_LowPassFilter(ogyro.z,IIRgz);
	Acc_Correct(&acc);
	Gyro_Correct(&gyro);
	IMUupdate(acc,gyro,&roll,&pitch,&yaw);
	if(GlobalStat & ACC_CALI)
		if(!Acc_Calibrate(acc))
			GlobalStat &=~ ACC_CALI;
	if(GlobalStat &GYRO_CALI)
		if(!Gyro_Calibrate(gyro))
			GlobalStat &=~ GYRO_CALI;
}

/***********************
ʧ�ر���.����������������ʱ����
*�෭����75��
*����2��δ�յ�ң���ź�
�������:
����10��δ�յ�ң���ź���ֱ������,����:
���ڽ���������ֱ������,
���������̬����,���ű���Ϊ��������
**********************/
void Fail_Safe(char state)
{
	if(state==3)
		GlobalStat&=~MOTOR_LOCK;
	if(throttle<NORMALSPEED-110)
		GlobalStat&=~MOTOR_LOCK;
	else
	{
		RCdata[0]=500;
		RCdata[1]=500;
		throttle=NORMALSPEED-100;
		RCdata[3]=500;
	}
}

/***********************
��ʱ����Ƿ��յ�ң�����ź��������ź�
*@period:100ms
**********************/
void RC_Monitor(void)
{
	ErrCnt++;
	if(ErrCnt>=ERR_TIME)
	{
		if(ErrCnt<LOST_TIME)
			Fail_Safe(2);
		else
		{
			Fail_Safe(3);
			ErrCnt--;
		}
	}
	if(STAT_PORT & STAT_Pin)
		LED3_PORT |= LED3_Pin;
	else
		LED3_PORT &=~ LED3_Pin;
}

/***********************
���յ���ң�����źŽ��д���.�յ�һ֡����ִ��һ��
*@period:100ms(Not strict)
**********************/
void RC_Processing(void)
{
	switch(FcnWord)
	{
	case P_STAT:
		if((RxTemp[0]&MOTOR_LOCK)&&(throttle<=LOWSPEED))
			GlobalStat|=MOTOR_LOCK;
		else
			GlobalStat&=~MOTOR_LOCK;
		if((RxTemp[0] & (REQ_MODE_SPEED+REQ_MODE_ATTI))==REQ_MODE_SPEED)
			GlobalStat|=SPEED_MODE;
		else if((RxTemp[0] & (REQ_MODE_SPEED+REQ_MODE_ATTI))==REQ_MODE_ATTI)
			GlobalStat&=~SPEED_MODE;
		break;
	case P_CTRL:
		RCdata[0]=(RxTemp[0]<<8) | RxTemp[1];
		RCdata[1]=(RxTemp[2]<<8) | RxTemp[3];
		RCdata[2]=(RxTemp[4]<<8) | RxTemp[5];
		RCdata[3]=(RxTemp[6]<<8) | RxTemp[7];
		throttle=moderate(RCdata[2],NORMALSPEED);
		break;
	case P_REQ1:
		ReqMsg1=RxTemp[0];
		break;
	case P_REQ2:
		ReqMsg2=RxTemp[0];
		if(ReqMsg2 & REQ_ACC_CALI)
			GlobalStat|=ACC_CALI;
		if(ReqMsg2 & REQ_GYRO_CALI)
			GlobalStat|=GYRO_CALI;
		break;
	case P_REQ3:
		ReqMsg3=RxTemp[0];
		break;
	case P_ROL_CTRL:
		adrR.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrR.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrR.KpOut=(RxTemp[4]*256.0f+RxTemp[5])/1000.0f;
		RolBias=(short)(RxTemp[6]*256.0f+RxTemp[7])/100;
		break;
	case P_PIT_CTRL:
		adrP.KpIn=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
		adrP.KdIn=(RxTemp[2]*256.0f+RxTemp[3])/1000.0f;
		adrP.KpOut=(RxTemp[4]*256.0f+RxTemp[5])/1000.0f;
		PitBias=(short)(RxTemp[6]*256.0f+RxTemp[7])/100;
	case P_YAW_CTRL:
		Kyaw=(RxTemp[0]*256.0f+RxTemp[1])/1000.0f;
	default:break;
	}
	if((roll>75)||(pitch>75))
	Fail_Safe(1);
}

void RC_Data_Send(void)
{
	ErrCnt=0;
	s16 sdata[6];
	//��λ������1
	if(ReqMsg1 & REQ_STAT)
	{
		u16 voltage=Get_Battery_Voltage();
		u8 udata[3]={GlobalStat,BYTE1(voltage),BYTE0(voltage)};
		XDAA_Send_U8_Data(udata,3,P_STAT);
		ReqMsg1 &=~ REQ_STAT;
	}
	if(ReqMsg1 & REQ_ATTI)
	{
		sdata[0]=(s16)(roll*100);
		sdata[1]=(s16)(pitch*100);
		sdata[2]=(s16)(yaw*100);
		XDAA_Send_S16_Data(sdata,3,P_ATTI);
		ReqMsg1 &=~ REQ_ATTI;
	}
	if(ReqMsg1 & REQ_SENSOR)
	{
		sdata[0]=acc.x;sdata[1]=acc.y;sdata[2]=acc.z;
		sdata[3]=gyro.x;sdata[4]=gyro.y;sdata[5]=gyro.z;
		XDAA_Send_S16_Data(sdata,6,P_SENSOR);
		ReqMsg1 &=~ REQ_SENSOR;
	}
	if(ReqMsg1 & REQ_RC)
	{
		XDAA_Send_S16_Data(RCdata,4,P_CTRL);
		ReqMsg1 &=~ REQ_RC;
	}
	if(ReqMsg1 & REQ_MOTOR)
	{
		sdata[0]=PwmOut[0];
		sdata[1]=PwmOut[1];
		sdata[2]=PwmOut[2];
		sdata[3]=PwmOut[3];
		XDAA_Send_S16_Data(sdata,4,P_MOTOR);
		ReqMsg1 &=~ REQ_MOTOR;
	}
	if(ReqMsg1 & REQ_QUATERNION)
	{
		sdata[0]=(s16)(q0*10000);
		sdata[1]=(s16)(q1*10000);
		sdata[2]=(s16)(q2*10000);
		sdata[3]=(s16)(q3*10000);
		XDAA_Send_S16_Data(sdata,4,P_QUATERNION);
		ReqMsg1 &=~ REQ_QUATERNION;
	}
	//��λ������2
	if(ReqMsg2 & REQ_ROL_CTRL)
	{
		sdata[0]=(s16)(adrR.KpIn*1000);
		sdata[1]=(s16)(adrR.KdIn*1000);
		sdata[2]=(s16)(adrR.KpOut*1000);
		sdata[3]=(s16)(RolBias*100);
		XDAA_Send_S16_Data(sdata,4,P_ROL_CTRL);
		ReqMsg2 &=~ REQ_ROL_CTRL;
	}
	if(ReqMsg2 & REQ_PIT_CTRL)
	{
		sdata[0]=(s16)(adrP.KpIn*1000);
		sdata[1]=(s16)(adrP.KdIn*1000);
		sdata[2]=(s16)(adrP.KpOut*1000);
		sdata[3]=(s16)(PitBias*100);
		XDAA_Send_S16_Data(sdata,4,P_PIT_CTRL);
		ReqMsg2 &=~ REQ_PIT_CTRL;
	}
	if(ReqMsg2 & REQ_YAW_CTRL)
	{
		sdata[0]=(s16)(Kyaw*1000);
		sdata[1]=0;
		sdata[2]=0;
		sdata[3]=0;
		XDAA_Send_S16_Data(sdata,4,P_YAW_CTRL);
		ReqMsg2 &=~ REQ_PIT_CTRL;
	}
	//��λ������3
	if(ReqMsg3 & 0x0F)
	{
		sdata[0]=0;
		sdata[1]=0;
		sdata[2]=0;
		sdata[3]=0;
		XDAA_Send_S16_Data(sdata,4,P_CHART1);
		ReqMsg3 &=~ 0x0F;
	}
	if(ReqMsg3 & 0xF0)
	{
		sdata[0]=(s16)(adrR.AccEst*100);
		sdata[1]=(s16)(adrR.u*100);
		sdata[2]=(s16)(adrR.w*100);
		sdata[3]=(s16)(adrR.SpeEst*100);
		XDAA_Send_S16_Data(sdata,4,P_CHART2);
		ReqMsg3 &=~ 0xF0;
	}
	Total_Send();
}
