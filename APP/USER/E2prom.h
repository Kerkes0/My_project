#ifndef _E2PROM_H_
#define _E2PROM_H_


extern u8 u8fg_E2_SysFirstIn; //��ͨ��Դ���ʼ�����½���

void E2prom_Manage(void);
void	Write24c02_Sub(uchar Address,uchar *Ptr,uchar ByteNum);
void	Read24c02_Sub(uchar Address,uchar *Ptr,uchar ByteNum);
void FunE2_SysPro(void);
//void FunE2_SysInit(void);



#endif

