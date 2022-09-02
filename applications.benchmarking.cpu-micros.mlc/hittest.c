

#if !defined(LINUX) && !defined(__APPLE__)
#include <windows.h>
#include <winbase.h>
#else
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
#endif

#include <stdlib.h>
#include <stdio.h>

#include "types.h"

#include "common.h"

//typedef  __int64 UINT64;
//typedef  __int64 UINT64;
#if defined(LINUX) || defined(__APPLE__)
#define VOID
#endif

#define HITTEST_PRINT_IN_MATRIX 0
#define HITTEST_PRINT_NORMAL 	1
#define HITTEST_PRINT_OFF 		2
#define HITTEST_PRINT_DETAIL    3

#define WRITER 	0
#define READER 	1
#define READER2	2

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x40000 /* arch specific */
#endif

/* This define must be the size of the largest buffer used below so that */
/* memory is allocated and initialized correctly. Be sure and keep the */
/* parenthesis because this is used in calculations */

void StartTheTest(VOID);

#define MAX_BUF_SIZE (64 * 1024 * 1024)

#define INT 4
#define PAGE (4*1024)
#define LINE 64
#define PAD (PAGE/INT)+(LINE/INT)

#define OP_CALIBRATE 'C'
#define OP_READ 'R'

extern __int64 * do_dependent_read(__int64* a);
void touch_buffer(char*buf, __int64 len);
void Create_random_pattern(__int64 *buf, __int64 size);
void evict_line_from_cache(volatile void *ptr);

int chk_full();

void do_calibrate(__int64* a);
void Calibrate(UINT64	*ClksPerSec);
void AllocateMemory(VOID);
int BindToCpu(int nodeid);
void CreatePstateThreadsOnOtherCores(char *pcpu_str);
void  check_for_null_parameter(char b, char arg);
void c2c_latency_matrix(int home_address);

#if !defined(LINUX) && !defined(__APPLE__)
extern BOOL enable_windows_large_pages();
HANDLE process;
HANDLE hToken;
#endif

extern int flush_cache;
extern int rfo_c2c;
int bril_hitm=0;
int Write_Affinity = 0;
int adj_sector_pref_on=0;
int extra_reader_thread_bytes=0;
int extra_bytes=0;

int num_runs;

struct _hitparams
{
	__int64 total_xfer;
	__int64 total_time;
	char *buf;
	char *remote_buf;
	char *write_buf;
	__int64 innerLines;
	__int64 numblks;
	__int64 buf_len;
	__int64 nLines;
	__int64 set_value;
	__int64 lookup_value;
	__int64 cpu_id;
	__int64 print_opt;
	__int64 junk[3]; // to ensure the whole structure fits in one 128 byte line
} hitparams[MAX_THREADS];

int mask=0;
int volatile slave_flag=1;
int volatile numLoadThreads=0;
#if !defined(LINUX) && !defined(__APPLE__)
HANDLE hExitEvent = NULL;						// Global exit event handle
int ret_val;
#else
#endif
extern __int64 loopsize;
extern int hittest;
int third_thread=-1;
char dbuf[1024];
__int64 max_iterations = (__int64)30000000000;

void c2c_latency_main(char matrix_mode)
{
    //Common initialization between matrix mode and non matrix mode
    hitparams[WRITER].print_opt = HITTEST_PRINT_OFF; //0 is the writer thread
    hitparams[READER].print_opt = HITTEST_PRINT_NORMAL; //1 is the reader thread
    hitparams[READER2].print_opt = HITTEST_PRINT_OFF; //2 is the third (reader) thread
    if(!c_provided) CPU_Affinity = pkg_map_reserved[0][0];
    if(!w_provided) Write_Affinity = pkg_map_reserved[0][1];
    if(!x_provided) num_iterations = 20e6;
    if(!C_provided) loopsize = (__int64) (cache_info[L1D_CACHE].total_size * 1.2); //Window size to be 120% of L1-D size
    if(!E_provided) extra_bytes = (int) (cache_info[L1D_CACHE].total_size * 1.2); //Writer reads 120% of L1-D size to evict M line to L2

    if(print_verbose1) {
        printf("L1-I cache (%d) size:%d KBytes\n",cache_info[L1I_CACHE].cache_type,cache_info[L1I_CACHE].total_size/1024);
        printf("L1-D cache (%d) size:%d KBytes\n",cache_info[L1D_CACHE].cache_type,cache_info[L1D_CACHE].total_size/1024);
        printf("L2 cache (%d) size:%d KBytes\n",cache_info[L2_CACHE].cache_type,cache_info[L2_CACHE].total_size/1024);
        printf("L3 cache (%d) size:%d KBytes\n",cache_info[L3_CACHE].cache_type,cache_info[L3_CACHE].total_size/1024);
        printf("Writer thread ID:%d\tReader thread ID:%d\tThird thread ID:%d\n",
					Write_Affinity,CPU_Affinity,third_thread);
    }

    printf("\n");
    printf("Measuring cache-to-cache transfer latency (in ns)...\n");

	if (pcpu_str[0] != 0)
	{
		CreatePstateThreadsOnOtherCores(pcpu_str);
	}

#if !defined(LINUX) && !defined(__APPLE__)
	ret_val = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);

	if (lp_enabled == 1 && enable_windows_large_pages())
	{
		//lp_enabled=1;
	}
	else
	{
		printf("Using small pages for allocating buffers\n");
		lp_enabled=0;	
	}
