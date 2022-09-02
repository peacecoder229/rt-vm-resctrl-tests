/*++

Copyright (c) 1990-98  Microsoft Corporation All Rights Reserved

Module Name:

    mlcdrv.c

Abstract:

    Purpose of this driver is to demonstrate how the four different types
    of IOCTLs can be used, and how the I/O manager handles the user I/O
    buffers in each case. This sample also helps to understand the usage of
    some of the memory manager functions.

    Added by Intel : The above driver was used as a template to write to MSRs

Environment:

    Kernel mode only.

--*/


//
// Include files. __readcr0
//


#include <ntddk.h>          // various NT definitions
#include <string.h>
//#include <intrin.h>

#include "sioctl.h"
#include "mlcdrv-inc.h"

#define NT_DEVICE_NAME      L"\\Device\\mlcdrv"
#define DOS_DEVICE_NAME     L"\\DosDevices\\mlcdrvTest"


//#if DBG
#define SIOCTL_KDPRINT(_x_) \
                DbgPrint("SIOCTL.SYS: ");\
                DbgPrint _x_;

//#else
//#define SIOCTL_KDPRINT(_x_)
//#endif


PCMDBUF CmdBuf ;
//
// Device driver routine declarations.
//
NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
);

NTSTATUS
SioctlCreateClose(
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp
);

NTSTATUS
SioctlDeviceControl(
    IN PDEVICE_OBJECT       DeviceObject,
    IN PIRP                 Irp
);

VOID
SioctlUnloadDriver(
    IN PDRIVER_OBJECT       DriverObject
);

/*
DRIVER_INITIALIZE DriverEntry;

DRIVER_DISPATCH SioctlCreateClose;

DRIVER_DISPATCH SioctlDeviceControl;

DRIVER_UNLOAD SioctlUnloadDriver;
*/
VOID
PrintIrpInfo(
    PIRP Irp
);
VOID
PrintChars(
    __in_ecount(CountChars) PCHAR BufferAddress,
    __in ULONG CountChars
);

#ifdef AMD64
#define gettsc()	__rdtsc()
#else
ULONG64 gettsc(void);
#endif
ULONG64 readMSR(int reg);
ULONG64 readMSRall(int reg, __int64 iter);
ULONG64 exec_cr3mov(__int64 iter);
void exec_wbinvd(void);

ULONG64 exec_pause_loops(__int64 regval);
ULONG64 readPMC(int reg);
ULONG64 exec_vmcall();
ULONG64 exec_vmmcall();
ULONG64 togglepge();
void writeMSR(ULONG reg, ULONG64 highval, ULONG64 lowval);
ULONG IORead(ULONG reg);
void IOWrite(ULONG value, ULONG reg);

void fence();
ULONG getcpuid(ULONG reg, ULONG* p1, ULONG* p2, ULONG* p3);

#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( PAGE, SioctlCreateClose)
#pragma alloc_text( PAGE, SioctlDeviceControl)
#pragma alloc_text( PAGE, SioctlUnloadDriver)
#pragma alloc_text( PAGE, PrintIrpInfo)
#pragma alloc_text( PAGE, PrintChars)

#endif // ALLOC_PRAGMA


NTSTATUS
DriverEntry(
    __in PDRIVER_OBJECT   DriverObject,
    __in PUNICODE_STRING      RegistryPath
)
/*++

Routine Description:
    This routine is called by the Operating System to initialize the driver.

    It creates the device object, fills in the dispatch entry points and
    completes the initialization.

Arguments:
    DriverObject - a pointer to the object that represents this device
    driver.

    RegistryPath - a pointer to our Services key in the registry.

Return Value:
    STATUS_SUCCESS if initialized; an error otherwise.

--*/

{
	NTSTATUS        ntStatus;
	UNICODE_STRING  ntUnicodeString;    // NT Device Name "\Device\SIOCTL"
	UNICODE_STRING  ntWin32NameString;    // Win32 Name "\DosDevices\IoctlTest"
	PDEVICE_OBJECT  deviceObject = NULL;    // ptr to device object
	SIOCTL_KDPRINT(("Device init\n"));
	RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);
	ntStatus = IoCreateDevice(
	               DriverObject,                   // Our Driver Object
	               0,                              // We don't use a device extension
	               &ntUnicodeString,               // Device name "\Device\SIOCTL"
	               FILE_DEVICE_UNKNOWN,            // Device type
	               FILE_DEVICE_SECURE_OPEN,     // Device characteristics
	               FALSE,                          // Not an exclusive device
	               &deviceObject);                 // Returned ptr to Device Object

	if (!NT_SUCCESS(ntStatus)) {
		SIOCTL_KDPRINT(("Couldn't create the device object\n"));
		return ntStatus;
	}

	//
	// Initialize the driver object with this driver's entry points.
	//
	DriverObject->MajorFunction[IRP_MJ_CREATE] = SioctlCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = SioctlCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SioctlDeviceControl;
	DriverObject->DriverUnload = SioctlUnloadDriver;
	//
	// Initialize a Unicode String containing the Win32 name
	// for our device.
	//
	RtlInitUnicodeString(&ntWin32NameString, DOS_DEVICE_NAME);
	//
	// Create a symbolic link between our device name  and the Win32 name
	//
	ntStatus = IoCreateSymbolicLink(
	               &ntWin32NameString, &ntUnicodeString);

	if (!NT_SUCCESS(ntStatus)) {
		//
		// Delete everything that this routine has allocated.
		//
		SIOCTL_KDPRINT(("Couldn't create symbolic link\n"));
		IoDeleteDevice(deviceObject);
	}

	return ntStatus;
}


