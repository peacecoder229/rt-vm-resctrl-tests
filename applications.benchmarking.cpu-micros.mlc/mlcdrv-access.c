/*++

Copyright (c) 1990-98  Microsoft Corporation All Rights Reserved

Module Name:

    testapp.c

Abstract:

Environment:

    Win32 console multi-threaded application

--*/
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strsafe.h>
#include "sioctl.h"
#include "mlcdrv-inc.h"
#include "types.h"
#include "common.h"

BOOL win_mlc_driver_initialized = FALSE;

BOOLEAN
ManageDriver(
    __in LPCTSTR  DriverName,
    __in LPCTSTR  ServiceName,
    __in USHORT   Function
);

BOOLEAN
SetupDriverName(
    __inout_bcount_full(BufferLength) PCHAR DriverLocation,
    __in ULONG BufferLength
);

static UCHAR driverLocation[MAX_PATH];

HANDLE initializeWindowsMlcDriver()
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	BOOL bRc;
	ULONG bytesReturned;
	DWORD errNum = 0;
	CMDBUF CmdBuf;
	ULONG i;
	win_mlc_driver_initialized = TRUE;

	if ((hDevice = CreateFile("\\\\.\\mlcdrvTest",
	                          GENERIC_READ | GENERIC_WRITE,
	                          0,
	                          NULL,
	                          CREATE_ALWAYS,
	                          FILE_ATTRIBUTE_NORMAL,
	                          NULL)) == INVALID_HANDLE_VALUE) {
		errNum = GetLastError();

		if (errNum != ERROR_FILE_NOT_FOUND) {
			if (print_verbose1) 
				printf("CreateFile failed!  ERROR_FILE_NOT_FOUND = %d\n", errNum);
			return INVALID_HANDLE_VALUE;
		}

		//
		// The driver is not started yet so let us the install the driver.
		// First setup full path to driver name.
		//

		if (!SetupDriverName(driverLocation, sizeof(driverLocation))) {
			return INVALID_HANDLE_VALUE;
		}

		if (!ManageDriver(DRIVER_NAME,
		                  driverLocation,
		                  DRIVER_FUNC_INSTALL
		                 )) {
			if (print_verbose1) 
				printf("Unable to install driver. \n");
			//
			// Error - remove driver.
			//
			ManageDriver(DRIVER_NAME,
			             driverLocation,
			             DRIVER_FUNC_REMOVE
			            );
			return INVALID_HANDLE_VALUE;
		}

		hDevice = CreateFile("\\\\.\\mlcdrvTest",
		                     GENERIC_READ | GENERIC_WRITE,
		                     0,
		                     NULL,
		                     CREATE_ALWAYS,
		                     FILE_ATTRIBUTE_NORMAL,
		                     NULL);

		if (hDevice == INVALID_HANDLE_VALUE) {
			if (print_verbose1) 
				printf("Error: CreatFile Failed : %d\n", GetLastError());
			return INVALID_HANDLE_VALUE;
		}
	} // Driver initialization done..

	return hDevice;
}

BOOL accessWindowsMlcDriver(HANDLE hDevice, PCMDBUF pCmdBuf)
{
	BOOL bRc;
	ULONG bytesReturned;
	
	if (print_verbose1) {
		printf("accessWindowsMlcDriver Handle %p : opcode %d  reg %x reg_reset_value %I64x result %I64x\n",
			hDevice,pCmdBuf->opcode,pCmdBuf->reg,pCmdBuf->reg_reset_value,pCmdBuf->result );
	}

	bRc = DeviceIoControl(hDevice,
	                      (DWORD) IOCTL_SIOCTL_METHOD_NEITHER,
	                      pCmdBuf,
	                      sizeof(CMDBUF),
	                      NULL,
	                      0,
	                      &bytesReturned,
	                      NULL
	                     );

	if (!bRc) {
		if (print_verbose1) 
			printf("Error in executing DeviceIoControl : %d\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

void closeWindowsMlcDriver(HANDLE hDevice)
{
	CloseHandle(hDevice);
	//
	// Unload the driver.  Ignore any errors.
	//
	ManageDriver(DRIVER_NAME,
	             driverLocation,
	             DRIVER_FUNC_REMOVE
	            );
	//
	// close the handle to the device.
	//
}


