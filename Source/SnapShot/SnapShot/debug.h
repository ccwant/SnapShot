#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifndef __AFX_H__
#if defined _DEBUG
#include "stdafx.h"
#include <stdio.h> /* vsprintf */
#include <crtdbg.h>

#define DPRINTF_BUF_SZ 1024
#define ASSERT(expr)\
	do{\
	if (!(expr)&&(1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expr)))\
{__asm { int 3 };}\
	}while(0) 
static __inline void TRACE(char *fmt, ...)
{
	va_list args;
	char buf[DPRINTF_BUF_SZ];
	va_start(args, fmt);
	vsprintf_s(buf, fmt, args);

	CString str = CString(buf);  
	USES_CONVERSION;  
	LPCWSTR wszClassName = A2CW(W2A(str));  
	str.ReleaseBuffer();  

	OutputDebugString(str);
	// FILE *fp = fopen("d:\\test.txt","a");
	// fwrite(buf,strlen(buf),1,fp);
	// fclose(fp);
}
#else
static __inline void TRACE(char *fmt, ...) {}
#define ASSERT(expr) if (expr) {NULL;}
#endif
#endif
#endif /* _DEBUG_H_ */