NTSTATUS
SioctlCreateClose(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
)
/*++

Routine Description:

    This routine is called by the I/O system when the SIOCTL is opened or
    closed.

    No action is performed other than completing the request successfully.

Arguments:

    DeviceObject - a pointer to the object that represents the device
    that I/O is to be done on.

    Irp - a pointer to the I/O Request Packet for this request.

Return Value:

    NT status code

--*/

{
	PAGED_CODE();
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

VOID
SioctlUnloadDriver(
    __in PDRIVER_OBJECT DriverObject
)
/*++

Routine Description:

    This routine is called by the I/O system to unload the driver.

    Any resources previously allocated must be freed.

Arguments:

    DriverObject - a pointer to the object that represents our driver.

Return Value:

    None
--*/

{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING uniWin32NameString;
	PAGED_CODE();
	//
	// Create counted string version of our Win32 device name.
	//
	RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);
	//
	// Delete the link from our device name to a name in the Win32 namespace.
	//
	IoDeleteSymbolicLink(&uniWin32NameString);

	if (deviceObject != NULL) {
		IoDeleteDevice(deviceObject);
	}
}


NTSTATUS
SioctlDeviceControl(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
)

/*++

Routine Description:

    This routine is called by the I/O system to perform a device I/O
    control function.

Arguments:

    DeviceObject - a pointer to the object that represents the device
        that I/O is to be done on.

    Irp - a pointer to the I/O Request Packet for this request.

Return Value:

    NT status code

--*/

{
	PIO_STACK_LOCATION  irpSp;// Pointer to current stack location
	NTSTATUS            ntStatus = STATUS_INVALID_PARAMETER;
	ULONG               inBufLength; // Input buffer length
	PCHAR               inBuf; // pointer to Input and output buffer
	PMDL                mdl = NULL;
	PCHAR               buffer = NULL;
	PHYSICAL_ADDRESS	Low, High, pa;
	PHYSICAL_ADDRESS	ioaddr;
	PVOID Buffer, UserVA;
	ULONG64 begin_tsc, end_tsc, total = 0;
	ULONG i;
	int regval, bFence;
	KIRQL oldIrql;
	PAGED_CODE();
	irpSp = IoGetCurrentIrpStackLocation(Irp);
	inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

	if (inBufLength != sizeof(CMDBUF)) {
		goto End;
	}

	//
	// Determine which I/O control code was specified.
	//

	switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
	case IOCTL_SIOCTL_METHOD_NEITHER: {
		inBuf = irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
		CmdBuf = (PCMDBUF)inBuf;

		// Currently we ignore any reg value and just support only reading/writing to MSR 0x1a4
		switch (CmdBuf->opcode) {
		case READ_MSR:
			CmdBuf->result = (ULONG64) __readmsr(0x1a4);
			ntStatus = STATUS_SUCCESS;
			break;

		case WRITE_MSR:
			__writemsr((unsigned long) 0x1a4,  CmdBuf->reg_reset_value);
			ntStatus = STATUS_SUCCESS;
			break;

		default:
			break;
		}

		//
		// Assign the length of the data copied to IoStatus.Information
		// of the Irp and complete the Irp.
		//
		Irp->IoStatus.Information = 0;
		break;
	}

	default:
		//
		// The specified I/O control code is unrecognized by this driver.
		//
		ntStatus = STATUS_INVALID_DEVICE_REQUEST;
		SIOCTL_KDPRINT(("ERROR: unrecognized IOCTL %x\n",
		                irpSp->Parameters.DeviceIoControl.IoControlCode));
		break;
	}

End:
	//
	// Finish the I/O operation by simply completing the packet and returning
	// the same status as in the packet itself.
	//
	Irp->IoStatus.Status = ntStatus;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return ntStatus;
}

VOID
PrintIrpInfo(
    PIRP Irp)
{
	PIO_STACK_LOCATION  irpSp;
	irpSp = IoGetCurrentIrpStackLocation(Irp);
	PAGED_CODE();
	SIOCTL_KDPRINT(("\tIrp->AssociatedIrp.SystemBuffer = 0x%p\n",
	                Irp->AssociatedIrp.SystemBuffer));
	SIOCTL_KDPRINT(("\tIrp->UserBuffer = 0x%p\n", Irp->UserBuffer));
	SIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.Type3InputBuffer = 0x%p\n",
	                irpSp->Parameters.DeviceIoControl.Type3InputBuffer));
	SIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.InputBufferLength = %d\n",
	                irpSp->Parameters.DeviceIoControl.InputBufferLength));
	SIOCTL_KDPRINT(("\tirpSp->Parameters.DeviceIoControl.OutputBufferLength = %d\n",
	                irpSp->Parameters.DeviceIoControl.OutputBufferLength));
	return;
}

VOID
PrintChars(
    __in_ecount(CountChars) PCHAR BufferAddress,
    __in ULONG CountChars
)
{
	PAGED_CODE();

	if (CountChars) {
		while (CountChars--) {
			if (*BufferAddress > 31
			        && *BufferAddress != 127) {
				KdPrint(("%c", *BufferAddress));
			} else {
				KdPrint(("."));
			}

			BufferAddress++;
		}

		KdPrint(("\n"));
	}

	return;
}