#endif


	Run_Time = (UINT64)time_in_seconds * (UINT64)((double)1000000000/(NsecClk));


#if !defined(LINUX) && !defined(__APPLE__)
	process = OpenProcess(PROCESS_SET_INFORMATION|PROCESS_QUERY_INFORMATION,
			FALSE, GetCurrentProcessId());
#endif
	/* Commented out as it is already done in main.c
	if (Random_Access)
	{
		srand( (unsigned)time( NULL ) );
		// initialize the buffer to enable random access
		//get_rand(rand_array);
	}
	*/

	if(!matrix_mode)
    {
        hitparams[READER].print_opt = HITTEST_PRINT_DETAIL;
		StartTheTest();
    }
	else
	{
		//L2 -> L2 HIT test
		if (AMD_cpu)	
			loopsize = 400*1024; // Use 400KB to make sure good chunk fits in L2. If it is too small then L1D values dominate
		else
			loopsize = (__int64) (cache_info[L1D_CACHE].total_size * 3); //Window size to be 3x of L1-D size
		hittest = 1;
		printf("Local Socket L2->L2 HIT  latency");
		StartTheTest();
		//L2 -> L2 HITM test
		extra_bytes = (int) (cache_info[L1D_CACHE].total_size * 1.2); //Writer reads 120% of L1-D size to evict M line to L2
		hittest = 0;
		printf("Local Socket L2->L2 HITM latency");
		StartTheTest();

		if(n_sockets > 1) {
			printf("Remote Socket L2->L2 HITM latency (data address homed in writer socket)\n");
            c2c_latency_matrix(WRITER);
			printf("Remote Socket L2->L2 HITM latency (data address homed in reader socket)\n");
            c2c_latency_matrix(READER);
		} //n_sockets
	} //!matrix mode else
}

