#pragma once
class crc32
{
public:
	crc32(void);
	crc32(HANDLE,DWORD);
	~crc32(void);
	DWORD crc32Value;
	
private:
	void init();
	void calc();
	
	static DWORD ploy;
	static DWORD initValue;
	static DWORD endValue;

	BYTE *buffer;
	DWORD *realSize;
	DWORD *crc32_table;
	
	
};

