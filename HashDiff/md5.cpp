#include "StdAfx.h"
#include "md5.h"


DWORD md5::A = 0x67452301;
DWORD md5::B = 0xEFCDAB89;
DWORD md5::C = 0x98BADCFE;
DWORD md5::D = 0x10325476;


DWORD md5::T[64] = {
	0xD76AA478,0xE8C7B756,0x242070DB,0xC1BDCEEE,0xF57C0FAF,0x4787C62A,0xA8304613,0xFD469501,
	0x698098D8,0x8B44F7AF,0xFFFF5BB1,0x895CD7BE,0x6B901122,0xFD987193,0xA679438E,0x49B40821,
	0xF61E2562,0xC040B340,0x265E5A51,0xE9B6C7AA,0xD62F105D,0x02441453,0xD8A1E681,0xE7D3FBC8,
	0x21E1CDE6,0xC33707D6,0xF4D50D87,0x455A14ED,0xA9E3E905,0xFCEFA3F8,0x676F02D9,0x8D2A4C8A,
	0xFFFA3942,0x8771F681,0x6D9D6122,0xFDE5380C,0xA4BEEA44,0x4BDECFA9,0xF6BB4B60,0xBEBFBC70,
	0x289B7EC6,0xEAA127FA,0xD4EF3085,0x04881D05,0xD9D4D039,0xE6DB99E5,0x1FA27CF8,0xC4AC5665,
	0xF4292244,0x432AFF97,0xAB9423A7,0xFC93A039,0x655B59C3,0x8F0CCC92,0xFFEFF47D,0x85845DD1,
	0x6FA87E4F,0xFE2CE6E0,0xA3014314,0x4E0811A1,0xF7537E82,0xBD3AF235,0x2AD7D2BB,0xEB86D391
	};

DWORD md5::s[64]={
	7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
	5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
	4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
	6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
};

DWORD md5::m[64]={
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12,
	5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2,
	0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9
};

md5::md5(HANDLE file,DWORD size)
{	

	  for(int i=0;i<33;i++){
			this->md5hash[i] = 0x0;
		}


		DWORD tempSize = size+10;
		while(tempSize%64!=0){
			tempSize++;
		}
		this->buffer = new BYTE[tempSize+1];
		
		this->realSize = new DWORD;
		this->Size = new DWORD;
		this->result = new DWORD[4];
		this->M = new DWORD[16];

		ReadFile(file,buffer,size,realSize,NULL);
		this->FillMessage();
//		this->ShowMessage();
		md5::A = 0x67452301;
		md5::B = 0xEFCDAB89;
		md5::C = 0x98BADCFE;
		md5::D = 0x10325476;
		
		for(DWORD i = 0;i<(*(this->Size) / 64);i++){	//每一次处理64字节（512位）
			this->result[0] = md5::A;
			this->result[1] = md5::B;
			this->result[2] = md5::C;
			this->result[3] = md5::D;
		
		    this->SeparateMessage(i*64);
			for(DWORD j=0;j<64;j+=4){
				switch(j/16){
				case 0:
					FF(this->result[0],this->result[1],this->result[2],this->result[3],M[m[j]],s[j],T[j]);
					FF(this->result[3],this->result[0],this->result[1],this->result[2],M[m[j+1]],s[j+1],T[j+1]);
					FF(this->result[2],this->result[3],this->result[0],this->result[1],M[m[j+2]],s[j+2],T[j+2]);
					FF(this->result[1],this->result[2],this->result[3],this->result[0],M[m[j+3]],s[j+3],T[j+3]);
					break;
				case 1:
					GG(this->result[0],this->result[1],this->result[2],this->result[3],M[m[j]],s[j],T[j]);
					GG(this->result[3],this->result[0],this->result[1],this->result[2],M[m[j+1]],s[j+1],T[j+1]);
					GG(this->result[2],this->result[3],this->result[0],this->result[1],M[m[j+2]],s[j+2],T[j+2]);
					GG(this->result[1],this->result[2],this->result[3],this->result[0],M[m[j+3]],s[j+3],T[j+3]);
					break;
				case 2:
					HH(this->result[0],this->result[1],this->result[2],this->result[3],M[m[j]],s[j],T[j]);
					HH(this->result[3],this->result[0],this->result[1],this->result[2],M[m[j+1]],s[j+1],T[j+1]);
					HH(this->result[2],this->result[3],this->result[0],this->result[1],M[m[j+2]],s[j+2],T[j+2]);
					HH(this->result[1],this->result[2],this->result[3],this->result[0],M[m[j+3]],s[j+3],T[j+3]);
					break;
				case 3:
					II(this->result[0],this->result[1],this->result[2],this->result[3],M[m[j]],s[j],T[j]);
					II(this->result[3],this->result[0],this->result[1],this->result[2],M[m[j+1]],s[j+1],T[j+1]);
					II(this->result[2],this->result[3],this->result[0],this->result[1],M[m[j+2]],s[j+2],T[j+2]);
					II(this->result[1],this->result[2],this->result[3],this->result[0],M[m[j+3]],s[j+3],T[j+3]);
				}
			}
			md5::A = this->result[0] + md5::A;
			md5::B = this->result[1] + md5::B;
			md5::C = this->result[2] + md5::C;
			md5::D = this->result[3] + md5::D;

		}
		for(DWORD i = 0;i<16;i++){
			//printf("%02X",*(((BYTE*)&md5::A)+i));
			char buffer[3] = {0x0};
			sprintf((char*)buffer,"%02x",*(((BYTE*)&md5::A)+i));
			strcat(this->md5hash,buffer);
		}

		file = NULL;
}