void c2c_latency_matrix(int home_address)
{
	int i, j;

	if(numa_node_access_enabled == 0) { //do socket level latency breakdown
		if (!print_verbose1) {
			printf("\t\tReader Socket\nWriter Socket\t");

			// format matrix first row
			for (i = 0; i < num_nodes; i++) {
				if (sockets[i] == SOCKET_PRESENT) {
					printf("%6d\t", i);
				}
			}

			printf("\n");
		}
		for (i = 0; i < num_nodes; i++) {
			if(home_address == WRITER)
				Init_Affinity = pkg_map_reserved[i][0];

			if (sockets[i] == SOCKET_PRESENT) {
				if (!print_verbose1) {
					printf("%13d\t", i);
				}

				Write_Affinity = pkg_map_reserved[i][0];

				for (j = 0; j < num_nodes; j++) {
					if(home_address == READER)
						Init_Affinity = pkg_map_reserved[j][0];

					if (sockets[j] == SOCKET_PRESENT) {
						if(i == j)
							printf("     -\t");
						else {
							if (cache_info[L2_CACHE].total_size >= 600*1024 && !AMD_cpu)  { // L2 size > 600KB (as on SKX), then we will set the buf size to be 512KB) 
								buffer_size = 512*1024; // in KB
								loopsize = 512 * 1024;
							}
							else if (AMD_cpu) { 
								buffer_size = 500 *1024; // in KB
								loopsize = 400 * 1024;
							}
							else {
								buffer_size = 512*1024; // in KB
								loopsize = 128 * 1024;  // L2 is only 256KB on older Xeons
							}
							extra_bytes = (int) (cache_info[L1D_CACHE].total_size * 1.2); //Writer reads 120% of L1-D size to evict M line to L2
							hittest = 0;
							CPU_Affinity = pkg_map_reserved[j][1];
							hitparams[READER].print_opt = HITTEST_PRINT_IN_MATRIX;

							if (print_verbose1) {
								hitparams[READER].print_opt = HITTEST_PRINT_NORMAL;
								printf("\nWriter socket%2d (core %3d) Reader socket%2d (core %3d) Memory socket%2d (core %3d)..",
										i, pkg_map_reserved[i][0], j, pkg_map_reserved[j][0], 
										home_address == WRITER ? i : j,
										home_address == WRITER ? pkg_map_reserved[i][0] : pkg_map_reserved[j][0]);
							}

							// now allocate memory and do the pointer chasing, with the latency measurement, in DoDependentLoads()
							StartTheTest();
						}
					} //socket present
				} //for j num_nodes
				printf("\n");
			} //socket present
		} //for i num_nodes
	} //numa_node_access_enabled == 0
	else {
		if (!print_verbose1) {
			printf("\t\t\tReader Numa Node\nWriter Numa Node");

			// format matrix first row
			for (i = 0; i < num_numa_nodes; i++) {
				if (numanode_cpu_map_ctr[i]) {
					printf("%6d\t", i);
				}
			}

			printf("\n");
		}
		for (i = 0; i < num_numa_nodes; i++) {
			if(home_address == WRITER)
				Init_Affinity = numanode_cpu_map_reserved[i][0];

			if (numanode_cpu_map_ctr[i]) {
				if (!print_verbose1) {
					printf("%13d\t", i);
				}

				Write_Affinity = numanode_cpu_map_reserved[i][0];

				for (j = 0; j < num_numa_nodes; j++) {
					if(home_address == READER)
						Init_Affinity = numanode_cpu_map_reserved[j][0];

					if (numanode_cpu_map_ctr[j]) {
						if(i == j)
							printf("     -\t");
						else {
							if (cache_info[L2_CACHE].total_size >= 600*1024 && !AMD_cpu)  { // L2 size > 600KB, then we will set the buf size to be 512KB) 
								buffer_size = 512*1024; // in KB
								loopsize = 512 * 1024;
							}
							else if (AMD_cpu) { 
								buffer_size = 500 *1024; // in KB
								loopsize = 400 * 1024;
							}
							else {
								buffer_size = 512*1024; // in KB
								loopsize = 128 * 1024; // L2 is only 256KB on older Xeons
							}
							extra_bytes = (int) (cache_info[L1D_CACHE].total_size * 1.2); //Writer reads 120% of L1-D size to evict M line to L2
							hittest = 0;
							CPU_Affinity = numanode_cpu_map_reserved[j][1];
							hitparams[READER].print_opt = HITTEST_PRINT_IN_MATRIX;

							if (print_verbose1) {
								hitparams[READER].print_opt = HITTEST_PRINT_NORMAL;
								printf("\nWriter numa node%2d (core %3d) Reader numa node%2d (core %3d) Memory numa node%2d (core %3d)..",
										i, numanode_cpu_map_reserved[i][0], j, numanode_cpu_map_reserved[j][0], 
										home_address == WRITER ? i : j,
										home_address == WRITER ? numanode_cpu_map_reserved[i][0] : numanode_cpu_map_reserved[j][0]);
							}

							// now allocate memory and do the pointer chasing, with the latency measurement, in DoDependentLoads()
							StartTheTest();
						}
					} //socket present
				} //for j num_nodes
				printf("\n");
			} //socket present
		} //for i num_nodes
	} //numa_node_access_enabled == 1
}

