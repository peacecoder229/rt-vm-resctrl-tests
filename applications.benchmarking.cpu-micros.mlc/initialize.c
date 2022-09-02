#if defined(LINUX) || defined(__APPLE__)
#include <unistd.h>
#include <sys/timeb.h>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>

#else // Windows specific
#include <windows.h>
#include <minwinbase.h>
#include <minwindef.h>
#include <sysinfoapi.h>
#include <stdio.h>
#endif


#include "types.h"
#include "common.h"

#if !defined(LINUX) && !defined(__APPLE__)
CRITICAL_SECTION cs;
#endif

#if defined(LINUX) || defined(__APPLE__)
#define FILE_NAME "/mnt/huge/hugepagefile"
#ifdef __ia64__
#define ADDR (void *)(0x8000000000000000UL)
#define FLAGS (MAP_SHARED | MAP_FIXED)
#else
#define ADDR (void *)(0x0UL)
#define FLAGS (MAP_SHARED)
#endif
#endif

#if defined(LINUX) || defined(__APPLE__)
#define COUNTER "b"
#define arrayPOSITION "d"
#define ENDP "d"
#define baseADDRESS "c"
#define LocalADDRESS "c"
#define RemoteADDRESS "D"
#define WriteADDRESS "S"
#else
#define COUNTER ebx
#define arrayPOSITION rdx
#define ENDP rdx
#define baseADDRESS rcx
#define LocalADDRESS rcx
#define RemoteADDRESS rdi
#define WriteADDRESS rsi
#endif


void Init_Array(char* p1, char* p2, __int64 len);
void Create_random_pattern(__int64** buf, __int64 size);

#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI ThruputMemAllocThread(PVOID Parm);
#else
unsigned int ThruputMemAllocThread(void* Parm);
#endif

__int64* do_dependent_read(__int64* a);
void touch_buffer(char* buf, __int64 len);
char* GetMemory(UINT64 len);
void parse(char* argp);

void Calibrate(UINT64*	ClksPerSec);
void AllocateMemoryForThruputThreads(VOID);
void  check_for_null_parameter(char b, char arg);

#if !defined(LINUX) && !defined(__APPLE__)
// definition of SetPrivilege function

HANDLE process;
HANDLE hToken;
#endif

#if !defined(LINUX) && !defined(__APPLE__)
SYSTEM_INFO		g_si;										// System Information structure
#endif

#if defined(LINUX) || defined(__APPLE__)
#else
HANDLE g_hExitEvent;						// Global exit event handle
HANDLE AllocEvent;
#endif

int read_pattern_file(char* filename);
void populate_cache_patterns();

// Routine to set large page privileges
#if !defined(LINUX) && !defined(__APPLE__)
#endif

// Allocate memory depending on whether large page option is specified or not
// Initialize the array so that memory accesses could be mixed between local
// and remote nodes for dependent load accesses
void Init_Array(char* p1, char* p2, __int64 len)
{
	__int64 j;
	char* p, *lp;
	__int64* initp;
	int local_loop, remote_loop;
	lp = p1;

	switch (NUMA_configured) {
	case 1: // For every 5 local reads, we have one remote read
			local_loop = 5;
		remote_loop = 1;
		break;

	case 2:
			local_loop = 2;
		remote_loop = 1;
		break;

	case 3:
			local_loop = 1;
		remote_loop = 1;
		break;

	default:
			local_loop = 5;
		remote_loop = 1;
		break;
	}

	initp = (__int64*) p1;

	for (;;) {
		for (j = 0; j < local_loop; j++) {
			p = p1 + 128; // we access 128 byte stride so h/w prefetcher does not kick in
			*((int**)(lp)) = (int*)p;
			lp = p;
			p1 = p;
		}

		if (lp >= (char*)initp + len) {
			break;
		}

		for (j = 0; j < remote_loop; j++) {
			p = p2 + 128; // we access 128 byte stride so h/w prefetcher does not kick in
			*((int**)(lp)) = (int*)p;
			lp = p;
			p2 = p;
		}
	}

	*((int**)(lp)) = (int*)initp;
}



// Initialize the entire buffer with random patterns
// Each line points to another line randomly selected and the tail points to the
// head so that the traversal can loop back
void Create_random_pattern(__int64** buf, __int64 size)
{
	__int64 num_blocks, i, j;
	__int64** p, **end_idx=NULL, **blk_start;
	int max_cache_lines;
	
	max_cache_lines = max_rand_size;
	num_blocks = size / (max_rand_size * LineSize_LatThd);
	if (num_blocks <=0) { // the buffer size is too small. Reduce the max_cache_lines to fit
		max_cache_lines = size/LineSize_LatThd;
		num_blocks=1;
		get_rand(rand_array, max_cache_lines);
	}
	p = buf;

	for (i = 0; i < num_blocks; i++) {
		blk_start = p;

		for (j = 0; j < max_cache_lines; j++) {
			if (rand_array[j] == max_cache_lines) {
				end_idx = p;
			}

			*p = (__int64*)(blk_start + (rand_array[j] * (LineSize_LatThd / sizeof(__int64*))));
			p += (LineSize_LatThd / sizeof(__int64*)); // move to next cache line
		}
	}

	if (end_idx) *end_idx = (__int64*)buf;
	
}


// parse the input lines from pattern file and pre-populate internal buffers so we don't need to do sscanf everytime from multiple threads which may not be thread safe
void populate_cache_patterns()
{
	unsigned int i, len, value;
	int lineidx = 0;

	for (i = 0; i < pattern_lines_present; i++) {
		len = 128; // 128 bytes of cache line data in the pattern file

		while (len) {
			//printf("str=%s\n", &pbuf[i][len-8]);
			value = strtol(&pbuf[i][len - 8], NULL, 16); //sscanf(&pbuf[i][len - 8], "%x", &value); // read in 4 bytes at a time
			pattern_4bytes[lineidx++] = value;
			//printf("value[%d]=%lx\n", lineidx-1, value);
			pbuf[i][len - 8] = 0;
			len -= 8;
		}
	}
}

// buffers are initialized with specified patterns
void write_pattern_into_buffer(char* buf, __int64 len)
{
	int* p;
	__int64 num_4byte_lines = len / 4, i;
	int pidx = 0;
	p = (int*)buf;

	for (i = 0; i < num_4byte_lines; i++) {
		p[i] = pattern_4bytes[pidx++];

		if (pidx >= (pattern_lines_present * 16)) {
			pidx = 0;    // when 64 bytes (16 ints) are written go back to the start of the pattern
		}
	}
}

