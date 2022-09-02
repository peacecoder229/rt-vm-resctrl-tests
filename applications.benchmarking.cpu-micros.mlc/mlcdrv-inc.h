#if !defined(LINUX) && !defined(__APPLE__)
#ifndef MLCDRV_H_INC
#define MLCDRV_H_INC
typedef struct _CMDBUF {
	int opcode;
	int	reg;
	ULONG64	reg_reset_value;
	ULONG64	result;

} CMDBUF, *PCMDBUF;


#define READ_MSR	1
#define WRITE_MSR	2

extern void closeWindowsMlcDriver(HANDLE hDevice);
extern int accessWindowsMlcDriver(HANDLE hDevice, PCMDBUF pCmdBuf);
extern HANDLE initializeWindowsMlcDriver();

#endif
#endif