// This thread reads all cache lines that are resident on another CPU's cache in modified state
// Every read from this thread will hit modified (HITM) on other processors' cache resulting in cache 
// cache transfer
#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI ReaderThread(PVOID Parm)
#else
unsigned int ReaderThread(void* Parm)
#endif
{
	__int64 *arr, **datap, **wrp;
	__int64 i, j;
	__int64 after, before, total_time=0, total_xfer=0;
	__int64 innerLines, numblks;
	struct _hitparams *p;
	int set_value, lookup_value;
	double latency, core_clks;
	__int64 iter;
	__int64 *extra_buf, *ep;
	__int64 extra_lines, junkcnt=0;

	p = (struct _hitparams *) Parm;

	arr =(__int64*)(p->buf);
	innerLines = p->innerLines;
	numblks = p->numblks;
	lookup_value= p->lookup_value;
	set_value= p->set_value;

	BindToCpu(p->cpu_id);
	if (extra_reader_thread_bytes)
	{
		extra_buf = (__int64*) GetMemory(extra_reader_thread_bytes);
		touch_buffer((char*)extra_buf, extra_reader_thread_bytes);
		extra_lines = extra_reader_thread_bytes / LineSize;
	}
	if (rfo_c2c) // Stores to lines cached in other core
	{
		for(iter=0; iter < num_iterations; )
		{
			datap = (__int64**)arr;
			for (j=0; j < numblks; j++)
			{
				while (slave_flag!=lookup_value); // Wait for the writer thread to finish its job and signal us
				before = THE_TSC;
				datap = (__int64**)do_write((__int64)innerLines,  (int*)datap, (__int64)LineSize_LatThd);
				after = THE_TSC;
				slave_flag=set_value; // reset the flag to signal the writer thread
				total_time += (after-before);
				total_xfer += innerLines;

			}
			iter = iter+(numblks*innerLines);
		}
	}
	else
	{
		for(iter=0; iter < num_iterations; )
		{
			datap = (__int64**)arr;
			for (j=0; j < numblks; j++)
			{
				while (slave_flag!=lookup_value); // Wait for the writer thread to finish its job and signal us
				wrp = datap;
				before = THE_TSC;
				// read a block of data containing 'innerLines' of cache lines
				datap = (__int64 **) do_specified_dependent_read((UINT64*)datap, (UINT64)innerLines);
				//do_dependent_read_one_iteration_only((int*)datap, innerLines);
				after = THE_TSC;
				if (extra_reader_thread_bytes)
				{
					ep = extra_buf;
					for (i=0; i < extra_lines; i++)
					{
						junkcnt += *ep;
						ep+=(LineSize/sizeof(__int64));
					}
				}

				slave_flag=set_value; // reset the flag to signal the writer thread
				total_time += (after-before);
				total_xfer += innerLines;

			}
			iter = iter+(numblks*innerLines);
		}
	}

	if (total_xfer != 0) {
		latency = (double)total_time*NsecClk / (double)total_xfer;
		core_clks = (double)total_time / (double)total_xfer;
	}
	else {
		latency = 0; core_clks = 0;
	}
	switch(p->print_opt)
	{
		case HITTEST_PRINT_IN_MATRIX:
			printf ("%6.1f\t", latency);
			break;
		case HITTEST_PRINT_NORMAL:
			printf ("\t%.1lf\n", latency);
			break;
		case HITTEST_PRINT_DETAIL:
			printf ("\nLatency = %.1lf base frequency clocks (%.1lf ns)\n", core_clks, latency);
			break;
		case HITTEST_PRINT_OFF:
			break;
	}

	//printf ("RESULT\t%d\t%d\t%d\t%.1lf\n", CPU_Affinity, Write_Affinity, Init_Affinity, latency);
#if defined(LINUX) || defined(__APPLE__)
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	numLoadThreads--;
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#else
	InterlockedDecrement((long *)&numLoadThreads);
#endif
	if (numLoadThreads ==0) // if I am the last thread leaving, signal the main thread to wrap up
	{
#if !defined(LINUX) && !defined(__APPLE__)
		SetEvent(hExitEvent);
#else
		assert(pthread_mutex_lock(&g_ExitMutex)==0);
		assert(pthread_cond_signal(&g_ExitCond)==0);
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif
	}
	return 0;
}

