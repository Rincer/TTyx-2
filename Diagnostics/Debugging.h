#ifndef _DEBUGGING_H_
#define _DEBUGGING_H_

void DebugPrintf(const char* format, ...);

bool FailAssert(const char* Expression);

#define Assert(Expression) (void)( (!!(Expression)) || FailAssert(#Expression) )


#endif _DEBUGGING_H_