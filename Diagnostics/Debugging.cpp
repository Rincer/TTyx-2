#include "stdafx.h"

void DebugPrintf(const char* format, ...)
{ 
	va_list args;
	char Str[256]; 
	va_start(args, format);
	sprintf_s(Str, 255, format, args);
	va_end(args);
	OutputDebugStringA(Str); 
}

bool FailAssert(const char* Expression)
{
	DebugPrintf("Assertion failed (%s)\n",Expression);
	__debugbreak(); 
	return false;
}
