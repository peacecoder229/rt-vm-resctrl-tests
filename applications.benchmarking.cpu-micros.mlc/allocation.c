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
#include <glob.h>
#include <sys/syscall.h>

#else // Windows specific
#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <process.h>
#include <Psapi.h>
#endif


#include "types.h"
#include "common.h"

char* mmap_files_dir = NULL;
size_t mmap_files_dir_len = 0;
int pmem_only = 0;

#if defined(LINUX) || defined(__APPLE__)
extern pthread_mutex_t getVirtAddrMutex;
extern pthread_mutex_t serialize_buf_alloc_mutex;
#else
extern int h_provided;
CRITICAL_SECTION cs;
#endif

static int is_pmem_proc(void *addr, size_t len);

void evict_line_from_cache(volatile void *ptr);
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
unsigned long getNextVirtAddr(UINT64 len);
void AllocateMemoryForThruputThreads(VOID);

struct _params params[MAX_THREADS] = { 0 };
void AllocDependentLoadMemory(struct _params* p);
int volatile numThruputThreads=0;

#if defined(LINUX) || defined(__APPLE__)
#else
HANDLE g_hExitEvent;						// Global exit event handle
HANDLE AllocEvent;
#endif

#if defined(LINUX) || defined(__APPLE__)
#else

/*
https://docs.microsoft.com/en-us/windows/desktop/api/psapi/nf-psapi-queryworkingsetex

:-( Buffer needs to be touched before QueryWorkingSetEx Needs to be called :-(
Not documented properly

*/

typedef struct BufStats {
	UINT64 InvalidPSPIWSInfo;
	UINT64 DidNotMeetNodeRequirement ;
	UINT64 DidNotMeetWin32Protection;
	UINT64 LockedPages;
	UINT64 SharedPages;
	UINT64 BadPages;
	UINT64 SmallPages;
	UINT64 LargePages;
	UINT64 HugePages;
	UINT64 InvalidSizePages ;
}BufStats;

void LogBufferInfo(int NumaNode,char * buf, UINT64 len)
{
	if (print_verbose1 && NULL != buf && 0 != len)
	{
		//clear the Buffer Stats
		BufStats stats = { 0 };
		SYSTEM_INFO SystemInfo = { 0 };

		touch_buffer(buf, len);

		GetSystemInfo(&SystemInfo);
		UINT64 NumPages = len / SystemInfo.dwPageSize;

		PPSAPI_WORKING_SET_EX_INFORMATION BufInfo = (PPSAPI_WORKING_SET_EX_INFORMATION) malloc(NumPages * sizeof(PSAPI_WORKING_SET_EX_INFORMATION));

		if(NULL == BufInfo)
		{
			printf("LogBufferInfo: Unable to allocatr BufInfo of Size %I64d \n", NumPages * sizeof(PSAPI_WORKING_SET_EX_INFORMATION));
			return;
		}

		for (UINT64 i = 0; i < NumPages; i++)
		{
			BufInfo[i].VirtualAddress = buf + (i * SystemInfo.dwPageSize);
		}

		if (!QueryWorkingSetEx(GetCurrentProcess(), BufInfo, NumPages * sizeof(PSAPI_WORKING_SET_EX_INFORMATION)))
		{
			printf("QueryWorkingSetEx failed: %ld\n", GetLastError());
			free(BufInfo);
			return;
		}

		for (UINT64 i = 0; i < NumPages; i++)
		{
			if (BufInfo[i].VirtualAttributes.Valid)
			{
				//
				if (BufInfo[i].VirtualAttributes.Node != NumaNode)
				{
					stats.DidNotMeetNodeRequirement++;
				}

				//
				if (BufInfo[i].VirtualAttributes.Bad)
				{
					stats.BadPages++;
				}

				//
				if (BufInfo[i].VirtualAttributes.Shared)
				{
					stats.SharedPages++;
				}

				//
				if (BufInfo[i].VirtualAttributes.Locked)
				{
					stats.LockedPages++;
				}

				//
				if (BufInfo[i].VirtualAttributes.LargePage)
				{
					stats.LargePages++;
				}
				else
				{
					stats.SmallPages++;
				}

				//
			}
			else
			{
				stats.InvalidPSPIWSInfo++;
			}
		}

		printf("LogBufferInfo: buf %p NumaNode %d InvalidPSPIWSInfo %I64d DidNotMeetNodeRequirement %I64d BadPages %I64d SharedPages %I64d LockedPages %I64d LargePages %I64d SmallPages %I64d \n ",
			buf,
			NumaNode,
			stats.InvalidPSPIWSInfo,
			stats.DidNotMeetNodeRequirement,
			stats.BadPages,
			stats.SharedPages,
			stats.LockedPages,
			stats.LargePages,
			stats.SmallPages
			);

		free(BufInfo);
	}
}
#endif


