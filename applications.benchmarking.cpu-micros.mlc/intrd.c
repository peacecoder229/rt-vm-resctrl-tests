/*
Copyright (c) 2013, Intel Corporation
Intel(R) Memory Latency Checker
Contributors: Vish Viswanathan, Karthik Kumar, Thomas Willhalm
 */

//#define LINUX
#if defined(LINUX) || defined(__APPLE__)
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
#else
#include <windows.h>
#include <synchapi.h>
#include <memoryapi.h>
#include <minwindef.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#endif

#include "types.h"
#include "topology.h"
#include "common.h"


/* The following structure definitions cpuid_info_t, APIC_ID_t are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

 */


/* functions used*/
void Create_random_pattern(UINT64** datap, UINT64 size);

#if defined(LINUX) || defined(__APPLE__)
static UINT64 GetTickCount();
#endif
void DoDependentLoads(int nodeid);
void exit_handler1(int);
int chk_full();
UINT64* AllocAndInitMemoryForLatencyThread(UINT64 buf_len, int nodeid);
void do_dependent_read_one_iteration_only(UINT64* a, UINT64 nLines);
void touch_buffer(char* buf, __int64 len);
void run_latency_measurement();
void  check_for_null_parameter1(char b, char arg);
void save_and_disable_pref();
char* GetMemory(UINT64 len);
void DeleteAllSCMFiles();

/* global variables*/

int evict_initialized_lines_once = 0;
int split_access = 0;

extern int flush_cache;
extern int tsc_freq;

int latency()
{

	// If this is the matrix mode for latencies, execute run_latency_measurement() and return to main
	if (latency_matrix_mode == 1) {
		matrix_mode1 = 1;
		if (allcore_latency == 1) 
			run_all_core_latency_measurement();
		else
			run_latency_measurement();
		return 0;
	}
	if (pcpu_str[0] != 0) {
		// If p is specified, create busy threads to keep 1 core in each socket active based on pcpu_str
		CreatePstateThreadsOnOtherCores(pcpu_str);
	} else {
		// if p is not specified, create busy threads on all sockets (note that CPU_Affinity core will be omitted)
		if (num_nodes > 1) CreatePstateThreadsOnAllSockets(-1); // do this only on >1 socket system
	}

	// Compute how long the test needs to run, in clocks
	Run_Time = (UINT64)time_in_seconds * (UINT64)((double)1000000000 / (NsecClk));
#ifdef DEBUG
	printf("Test needs to run for %d seconds, or %llu clocks, as clocks per second for this processor is %.2f\n", time_in_seconds, Run_Time, NsecClk);
#endif

	// If random access specified, initialize accordingly */
	/* The following block is already done in main.c and hence commented out here
	if (Random_Access) {
		srand((unsigned)time(NULL));
		//get_rand();
	}
	*/

	// Now Allocate Memory and do the pointer chasing, with the latency measurement, in DoDependentLoads()
	DoDependentLoads(-1);

	DeleteAllSCMFiles();

	printf("\n");
	return 0;
}
void run_latency_measurement()
{
	// begin with CPU 0 and memory 0
	int i, j;
	CPU_Affinity = 0;
	Init_Affinity = 0;
	// keep one thread active on every socket, we have hard-coded this to be the 1st core (core 0) on each
	// package (pkg_map_reserved[i][0].os_id;), -1 indicates no socket is omitted for latency matrix,
	// since we never pick core 0 on a package for measurements in this mode and hence there wont be a core conflict
	if (num_nodes > 1) CreatePstateThreadsOnAllSockets(-1); // not needed for 1S systems
	printf("Measuring idle latencies (in ns)...\n");

	if (!print_verbose1) {
		if (numa_node_access_enabled == 0) {

			//if pmem_only enabled, no need to loop on numa since mem will be maped from SCM file
			if (1 == pmem_only)
			{
				printf("\tSCM Drive\nSocket\t");
				printf("%s", mmap_files_dir);
			}
			else
			{
				printf("\tMemory node\nSocket\t");
				// format matrix first row
				for (i = 0; i < num_nodes; i++) {
					if (sockets[i] == SOCKET_PRESENT) {
						printf("%6d\t", i);
					}
				}
			}

		}
		else {
			if (1 == pmem_only)
			{
				printf("\tSCM Drive\nNuma node\t");
				printf("%s", mmap_files_dir);
			}
			else
			{
				printf("\t\tNuma node\nNuma node\t");
				// format matrix first row
				for (i = 0; i < num_numa_nodes; i++) {
					printf("%6d\t", i);
				}
			}
		}
		printf("\n");
	}

	// compute how long to run the test
	Run_Time = (UINT64)time_in_seconds * (UINT64)((double)1000000000 / (NsecClk));
	// step through each socket, and use the second core core1 (pkg_map_reserved[i][1].os_id) for CPU and memory allocation
	if (numa_node_access_enabled == 0) { // do socket level latency breakdown
		for (i = 0; i < num_nodes; i++) {
			if (sockets[i] == SOCKET_PRESENT) {
				if (!print_verbose1) {
					printf("%6d\t", i);
				}
				CPU_Affinity = pkg_map_reserved[i][LATENCY_THREAD_CORE_OFFSET];
				for (j = 0; j < num_nodes; j++) {
					if (sockets[j] == SOCKET_PRESENT) {
						Init_Affinity = pkg_map_reserved[j][LATENCY_THREAD_CORE_OFFSET];

						if (print_verbose1) {
							printf("\nSocket %2d (core %3d) measuring latency to memory on socket %2d (allocated by core %3d)..\n", i,
								   pkg_map_reserved[i][LATENCY_THREAD_CORE_OFFSET], j, pkg_map_reserved[j][LATENCY_THREAD_CORE_OFFSET]);
						}

						// now allocate memory and do the pointer chasing, with the latency measurement, in DoDependentLoads()
						DoDependentLoads(-1);

						//if pmem_only enabled, no need to loop on numa since mem will be maped from SCM file
						if (1 == pmem_only)
						{
							DeleteAllSCMFiles();
							break;
						}
					}
				}

				printf("\n");
			}
		}
	}
	else { // Do numa level breakdown instead of socket level
		for (i = 0; i < num_numa_nodes; i++) {
			if (numanode_cpu_map_ctr[i]){ // check whether this numa node has cpu resources
				if (!print_verbose1) {
					printf("%8d\t", i);
				}
				CPU_Affinity = numanode_cpu_map_reserved[i][LATENCY_THREAD_CORE_OFFSET];
				for (j = 0; j < num_numa_nodes; j++) {
					if (print_verbose1) {
						printf("\nNuma node %2d (core %3d) measuring latency to memory on numa node %2d ..\n", i,
							   numanode_cpu_map_reserved[i][LATENCY_THREAD_CORE_OFFSET], j);
					}
					// now allocate memory from numa node j and do the pointer chasing, 
					if (freemem_onnode(j) == 0) // if there is no free memory on node j, then skip
						printf("-\t");
					else
						DoDependentLoads(j);

					//if pmem_only enabled, no need to loop on numa since mem will be maped from SCM file
					if (1 == pmem_only)
					{
						DeleteAllSCMFiles();
						break;
					}

				}
				printf("\n");
			}
		}
	}
}

