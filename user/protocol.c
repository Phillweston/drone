#include "protocol.h"
/**************�ļ�˵��**********************
����վ�ͷɿ�ͨ�����ڽ�������ͨ��
********************************************/

/*���ڽ��ղ���**********************************/

u8 RxData;  //�Ӵ����յ���һ���ֽ�
//���±����������ļ���ȡ
u8 FcnWord;  //�����ֽ�
u8 LenWord;  //�����ֽ�
u8 RxTemp[12];  //��ʱ���洮�ڽ��յ��Ĵ�������
u8 GlobalStat=0;  //ȫ��״̬
u8 RcvCnt=0;  //�����������֡����

/***********************
����DMA����ͨ��,�ӵ���վ/ң������������
**********************/
void Protocol_Init(void)
{
	HAL_UART_Receive_DMA(&huart2,&RxData,1);
}
/***********************
�����ֽڴ���
**********************/
u8 XDAA_Data_Receive_Precess(void)
{
	static u8 zRxState=0,sum=0,i=0;
	switch(zRxState)
	{
	case 0:  //֡ͷУ��
		if(RxData=='<')
		{
			sum=RxData;
			zRxState=1;
		}
		break;
	case 1:  //������У���뱣��
		sum+=RxData;
		FcnWord=RxData;
		zRxState=2;
		break;
	case 2:  //���ݳ���У���뱣��
		if(RxData<=12)
		{
			sum+=RxData;
			LenWord=RxData;
			zRxState=3;
		}
		else
		{
			sum=0;
			zRxState=0;
		}
		break;
	case 3:  //��ʱ�����������
		sum+=RxData;
		RxTemp[i++]=RxData;
		if(i>=LenWord)
			zRxState=4;
		break;
	case 4:  //ƥ��У���
		zRxState=0;
		i=0;
		if(sum==RxData)
			return 0;  //����������Ч
		else
			return 1;
	default:
		zRxState=0;
		return 1;
	}
	return 1;  //����������Ч
}
/***********************
����ͨ��DMA��ʽ���յ�һ���ֽ�
**********************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance!=USART2) return;
	if(XDAA_Data_Receive_Precess()) return;
	RcvCnt++;
}


/*���ڷ��Ͳ���**********************************/

u8 DataToSend[16];  //�����͵�����
u8 SendBuff[SENDBUF_SIZE];  //���ͻ�����
u8 SendBuff2[SENDBUF_SIZE];  //���ͻ�����2
u16 TotalLen=0;  //���ͻ��������������ݳ���
/***********************
����ͨ��DMA��ʽ���յ�һ���ֽ�
**********************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance!=USART2)	return;
	GlobalStat&=~TX_BUSY;
}
/***********************
�����������ݴ��뻺��
**********************/
void DMA_Stuff(u8 *Data,u8 len)
{
	for(u8 i=0;i<len;i++)
	{
		if(TotalLen+i>=SENDBUF_SIZE)
			return;  //���������ʲ����쵼�»�������������µ�����
		SendBuff2[TotalLen+i]=Data[i];
	}
	TotalLen+=len;
}
/***********************
�����ͻ���������������DMA���Ͳ���շ��ͻ�����
**********************/
void Total_Send(void)
{
	if(TotalLen==0)	return;
	if(GlobalStat & TX_BUSY) return;
	for(u8 i=0;i<TotalLen;i++)
		SendBuff[i]=SendBuff2[i];
	HAL_UART_Transmit_DMA(&huart2,SendBuff,TotalLen);
	GlobalStat|=TX_BUSY;
	TotalLen=0;
}
/***********************
*@data:s16������
*@len:���ݸ���
*@fcn:������
**********************/
void XDAA_Send_S16_Data(s16 *data,u8 len,u8 fcn)
{
	u8 i,cnt=0,checksum=0;
	DataToSend[cnt++]='>';
	DataToSend[cnt++]=fcn;
	DataToSend[cnt++]=len*2;
	for(i=0;i<len;i++)
	{
		DataToSend[cnt++]=BYTE1(data[i]);
		DataToSend[cnt++]=BYTE0(data[i]);
	}
	for(i=0;i<cnt;i++)
		checksum+=DataToSend[i];
	DataToSend[cnt++]=checksum;
	DMA_Stuff(DataToSend,cnt);
}
/***********************
*@data:u8������
*@len:���ݸ���
*@fcn:������
**********************/
void XDAA_Send_U8_Data(u8 *data,u8 len,u8 fcn)
{
	u8 i,cnt=0,checksum=0;
	DataToSend[cnt++]='>';
	DataToSend[cnt++]=fcn;
	DataToSend[cnt++]=len;
	for(i=0;i<len;i++)
		DataToSend[cnt++]=data[i];
	for(i=0;i<cnt;i++)
		checksum+=DataToSend[i];
	DataToSend[cnt++]=checksum;
	DMA_Stuff(DataToSend,cnt);
}
/***********************
*@data:s32������
*@len:���ݸ���
*@fcn:������
**********************/
void XDAA_Send_HighSpeed_Data(float data1,float data2)
{
	u8 i,cnt=0,checksum=0;
	int temp=data1*65536;
	DataToSend[cnt++]='@';
	DataToSend[cnt++]=BYTE3(temp);
	DataToSend[cnt++]=BYTE2(temp);
	DataToSend[cnt++]=BYTE1(temp);
	DataToSend[cnt++]=BYTE0(temp);
	temp=data2*65536;
	DataToSend[cnt++]=BYTE3(temp);
	DataToSend[cnt++]=BYTE2(temp);
	DataToSend[cnt++]=BYTE1(temp);
	DataToSend[cnt++]=BYTE0(temp);
	for(i=0;i<cnt;i++)
		checksum+=DataToSend[i];
	DataToSend[cnt++]=checksum;
	DMA_Stuff(DataToSend,cnt);
}