void AllocateMemoryForThruputThreads(VOID)
{
	int i;
#if !defined(LINUX) && !defined(__APPLE__)
  int tId;
#endif
#if !defined(LINUX) && !defined(__APPLE__)
	PHANDLE* ThreadData;
#else
	pthread_t* ThreadData;
	pthread_attr_t attr;
	int rc;
#endif
#if !defined(LINUX) && !defined(__APPLE__)
	AllocEvent = CreateEvent(NULL, TRUE, FALSE,	0);
#else
	pthread_mutex_init(&AllocMutex, NULL);
	assert (pthread_cond_init(&AllocCond, NULL) ==0);
#endif
	numThruputThreads = 0;
#if !defined(LINUX) && !defined(__APPLE__)

	// Allocate storage for our thread handle array
	if ((ThreadData = (PHANDLE*)
	                  HeapAlloc(GetProcessHeap(),
	                            0,
	                            sizeof(PHANDLE) * MAX_THREADS)) == NULL) {
		printf("g_phThread HeapAlloc error!\n");
		return;
	}

#else

	if ((ThreadData = (pthread_t*) malloc(sizeof(pthread_t) * MAX_THREADS)) == NULL) {
		printf("g_phThread malloc error!\n");
		return;
	}
	if (pthread_attr_init(&attr) !=0)
		app_exit (EACCES, "pthread_attr_init error\n");
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
		app_exit (EACCES, "pthread_attr_setdetachstate error\n");
#endif

	for (i = 0; i < numcpus; i++) {
		// set all values based on specified inputs
		// we should execute the following 15 lines of code only in loaded_latency_mode?
		params[i].local_id = cpu_list[i];
		params[i].buf_size=buf_sizes[cpu_list[i]];
		params[i].rw_mix=traffic_pattern[cpu_list[i]];
		params[i].randombw=randombw[cpu_list[i]];
		params[i].src_location_id = remote_source[cpu_list[i]];
		params[i].src_location_id_2 = remote_source_2[cpu_list[i]];
		params[i].address_stream_mix_ratio = address_stream_mix_ratio[cpu_list[i]];
		params[i].per_thread_delay = per_thread_delay[cpu_list[i]];
		if (force_clflush_all) params[i].use_clflush = 1;

#if defined(LINUX) || defined(__APPLE__)
		if (is_persistent[cpu_list[i]]) {
			params[i].src_location_type = NV_RAM;
			params[i].src_location_path = sxp_path[cpu_list[i]]; 
			if (force_clflush_pmem) params[i].use_clflush = 1;
		}
		else if (params[i].src_location_id != -1) {
			params[i].src_location_type = NUMA_NODE;
		}
		else {
			params[i].src_location_type = LOCAL_MEM;
			// currently, src_location_id is ignored and we pin the cpu and allocate local memory
			// later, we can possibly support NUMA allocation based on this value
		}
#else
		if (is_persistent[cpu_list[i]] || (pmem_only == 1)) {
			params[i].src_location_type = NV_RAM;
			if (o_provided)
			{
				sxp_path[cpu_list[i]] = params[i].src_location_path = sxp_path[cpu_list[i]]; //**CR**
			}
			else
			{
				sxp_path[cpu_list[i]] = params[i].src_location_path = mmap_files_dir;	// sxp_path[cpu_list[i]]; //**CR**
			}
			if (print_verbose1) { printf("params[i].src_location_path: %d mmap_files_dir: %s sxp_path: %s \n", i, mmap_files_dir, sxp_path[cpu_list[i]]); }
			if (force_clflush_pmem) params[i].use_clflush = 1;
		} else if (params[i].src_location_id != -1) { 
			params[i].src_location_type = NUMA_NODE;
		}
		else {
			params[i].src_location_type = LOCAL_MEM;
			// currently, src_location_id is ignored and we pin the cpu and allocate local memory
			// later, we can possibly support NUMA allocation based on this value
		}
#endif
		//printf("params values for buffer size for core %d out of %d is %d\n",i,numcpus-1,params[i].buf_size);
		if (params[i].rw_mix >= 20) { // this means the traffic stream will have mixed address streams from 2 data sources
			if (is_persistent_2[cpu_list[i]]) {
				params[i].src_location_type_2 = NV_RAM;
				params[i].src_location_path_2 = sxp_path_2[cpu_list[i]]; 
				if (print_verbose1) { printf("params[i].src_location_path_2:  sxp_path_2: %s \n", sxp_path_2[cpu_list[i]]); }
			}
			else if (params[i].src_location_id_2 != -1) {
				params[i].src_location_type_2 = NUMA_NODE;
			}
			else {
				params[i].src_location_type_2 = LOCAL_MEM;
				// currently, src_location_id is ignored and we pin the cpu and allocate local memory
				// later, we can possibly support NUMA allocation based on this value
			}
			params[i].address_stream_mix_ratio = address_stream_mix_ratio[cpu_list[i]];
		}
		
		if (NUMA_configured || remote_mem_alloc || matrix_mode) {
			if (Numa_file_provided) {
				//printf("numa file provided, cpu %d maps to %d\n",cpu_list[i],numa_array[params[i].local_id]);
				params[i].remote_id = numa_array[params[i].local_id];
			} else {
				params[i].remote_id = (total_cpus_present - 1) - params[i].local_id;
			}
		} else {
			params[i].remote_id = params[i].local_id;
		}
		

#if !defined(LINUX) && !defined(__APPLE__)
		InterlockedIncrement((long*)&numThruputThreads);
		ThreadData[i] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThruputMemAllocThread,
		                                       (void*)&params[i], 0, &tId);

		if (ThreadData[i] == NULL) {
			printf("Error in create thread\n");
		}

#else
        assert (pthread_mutex_lock(&AllocMutex) == 0);
        numThruputThreads++;
        assert(pthread_mutex_unlock(&AllocMutex) == 0);
		rc = pthread_create(&ThreadData[i], &attr, (void*)ThruputMemAllocThread, (void*)&params[i]);

		if (rc != 0) {
			printf("Error in create thread (error %d)\n", rc);
		}

#endif
	}

