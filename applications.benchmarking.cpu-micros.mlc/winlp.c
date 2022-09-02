
// This is sample code taken from msdn.microsoft.com (http://msdn.microsoft.com/en-us/library/windows/desktop/aa366543(v=vs.85).aspx)
// to enable large pages


#include <windows.h>
#include <stdio.h>

BOOL Privilege(char* pszPrivilege, BOOL bEnable)
{
	HANDLE           hToken;
	TOKEN_PRIVILEGES tp;
	BOOL             status;
	DWORD            error;

	// open process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return FALSE;
	}

	// get the luid
	if (!LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid)) {
		CloseHandle(hToken);
		return FALSE;
	}

	tp.PrivilegeCount = 1;

	// enable or disable privilege
	if (bEnable) {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	} else {
		tp.Privileges[0].Attributes = 0;
	}

	// enable or disable privilege
	status = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	// It is possible for AdjustTokenPrivileges to return TRUE and still not succeed.
	// So always check for the last error value.
	error = GetLastError();

	if (!status || (error != ERROR_SUCCESS)) {
		CloseHandle(hToken);
		return FALSE;
	}

	// close the handle
	CloseHandle(hToken);
	return TRUE;
}

BOOL enable_windows_large_pages()
{
	if (!Privilege("SeLockMemoryPrivilege", TRUE)) {
		printf("Unable to enable large page allocation\n");
		return FALSE;
	}

	return TRUE;
}
