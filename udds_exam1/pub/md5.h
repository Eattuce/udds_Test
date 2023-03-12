#pragma once

#include <iostream>

typedef unsigned int int_32;

typedef unsigned char md5byte;
typedef int_32 UWORD32;

struct MD5Context {//*MD5�ṹ
	UWORD32 buf[4];
	UWORD32 bytes[2];
	UWORD32 in[16];
};

//--internal
static void MD5Init(struct MD5Context* context);//* ��ʼ��MD5�ṹ
static void MD5Update(struct MD5Context* context, md5byte const* buf, unsigned len); //* ����Ҫ����MD5������
static void MD5Final(struct MD5Context* context, unsigned char digest[16]); //* ��������MD5ֵ
static void MD5Transform(UWORD32 buf[4], UWORD32 const in[16]);
static void byteSwap(UWORD32* buf, unsigned words); //* ����ת��
//.end internal

//* ���㲢����data��Ӧ��MD5 - 16�����ַ�
std::string GetDataMD5(const std::string data);
std::string GetDataMD5(unsigned long d);