// This thread modifies a bunch of lines (in case of HITM test) in its cache and 
// transfers control to the reader thread
#if !defined(LINUX) && !defined(__APPLE__)
UINT WINAPI WriterThread(PVOID Parm)
#else
unsigned int WriterThread(void* Parm)
#endif
{
	__int64 *arr, **datap;
	__int64 i, j;
	struct _hitparams *p;
	__int64	innerLines, numblks;
	__int64 iter;
	__int64 *extra_buf, *ep;
	__int64 extra_lines;
	int set_value, lookup_value;

	p = (struct _hitparams *) Parm;

	arr =(__int64*)(p->buf);
	innerLines = p->innerLines;
	numblks = p->numblks;
	lookup_value= p->lookup_value;
	set_value= p->set_value;

	BindToCpu(p->cpu_id);
	if (extra_bytes)
	{
	    extra_buf = (__int64*) GetMemory(extra_bytes);
	    touch_buffer((char*)extra_buf, extra_bytes);
    	extra_lines = extra_bytes / LineSize;
    }
	
	if (hittest) // in case of HIT test, the line will NOT be modified
	{
		for(iter=0; iter < num_iterations; )
		{
			int junkcnt=0;
			datap = (__int64**)arr;
			for (j=0; j < numblks; j++)
			{
				while (slave_flag!=lookup_value);
				for (i=0; i < innerLines; i++)
				{
					datap = (__int64**)*datap;
				}
				if (extra_bytes)
				{
					ep = extra_buf;
					for (i=0; i < extra_lines; i++)
					{
						junkcnt += *ep;
						ep+=(LineSize/sizeof(__int64));
					}
				}
				slave_flag=set_value;
			}
			iter = iter+(numblks*innerLines);
		}
	}
	else // the line will be modified so that the reader thread when reading this line will receive HITM snoop response
	{
		for(iter=0; iter < num_iterations;)
		{
			int junkcnt=0;
			datap = (__int64**)arr;
			for (j=0; j < numblks; j++)
			{
				while (slave_flag!=lookup_value);

				datap = (__int64**)do_write((__int64)innerLines,  (int*)datap, (__int64)LineSize_LatThd);
				// In case of Merom, modified data may still reside in L1-D and due to on-core L1D to L1D
				// transfer latency differences, it is important to evict all the modified 
				// lines out of L1 and into L2 
				if (extra_bytes)
				{
					ep = extra_buf;
					for (i=0; i < extra_lines; i++)
					{
						junkcnt += *ep;
						ep+=(LineSize/sizeof(__int64));
					}
				}
				
				slave_flag=set_value;
			}
			iter = iter+(numblks*innerLines);
		}
	}
	stop_load = 1;

#if defined(LINUX) || defined(__APPLE__)
	assert(pthread_mutex_lock(&g_ExitMutex)==0);
	numLoadThreads--;
	assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#else
	InterlockedDecrement((long *)&numLoadThreads);
#endif
	if (numLoadThreads ==0) // if I am the last thread leaving, signal the main thread to wrap up
	{
#if !defined(LINUX) && !defined(__APPLE__)
		SetEvent(hExitEvent);
#else
		assert(pthread_mutex_lock(&g_ExitMutex)==0);
		assert(pthread_cond_signal(&g_ExitCond)==0);
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);
#endif
	}

	return 0;
}