#if !defined(LINUX) && !defined(__APPLE__)
	Sleep(1000); // Give some time for the blaster threads to initialize
	WaitForSingleObject(AllocEvent, INFINITE);
#else
	assert(pthread_mutex_lock(&AllocMutex) == 0);
	assert(pthread_cond_wait(&AllocCond, &AllocMutex) == 0);
	assert(pthread_mutex_unlock(&AllocMutex) == 0);
#endif
#if !defined(LINUX) && !defined(__APPLE__)
	// The following clean up code has to be commented out as it is resulting in random crashes
	// HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, ThreadData);
#else
	free(ThreadData);
	pthread_attr_destroy(&attr);
#endif
}


// A thread runs on each cpu (called from AllocateMemoryForThruputThreads
// that does local and/or remote allocation
#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI ThruputMemAllocThread(PVOID Parm)
#else
unsigned int ThruputMemAllocThread(void* Parm)
#endif
{
	char* buf=NULL;
	__int64 buf_len;
	struct _params* p;
	if (share_buffers_across_all_threads) {
#if !defined(LINUX) && !defined(__APPLE__)
		EnterCriticalSection(&cs);
#else
		assert (pthread_mutex_lock(&serialize_buf_alloc_mutex) == 0);
#endif
	}
	p = (struct _params*) Parm;
	p->total_xfer = 0;
	p->total_time = 0;

	buf_len = (__int64)(p->buf_size) * 1024;
	if (remote_mem_alloc) {
		BindToCpu(p->remote_id);
	} else {
		BindToCpu(p->local_id);
	}
	
	if(print_verbose1) {
		printf("ThruputMemAllocThread: NUMA_configured %d rw_mix: %d src_location_type: %d,src_location_id: %d,src_location_path: %s  src_location_type_2: %d,src_location_id_2: %d,src_location_path_2: %s \n", NUMA_configured, p->rw_mix, p->src_location_type, p->src_location_id, p->src_location_path,p->src_location_type_2, p->src_location_id_2, p->src_location_path_2);
	}

	if (p->rw_mix == 10 || p->rw_mix == 11 ) { // support for 2 address streams
		// allocate 2 address streams; we will use only half the size for allocation
		buf = (char*) GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf);
		p->local_buf = buf;
		touch_buffer(buf, buf_len / 2);
		buf = (char*) GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf);
		p->local_buf2 = buf;
		touch_buffer(buf, buf_len / 2);
	} 
	else if (p->rw_mix >= 21 && p->rw_mix <=29) { // support for 2 address streams
		// allocate 2 address streams
		buf = (char*) GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf);
		p->local_buf = buf;
		touch_buffer(buf, buf_len);
		if (p->rw_mix == 23 || p->rw_mix == 27) { // need to allocate buffers for writes too
			buf = (char*) GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
			assert(buf);
			p->write_buf = buf;
			touch_buffer(buf, buf_len);
		}

		if (print_verbose1) printf("ThruputMemAllocThread: rw_mix: %d src_location_type_2: %d,src_location_id_2: %d,src_location_path_2: %s \n", p->rw_mix, p->src_location_type_2, p->src_location_id_2, p->src_location_path_2);
		buf = (char*) GrabMemory(buf_len, p->src_location_type_2, p->src_location_id_2, p->src_location_path_2);
		assert(buf);
		p->local_buf2 = buf;
		touch_buffer(buf, buf_len);
		if (p->rw_mix == 23 || p->rw_mix == 27) { // need to allocate buffers for writes too
			buf = (char*) GrabMemory(buf_len, p->src_location_type_2, p->src_location_id_2, p->src_location_path_2);
			assert(buf);
			p->write_buf2 = buf;
			touch_buffer(buf, buf_len);
		}
	} 
	else  {
		if (NUMA_configured >= 1 && NUMA_configured <= 3) { // do the local allocation first
			buf = (char*)GrabMemory(buf_len, LOCAL_MEM, p->src_location_id, p->src_location_path);
		}
		else if (NUMA_configured != 4 && p->rw_mix != 6) { 
			// for N=4 (all remote), there is no need to allocate local memory at all
			// Also if traffic type is non-temporal stores no need for read buffer; only write is required
			buf = (char*)GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		}
		// Now initialize the memory
		if (NUMA_configured != 4 && p->rw_mix != 6) { // For -N4, memory will be allocated later. For nt stores, no reads
			assert(buf);
			p->local_buf = buf;
			touch_buffer(buf, buf_len);
		}
	}

// addrchk

	if (p->rw_mix!=1 || (NUMA_configured >= 1 && NUMA_configured <= 3)) {
		buf = (char*)  GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf);
		p->write_buf = buf;
		touch_buffer(buf, buf_len);
	}

	if (NUMA_configured) { // allocate some buffers on the remote node
		BindToCpu(p->remote_id);
		buf = (char*) GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		//buf = (char*)  GetMemory(buf_len + 1024);
		assert(buf);
		p->remote_buf = buf;
		touch_buffer(buf, buf_len);

		if (NUMA_configured == 4) { // if it is all remote, then do allocation here
			// allocate write buffer
			p->local_buf = p->remote_buf; // for all remote case, just treat remote_buf as local_buf as well
			buf = (char*)  GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
			assert(buf);
			p->write_buf = buf;
			touch_buffer(buf, buf_len);
		}
	}

	p->buf_len  = buf_len;
	//p->buf_len = p->buf_size;
	//printf("p->buf_len is %d\nYYY\n",p->buf_len);

	if (numThruputThreads) {
#if defined(LINUX) || defined(__APPLE__)
		assert(pthread_mutex_lock(&AllocMutex) == 0);
		numThruputThreads--;
		assert(pthread_mutex_unlock(&AllocMutex) == 0);
#else
		InterlockedDecrement((long*)&numThruputThreads);
#endif
	}

	if (numThruputThreads <= 0) { // if I am the last thread leaving, signal the main thread to wrap up
#if !defined(LINUX) && !defined(__APPLE__)
		SetEvent(AllocEvent);
#else
		assert(pthread_mutex_lock(&AllocMutex) == 0);
		assert(pthread_cond_signal(&AllocCond) == 0);
		assert(pthread_mutex_unlock(&AllocMutex) == 0);
#endif
	}

	if (share_buffers_across_all_threads) {
#if !defined(LINUX) && !defined(__APPLE__)
		LeaveCriticalSection(&cs);
#else
		assert (pthread_mutex_unlock(&serialize_buf_alloc_mutex) == 0);
#endif
	}
	return 1;
}