void run_all_core_latency_measurement()
{
	int i, k;

	printf("Measuring idle latencies (in ns) for all cores...\n");

	if (!print_verbose1) {
		if (numa_node_access_enabled == 0) {
			if (1 == pmem_only)
			{
				printf("\tSCM Drive\nCPU\t");
				printf("%s", mmap_files_dir);
			}
			else
			{
				printf("\tMemory node\nCPU\t");
				// format matrix first row
				for (i = 0; i < num_nodes; i++) {
					if (sockets[i] == SOCKET_PRESENT) {
						printf("%6d\t", i);
					}
				}
			}
		}
		else {
			if (1 == pmem_only)
			{
				printf("\tSCM Drive\nCPU\t");
				printf("%s", mmap_files_dir);
			}
			else
			{
				printf("\t\tNuma node\nCPU\t");
				// format matrix first row
				for (i = 0; i < num_numa_nodes; i++) {
					printf("%6d\t", i);
				}
			}
		}
		printf("\n");
	}

	// compute how long to run the test
	Run_Time = (UINT64)time_in_seconds * (UINT64)((double)1000000000 / (NsecClk));
	// step through all the h/w threads
	for (i = 0; i < os_cpu_count; i++) {
		// if use_1tpc is set, then use only the 1st thread in each core
		if (use_1tpc_bandwidth == 0 || (use_1tpc_bandwidth ==1 && os_map[i].smt_id==0)){
			CPU_Affinity = i;
			if (!print_verbose1) printf("%6d\t", CPU_Affinity);
			// If more than one socket is present, then run a busy thread in each of the other sockets
			if (num_nodes > 1) CreatePstateThreadsOnAllSockets(os_map[CPU_Affinity].pkg_id);
			if (numa_node_access_enabled) {
				for (k=0; k < num_numa_nodes; k++) {
					DoDependentLoads(k);

					//if pmem_only enabled, no need to loop on numa since mem will be maped from SCM file
					if (1 == pmem_only)
					{
						DeleteAllSCMFiles();
						break;
					}
				}
			}
			else {
				for (k=0; k < num_nodes; k++) {  // iterate through all sockets
					// allocate memory by pinning to CPU specified by Init_Affinity
					Init_Affinity = pkg_map_reserved[k][LATENCY_THREAD_CORE_OFFSET];
					DoDependentLoads(-1); // use -1 as NUMA allocation should not be done
				  //if pmem_only enabled, no need to loop on numa since mem will be maped from SCM file
					if (1 == pmem_only)
					{
						DeleteAllSCMFiles();
						break;
					}
				}
			}
			// If busy(dummy) threads were launched, it should be stopped by setting stop_busyloop_threads
			if (num_nodes > 1) {
				stop_busyloop_threads = 1;
				// Give time for the dummy threads to stop
#if defined(LINUX) || defined(__APPLE__)
				sleep(1);
#else
				Sleep(1000);
#endif
				stop_busyloop_threads = 0; // reset the flag so more dummy threads can be run during next iteration
			}
			printf("\n");
			if (print_verbose1) {
				//printf("\nNuma node %2d (core %3d) measuring latency to memory on numa node %2d ..\n", i,
				//	   numanode_cpu_map_reserved[i][LATENCY_THREAD_CORE_OFFSET], j);
			}
				// now allocate memory from numa node j and do the pointer chasing, 
			
		}
	}
	
}

