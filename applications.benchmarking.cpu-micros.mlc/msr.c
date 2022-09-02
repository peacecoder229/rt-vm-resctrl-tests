#ifdef LINUX
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>

#include "types.h"
#include "msr.h"
#include "topology.h"
#include "common.h"

// writes the buffer pointed to by 'value' to the MSRs at 'address', again assumption is that the buffer has an entry for each os_cpu

/* THE following functions read_msr, write_msr are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

 */
int write_msr(unsigned int address, uint64_t* value, int latency_core)
{
    int err = 0;
    char  msr_path[32];
    int fp;
    snprintf(msr_path, sizeof(msr_path)-1, "/dev/cpu/%d/msr", latency_core);
	msr_path[sizeof(msr_path)-1] = 0; // make sure to terminate just in case the src buffer is too big
    
    if ((fp = open(msr_path, O_WRONLY)) < 0) {
		if (print_verbose1) 
			printf("MSR module not detected. Try 'modprobe msr'");
		return 0;
    }
	else
		err = pwrite(fp, value, sizeof(uint64_t), address);
    if (err != sizeof(value)) {
		if (print_verbose1) 
			printf("MSR fwrite error!\n");
		close(fp);
		return 0;
    }
    if (fp >=0) close(fp);
    return err;
}

// This function won't log any error as we use this to read SMI counts
// Does not matter to log if this read fails as this is only advisory in nature
int read_msr_without_fail(unsigned int address, UINT64* value, int latency_core)
{
    int fp;
    int err = 0;
    char msr_path[32];
    snprintf(msr_path, sizeof(msr_path)-1, "/dev/cpu/%d/msr", latency_core);
	msr_path[sizeof(msr_path)-1] = 0; // make sure to terminate just in case the src buffer is too big

	*value=0;
    
    if ((fp = open(msr_path, O_RDONLY)) < 0) {
		return -1;
    }
    
    err = pread(fp, value, sizeof(uint64_t), address);
    if(err != sizeof(value)) {
		close(fp);
		return -1;
    }
    
#ifdef DEBUG
    printf("MSR for core %2d (socket %d) reads %2d \n", os_map[i].os_id, os_map[i].pkg_id, *value);
#endif
    //if (fp != NULL) fclose(fp);
    close(fp);
    return err;
}

// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr(unsigned int address, uint64_t* value, int latency_core)
{
    int fp;
    int err = 0;
    char msr_path[32];
    snprintf(msr_path, sizeof(msr_path)-1, "/dev/cpu/%d/msr", latency_core);
	msr_path[sizeof(msr_path)-1] = 0; // make sure to terminate just in case the src buffer is too big
    
    if ((fp = open(msr_path, O_RDONLY)) < 0) {
		if (print_verbose1) {
			printf("Failed to open /dev/cpu/%d/msr\n", latency_core);
		}
        return 0;
    }
    
    err = pread(fp, value, sizeof(uint64_t), address);
    if(err != sizeof(value)) {
		if (print_verbose1)
			printf("MSR fread error!\n");
		close(fp);
        return 0;
    }
    close(fp);
    return err;
}

// writes the buffer pointed to by 'value' to the MSRs at 'address', again assumption is that the buffer has an entry for each os_cpu
int write_msr_all(unsigned int address, uint64_t* value)
{
    int err = 0;
    int i = 0;
    
    for (i = 0; i < os_cpu_count; i++) {
        write_msr(address,value,i);
        value++;
    }
    
    return err;
}


// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr_all(unsigned int address, uint64_t* value)
{
    int err = 0;
    int i = 0;
    
    for (i = 0; i < os_cpu_count; i++) {
        read_msr(address,value,i);
        value++;
    }
    
    return err;
}

#elif defined (__APPLE__)
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>


#include "types.h"
#include "msr.h"
#include "topology.h"

//#include <sys/types.h>
#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#include "MLCDriverClientInterface.h"
#include "common.h"

int os_cpu_count;
int write_msr(unsigned int address, uint64_t* value, int latency_core)
{
    int err = 0;
    
    kern_return_t kernResult = writeMSR(myConnect, latency_core, address, value);
    if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "[error] writeMSR returned 0x%08x.\n", kernResult);
        return 0;
    }
    
#ifdef DEBUG
    printf("MSR %x for core %2d reads 0x%llx \n", address, latency_core, *value);
#endif
    
    return err;
}

// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr(unsigned int address, uint64_t* value, int latency_core)
{
    int err = 0;
    
    kern_return_t kernResult = readMSR(myConnect, latency_core, address, value);
    if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "[error] readMSR returned 0x%08x.\n", kernResult);
        return 0;
    }
    
#ifdef DEBUG
    printf("MSR %x for core %2d reads 0x%llx \n", address, latency_core, *value);
#endif
    
    return err;
}

// writes the buffer pointed to by 'value' to the MSRs at 'address', again assumption is that the buffer has an entry for each os_cpu
int write_msr_all(unsigned int address, uint64_t* value)
{
    int err = 0;
    
    int i = 0;
    
    kern_return_t kernResult = writeMSRAll(myConnect, os_cpu_count, address, value);
    if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "[error] writeMSRAll returned 0x%08x.\n", kernResult);
        return 0;
    }
    
    for (i = 0; i < os_cpu_count; i++) {
#ifdef DEBUG
        printf("MSR %x for core %2d wrote 0x%llx \n", address, i, *value);
#endif
        value++;
        
    }
    
    return err;
}


// reads the MSR address, for all os_cpus, using the buffer pointed to by value for storing the MSR values. It is assumed that the buffer is big enough to hold one entry for each os_cpu
int read_msr_all(unsigned int address, uint64_t* value)
{
    int err = 0;
    int i = 0;
    
    kern_return_t kernResult = readMSRAll(myConnect, os_cpu_count, address, value);
    if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "[error] readMSRAll returned 0x%08x.\n", kernResult);
        return 0;
    }
    
    for (i = 0; i < os_cpu_count; i++) {
#ifdef DEBUG
        printf("MSR %x for core %2d reads 0x%llx \n", address, i, *value);
#endif
        value++;
        
    }
    return err;
}

#endif


