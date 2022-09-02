#ifndef LINUX
#include <windows.h>
#include <minwindef.h>

#include "types.h"
#include "msr.h"
#include "topology.h"
#include "mlcdrv-inc.h"

extern int os_cpu_count;

// writes the buffer pointed to by 'value' to the MSRs at 'address', again assumption is that the buffer has an entry for each os_cpu

/* THE following functions read_msr, write_msr are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

 */

static BOOL win_mlc_driver_initialized = FALSE;
static CMDBUF CmdBuf;
static HANDLE hMlcDriver;

BOOL init_windows_msr_access()
{
	hMlcDriver = initializeWindowsMlcDriver();
	if (INVALID_HANDLE_VALUE == hMlcDriver) {
		return FALSE;
	}
	win_mlc_driver_initialized = TRUE;
	return TRUE;
}

BOOL close_windows_msr_access()
{
	if (win_mlc_driver_initialized) {
		closeWindowsMlcDriver(hMlcDriver);
		win_mlc_driver_initialized = FALSE;
	}

	return TRUE;
}

int write_msr(unsigned int address, UINT64* value, int latency_core)
{
	GROUP_AFFINITY ga = {0};
	getCurrentAffinity(&ga); // save current affinity
	// bind to the correct cpu so MLC driver can access its MSR
	BindToCpu(latency_core);
	CmdBuf.opcode = WRITE_MSR;
	CmdBuf.reg_reset_value = *value;
	accessWindowsMlcDriver(hMlcDriver, &CmdBuf);
	setCurrentAffinity(ga);
	return TRUE;
}

// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr(unsigned int address, UINT64* value, int latency_core)
{
	GROUP_AFFINITY ga = {0};
	getCurrentAffinity(&ga); // save current affinity
	// bind to the correct cpu so MLC driver can access its MSR
	BindToCpu(latency_core);
	CmdBuf.opcode = READ_MSR;
	accessWindowsMlcDriver(hMlcDriver, &CmdBuf);
	*value = CmdBuf.result;
	setCurrentAffinity(ga);
	return TRUE;
}

// writes the buffer pointed to by 'value' to the MSRs at 'address', again assumption is that the buffer has an entry for each os_cpu
int write_msr_all(unsigned int address, UINT64* value)
{
	GROUP_AFFINITY ga = {0};
	int i;
	getCurrentAffinity(&ga); // save current affinity

	for (i = 0; i < os_cpu_count; i++) {
		BindToCpu(i);
		CmdBuf.opcode = WRITE_MSR;
		CmdBuf.reg_reset_value = *value++;
		accessWindowsMlcDriver(hMlcDriver, &CmdBuf);
	}

	setCurrentAffinity(ga);
	return TRUE;
}


// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr_all(unsigned int address, UINT64* value)
{
	GROUP_AFFINITY ga = {0};
	int i;
	getCurrentAffinity(&ga); // save current affinity

	for (i = 0; i < os_cpu_count; i++) {
		BindToCpu(i);
		CmdBuf.opcode = READ_MSR;
		accessWindowsMlcDriver(hMlcDriver, &CmdBuf);
		*value = CmdBuf.result;
		value++;
	}

	setCurrentAffinity(ga);
	return TRUE;
}

#endif
