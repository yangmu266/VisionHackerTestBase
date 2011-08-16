//
//  winTypes.h
//  MultiFace_Mac
//
//  Created by Tang Wenbin on 4/23/11.
//  Copyright 2011 Tsinghua University. All rights reserved.
//

#ifndef __WIN_TYPE__
#define __WIN_TYPE__

typedef unsigned char BYTE;
typedef long long __int64;
typedef int __int32;
typedef short int __int16;

typedef unsigned long ULONG;

typedef long            HRESULT;

#define _HRESULT_TYPEDEF_(x) (x)

#define E_NOTIMPL                                          _HRESULT_TYPEDEF_(0x80004001L)
#define E_NOINTERFACE                                      _HRESULT_TYPEDEF_(0x80004002L)
#define E_POINTER                                          _HRESULT_TYPEDEF_(0x80004003L)
#define E_ABORT                                            _HRESULT_TYPEDEF_(0x80004004L)
#define E_FAIL                                             _HRESULT_TYPEDEF_(0x80004005L)

#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)

//#define min(a,b) ((a) < (b) ? (a) : (b))
//#define max(a,b) ((a) > (b) ? (a) : (b))

//#define __out
//#define __in
//#define __inout 

#define __in_opt 
#define __out_opt
#define __inout_opt 

#define __in_ecount(THING) 
#define __in_ecount_opt(THING) 
#define __out_ecount(THING) 
#define __out_ecount_opt(THING) 
#define __inout_ecount(THING) 
#define __inout_ecount_opt(THING) 

#define __in_bcount_opt(THING) 
#define __out_bcount_opt(THING) 
#define __inout_bcount_opt(THING)


typedef unsigned long DWORD;
typedef unsigned short WORD;

typedef unsigned char UCHAR;

typedef long HMODULE;
#define HRSRC DWORD

typedef struct tagBITMAPFILEHEADER {
    WORD bfType; 
    DWORD bfSize; 
    WORD bfReserved1; 
    WORD bfReserved2; 
    DWORD bfOffBits; 
} BITMAPFILEHEADER; 

#define fopen_s(f,name,mode) *(f)=fopen((name),(mode))
#define strcpy_s(dst,len,src) strcpy((dst),(src))

struct CRect {
    int left, right, top, bottom;
    
    CRect(){}
    CRect(int a, int b, int c, int d) : left(a), top(b), right(c), bottom(d) {}
};

#define _cdecl


//array & list
/*
 GetHeadPosition
 GetNext
 Add
 GetCount
 AddTail
 GetData
 RemoveAll
 SetCount
 RemoveAt
 IsEmpty 
 */

#define HR_RET(exp) {\
if((hr=exp)!=S_OK)\
{\
return(hr);\
}\
}

#endif //__WIN_TYPE__