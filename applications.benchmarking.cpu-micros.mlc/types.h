// (C) 2012-2014 Intel Corporation
//
// - initial version extracted main (Thomas Willhalm)

#ifndef TYPES_INC
#define TYPES_INC

#if defined(LINUX) || defined(__APPLE__)
#define VOID
#endif
typedef unsigned long long int UINT64;
#if defined(LINUX) || defined(__APPLE__)
typedef long int __int64;
#else
typedef unsigned long long int uint64_t;
#endif
typedef unsigned long DWORD;
typedef unsigned int UINT;
typedef int BOOL;

#define MY_ERROR -1


#endif
