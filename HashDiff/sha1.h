#pragma once
#include <Windows.h>
class sha1
{
public:
	sha1(HANDLE,DWORD);
	~sha1(void);
	char sha1hash[41];
private:
	static DWORD A;
	static DWORD B;
	static DWORD C;
	static DWORD D;
	static DWORD E;
	static DWORD k[4];


	DWORD *realSize;
	DWORD *Size;
	DWORD *result;
	DWORD *M;
	BYTE  *buffer;
	
	
	void FillMessage() ; //填充信息，使得内容满足要求
	void ShowMessage() ; //输出信息，看看有没有错
	void SeparateMessage(DWORD begin);
};  