#if defined(LINUX) || defined(__APPLE__)
static UINT64 GetTickCount()  //Return ns counts
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}
#endif

/* Evict the specified cache line from all levels */
#if defined(LINUX) || defined(__APPLE__)
void evict_line_from_cache(volatile void *ptr)
{
    asm volatile ("clflush (%0)" :: "r"(ptr));
}
#else
void evict_line_from_cache(volatile void *ptr)
{
	__asm {
		clflush [rcx]
		}
}
#endif


// This function counts how many times it can read the buffer (in a loop) in the given 't' seconds
// Each of those reads will be a miss and the resulting time per iteration gives the average access latency
void DoDependentLoads(int nodeid)
{
	UINT64* buf1, *curbufp1;
	UINT64 i1, nLines1 = 0, buf_len1, after1, before1, total_iter1 = 0, total_time1 = 0;
	__int64 num_iterations_tmp1 = 0, i;
	UINT64 iter_cnt1 = (UINT64)ITERATIONS;
	// convert buffer size to bytes from kB
    buf_len1 = (UINT64) buffer_size * (UINT64) 1024;
	// allocate memory based on buffer size, compute number of cachelines
	buf1 = AllocAndInitMemoryForLatencyThread(buf_len1, nodeid);
	
	nLines1 = buf_len1 / LineSize_LatThd;
	/* Skylake's non-inclusive cache has policies like dead-block
	predictor
	 that can lead to caching some of the addresses. To ensure all cache 
	 misses, we need to evict all the addresses we care about so they 
	 are read from memory
	*/
	if (flush_cache) {
		for (i=0; i < nLines1; i++) {
			char *ptr = (char*)buf1 + (i*LineSize_LatThd);
			evict_line_from_cache ((void *)(ptr));
		}
		/* execute mfence to ensure all lines are flushed out */
#if defined(LINUX) || defined(__APPLE__)
		asm ("mfence");
#else
		__asm { mfence } 
#endif
		
	}
	//printf("buf=%ld, buf_len=%d, nLines=%d\n",buf,buf_len,nLines);
	// bind to the CPU specified to run the test
	BindToCpu(CPU_Affinity);

	if (print_verbose1) {
		printf("Start loop for latency measurement...\n");
	}

	num_iterations_tmp1 = num_iterations;
	// read the tsc to begin time measurement
	before1 = THE_TSC;
	after1 = before1;
	// if only one iteration is specified (n0)
	
	if (one_iteration_only) {
		if (print_verbose1) {
			printf("Running one iteration...for %ld cache lines\n", (unsigned long)nLines1);
		}

		do_dependent_read_one_iteration_only(buf1, nLines1);
		after1 = THE_TSC;
		total_iter1 = nLines1;
	}
	// if number of iterations is specified instead of time to run the test
	else if (num_iterations_tmp1) {
		curbufp1 = buf1;

		// if number of iterations specified is less than the default of 10M, then set the iteration count to user specified value
		if (num_iterations_tmp1 < ITERATIONS) {
			iter_cnt1 = num_iterations_tmp1;
		}

		while (num_iterations_tmp1 > 0) {
			curbufp1 = do_specified_dependent_read(curbufp1, iter_cnt1);
			total_iter1 += iter_cnt1;
			num_iterations_tmp1 -= iter_cnt1;
			//after1 = THE_TSC;	//**sri1....why this here? it should be after the while loop
			//        	if(print_verbose1){printf("Running %d iterations..%lld iterations left\n",total_iter,num_iterations);}
		}
		after1 = THE_TSC;	//**sri1....why this here? it should be after the while loop....moved

	}
	// if time to run the test is specified
	else {
		curbufp1 = buf1;
		// do one iteration to populate the directory so we can measure dirty directory hit latencies properly
		do_dependent_read_one_iteration_only(buf1, nLines1);
		before1 = THE_TSC;
		after1 = before1;

		for (i1 = 0; (after1 - before1) < Run_Time ; i1++) {
#ifdef DEBUG
			//printf("after=%llu,before=%llu, after-before=%llu,Run_Time=%llu\n",after,before,after-before,Run_Time);
#endif
			curbufp1 = do_specified_dependent_read(curbufp1, iter_cnt1);
			total_iter1 += iter_cnt1;
			after1 = THE_TSC;
		}
	}

	// read the tsc to end time measurement, and obtain total time
	total_time1 = after1 - before1;
#ifdef DEBUG
	//printf("I is %d\n",i);
	printf("Measured latency over %llu core clocks, %11.11f nanoseconds,%llu iterations\n", total_time1, total_time1 * NsecClk, total_iter1);
#endif

	if (matrix_mode1 == 1 && (!print_verbose1)) {
		// print the output time in nanoseconds
		if (total_iter1)
			printf("%6.1lf\t", (total_time1 * NsecClk / total_iter1));
	} else {
		if (total_iter1 != 0)
			printf("Each iteration took %.1lf base frequency clocks (\t%.1lf\tns)\n", ((double)total_time1 / (double)total_iter1), (total_time1 * NsecClk / total_iter1));
	}
#if defined(LINUX) || defined(__APPLE__)
	munmap(buf1, getActualLenToAllocate(buf_len1));
#else
	VirtualFree(buf1, 0, MEM_RELEASE);
#endif
	before1 = THE_TSC;

}

