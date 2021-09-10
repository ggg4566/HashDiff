#include "StdAfx.h"
#include "sha1.h"

DWORD sha1::A = 0x67452301;
DWORD sha1::B = 0xEFCDAB89;
DWORD sha1::C = 0x98BADCFE;
DWORD sha1::D = 0x10325476;
DWORD sha1::E = 0xC3D2E1F0;


DWORD sha1::k[4] = {0x5A827999,0x6ED9EBA1,0x8F1BBCDC,0xCA62C1D6};

sha1::sha1(HANDLE file,DWORD size)
{

		sha1::A = 0x67452301;
		sha1::B = 0xEFCDAB89;
		sha1::C = 0x98BADCFE;
		sha1::D = 0x10325476;
		sha1::E = 0xC3D2E1F0;
	
		for(int i=0;i<41;i++){
			this->sha1hash[i] = 0x0;
		}
		SetFilePointer(file,0,0,0);
		this->realSize = new DWORD;
		this->Size = new DWORD;
		DWORD tempSize = size+10; //注意这里是否存在数组越界，当文件大小刚刚好0 mod 64的时候不会填充0导致数组访问时越界
		while(tempSize%64!=0){
			tempSize++;
		}
		this->buffer = new BYTE[tempSize];
		this->result = new DWORD[5];
		this->M = new DWORD[80];
		ReadFile(file,buffer,size,realSize,NULL);
		this->FillMessage(); 
		//this->ShowMessage();
		for(DWORD i = 0;i<(*(this->Size)/64);i++){
			this->SeparateMessage(i*64);
			this->result[0] = sha1::A;
			this->result[1] = sha1::B;
			this->result[2] = sha1::C;
			this->result[3] = sha1::D;
			this->result[4] = sha1::E;
			for(DWORD j = 0;j<80;j++){
				DWORD temp;
				DWORD temp2 = this->result[0]<<5|this->result[0]>>27;
				switch(j/20){
				case 0:
					temp = (this->result[1]&this->result[2])|((~(this->result[1]))&this->result[3]);
					temp += sha1::k[0];
					break;
				case 1:
					temp = (this->result[1]^this->result[2]^this->result[3]);
					temp += sha1::k[1];
					break;
				case 2:
					temp = (this->result[1]&this->result[2])|(this->result[1]&this->result[3])|(this->result[2]&this->result[3]);
					temp += sha1::k[2];
					break;
				case 3:
					temp = (this->result[1]^this->result[2]^this->result[3]);
					temp += sha1::k[3];
					break;
				}
				temp += temp2 + result[4] + M[j];
				this->result[4] = this->result[3];
				this->result[3] = this->result[2];
				this->result[2] = this->result[1]<<30 | this->result[1]>>2;
				this->result[1] = this->result[0];
				this->result[0] = temp;
			}
			sha1::A += result[0];
			sha1::B += result[1];
			sha1::C += result[2];
			sha1::D += result[3];
			sha1::E += result[4];

		}
		//printf("%0X%0X%0X%0X%0X\n",sha1::A,sha1::B,sha1::C,sha1::D,sha1::E);
		sprintf(sha1hash,"%08x%08x%08x%08x%08x",sha1::A,sha1::B,sha1::C,sha1::D,sha1::E);
		/*for(DWORD i = 0;i<20;i++){
			printf("%02X",*(((BYTE*)&sha1::A)+i));
		}*/
	
}


sha1::~sha1(void)
{
	
		delete[] this->buffer;
		delete[] this->M;
		delete[] this->result;
		delete this->realSize;
		delete this->Size;
	
}


void sha1::FillMessage(){
	DWORD pos = *(this->realSize);
	if(*(this->realSize)% 64 != 56){
		buffer[pos++] = 0x80;
	}
	while(pos % 64 != 56){
		buffer[pos++] = 0;
	}
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = (*(this->realSize)*8 & 0xFF000000)/0x1000000;
	buffer[pos++] = (*(this->realSize)*8 & 0x00FF0000)/0x10000;
	buffer[pos++] = (*(this->realSize)*8 & 0x0000FF00)/0x100;
	buffer[pos++] = *(this->realSize)*8 & 0x000000FF;
	*(this->Size) = pos;
}

void sha1::ShowMessage(){
	for(DWORD i = 0;i<*(this->Size);i++){
	    
		if(i!=0 && i%16==0){
		    printf("\n");
		}
		printf("%02X ",buffer[i]);
	}
	//printf("\n%d",*(this->Size));
}


void sha1::SeparateMessage(DWORD begin){
	
	for(DWORD i = 0;i<16;i++){
	   // this->M[i] = buffer[begin]|buffer[begin+1]*0x100|buffer[begin+2]*0x10000|buffer[begin+3]*0x1000000;
		this->M[i] = buffer[begin]*0x1000000|buffer[begin+1]*0x10000|buffer[begin+2]*0x100|buffer[begin+3];
		/*	if(i%4==0)
			printf("\n");
		printf("%X ",this->M[i]);*/
		begin+=4;
	}
	for(DWORD i = 16;i<80;i++){
		DWORD temp = this->M[i-3]^this->M[i-8]^this->M[i-14]^this->M[i-16];
		this->M[i] = temp<<1 | temp>>31;
	}
}