md5::~md5(void)
{
	
		delete[] this->buffer;
		delete[] this->result;
		delete[] this->M;
		delete this->realSize;
		delete this->Size;
}

void md5::FillMessage(){
	DWORD pos = *(this->realSize);
	if(*(this->realSize)% 64 != 56){
		buffer[pos++] = 0x80;
	}
	while(pos % 64 != 56){
		buffer[pos++] = 0;
	}
	buffer[pos++] = *(this->realSize)*8 & 0x000000FF;
	buffer[pos++] = (*(this->realSize)*8 & 0x0000FF00)/0x100;
	buffer[pos++] = (*(this->realSize)*8 & 0x00FF0000)/0x10000;
	buffer[pos++] = (*(this->realSize)*8 & 0xFF000000)/0x1000000;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	*(this->Size) = pos;
}

void md5::ShowMessage(){
	for(DWORD i = 0;i<*(this->Size);i++){
	    
		if(i!=0 && i%16==0){
		    printf("\n");
		}
		printf("%02X ",buffer[i]);
	}
	//printf("\n%d",*(this->Size));
}


void md5::SeparateMessage(DWORD begin){
	
	for(DWORD i = 0;i<16;i++){
	    this->M[i] = buffer[begin]|buffer[begin+1]*0x100|buffer[begin+2]*0x10000|buffer[begin+3]*0x1000000;
	/*	if(i%4==0)
			printf("\n");
		printf("%X ",this->M[i]);*/
		begin+=4;
	}
}

DWORD md5::F(DWORD X,DWORD Y,DWORD Z){
	return (X&Y)|((~X)&Z);
}

DWORD md5::G(DWORD X,DWORD Y,DWORD Z){
	return (X&Z)|(Y&(~Z));
}

DWORD md5::H(DWORD X,DWORD Y,DWORD Z){
	return X^Y^Z;
}

DWORD md5::I(DWORD X,DWORD Y,DWORD Z){
	return Y^(X|(~Z));
}

void md5::FF(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti){
	DWORD temp = F(b,c,d) + a + mj + ti;
	temp = (temp<<s)|(temp>>(32-s));
	a = b + temp; 
}

void md5::GG(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti){
	DWORD temp = G(b,c,d) + a + mj + ti;
	temp = (temp<<s)|(temp>>(32-s));
	a = b + temp; 
}

void md5::HH(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti){
	DWORD temp = H(b,c,d) + a + mj + ti;
	temp = (temp<<s)|(temp>>(32-s));
	a = b + temp; 
}

void md5::II(DWORD &a,DWORD &b,DWORD &c,DWORD &d,DWORD mj,DWORD s,DWORD ti){
	DWORD temp = I(b,c,d) + a + mj + ti;
	temp = (temp<<s)|(temp>>(32-s));
	a = b + temp; 
}