// Initialize the buffer for dependent load memory accesses
void AllocDependentLoadMemory(struct _params* p)
{
	__int64* buf, *buf2, **datap;
	__int64 i, nLines = 0, LineIncrement, buf_len;

	if (thruput_only) {
		return;    // no latency thread wanted - so don't allocate any memory
	}
	if(k_provided) {  // list of cpus provided on command line to --loaded_latency mode
		//make sure memory comes from the same memory_node if specified
		if (specify_mem_by_node)
			remote_source[latency_thread_run_cpuid] = memory_node;
	}	

	p->local_id = latency_thread_run_cpuid;
	p->buf_size=buf_sizes[latency_thread_run_cpuid];
	if (p->buf_size == 0)
	{
		p->buf_size = buffer_size; // use global buf_size value
	}
	
	// For AMD processor, the latency thread should do 128 byte stride to beat the prefetchers
	// So, modify the buffer size to be larger 
	// this can be done only for latency thread and not b/w thread as that would still use 64 byte stride
	
	if (AMD_cpu && !l_provided) {  
		buf_len = (__int64)600000 * 1024; // use 600MB size
	}
	else {
		buf_len = (__int64)p->buf_size * 1024;
	}
	
	p->buf_len = buf_len;
	p->rw_mix=1;// can only be 100% read for the latency thread
	p->randombw=randombw[latency_thread_run_cpuid];
	p->src_location_id = remote_source[latency_thread_run_cpuid];
#if defined(LINUX) || defined(__APPLE__)
	if (is_persistent[latency_thread_run_cpuid]) {
		p->src_location_type = NV_RAM;
		p->src_location_path = sxp_path[latency_thread_run_cpuid];
	}
	else if (p->src_location_id != -1) {
		p->src_location_type = NUMA_NODE;
	}
	else
	{
		p->src_location_type = LOCAL_MEM;
	}
#else
	if (is_persistent[latency_thread_run_cpuid] || (pmem_only == 1)) {
		p->src_location_type = NV_RAM;
		if (o_provided)
		{
			p->src_location_path = sxp_path[latency_thread_run_cpuid];
		}
		else
		{
			p->src_location_path = mmap_files_dir; // sxp_path[latency_thread_run_cpuid];
		}
	} else if (p->src_location_id != -1) { 
		p->src_location_type = NUMA_NODE;
	} else
	{
		p->src_location_type = LOCAL_MEM;
	}
#endif

	p->total_xfer = 0;
	p->total_time = 0;

	if (NUMA_configured >= 1 && NUMA_configured <= 3) {
		// For NUMA accesses, allocate some memory locally and then on remote nodes as well
		BindToCpu(latency_thread_run_cpuid);
		buf = (__int64*)GrabMemory(buf_len, LOCAL_MEM, p->src_location_id, p->src_location_path);
		assert(buf);
		touch_buffer((char*)buf, buf_len);

		// Bind to a remote node and allocate remote node memory
		if (Numa_file_provided) {
			BindToCpu(numa_array[latency_thread_run_cpuid]);
		} else {
			BindToCpu(total_cpus_present - 1);
		}

		buf2 = (__int64*)GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf2);
		touch_buffer((char*)buf2, buf_len);
		BindToCpu(latency_thread_run_cpuid);
		// initialize both local and remote buffers so that every nth access goes to a remote node
		Init_Array((char*)buf, (char*)buf2, buf_len);
	} else {
		if (NUMA_configured == 4) { // all accesses are remote and no local node access at all
			if (Numa_file_provided) {
				BindToCpu(numa_array[latency_thread_run_cpuid]);
			} else {
				BindToCpu(total_cpus_present - 1);
			}
		} else {
			if (remote_mem_alloc) {
				BindToCpu(numa_array[latency_thread_alloc_cpuid]);
			} else {
				BindToCpu(latency_thread_alloc_cpuid);
			}
		}

		if (latency_thread_alloc_cpuid_specified) {
			if (remote_mem_alloc) {
				BindToCpu(numa_array[latency_thread_alloc_cpuid]);
			} else {
				BindToCpu(latency_thread_alloc_cpuid);
			}
		}

		buf = (__int64*)GrabMemory(buf_len, p->src_location_type, p->src_location_id, p->src_location_path);
		assert(buf);
		
		LineIncrement = LineSize_LatThd / sizeof(*datap); // lineincrement is in terms of integers
		nLines = (__int64)(buf_len) / LineSize_LatThd;
		datap = (__int64**)buf;

		// All memory now is either 100% local or 100% remote
		// Create a random pattern so prefetcher would be useless
		if (Random_Access) {
			Create_random_pattern(datap, buf_len);
		} else {
			for (i = 0; i < nLines - 1; i++) {
				*(__int64**)datap = (__int64*)(datap + LineIncrement);
				datap = datap + LineIncrement;
			}

			*(__int64**)datap = buf;
		}
	}

	BindToCpu(latency_thread_run_cpuid);
	p->local_buf = (char*)buf;
}

