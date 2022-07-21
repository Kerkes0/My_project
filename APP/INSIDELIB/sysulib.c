/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-09-19 13:35:44
 * @LastEditTime: 2019-09-20 15:28:05
 * @LastEditors: Please set LastEditors
 */
#include "sysulib.h"

//校验和计算
u8 CheckSum(u8 data[],u8 len)
{
	u8 i,sum=0;
	
	for (i=0;i<len;i++)
		sum += data[i];
	sum = ~sum;
	sum += 0xAA;
	
	return sum;
}

u8 CheckSum2(u8 data[],u8 len)
{
	u8 i,sum=0;
	
	for (i=0;i<len;i++)
		sum += data[i];
	
	return sum;
}

//比较两个数组前n个值
s8 umemcmp(const u8 str1[], const u8 str2[], u16 n){
	u16 i;
	for(i=0;i<n;i++){
		if(str1[i]!=str2[i])
			return (s8)(str1[i]-str2[i]);
	}
	return 0;
}

//把数组src前n个值复制到数组des
void umemcpy(u8 des[], const u8 src[], u16 n){
	u16 i;
	for(i=0;i<n;i++)
		des[i]=src[i];
}

u8 u8tora(u8 num,char str[],u8 n){
	u8 ret;
	do{
		str[--n]=num%10+'0';
	}while(n && (num/=10)>0);
	ret=n;
	while(n)
		str[--n]=' ';
	return ret;
}

u8 u8tona(u8 num,char str[],u8 n){
	u8 ret=0;
	char b=0;
	u8 i=0,j=0;
	do{
		str[i++]=num%10+'0';
	}while(i<n && (num/=10)>0);
	ret=i--;
	n=i/2;
	while(n<i){
		b=str[j];
		str[j++]=str[i];
		str[i--]=b;
	}
	return ret;
}

void u8tola(u8 num,char str[],u8 n){
	u8 i=u8tona(num,str,n);
	while(i<n)
		str[i++]=' ';
}

void s8tora(s8 num,char str[],u8 n){
	if(num<0){
		n=u8tora(-num,str,n);
		if(n)
			str[n-1]='-';
	}else
		u8tora(num,str,n);
}

u32 FastLog2(u32 x){
	float fx;
	u32 ix;
	fx = (float)x;
	ix = *(u32*)&fx;
	ix = (ix >> 23) & 0xFF;
	return ix - 127;
}
