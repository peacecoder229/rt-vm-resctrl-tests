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
#include <winbase.h>
#include <stdlib.h>
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

void Runtime_exit(const char* error);

extern char* do_independent_load_rdonly_64B(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx256_64B(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx512_64B(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_64B_verify_data_avx512(char* local_buf,  char* endp, int delay_t, char *new_chksum_value_p);
extern char* do_independent_load_rdonly_64B_verify_data_avx2(char* local_buf,  char* endp, int delay_t, char *new_chksum_value_p);
extern char* do_independent_load_rdonly_64B_verify_data_sse2(char* local_buf,  char* endp, int delay_t, char *new_chksum_value_p);

extern char* do_independent_load_rwlocal_2R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_3R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_50R_50W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_50R_50W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_50R_50W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);


extern char* do_independent_random_load_rdonly(char* local_buf,  __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rdonly_avx512(char* local_buf,  __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rdonly_avx512_128B_block(char* local_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rdonly_avx512_256B_block(char* local_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_load_rdonly(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx256(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx512(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx512_bsz1(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx512_bsz2(char* local_buf,  char* endp, int delay_t);
extern char* do_independent_load_rdonly_avx512_bsz4(char* local_buf,  char* endp, int delay_t);

extern char* do_independent_load_rwlocal_80R_20W(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_80R_20W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_80R_20W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_5R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//extern char* do_independent_load_rwlocal_80R_20W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//extern char* do_independent_load_rwlocal_80R_20W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);



extern char* do_independent_load_rwlocal_3R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_random_load_rwlocal_2R_1W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_2R_1W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_2R_1W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_2R_1W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_60R_40W(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_60R_40W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_60R_40W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_random_load_rwlocal_50R_50W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_50R_50W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_50R_50W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_50R_50W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);

extern char* do_independent_load_rwlocal_50R_50W(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_50R_50W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_50R_50W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_random_load_rwlocal_100W(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_random_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_random_load_rwlocal_100W_avx512_128B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_random_load_rwlocal_100W_avx512_256B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_load_rwlocal_100W(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_load_rwlocal_100W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
extern char* do_independent_load_rwlocal_100W_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);

extern char* do_independent_load_rwlocal_1R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_1R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_1R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_random_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_random_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_3R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rwlocal_3R_1W_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);

extern char* do_independent_load_rw_all_remote(char* remote_buf, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_NUMA1(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);
extern char* do_independent_load_NUMA2(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);
extern char* do_independent_load_NUMA3_rdonly(char* local_buf, char* remote_buf, char* endp, int delay_t);
extern char* do_independent_load_NUMA3(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);

extern char* do_independent_load_rwlocal_100R_two_sources_1to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_100R_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_100R_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R1W_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_3R1W_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R1NTW_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
extern char* do_independent_load_rwlocal_2R1NTW_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);

extern __int64* do_dependent_read(__int64* a);

#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI ThruputMemAllocThread(PVOID Parm);
#else
unsigned int ThruputMemAllocThread(void* Parm);
#endif

void AllocateMemoryForThruputThreads(VOID);

extern struct _params params[];

void AllocDependentLoadMemory(struct _params* p);
long volatile start_load, stop_load;

#if defined(LINUX) || defined(__APPLE__)
#else
HANDLE g_hExitEvent;						// Global exit event handle
HANDLE AllocEvent;
#endif

int read_pattern_file(char* filename);
void populate_cache_patterns();
void write_pattern_into_buffer(char* buf, __int64 len);


#if !defined(LINUX) && !defined(__APPLE__)
	PHANDLE* ThreadData;
#else
	pthread_t* ThreadData;
#endif


// this thread does dependent reads (basically each read has a pointer that points to the next line to be read)
// by measuring this latency, we get a true picture of the latency under loaded conditions
#if defined(LINUX) || defined(__APPLE__)
unsigned int DependentLoadThread(void* Parm)
#else
UINT WINAPI DependentLoadThread(PVOID Parm)
#endif
{
	__int64* buf, *curbufp;
	int i;
	__int64 buf_len;
	__int64 after;
	__int64 before;
	struct _params* p;
	p = (struct _params*) Parm;
	p->total_xfer = 0;
	p->total_time = 0;
	BindToCpu(latency_thread_run_cpuid);
	if (print_verbose1) {
		printf("Thruput only - %d : Launching latency thread on cpu %d\n", thruput_only, latency_thread_run_cpuid);
	}
	
	buf_len = p->buf_len;
	buf = (__int64*)(p->local_buf);
#if !defined(LINUX) && !defined(__APPLE__)
	InterlockedIncrement((LONG volatile*)&start_load);
#else
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	start_load++;
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif
	curbufp = buf;
	after = before = THE_TSC;

	for (i = 0; (after - before) < Run_Time && !stop_load; i++) { // if stop_load is set by runtime_exit on SIGTERM, need to stop this
		if (thruput_only) { // no need for latency thread
#if !defined(LINUX) && !defined(__APPLE__)
			Sleep(1000);
#else
			sleep(1);
#endif
		} else {
			curbufp = do_dependent_read(curbufp);
			p->total_xfer += ITERATIONS;
		}

		after = THE_TSC;
		// this can happen only when -x0 option is specified in loaded_latency mode. in that case, thruput threads would stop 
		// on their win. Otherwise only this thread will signal thruput threads to stop
		if (numThruputThreads == 0) break; 
	}

	p->total_time = after - before;
#if !defined(LINUX) && !defined(__APPLE__)
	InterlockedIncrement((LONG volatile*)&stop_load);
#else
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	stop_load++;
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif

	//if (verbose) printf("stopping the throughput threads..\n");
	if (numThruputThreads == 0) {
#if !defined(LINUX) && !defined(__APPLE__)
		SetEvent(g_hExitEvent);
#else
		assert(pthread_mutex_lock(&g_ExitMutex)==0);
		assert(pthread_cond_signal(&g_ExitCond)==0);
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif
	}

	return 0;
}


// Each one of these threads (running on CPUs defined by m/o/k options
// generates non-dependent reads (basically data read is never used) and
// creates the traffic on the bus
#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI ThruputThread(PVOID Parm)
#else
unsigned int ThruputThread(void* Parm)
#endif
{
	char* endp;
	__int64 n_lines, after, before;
	UINT64 total_read = 0;
	struct _params* p;
	char* processedp=NULL;
	char *chksum_buf;
	int delay_value;
	int special_64byte_stride_process = 0;
	
	p = (struct _params*) Parm;
	p->total_xfer = 0;
	p->total_time = 0;
	BindToCpu(p->local_id);
	if (print_verbose1) {
		printf("Launching BW thread on cpu %d\n", p->local_id);
	}
	n_lines = p->buf_len / LineSize;
	//printf("id=%d\tbuf_len=%I64d\t rw_mix=%d\n",p->local_id, p->buf_len, p->rw_mix);	
	endp = (p->local_buf) + (p->buf_len);

	if (per_thread_delay_specified) {
		delay_value = p->per_thread_delay;
		//printf("tid: %d, delay %d\n", p->local_id, delay_value);
	}
	else delay_value = delay_time; //take the global delay

	// Data integrity check 
#ifdef LINUX
	if (verify_data_on_read )  { // if -o option is selected, then you can have many threads with different traffic mixes
		if ((p->rw_mix != 1 || LineSize != 64 || p->randombw !=0 ) && !o_provided) { // data integrity supported only for 100% reads w/ 64 byte stride
			printf("Data integrity check is not supported with the specified options\n");
			printf("It is supported only for 100%% reads with sequential 64 byte stride\n");
			app_exit(EINVAL, NULL);
		}

		chksum_buf = GetMemory(64); // to serve as scratch buffer during XOR compute while loading the data from memory
		// compute the original checksum by XORing the data DWORDs
		if (p->rw_mix == 1) { // only 100% reads
			if (cpu_supports_avx512 && use_avx512)
				p->chksum_value = compute_checksum_avx512(p->local_buf, p->buf_len, chksum_buf);
			else if (cpu_supports_avx2 && use_avx256)
				p->chksum_value = compute_checksum_avx2(p->local_buf, p->buf_len, chksum_buf);
			else
				p->chksum_value = compute_checksum_sse2(p->local_buf, p->buf_len, chksum_buf);
		}
	}
#endif
	// For SKX, do vzeroupper to make sure we don't get into weird AVX license level
	if (is_non_inclusive_cache_with_dbp_enabled()) {
#if !defined(LINUX) && !defined(__APPLE__)
		__asm {
			vzeroupper
		}

#else
		asm ("vzeroupper");
#endif

	}
	if (p->buf_len <= 50*1024*1024 && LineSize == 64 && p->use_clflush ==0 && p->randombw ==0 && 
		(p->rw_mix == 1 || p->rw_mix == 2 || p->rw_mix == 3 || p->rw_mix == 5))
		special_64byte_stride_process = 1;
	
	if (partial_cache_line_access == 1) {
		if (p->rw_mix == 1 || p->rw_mix == 2 || p->rw_mix == 3 || p->rw_mix == 5) special_64byte_stride_process = 1;
		else special_64byte_stride_process = 0;
	}
	else if (partial_cache_line_access == 0) // explicitly forced in the command line with -K0
		special_64byte_stride_process = 0;
		
	// if user has not specified -K option to change partial cache line access behavior and we are on ICX/SPR then
	// force partial line access for traffic involving stores
	if (rfos_converted_to_itom && (p->rw_mix == 2 || p->rw_mix == 3 || p->rw_mix == 5))
		special_64byte_stride_process = 1;
		
	// Wait for the dependent load thread to signal 'GO'
	while (start_load == 0);

	after = before = THE_TSC;

	for (;;) {
#ifdef AVX512_SUPPORT
		if (p->use_clflush) {
			switch (p->rw_mix) {
			case 1: // there is no CLFLUSH for reads; so use the same as for regular reads
				if (p->randombw)
					processedp = do_independent_random_load_rdonly_avx512(p->local_buf, p->buf_len, endp, delay_value);
				else
					processedp = do_independent_load_rdonly_avx512(p->local_buf, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines;
				break;
			case 2:
				if (p->randombw)
					processedp = do_independent_random_load_rwlocal_2R_1W_avx512_clflush(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				else
					processedp = do_independent_load_rwlocal_2R_1W_avx512_clflush(p->local_buf, p->write_buf, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 3;
				break;
			case 3: // 75% reads + 25% writes
				processedp = do_independent_load_rwlocal_3R_1W_avx512_clflush(p->local_buf, p->write_buf, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 2;
				break;
			case 5: // 50% reads ; 50% writes
				endp = (p->write_buf) + (p->buf_len);

				if (p->randombw)
					processedp = do_independent_random_load_rwlocal_50R_50W_avx512_clflush(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				else 
					processedp = do_independent_load_rwlocal_50R_50W_avx512_clflush(p->local_buf, p->write_buf, endp, delay_value);

				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines * 2);
				break;
			// ugly hack. To support issuing mfence after NT writes, we are adding this case under CLFLUSH option as we don't want
			// to define another explicit flag for this . This is done for both W6 and W7 cases. Hate doing this...
			case 6: // 100% NT writes 
				endp = (p->write_buf) + (p->buf_len);
				processedp = do_independent_load_rwlocal_100W_avx512_mfence(p->local_buf, p->write_buf, endp, delay_value, (__int64)ntw_partial);
				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines);
				break;
			case 7:
				processedp = do_independent_load_rwlocal_2R_1NTW_avx512_mfence(p->local_buf, p->write_buf, endp, delay_value);	
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += ((n_lines * 3) / 2);
				break;
			default:
				app_exit(EINVAL, "Unsupported traffic types with CLFLUSH option\n");
			}
		} else 
#endif
		if (special_64byte_stride_process) {
			switch (p->rw_mix) {
			case 1:
#ifdef AVX512_SUPPORT

				if (verify_data_on_read) {
			
#ifdef LINUX
					if (cpu_supports_avx512 && use_avx512)
						processedp = do_independent_load_rdonly_64B_verify_data_avx512(p->local_buf, endp, delay_value, chksum_buf);
					else if (cpu_supports_avx2 && use_avx256)
						processedp = do_independent_load_rdonly_64B_verify_data_avx2(p->local_buf, endp, delay_value, chksum_buf);
					else
						processedp = do_independent_load_rdonly_64B_verify_data_sse2(p->local_buf, endp, delay_value, chksum_buf);
					if (p->chksum_value != compute_checksum_from_64B(chksum_buf))
					{
						app_exit(EFAULT, "\nChecksum mismatch. Data corruption detected. Exiting..\n");
					}
#endif
				}
				else if (use_avx512) {
					processedp = do_independent_load_rdonly_avx512_64B(p->local_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rdonly_avx256_64B(p->local_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rdonly_64B(p->local_buf, endp, delay_value);
					}

				n_lines = (processedp - p->local_buf) >> linesize_powerof2bits;
				total_read += n_lines;
				break;

			case 2: // 66% reads + 33% writes
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_2R_1W_avx512_64B(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_2R_1W_avx256_64B(p->local_buf, p->write_buf, endp, delay_value);
					} else {
						processedp = do_independent_load_rwlocal_2R_1W_64B(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (processedp - p->local_buf) >> linesize_powerof2bits ;
				total_read += n_lines * 3;
				break;

			case 3: // 75% reads + 25% writes
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_3R_1W_avx512_64B(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_3R_1W_avx256_64B(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_3R_1W_64B(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 2;
				break;

			case 5: // 50% reads ; 50% writes
				endp = (p->write_buf) + (p->buf_len);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					processedp = do_independent_load_rwlocal_50R_50W_avx512_64B(p->local_buf, p->write_buf, endp, delay_value);					
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_50R_50W_avx256_64B(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_50R_50W_64B(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines * 2);
				break;

			default:
				app_exit(EINVAL, "Unsupported traffic type in 64byte stride processing\n");
				break;
			}
		} else
		{
			switch (p->rw_mix) {
			case 1:
#ifdef AVX512_SUPPORT

				if (verify_data_on_read) {
					
#ifdef LINUX
					if (cpu_supports_avx512 && use_avx512)
						processedp = do_independent_load_rdonly_64B_verify_data_avx512(p->local_buf, endp, delay_value, chksum_buf);
					else if (cpu_supports_avx2 && use_avx256)
						processedp = do_independent_load_rdonly_64B_verify_data_avx2(p->local_buf, endp, delay_value, chksum_buf);
					else
						processedp = do_independent_load_rdonly_64B_verify_data_sse2(p->local_buf, endp, delay_value, chksum_buf);
					if (p->chksum_value != compute_checksum_from_64B(chksum_buf))
					{
						app_exit(EFAULT, "\nChecksum mismatch. Data corruption detected. Exiting..\n");
					}
#endif
				}
				else if (use_avx512) {							
					if (p->randombw)
						processedp = do_independent_random_load_rdonly_avx512(p->local_buf, p->buf_len, endp, delay_value);
					else {
						if (burst_size==1)
							processedp = do_independent_load_rdonly_avx512_bsz1(p->local_buf, endp, delay_value);
						else if (burst_size==2)
							processedp = do_independent_load_rdonly_avx512_bsz2(p->local_buf, endp, delay_value);
						else if (burst_size==4)
							processedp = do_independent_load_rdonly_avx512_bsz4(p->local_buf, endp, delay_value);
						else
							processedp = do_independent_load_rdonly_avx512(p->local_buf, endp, delay_value);
					}
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rdonly_avx256(p->local_buf, endp, delay_value);
					} else
					{
						if (p->randombw)
							processedp = do_independent_random_load_rdonly(p->local_buf, p->buf_len, endp, delay_value);
						else
							processedp = do_independent_load_rdonly(p->local_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines;
				break;
			
			case 2: // 66% reads + 33% writes
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					if (p->randombw)
						processedp = do_independent_random_load_rwlocal_2R_1W_avx512(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
					else
						processedp = do_independent_load_rwlocal_2R_1W_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_2R_1W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else {
						if (p->randombw)
							processedp = do_independent_random_load_rwlocal_2R_1W(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
						else
							processedp = do_independent_load_rwlocal_2R_1W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 3;
				break;

			case 3: // 75% reads + 25% writes
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_3R_1W_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_3R_1W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_3R_1W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 2;
				break;

			case 4: // 60% reads + 40% writes
				endp = (p->write_buf) + (p->buf_len);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					processedp = do_independent_load_rwlocal_60R_40W_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_60R_40W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_60R_40W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += ((n_lines * 5) / 2);
				break;

			case 5: // 50% reads ; 50% writes
				endp = (p->write_buf) + (p->buf_len);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					if (p->randombw)
						processedp = do_independent_random_load_rwlocal_50R_50W_avx512(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
					else 
						processedp = do_independent_load_rwlocal_50R_50W_avx512(p->local_buf, p->write_buf, endp, delay_value);
					
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_50R_50W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						if (p->randombw)
							processedp = do_independent_random_load_rwlocal_50R_50W(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
						else
							processedp = do_independent_load_rwlocal_50R_50W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines * 2);
				break;

			case 6: // 100% Non-temporal writes
				endp = (p->write_buf) + (p->buf_len);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					if (p->randombw)
						processedp = do_independent_random_load_rwlocal_100W_avx512(p->local_buf, p->write_buf, p->buf_len, endp, delay_value, (__int64)ntw_partial);
					else 
						processedp = do_independent_load_rwlocal_100W_avx512(p->local_buf, p->write_buf, endp, delay_value, (__int64)ntw_partial);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_100W_avx256(p->local_buf, p->write_buf, endp, delay_value, (__int64)ntw_partial);
					} else
					{
							if (p->randombw)
								processedp = do_independent_random_load_rwlocal_100W(p->local_buf, p->write_buf, p->buf_len, endp, delay_value, (__int64)ntw_partial);
							else
								processedp = do_independent_load_rwlocal_100W(p->local_buf, p->write_buf, endp, delay_value, (__int64)ntw_partial);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines);
				break;

			case 7: // 2 reads and 1 NT write
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					if (p->randombw)
						processedp = do_independent_random_load_rwlocal_2R_1NTW_avx512(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
					else
						processedp = do_independent_load_rwlocal_2R_1NTW_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_2R_1NTW_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						if (p->randombw)
							processedp = do_independent_random_load_rwlocal_2R_1NTW(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
						else
							processedp = do_independent_load_rwlocal_2R_1NTW(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += ((n_lines * 3) / 2);
				break;

			case 8: // 1 read and 1 NT write
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_1R_1NTW_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_1R_1NTW_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_1R_1NTW(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += (n_lines * 2);
				break;

			case 9: // 3 reads and 1 NT write
				processedp = do_independent_load_rwlocal_3R_1NTW(p->local_buf, p->write_buf, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += ((n_lines * 4) / 3);
				break;

			case 10: // 2 reads and 1 NT write - but both reads are on two differen address streams
				endp = (p->local_buf) + (p->buf_len / 2);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					processedp = do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx512(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx256(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_2R_1NTW_diff_addr_stream(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += (n_lines * 3);
				break;

			case 11: // 3 reads and 1 write - but both reads are on two different address streams
				endp = (p->local_buf) + (p->buf_len / 2);
#ifdef AVX512_SUPPORT

				if (use_avx512) {
					processedp = do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx512(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx256(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_3R_1W_diff_addr_stream(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += (n_lines * 4); // for every read, you will have a total of 4 transfers (3rd+1wr)
				break;

			case 12: // 80% reads + 20% writes
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_80R_20W_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_80R_20W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{
						processedp = do_independent_load_rwlocal_80R_20W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += ((n_lines * 5) / 3);
				break;
                        case 13: // Rohan case
#ifdef AVX512_SUPPORT
				if (use_avx512) {
					processedp = do_independent_load_rwlocal_80R_20W_avx512(p->local_buf, p->write_buf, endp, delay_value);
				} else
#endif
					if (use_avx256) {
						processedp = do_independent_load_rwlocal_80R_20W_avx256(p->local_buf, p->write_buf, endp, delay_value);
					} else
					{       
						
						processedp = do_independent_load_rwlocal_5R_1W(p->local_buf, p->write_buf, endp, delay_value);
					}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += ((n_lines * 5) / 3);


			        break;	
//#ifdef LINUX Trying to add case 21
			case 21: // All reads from two different address sources in a single stream
				if (p->address_stream_mix_ratio == 50)
					processedp = do_independent_load_rwlocal_100R_two_sources_1to1_ratio(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
				else if (p->address_stream_mix_ratio == 25)
					processedp = do_independent_load_rwlocal_100R_two_sources_3to1_ratio(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
				else if (p->address_stream_mix_ratio == 10)
					processedp = do_independent_load_rwlocal_100R_two_sources_9to1_ratio(p->local_buf, p->local_buf2, p->write_buf, endp, delay_value);
				else {
					printf("invalid address stream ratio %d\n", p->address_stream_mix_ratio);
					app_exit(EINVAL, NULL);
				}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				if (p->address_stream_mix_ratio == 50) total_read += (n_lines * 2);
				else if (p->address_stream_mix_ratio == 25) total_read += ((n_lines * 4) / 3);
				else if (p->address_stream_mix_ratio == 10) total_read += ((n_lines * 10) / 9);
				break;

			case 23: // All reads from two different address sources in a single stream
				if (p->address_stream_mix_ratio == 25)
					processedp = do_independent_load_rwlocal_3R1W_two_sources_3to1_ratio(p->local_buf, p->local_buf2, p->write_buf, p->write_buf2, endp, delay_value);
				else if (p->address_stream_mix_ratio == 10)
					processedp = do_independent_load_rwlocal_3R1W_two_sources_9to1_ratio(p->local_buf, p->local_buf2, p->write_buf, p->write_buf2, endp, delay_value);
				else {
					printf("invalid address stream ratio %d\n", p->address_stream_mix_ratio);
					app_exit(EINVAL, NULL);
				}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				if (p->address_stream_mix_ratio == 25) total_read += ((n_lines * 8)/3);  // 2 * (4/3)
				if (p->address_stream_mix_ratio == 10) total_read += ((n_lines * 20)/9); // 2*( 10/9)
				break;

			case 27: // All reads from two different address sources in a single stream
				if (p->address_stream_mix_ratio == 25)
					processedp = do_independent_load_rwlocal_2R1NTW_two_sources_3to1_ratio(p->local_buf, p->local_buf2, p->write_buf, p->write_buf2, endp, delay_value);
				else if (p->address_stream_mix_ratio == 10)
					processedp = do_independent_load_rwlocal_2R1NTW_two_sources_9to1_ratio(p->local_buf, p->local_buf2, p->write_buf, p->write_buf2, endp, delay_value);
				else {
					printf("invalid address stream ratio %d\n", p->address_stream_mix_ratio);
					app_exit(EINVAL, NULL);
				}

				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				if (p->address_stream_mix_ratio == 25) total_read += (n_lines * 2);  // (3/2) * (4/3)
				if (p->address_stream_mix_ratio == 10) total_read += ((n_lines * 5)/3); // (3/2)*( 10/9)
				break;
#ifdef AVX512_SUPPORT
			case 31:
				processedp = do_independent_random_load_rdonly_avx512_256B_block(p->local_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines;


				break;
			case 32:
				processedp = do_independent_random_load_rwlocal_2R_1W_avx512_256B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 3;
				break;

			case 35: // 50% reads ; 50% writes
				endp = (p->write_buf) + (p->buf_len);
				processedp = do_independent_random_load_rwlocal_50R_50W_avx512_256B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines * 2);
				break;
				
			case 36: // 100% Non-temporal writes in 256B block
				endp = (p->write_buf) + (p->buf_len);
				processedp = do_independent_random_load_rwlocal_100W_avx512_256B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value, (__int64)ntw_partial);
				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines);
				break;
			case 41:
				processedp = do_independent_random_load_rdonly_avx512_128B_block(p->local_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines;


				break;
			case 42:
				processedp = do_independent_random_load_rwlocal_2R_1W_avx512_128B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->local_buf) >> linesize_powerof2bits : (processedp - p->local_buf)/LineSize;
				total_read += n_lines * 3;
				break;

			case 45: // 50% reads ; 50% writes
				endp = (p->write_buf) + (p->buf_len);
				processedp = do_independent_random_load_rwlocal_50R_50W_avx512_128B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value);
				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines * 2);
				break;
				
			case 46: // 100% Non-temporal writes in 256B block
				endp = (p->write_buf) + (p->buf_len);
				processedp = do_independent_random_load_rwlocal_100W_avx512_128B_block(p->local_buf, p->write_buf, p->buf_len, endp, delay_value, (__int64)ntw_partial);
				n_lines = (linesize_powerof2bits)? (processedp - p->write_buf) >> linesize_powerof2bits : (processedp - p->write_buf)/LineSize;
				total_read += (n_lines);
				break;
#endif				
//#endif				
			}
		}

		if (stop_load != 0 || one_iteration_only) {
			after = THE_TSC;
			break;
		}
	}

	p->total_xfer = total_read;
	p->total_time = after - before;
	//printf("thread id %d -- after: %lld, before: %lld, difference:%lld\n",p->local_id,after,before,after-before);
	
#if defined(LINUX) || defined(__APPLE__)
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	numThruputThreads--;
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#else
	InterlockedDecrement((long*)&numThruputThreads);
#endif

	if (numThruputThreads == 0) { // if I am the last thread leaving, signal the main thread to wrap up
#if defined(LINUX) || defined(__APPLE__)
		assert(pthread_mutex_lock(&g_ExitMutex)==0);
		assert(pthread_cond_signal(&g_ExitCond)==0);
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#else
		SetEvent(g_hExitEvent);
#endif
	}

	return 0;
}
// This function will launch all b/w and latency threads and measure b/w from each
// and also aggregate all the values to provide the total b/w
// when 'print_details' is set to 1, this function will print the b/w details. Else, it will let the
// caller print in any format

void get_bw_and_latency ( int print_details, int delay_time, double* bw_p, double *latency_p)
{
#if !defined(LINUX) && !defined(__APPLE__)
  int tId;
#else
	int rc;
	pthread_attr_t attr;
#endif
	int i;
	double total_indep_load = 0, Mbytes_per_sec = 0,  latency = 0;


	numThruputThreads = 0;
	start_load = 0;
	stop_load = 0;

	if (print_details && matrix_mode == 0) {
		printf("%6.5d\t", delay_time);
	}

	if (print_details && print_per_thread_throughput) {
		printf("\n");
	}
#if defined(LINUX) || defined(__APPLE__)
	if (pthread_attr_init(&attr) !=0)
		app_exit(EACCES, "pthread_attr_init error\n");
	else {
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
			app_exit(EACCES, "pthread_attr_setdetachstate error\n");	
	}

#endif
	if (print_verbose1) printf("Using %d BW generation threads\n", numcpus);
	//Start all the throughput threads
	for (i = 0; i < numcpus; i++) {
#if !defined(LINUX) && !defined(__APPLE__)
		InterlockedIncrement((long*)&numThruputThreads);
		ThreadData[i] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ThruputThread,
											   (void*)&params[i], 0, &tId);

		if (ThreadData[i] == NULL) {
			printf("Error in create thread\n");
			app_exit(EPERM, NULL);
		}

#else
		assert(pthread_mutex_lock(&g_ExitMutex)==0);
		numThruputThreads++;
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);
		rc = pthread_create(&ThreadData[i], NULL, (void*)ThruputThread, (void*)&params[i]);
		if (rc != 0) {
			printf("Error in create thread (error %d)\n", rc);
			app_exit(EPERM, NULL);
		}

#endif
	}
#if !defined(LINUX) && !defined(__APPLE__)
	Sleep(1000); // Give some time for the throughput threads to initialize
#else
	sleep(1);
#endif
#if !defined(LINUX) && !defined(__APPLE__)
	ThreadData[numcpus] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&DependentLoadThread,
						  (void*)&params[numcpus], 0, &tId);
	// Wait for everyone to finish
	WaitForSingleObject(g_hExitEvent, INFINITE);
	ResetEvent(g_hExitEvent);
#else
	rc = pthread_create(&ThreadData[i], NULL, (void*)DependentLoadThread, (void*)&params[i]);
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	assert(pthread_cond_wait(&g_ExitCond, &g_ExitMutex)==0);
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif
	total_indep_load = 0; // total loads from throughput threads
	Mbytes_per_sec=0;

	//printf("reached x\n");
	for (i = 0; i < numcpus + 1; i++) {
#if !defined(LINUX) && !defined(__APPLE__)
		CloseHandle(ThreadData[i]);
#else

		if (pthread_join(ThreadData[i], NULL) != 0) {
			printf("pthread_Join error\n");
		}

#endif
	
		// calculate the load generated by each throughput thread
		Mbytes_per_sec = (((double)params[i].total_xfer * 64) / (((double)params[i].total_time * NsecClk) / 1000000000)) / (1000 * 1000);
		if (i != numcpus) { // i -- numcpus indicate the last thread which is the latency thread
			if (print_details && print_per_thread_throughput) {
				printf("thread\t%d\t%.1f\n", params[i].local_id, Mbytes_per_sec);
			}

			total_indep_load += Mbytes_per_sec;
			//printf("total_indep_load = %f\n", total_indep_load);
		} else {

			if (thruput_only) {
				latency = 0;
			} else {
				// this case is for dependent loads only as we are not measuring latency for throughput threads as they are just
				// blasting traffic
				latency = (double)params[i].total_time * NsecClk / (double)params[i].total_xfer;
			}

			if (matrix_mode == 0) {
				if (print_details && !print_per_thread_throughput) {
					printf("%6.2f\t", latency);
				}
			}
		}
	}

	//printf("%.1f\t%.1f MBytes-per-sec\n", total_indep_load, total_indep_load+Mbytes_per_sec);
	if (print_details && matrix_mode == 1) {
		printf("%7.1f\t", (total_indep_load + Mbytes_per_sec));
	} else if (print_details && matrix_mode != 1) {
		printf("%9.1f\n", total_indep_load + Mbytes_per_sec);
	}
	if (bw_p) { *bw_p = total_indep_load + Mbytes_per_sec; }
	if (latency_p) { *latency_p = latency; }

#if defined(LINUX) || defined(__APPLE__)
	assert(pthread_attr_destroy(&attr)==0);
#endif
}

// Allocates memory for throughput and latency threads
void AllocMemAndInitializeThreads()
{
	if (numcpus) {
		AllocateMemoryForThruputThreads();
	}
#if !defined(LINUX) && !defined(__APPLE__)
	g_hExitEvent = CreateEvent(NULL, TRUE,	FALSE,	0);
#else
	assert(pthread_mutex_init(&g_ExitMutex, NULL)==0);
	assert(pthread_cond_init(&g_ExitCond, NULL)==0);
#endif
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

#endif
	AllocDependentLoadMemory(&params[numcpus]);

	if (print_verbose1) {
		printf("\nAllocated Memory!\n");
	}

	// Start the latency thread (also called dependent load thread
	if (matrix_mode == 0 || thruput_only == 0) {
		printf("Inject\tLatency\tBandwidth\n");
		printf("Delay\t(ns)\tMB/sec\n");
		printf("==========================\n");
	}

// add here
// deallocate memory

	if (print_verbose1) {
		printf("\nDeallocating memory!\n");
	}
}

void DeallocAndCleanup()
{
	int i;
	__int64 len;
	
	for (i = 0; i < numcpus; i++) {
		len = getActualLenToAllocate(params[i].buf_len); //get the actual length of the buffer allocated
	
		if (params[i].local_buf != NULL) {
		
#if defined(LINUX) || defined(__APPLE__)
			munmap(params[i].local_buf, len);
#else
			VirtualFree(params[i].local_buf, 0, MEM_RELEASE);
#endif
		}

		if (params[i].local_buf2 != NULL) {
#if defined(LINUX) || defined(__APPLE__)
			munmap(params[i].local_buf2, len);
#else
			VirtualFree(params[i].local_buf2, 0, MEM_RELEASE);
#endif
		}

		if (params[i].remote_buf != NULL) {
#if defined(LINUX) || defined(__APPLE__)
			munmap(params[i].remote_buf, len);
#else
			VirtualFree(params[i].remote_buf, 0, MEM_RELEASE);
#endif
		}

		if (params[i].write_buf != NULL) {
#if defined(LINUX) || defined(__APPLE__)
			munmap(params[i].write_buf, len);
#else
			VirtualFree(params[i].write_buf, 0, MEM_RELEASE);
#endif
		}
		if (params[i].write_buf2 != NULL) {
#if defined(LINUX) || defined(__APPLE__)
			munmap(params[i].write_buf2, len);
#else
			VirtualFree(params[i].write_buf2, 0, MEM_RELEASE);
#endif
		}

		params[i].local_buf = NULL;
		params[i].local_buf2 = NULL;
		params[i].remote_buf = NULL;
		params[i].write_buf = NULL;
		params[i].write_buf2 = NULL;
		//printf("deallocaton done!\n");
	}
#if !defined(LINUX) && !defined(__APPLE__)
	// The following clean up code has to be commented out as it is resulting in random crashes
	//HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, ThreadData);
#else
		free(ThreadData);
#endif
}

// Main function called after all arguments are parsed
// This supports b/w measurement in two ways
// 1. MAX_INJECTION_BW - loads/stores are generated at max rate but this may or may not produce max b/w
// 2. MAX_BW_AT_ANY_INJECTION - We automatically try different injection rates to identify the max possible b/w
// The 'type' parameter to this function would take one of the two values mentioned above
void StartLoading(int type)
{
	static int delays[] = { 0, 15, 30, 45, 60, 75, 90, -1};
	double max_bandwidth = 0, total_bandwidth = 0;
	int iter, best_delay_value = 0;

    AllocMemAndInitializeThreads();

	for (iter = 0;; iter++) {
		//go through all the delay injection values
		if (type == MAX_INJECTION_BW) {
			delay_time = delay_array[iter];
			if (delay_time == -1) break;
			get_bw_and_latency ( 1, delay_time, NULL, NULL);
		}
		else if (type == MAX_BW_AT_ANY_INJECTION) {
			// Try a few fixed injection delay values and get the max b/w out of all those
			delay_time = delays[iter];
			if (delay_time == -1) break;
			get_bw_and_latency ( 0, delay_time, &total_bandwidth, NULL);
			if (total_bandwidth > max_bandwidth) {
				max_bandwidth = total_bandwidth;
				best_delay_value = delay_time;
			}

		}
	}
	if (type == MAX_BW_AT_ANY_INJECTION) {
		if (print_verbose1) {
			printf("max_bandwidth -- %6.2f MB/sec measured at injection delay of %d\n", max_bandwidth, best_delay_value);
		}
		printf("%6.2f", max_bandwidth);
	}
	DeallocAndCleanup();

}
