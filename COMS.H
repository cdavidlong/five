#ifndef __coms_h__    
#define __coms_h__
#include <windows.h>

//Serial comms data
LPBYTE gloveData;
extern BOOL FAR PASCAL GloveInit(LPCSTR lpszPort);
extern BYTE FAR PASCAL GloveGetData(int nIndex);
extern LPBYTE FAR PASCAL GloveGetDataPtr(void);

#endif