// Write once to all the buffers so that they actually get allocated
void touch_buffer(char* buf, __int64 len)
{
	__int64 i, *ptr;

	if (pattern_file_provided) {
		write_pattern_into_buffer(buf, len);
	} else if (zero_out_memory) {
		// initialize the entire buffer to zero
		memset(buf, 0, len);
	}
	else {
		// write non-zero values and possibly unique so kernel won't merge similar pages
		ptr = (__int64*)buf;
		if (pmem_only != 1)
		{
			for (i = 0; i < len / 8; i++) {
				ptr[i] = i;
			}
		}
		ptr = (__int64*)buf;
		for (i = 0; i < len / 8; i += 8) {
			evict_line_from_cache(&ptr[i]);
		}
	}
}

char *GrabMemory(UINT64 len, int src_location_type, int nodeid, char* sxp_path)
{
	len = getActualLenToAllocate(len);

	if (src_location_type == NUMA_NODE)
		return GrabMemoryFromNumaNode(len, nodeid);
	if (src_location_type == NV_RAM)
		return GrabMemoryFromNvRam(len, sxp_path);
	if (src_location_type == LOCAL_MEM)
		return GrabMemoryFromLocal(len, nodeid);
	return NULL; // if it does not match any of the cases
}

// Allocate memory depending on whether large page option is specified or not

char *GrabMemoryFromNumaNode(UINT64 len, int nodeid)
{
	char* buf;

	if (print_verbose1) printf("GrabMemoryFromNumaNode(): Allocate %ld bytes from numa node%d\n", (unsigned long)len, nodeid);
    buf = alloc_mem_onnode(len, nodeid);
	if (buf == NULL) {
		app_exit (ENOMEM, "Buffer allocation failed!\n");
	}
	return buf;
}

char *GrabMemoryFromLocal(UINT64 len, int nodeid)
{
	// Nodeid is not being used currently as the expectation is that the caller
	// would have pinned the thread to the right CPU. So, we just do the allocation and 
	// then expect the caller to touch the allocated memory 

	char* buf;
	static char *shared_buf=NULL;
#if !defined(LINUX) && !defined(__APPLE__)	
	__int64 num_pages;
#endif
	
	// If we want to have the same buffer used by all the threads then check for that
	if (share_buffers_across_all_threads && shared_buf != NULL)
		return shared_buf;

	
	if (print_verbose1) printf("GrabMemoryFromLocal(): Allocate %ld bytes from local memory\n", (unsigned long)len);
#if defined(LINUX) || defined(__APPLE__)
	if (lp_1GB_enabled) {
		buf= (char *)mmap(0,len,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_NORESERVE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_1GB,0, 0);
	}
	else if (lp_enabled) { // only 2MB huge pages requested
		buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, 0, 0);
	} else {
		buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	}
	if (buf == MAP_FAILED) {
		perror("mmap");
		app_exit(ENOMEM, "Buffer allocation failed!\n");
	}
#else
	if (lp_enabled) {
		buf = (char*)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
	} else {
		buf = (char*)VirtualAlloc(0, len, MEM_COMMIT, PAGE_READWRITE);
	}
	if (buf == NULL) {
		app_exit(ENOMEM, "Buffer allocation failed!\n");
	}

	LogBufferInfo(nodeid, buf, len);
#endif

	if (share_buffers_across_all_threads && shared_buf == NULL)
		shared_buf = buf;

	return buf;
}

// Currently, nodeid parameter is not being used. Later if persistent memory regions
// are exposed as NUMA regions, then it can be used

#if !defined(LINUX) && !defined(__APPLE__)
typedef struct 
{
	HANDLE hPMFile;
	HANDLE hPMFileMap;
	PVOID  pFileMapAddr;
	char filename[256];
} nvram_alloc;

typedef struct {
	LONG idx;
	nvram_alloc alloc[1024];
}nvram_alloc_table ;

nvram_alloc_table nvrt = { 0 };

void UnGrabAllMemoryFromNvRam()
{
	while (nvrt.alloc[nvrt.idx].hPMFile)
	{
		UnmapViewOfFile(nvrt.alloc[nvrt.idx].pFileMapAddr);
		CloseHandle(nvrt.alloc[nvrt.idx].hPMFileMap);
		CloseHandle(nvrt.alloc[nvrt.idx].hPMFile);
		nvrt.alloc[nvrt.idx].hPMFile = NULL;

		DeleteFile(nvrt.alloc[nvrt.idx].filename);
		InterlockedDecrement(&nvrt.idx);
	}
}
#endif

void DeleteAllSCMFiles()
{
#if !defined(LINUX) && !defined(__APPLE__)
	if (pmem_only || pmem_alloc_requested)
	{
		UnGrabAllMemoryFromNvRam();
	}
#endif

}

#define GB_SIZE (1024LL*1024LL*1024LL)
#define MB_SIZE (1024LL*1024LL)
#define KB_SIZE (1024LL)