UINT64* AllocAndInitMemoryForLatencyThread(UINT64 buf_len, int nodeid)
{
	//__int64* buf, **datap;
	UINT64 *buf1, *datap1;
	UINT64 i1, nLines1 = 0, LineIncrement1;
	// Bind to core on the socket where we want to allocate memory
	if (specify_mem_by_node == 0 && nodeid == -1) { // numa node id not specified
		BindToCpu(Init_Affinity);
		buf1 = (UINT64*) GetMemory(buf_len);
	}
	else {
		if (specify_mem_by_node) nodeid=memory_node;
		if (pmem_only == 1)
		{
			buf1 = (UINT64*)GrabMemory(buf_len, NV_RAM, nodeid, mmap_files_dir);
		}
		else
		{
			if (print_verbose1) { printf("AllocAndInitMemoryForLatencyThread: buf_len %lld,NUMA_NODE %d nodeId %d \n", buf_len, NUMA_NODE, nodeid); }
			buf1 = (UINT64*)GrabMemory(buf_len, NUMA_NODE, nodeid, NULL);
		}
	}
	// Allocate the memory of size buf_len
	assert(buf1);
	if (split_access) {
		buf1 = (UINT64*)((char*)buf1 + 63);
	}

	LineIncrement1 = LineSize_LatThd / sizeof(UINT64);
	nLines1 = (buf_len) / LineSize_LatThd;
	if (print_verbose1) {
		printf("Allocated %ld cache lines...\n", (unsigned long)nLines1);
	}

	datap1 = (UINT64*)buf1;

	if (Random_Access) {
		Create_random_pattern((UINT64**)datap1, buf_len);
	} else {
		// If random access not specified, initialize memory sequentially
		for (i1 = 0; i1 < nLines1-1; i1++) {
			*(UINT64**)datap1 = (UINT64*)(datap1 + LineIncrement1);
			datap1 = datap1 + LineIncrement1;
		}

		if (print_verbose1) {
			printf("memory initialized 1\n");
		}

		*(UINT64**)datap1 = buf1;

		if (print_verbose1) {
			printf("memory initialized 2\n");
		}
	}

	if (evict_initialized_lines_once) {
		char* tmp;
		// allocate 64M and walk through the lines so previously allocated lines get evicted
		
		if (print_verbose1) printf("Getting memory so it can be evicted\n");
	    tmp = GetMemory(64 * 1024 * 1024);
		if (print_verbose1) printf("Touching memory so it can be evicted %p \n", tmp);
		touch_buffer(tmp, 64 * 1024 * 1024);
	}
	return buf1;
}
