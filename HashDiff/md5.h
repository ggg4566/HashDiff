#include <Windows.h>
#include <iostream>
#include <sstream>
using namespace std;
#pragma once
class md5
{
public:
	
	md5(HANDLE ,DWORD);

	~md5(void);
	 char md5hash[33];

private: 

	BYTE *buffer; //这里是内容保存位置
	DWORD *realSize; //实际读入的文件大小
	DWORD *result;  //结果保存位置
	DWORD *Size;    //填充信息后的信息大小
	DWORD *M ;      //用来保存每次要处理的64个字节
	//由于是MD5是大端字节序表示，所以要反过来存
	static DWORD A ;
	static DWORD B ;
	static DWORD C ;
	static DWORD D ;
	static DWORD T[64];		//常数值
	static DWORD s[64];		//左移的位数
	static DWORD m[64];		//每次处理的分段顺序
	


	void FillMessage() ; //填充信息，使得内容满足要求
	void ShowMessage() ; //输出信息，看看有没有错
	void SeparateMessage(DWORD begin); //将信息分成16份，存到M中
	void FF(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti);
	void GG(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti);
	void HH(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti);
	void II(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti);
	DWORD F(DWORD X,DWORD Y,DWORD Z);
	DWORD G(DWORD X,DWORD Y,DWORD Z);
	DWORD H(DWORD X,DWORD Y,DWORD Z);
	DWORD I(DWORD X,DWORD Y,DWORD Z);
};