char *GrabMemoryFromNvRam(UINT64 len, char *sxp_path)
{
#ifdef LINUX
	char* buf=NULL;
	int mmap_pathlen;
	int fd;

	
	//==================================================================
	//
	//  We create a temp file, unlink it and close the descriptor just after the mapping.
	//  This ensures that everything will be cleaned up after process termination.
	 //
	if( sxp_path ){
		static char template[] = "/vmem.XXXXXX";
		mmap_pathlen = strnlen_s(sxp_path, 256);
		char fullname[mmap_pathlen + sizeof(template)];
		(void)strcpy_s(fullname, mmap_pathlen+1, sxp_path);
		(void)strcat_s(fullname, mmap_pathlen + sizeof(template)+1, template);

		if( (fd = mkstemp(fullname)) < 0 ){
			fprintf(stderr, "Path: %s\n", fullname);
			perror("mkstemp");
			app_exit(ENOMEM, "Failed to create file, check if --mdir value is correct, mkstemp error\n");
		}

		(void)unlink(fullname);
		if( posix_fallocate(fd, 0, len) ){
			(void)close(fd);
			perror("posix_fallocate");
			app_exit(ENOMEM, "Failed to allocate file size, posix_fallocate error\n");
		}
	}
	//==================================================================
	if (lp_enabled) {
		if( sxp_path ){
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_HUGETLB, fd, 0);
			(void)close(fd);
		}
	}
	else {
		// regular memory allocation
		if( sxp_path ){
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
			(void)close(fd);
		}
	}
	
	if (buf == MAP_FAILED) {
		perror("mmap");
		app_exit(EACCES, "Buffer allocation failed!\n");
	}

	return buf;
#else
	
// Windows and OS X version of GetMemory
	//Runtime_exit("Error: Windows version does not support SXP memory yet\n");
	if (print_verbose1) printf("GrabMemoryFromNvRam: len: %llu  sxp_path: %s\n", len, sxp_path);
	if (NULL == sxp_path) { 
		app_exit(EINVAL, "GrabMemoryFromNvRam: Invalid sxp_path");
	}

	char idx_str[64] = { 0 };
	char sxp_path_full[256] = { 0 };
	long cur_idx=InterlockedIncrement(&nvrt.idx);
	sprintf_s(idx_str, sizeof(idx_str), "\\mlc_MapViewFile%d.tmp", cur_idx);
	if (sxp_path) {
		strcpy_s(sxp_path_full, sizeof(sxp_path_full), sxp_path); //mmap_files_dir); //
		strcat_s(sxp_path_full, sizeof(sxp_path_full), idx_str);
	}

	memcpy_s((void *const)nvrt.alloc[cur_idx].filename, sizeof(sxp_path_full), sxp_path_full, sizeof(sxp_path_full));
	//len = ((len / 4096) * 4096 ) + 4096;		//make sure len is in 4K boundary
	//nvrt.alloc[cur_idx].hPMFile = CreateFile(sxp_path_full, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
	//nvrt.alloc[cur_idx].hPMFile = CreateFile(sxp_path_full, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, NULL);
	//nvrt.alloc[cur_idx].hPMFile = CreateFile(sxp_path_full, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
	nvrt.alloc[cur_idx].hPMFile = CreateFile(sxp_path_full, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH, NULL);
	if (INVALID_HANDLE_VALUE == nvrt.alloc[cur_idx].hPMFile)
	{
		printf("CreateFile Failed for Path %s  ErrCode %ld \n", sxp_path_full, GetLastError());
		app_exit(EACCES, NULL); 
	}else
	{
		if (h_provided)
		{
			SET_DAX_ALLOC_ALIGNMENT_HINT_INPUT hint = { 0 };
			char ioctl_out_buf[128] = { 0 };
			DWORD ioctl_return_bytes = 0;

			hint.Flags = DAX_ALLOC_ALIGNMENT_FLAG_MANDATORY;
			hint.AlignmentShift = 30; //1G aligned
			hint.FallbackAlignmentShift = 21; // 2M Aligned
			hint.FileOffsetToAlign = 0x0; // 0x40000000; // File 1GB alined
			if (FALSE == DeviceIoControl(nvrt.alloc[cur_idx].hPMFile, FSCTL_SET_DAX_ALLOC_ALIGNMENT_HINT, &hint, sizeof(SET_DAX_ALLOC_ALIGNMENT_HINT_INPUT), ioctl_out_buf, sizeof(ioctl_out_buf), &ioctl_return_bytes, NULL))
			{
				printf("FSCTL_SET_DAX_ALLOC_ALIGNMENT_HINT Failed... Errorcode: %ld \n ",GetLastError() );
			}
			else
			{
				//printf("FSCTL_SET_DAX_ALLOC_ALIGNMENT_HINT Success!!! \n");
			}
		}
	}

#if 0
	nvrt.alloc[cur_idx].hPMFileMap = CreateFileMapping(nvrt.alloc[cur_idx].hPMFile,NULL,
											PAGE_READWRITE | SEC_COMMIT | (lp_enabled ? SEC_LARGE_PAGES : 0) ,
											len >> 32, len, NULL);
#else
	nvrt.alloc[cur_idx].hPMFileMap = CreateFileMapping(nvrt.alloc[cur_idx].hPMFile, NULL,
										PAGE_READWRITE | SEC_COMMIT,
										len >> 32, len, NULL);