void StartTheTest(VOID)
{
	__int64 i;
	__int64 idx;
	char *buf;
#if !defined(LINUX) && !defined(__APPLE__)
	PHANDLE *ThreadData;
	int tId;
#else
	pthread_t *ThreadData;
	pthread_attr_t attr;
#endif
	__int64 LineIncrement, nLines, buf_len, innerLines, numblks;
	__int64 **datap;
	

	//if (AMD_cpu && !l_provided)
		//LineSize = AMD_LINE_STRIDE_SIZE; // to beat adjacent line prefetch
	BindToCpu(Init_Affinity);

    buf_len = (__int64)buffer_size*1024LL;
	buf = GetMemory(buf_len);
	touch_buffer((char*)buf, buf_len);

	if(print_verbose1) printf("\tbuf_size=%d KB, loopsize=%ld KB, extra bytes=%d KB\n", buffer_size, loopsize/1024, extra_bytes/1024);
	LineIncrement = LineSize_LatThd/sizeof(__int64); 

	nLines = (buf_len) / LineSize_LatThd;
	datap = (__int64**)buf;

	if (Random_Access)
	{
		Create_random_pattern((__int64*)datap, buf_len);
	}
	else
	{
		// Initialize the lines sequentially
		for (i=0; i < nLines-1; i++)
		{
			*datap = (__int64*)((char*)datap + LineSize_LatThd);
			datap=(__int64**)((char*)datap + LineSize_LatThd);
		}
		*datap=(__int64*)buf;
	}
	innerLines = loopsize/LineSize_LatThd;
	numblks = nLines/innerLines;
	if (flush_cache) {
		for (idx=0; idx < nLines; idx++) {
			char *ptr = (char*)buf + (idx*LineSize_LatThd);
			evict_line_from_cache ((void *)(ptr));
		}
	}
		/* execute mfence to ensure all lines are flushed out */
#if defined(LINUX) || defined(__APPLE__)
		asm ("mfence");
#else
		__asm { mfence } 
#endif
//printf("inner lines=%d, numblks = %d\n", innerLines, numblks);
#if !defined(LINUX) && !defined(__APPLE__)
 	hExitEvent = CreateEvent(NULL, TRUE,	FALSE,	0);
#else
	assert(pthread_mutex_init(&g_ExitMutex,NULL)==0);
	assert(pthread_cond_init(&g_ExitCond,NULL)==0);
#endif
#if !defined(LINUX) && !defined(__APPLE__)
	// Allocate storage for our thread handle array
	if ((ThreadData = (PHANDLE *)
			HeapAlloc(GetProcessHeap(),
					0,
					sizeof(PHANDLE) * MAX_THREADS)) == NULL)
	{
		printf("g_phThread HeapAlloc error!\n");
		return;
	}
#else
	if ((ThreadData = (pthread_t*) malloc(sizeof(pthread_t) * MAX_THREADS)) == NULL)
	{
		printf("g_phThread malloc error!\n");
		return;		
	}
	if (pthread_attr_init(&attr) !=0) {
		app_exit (EACCES, "pthread_attr_init error\n");
	}
	else
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
		app_exit (EACCES, "pthread_attr_setdetachstate error\n");
#endif	
	for (i=0; i < 3; i++)
	{
		hitparams[i].buf=buf;
		hitparams[i].nLines=nLines;
		hitparams[i].buf_len = buf_len;
		hitparams[i].innerLines = innerLines;
		hitparams[i].numblks = numblks;
	}
	numLoadThreads=2;
	slave_flag=2;
	hitparams[WRITER].lookup_value=2;
	hitparams[WRITER].set_value= (third_thread == -1)?0:1;
	hitparams[WRITER].cpu_id = Write_Affinity;
#if !defined(LINUX) && !defined(__APPLE__)
	ThreadData[WRITER] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&WriterThread,
		(void*)&hitparams[WRITER], 0, &tId);
#else
	pthread_create(&ThreadData[WRITER], &attr, (void *)WriterThread, (void*)&hitparams[WRITER]);
#endif
	hitparams[READER].lookup_value=0;
	hitparams[READER].cpu_id = CPU_Affinity;
	hitparams[READER].set_value=2;
#if !defined(LINUX) && !defined(__APPLE__)
	Sleep(2000); // sleep a bit so the writer thread can start; it is essential that the writer thread
	// runs first otherwise, we will end up with dead-lock
	ThreadData[READER] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ReaderThread, (void*)&hitparams[READER], 0, &tId);
#else
	sleep(2);
	pthread_create(&ThreadData[READER], &attr, (void *)ReaderThread, (void*)&hitparams[READER]);
#endif
	if (third_thread != -1)
	{
		numLoadThreads++;
		hitparams[READER2].lookup_value=1;
		hitparams[READER2].set_value=0;
		hitparams[READER2].cpu_id = third_thread;
#if !defined(LINUX) && !defined(__APPLE__)
		ThreadData[READER2] = (PHANDLE) CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ReaderThread, (void*)&hitparams[READER2], 0, &tId);
#else
		pthread_create(&ThreadData[READER2], &attr, (void *)ReaderThread, (void*)&hitparams[READER2]);
#endif
	}

#if !defined(LINUX) && !defined(__APPLE__)
		WaitForSingleObject(hExitEvent, INFINITE);
		ResetEvent(hExitEvent);
		VirtualFree(buf, 0, MEM_RELEASE);
#else
		free(ThreadData);
		assert(pthread_attr_destroy(&attr)==0);
		assert(pthread_mutex_lock(&g_ExitMutex)==0);;
		assert(pthread_cond_wait(&g_ExitCond,&g_ExitMutex)==0);;
		assert(pthread_mutex_unlock(&g_ExitMutex)==0);;
		munmap(buf, getActualLenToAllocate(buf_len));

#endif

}
