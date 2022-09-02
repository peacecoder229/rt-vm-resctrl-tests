#include "busythread.h"

//
// Create busy threads that spin on a global variable based on a mask, or on all sockets
// bind threads to a specified core
/*

Copyright (c) 2014, Intel Corporation
All rights reserved.

 */
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
#include <string.h>
#include <sched.h>
#else // Windows specific
#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#include "types.h"
#include "topology.h"
#include "common.h"

#if defined(LINUX) || defined(__APPLE__)
unsigned int BusyLoopThread(void* core);
#else
UINT WINAPI BusyLoopThread(PVOID Parm);
#define strtok_r(a,b,c) strtok_s(a,b,c)
#endif

extern int** pkg_map_reserved;
extern int no_2_cores_per_socket;

void CreatePstateThreadsOnOtherCores(char* str)
{
	char* pch=NULL, *nextToken=NULL;
	char orig_str[256];
	int cpu[os_cpu_count], idx = 0;
#if !defined(LINUX) && !defined(__APPLE__)
  int tId;
  HANDLE thdHandle;
#else
	pthread_attr_t attr;
#endif
#if defined(LINUX) || defined(__APPLE__)
	pthread_t* ThreadData;
#endif
#if defined(LINUX) || defined(__APPLE__)

	if ((ThreadData = (pthread_t*) malloc(sizeof(pthread_t) * MAX_THREADS)) == NULL) {
		printf("g_phThread malloc error!\n");
		return;
	}
	assert(pthread_attr_init(&attr) ==0);
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
		app_exit(EACCES, "pthread_attr_setdetachstate error\n");
#endif
	strcpy_s(orig_str, sizeof(orig_str)-1, str);
	// Creates the busy loop thread on each core specified by 'p' and separated by a comma
	if (str != NULL)
		pch = strtok_r(str, ",", &nextToken);

	while (pch != NULL && idx < os_cpu_count) {
		if (!is_number(pch)) {
			printf("Non integer values in the string %s\n", orig_str);
			app_exit(EINVAL, NULL);
		}
		cpu[idx] = atoi(pch);
#if defined(LINUX) || defined(__APPLE__)
		pthread_create(&ThreadData[idx], &attr, (void*)BusyLoopThread, (void*)&cpu[idx]);
#else
		thdHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&BusyLoopThread,
		             (void*)&cpu[idx], 0, &tId);
		CloseHandle(thdHandle);
#endif
		idx++;
		pch = strtok_r(NULL, " ,.-", &nextToken);
	}

	// give some time for these threads to kick-in
#if defined(LINUX) || defined(__APPLE__)
	sleep(1);
	free(ThreadData);
	pthread_attr_destroy(&attr);
#else
	Sleep(1000);
#endif
}

void CreatePstateThreadsOnAllSockets(int socket_to_omit)
{
	int cpu[num_nodes], idx = 0, i;
	int core_to_omit;
#if !defined(LINUX) && !defined(__APPLE__)
  int tId;
  HANDLE thdHandle;
#else
	pthread_attr_t attr;
#endif
#if defined(LINUX) || defined(__APPLE__)
	pthread_t* ThreadData;

	if (no_2_cores_per_socket) return; // we don't have another core to use for spinning
	
	if ((ThreadData = (pthread_t*) malloc(sizeof(pthread_t) * MAX_THREADS)) == NULL) {
		printf("g_phThread malloc error!\n");
		return;
	}
	if (pthread_attr_init(&attr) !=0)
		app_exit(EACCES, "pthread_attr_init error\n");
	else {
		if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
			app_exit(EACCES, "pthread_attr_setdetachstate error\n");
	}
#endif

	if (socket_to_omit == -1) {
		// dont omit any core
		core_to_omit = -1;
	} else {
		// omit the core we use for busy loop creation on this socket
		core_to_omit = pkg_map_reserved[socket_to_omit][DUMMY_THREAD_CORE_OFFSET];

		if (print_verbose1) {
			printf("core to omit is %d on socket %d\n", core_to_omit, socket_to_omit);
		}
	}

	// Creates one busy loop thread on the 3rd core of each socket
	for (i = 0; i < num_nodes; i++) {
		if (sockets[i] == SOCKET_PRESENT) {
			if (core_to_omit != pkg_map_reserved[i][DUMMY_THREAD_CORE_OFFSET] &&
			        CPU_Affinity_user_selected != pkg_map_reserved[i][DUMMY_THREAD_CORE_OFFSET]) {
				cpu[idx] = pkg_map_reserved[i][DUMMY_THREAD_CORE_OFFSET];
#if defined(LINUX) || defined(__APPLE__)
				pthread_create(&ThreadData[idx], &attr, (void*)BusyLoopThread, (void*)&cpu[idx]);
#else
				thdHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&BusyLoopThread,
				             (void*)&cpu[idx], 0, &tId);
				CloseHandle(thdHandle);
#endif
				idx++;
			} else {
				//cpu[idx] = pkg_map_reserved[i][3];
				//this is not required as latency thread is anyway already running on that socket
			}
		}
	}

	// give some time for these threads to kick-in
#if defined(LINUX) || defined(__APPLE__)
	sleep(1);
	free(ThreadData);
	pthread_attr_destroy(&attr); 
#else
	Sleep(1000);
#endif
}


unsigned int BusyLoopThread(void* core)
{
	// keeps core active by counting
	int cpu = (*(int*)core);
	volatile int cnt = 0;

	if (print_verbose1) {
		printf("Core %d is running a busy loop to keep socket %d from low frequency states\n", cpu, os_map[cpu].pkg_id);
	}

	BindToCpu(cpu);


	while (stop_busyloop_threads != 1) {
		// Always yield control to any other thread that needs to run
#if !defined(LINUX) && !defined(__APPLE__)
		SwitchToThread();
#else
		sched_yield();
#endif
		cnt++;
	}

	if (print_verbose1) {
		printf("Busy loop running on core %d to keep socket %d from low frequency states is exiting\n", cpu, os_map[cpu].pkg_id);
	}
	return cnt;
}