#endif


	if (NULL == nvrt.alloc[cur_idx].hPMFileMap)
	{
		LONG SectorSize = 0;
		LONG SectorPerCluster = 0;
		LONG NumberOfFreeCluster = 0;
		LONG TotalNumberOfCluster = 0;
		DWORD CreateFileMappingErr = GetLastError();

		GetDiskFreeSpace(sxp_path, &SectorSize, &SectorPerCluster, &NumberOfFreeCluster, &TotalNumberOfCluster);
		printf("CreateFileMapping Failed for File %s FileHandle %p MapSize %lld  ErrCode %ld  FreeBytes: %ld MB \n", sxp_path_full, nvrt.alloc[cur_idx].hPMFileMap,len, CreateFileMappingErr, (NumberOfFreeCluster*SectorPerCluster*SectorSize)/(1024*1024));
		app_exit(EACCES, 0);
	}

	//
	//nvrt.alloc[cur_idx].pFileMapAddr = MapViewOfFile(nvrt.alloc[cur_idx].hPMFileMap, FILE_MAP_WRITE | FILE_MAP_READ | FILE_MAP_EXECUTE , 0, 0, len);
#if 1
	nvrt.alloc[cur_idx].pFileMapAddr = MapViewOfFile(nvrt.alloc[cur_idx].hPMFileMap,
										FILE_MAP_WRITE | FILE_MAP_READ | (h_provided ? FILE_MAP_LARGE_PAGES : 0),
										0, 0, 0);
#else
	nvrt.alloc[cur_idx].pFileMapAddr = MapViewOfFile2(nvrt.alloc[cur_idx].hPMFileMap,GetCurrentProcess(),0,NULL,0, 
												(h_provided ? MEM_LARGE_PAGES : 0),
												FILE_MAP_WRITE | FILE_MAP_READ | (h_provided ? FILE_MAP_LARGE_PAGES : 0));
#endif

	if (NULL == nvrt.alloc[cur_idx].pFileMapAddr)
	{
		printf("MapViewOfFile Failed for FileMapping %p  ErrCode %ld \n", nvrt.alloc[cur_idx].hPMFileMap, GetLastError());
		app_exit(EACCES, NULL);
	}

	if (print_verbose1)
		printf("GrabMemoryFromNvRam len: %llu sxp_path: %s hPMFile: %p hPMFileMap: %p pPmFileAddr: %p \n", len, sxp_path_full, nvrt.alloc[cur_idx].hPMFile, nvrt.alloc[cur_idx].hPMFileMap, nvrt.alloc[cur_idx].pFileMapAddr);			//**CR**

	//Write known pattern and flush it
	memset(nvrt.alloc[cur_idx].pFileMapAddr, 'z', len);
	if (0 == FlushViewOfFile(nvrt.alloc[cur_idx].pFileMapAddr, len))
	{
		printf("FlushViewOfFile Failed for Address %p of Len %lld. ErrorCode %ld", nvrt.alloc[cur_idx].pFileMapAddr, len, GetLastError());
	}

	return nvrt.alloc[cur_idx].pFileMapAddr;
#endif
}

#if defined(LINUX) || defined(__APPLE__)
char* GetMemory(UINT64 len)
{
	char* buf;

	//==================================================================
	//
	//  We create a temp file, unlink it and close the descriptor just after the mapping.
	//  This ensures that everything will be cleaned up after process termination.
	 //
	int fd;

	len = getActualLenToAllocate(len);
	if (print_verbose1) printf("GetMemory(): Allocate %llu bytes from local memory\n", len);

	if (specify_mem_by_node) {
		buf = alloc_mem_onnode(len, memory_node);
		if (buf == NULL) {
			app_exit(ENOMEM, "Buffer allocation failed!\n");
		}
		return buf;
	}
	
	if( mmap_files_dir ){
		static char template[] = "/vmem.XXXXXX";
		char fullname[mmap_files_dir_len + sizeof(template)];
		(void)strcpy_s(fullname, mmap_files_dir_len + sizeof(template), mmap_files_dir);
		(void)strcat_s(fullname, mmap_files_dir_len + sizeof(template), template);

		if( (fd = mkstemp(fullname)) < 0 ){
			fprintf(stderr, "Path: %s\n", fullname);
			perror("mkstemp");
			app_exit(EACCES, "Failed to create file, check if --mdir value is correct, mkstemp error\n");
		}

		(void)unlink(fullname);
		if( posix_fallocate(fd, 0, len) ){
			(void)close(fd);
			perror("posix_fallocate");
			app_exit(EACCES, "Failed to allocate file size, posix_fallocate error\n");
		}
	}

	//==================================================================

	if (lp_1GB_enabled) {
		if( mmap_files_dir ){
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_HUGETLB | MAP_HUGE_1GB, fd, 0);
			(void)close(fd);
		}else{
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_1GB, 0, 0);
		}
		if (buf == MAP_FAILED) {
			perror("mmap");
			app_exit(EACCES, "Likely insufficient large pages in the system, mmap error\n");
		}
	}
	else if (lp_enabled) {
		if( mmap_files_dir ){
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_SHARED | MAP_HUGETLB, fd, 0);
			(void)close(fd);
		}else{
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, 0, 0);
		}
		if (buf == MAP_FAILED) {
			perror("mmap");
			app_exit(EACCES,"Likely insufficient large pages in the system, mmap error\n");
		}
	}
	else {
		// regular memory allocation
		if( mmap_files_dir ){
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
			(void)close(fd);
		}else{
			buf = (char*)mmap(0, len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
			//printf("allocated buffer of size %d\n",len);
		}
		if (buf == MAP_FAILED) {
			perror("mmap");
			app_exit(ENOMEM,"Buffer allocation failed!\n");
		}
	}
	
	return buf;
}
#else

char* GetMemory(UINT64 len)
{
	char* buf;
	__int64 num_pages;

	len = getActualLenToAllocate(len);
	if (print_verbose1) printf("GetMemory(): pmem_only %d %lld\n", pmem_only, len);

	if (pmem_only)
	{
		buf = GrabMemoryFromNvRam(len, mmap_files_dir);
	}
	else
	{
		if (lp_enabled) {
			buf = (char*)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
		}
		else {
			if (specify_mem_by_node)
				buf = alloc_mem_onnode(len, memory_node);
			else
				buf = (char*)VirtualAlloc(0, len, MEM_COMMIT, PAGE_READWRITE);
		}
	}

	return buf;
}
#endif

#if defined(LINUX) || defined(__APPLE__)

/*
 * Copied from NVML
 */
static int
is_pmem_proc(void *addr, size_t len)
{

	// this function is commented out as it won't work anymore on newer kernels
	return 1;
}

#endif
int get_num_numa_nodes()
{
#if defined(LINUX) || defined(__APPLE__)
	glob_t g;
	glob("/sys/devices/system/node/node*/meminfo", 0, NULL, &g); 
	num_numa_nodes = g.gl_pathc; 
	globfree(&g); 

#else
#if 0
	unsigned long nn;
	if (GetNumaHighestNodeNumber(&nn) == 0) {
		app_exit(EACCES, "Windows GetNumaHighestNodeNumber() failed\n");
	}
	num_numa_nodes = nn+1; // add 1 as the numa nodes are numbered from 0
#else
	num_numa_nodes = GetMaxNuma() + 1;
#endif

	//printf("num numa node=%d\n", num_numa_nodes);
#endif
	return num_numa_nodes;
}

/*
 * mbind() has a bug in many kernel distributions. Even when we ask for memory on a specific
 * numa node, it gets it on some other node. Dave Hansen from OTC debugged it and suggested
 * passing in a virtual address. So, we are generating a new unique virtual address for every
 * mbind call by calling this function
 */
#ifdef LINUX
unsigned long getNextVirtAddr(UINT64 len)
{
	static unsigned long last_ending_VA = 0xB000000000L;
	unsigned long vaaddr;
#if 0	
	assert(pthread_mutex_lock(&getVirtAddrMutex)==0);
	lastVA += len*2;
	lastVA = (lastVA + (unsigned long)0xfff) & ~(unsigned long)0xfff;
	vaaddr=lastVA;
	assert(pthread_mutex_unlock(&getVirtAddrMutex)==0);
	return vaaddr;
#endif
	assert(pthread_mutex_lock(&getVirtAddrMutex)==0);
	if (lp_enabled)
		vaaddr = (last_ending_VA + (unsigned long)0x200000LL) & ~(unsigned long)0x1fffffLL;
	else
		vaaddr = (last_ending_VA + (unsigned long)0x1000) & ~(unsigned long)0xfff;
	last_ending_VA =  vaaddr + len + 1;
	assert(pthread_mutex_unlock(&getVirtAddrMutex)==0);
	return vaaddr;
	

}
#endif
/*
 *	To avoid libNUMA dependency (as that would require end users to have libnuma installed)
 *	we are directly using syscall to do mbind
 *	It is assumed that 'len' is already multiple of huge pages if huge pages enabled
 *	basically, prior to calling this function, call getActualLenToAllocate() to get the length padded and rounded up
 */

char* alloc_mem_onnode(UINT64 len, int nodeid)
{
	char* buf=NULL;
	
#ifdef LINUX
	int i;
	long status;
	unsigned long mask[4];
	unsigned int bits_per_UL = sizeof(unsigned long)*8;
	
#endif
	

#ifdef LINUX
	for (i=0; i < 4; i++) mask[i]=0;
	mask[nodeid/bits_per_UL] |= 1UL << (nodeid % bits_per_UL);

	if (lp_1GB_enabled) {
		buf= (char *)mmap((void*)getNextVirtAddr(len),len,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_HUGE_1GB,0, 0);
	}
	else if (lp_enabled) { // 2MB page
		buf = (char*)mmap((void*)getNextVirtAddr(len), len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, 0, 0);
	}
	else // 4KB page
		buf = (char*)mmap((void*)getNextVirtAddr(len), len, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	if (buf == NULL) { printf("Unable to mmap\n"); return buf; }
	status = syscall(__NR_mbind, buf, len, 2, &mask, nodeid+2, 1);
	if (status != 0) {
		perror("alloc_mem_onnode(): unable to mbind: ");
		return NULL;
	}
#elif defined(__APPLE__)
#else
	buf = VirtualAllocExNuma (GetCurrentProcess(), NULL, len, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE, nodeid);
	LogBufferInfo(nodeid, buf, len);
#endif
	return buf;
}

__int64 getActualLenToAllocate(__int64 len)
{
	__int64 page2m=2*1024*1024;
	__int64 page1g=1*1024*1024*1024LL;
	__int64 padding;
	
	// We need to add some extra padding as most of the throughput generation routines have multiple line loads/stores
	// so to make sure we don't go past the allocated length, it is safer to add 8 more lines to the requested size
	padding = LineSize * 8; 
	len += padding;
	if (lp_1GB_enabled && (len % page1g) != 0)
		len = ((len / page1g) + 1) * page1g;
	else if (lp_enabled && (len % page2m) !=0) // 2MB pages requested
		len = ((len / page2m) + 1) * page2m;
	return len;
	
}
