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
#else // Windows specific
#include <windows.h>
#include <synchapi.h>
#include <stdio.h>
#endif

#include "types.h"
#include "topology.h"
#include "common.h"

void measure_bandwidth_matrix()
{
	int i, j, k, l;
	Numa_file_provided = 1;
	delay_array[0] = 0;
	delay_array[1] = -1;
	thruput_only = 1;
	remote_mem_alloc = 1;
	matrix_mode = 1;

	if (R_provided == 0 && W_provided == 0 && o_provided == 0) {
		RDONLY_load = 1;
	}

	if (LineSize > 64) {
		if ((RDONLY_load != 1 && rw_mix != 2 && rw_mix != 3 && rw_mix != 5) || NUMA_configured != 0) {
			//if ((RDONLY_load !=1 && rw_mix !=2 && rw_mix !=3 && rw_mix != 5)){
			Usage("Stride size > 64 bytes is not supported for this traffic type\n");
		}
	}

	printf("Measuring Memory Bandwidths between nodes within system \nBandwidths are in MB/sec (1 MB/sec = 1,000,000 Bytes/sec)\n");
	if (use_1tpc_bandwidth != 0)
		printf("Using only one thread from each core if Hyper-threading is enabled\n");
	else
		printf("Using all the threads from each core if Hyper-threading is enabled\n");

	// Initialize buffer with patterns
	if (pattern_file_provided) {
		read_pattern_file(pattern_file_name);
		populate_cache_patterns();
	}

	if (R_provided == 0 && W_provided == 0 && o_provided == 0) {
		printf("Using Read-only traffic type\n");
	}

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

	if (numa_node_access_enabled == 0) {
		for (i = 0; i < num_nodes; i++) {
			if (sockets[i] == SOCKET_PRESENT) {
				if(use_1tpc_bandwidth==0){
					numcpus = pkg_map_ctr[i];
					//printf("numcpus is %d\n",numcpus);
				}
				else{
					 numcpus = pkg_map_1tpc_ctr[i];
				}

				if (!print_verbose1) {
					printf("%6d\t", i);
				}
				if(use_1tpc_bandwidth==0){
					for (j = 0; j < pkg_map_ctr[i]; j++) {
						cpu_list[j] = pkg_map[i][j];
						//printf("cpulist[%d]= %d\n",j,cpu_list[j]);
					}
				}else{
					for (j = 0; j < pkg_map_ctr[i]; j++) {
						cpu_list[j] = pkg_map_1tpc[i][j];
						//printf("cpulist[%d]= %d\n",j,cpu_list[j]);
					}
				}
				
			}
			for (k = 0; k < num_nodes; k++) {
				if (sockets[k] == SOCKET_PRESENT) {
					for (l = 0; l < numcpus; l++) {
						if(use_1tpc_bandwidth==0){
							numa_array[cpu_list[l]] = pkg_map[k][l];
							 //printf("numa_array[%d]= %d\n",cpu_list[l],numa_array[cpu_list[l]]);
						} else{
							numa_array[cpu_list[l]] = pkg_map_1tpc[k][l];	
						}
						// if(print_verbose1) printf("numa_array[%d]=%d\n",cpu_list[l],numa_array[cpu_list[l]]); */
					}
					if (print_verbose1) {
						printf("Measuring Bandwidth from CPU on socket %d to memory on socket %d", i, k);
					}
					// unset stop_busy_loop flag to enable p_threads to poll
					stop_busyloop_threads = 0;
					// create busy threads on all the sockets except the current cpu socket i
					CreatePstateThreadsOnAllSockets(i);
					// allow the threads to kick in
#if defined(LINUX) || defined(__APPLE__)
					sleep(1);
#else
					Sleep(1000);
#endif
					// do the test
					StartLoading(MAX_INJECTION_BW);
					// set stop_busy_loop flag to stop  p_threads that are polling
					stop_busyloop_threads = 1;
					// give a second for the polling threads to stop
#if defined(LINUX) || defined(__APPLE__)
					sleep(1);
#else
					Sleep(1000);
#endif
					if (1 == pmem_only)
					{
						break;
					}
				}
			}
			printf("\n");
		}
	} else {
		int orig_buf_size = buffer_size; // save now to restore later on completion of this routine
		change_traffic(rw_mix);
		for (i = 0; i < num_numa_nodes; i++) {
			if (numanode_cpu_map_ctr[i]) { // check whether this numa node has cpu resources
				if(use_1tpc_bandwidth==0){
					numcpus = numanode_cpu_map_ctr[i];
				}
				else{
					 numcpus = numanode_cpu_map_1tpc_ctr[i];
				}

				if (!print_verbose1) {
					printf("%8d\t", i);
				}
				if(use_1tpc_bandwidth==0){
					for (j = 0; j < numanode_cpu_map_ctr[i]; j++) {
						cpu_list[j] = numanode_cpu_map[i][j];
						//printf("cpulist[%d]= %d\n",j,cpu_list[j]);
					}
				}else{
					for (j = 0; j < numanode_cpu_map_1tpc_ctr[i]; j++) {
						cpu_list[j] = numanode_cpu_map_1tpc[i][j];
						//printf("cpulist[%d]= %d\n",j,cpu_list[j]);
					}
				}
				
				for (k = 0; k < num_numa_nodes; k++) {
						for (l = 0; l < numcpus; l++) {
							remote_source[cpu_list[l]] = k; // use numa node k
						}
						if (freemem_onnode(k) == 0) {
							// skip this node as there is no free memory available
							if (print_verbose1) 
								printf("skipping node %d as there is no free memory available", k);
							printf("-\t");
							continue;
						}
						if (print_verbose1) {
							printf("Measuring Bandwidth from CPU on node %d to memory on node %d", i, k);
						}

						change_bufsize (get_safe_buf_size(numcpus, k));
						// unset stop_busy_loop flag to enable p_threads to poll
						
						stop_busyloop_threads = 0;
						// create busy threads on all the sockets except on socket where numa node i resides
						CreatePstateThreadsOnAllSockets(get_socketid_of_numanode(i));
						// allow the threads to kick in
	#if defined(LINUX) || defined(__APPLE__)
						sleep(1);
	#else
						Sleep(1000);
	#endif
						// do the test
						StartLoading(MAX_INJECTION_BW);
						// set stop_busy_loop flag to stop  p_threads that are polling
						stop_busyloop_threads = 1;
						// give a second for the polling threads to stop
	#if defined(LINUX) || defined(__APPLE__)
						sleep(1);
	#else
						Sleep(1000);
	#endif
					
				}
			}
			printf("\n");
		}
		change_bufsize(orig_buf_size);
		initialize_perthread_defaults(); // restore the perthread defaults
	}
}


void DeleteAllSCMFiles(void);

void measure_bandwidth(int type)
{
	int i;
	matrix_mode = 1;
	Numa_file_provided = 1;
	delay_array[0] = 0;
	delay_array[1] = -1;
	thruput_only = 1;
	RDONLY_load = 1;
	remote_mem_alloc = 0; // should be set to zero



	if (LineSize > 64) {
		if ((RDONLY_load != 1 && rw_mix != 2 && rw_mix != 3 && rw_mix != 5) || NUMA_configured != 0) {
			Usage("Stride size > 64 bytes is not supported for this traffic type\n");
		}
	}

	if (pattern_file_provided) {
		read_pattern_file(pattern_file_name);
		populate_cache_patterns();
	}

	printf("\n");
	if (type == MAX_INJECTION_BW) 
		printf("Measuring Peak Injection Memory Bandwidths for the system\nBandwidths are in MB/sec (1 MB/sec = 1,000,000 Bytes/sec)\n");
	else {
		printf("Measuring Maximum Memory Bandwidths for the system\n");
		printf("Will take several minutes to complete as multiple injection rates will be tried to get the best bandwidth\n");
		printf ("Bandwidths are in MB/sec (1 MB/sec = 1,000,000 Bytes/sec)\n");
	}
	
	if (use_1tpc_bandwidth != 0)
		printf("Using only one thread from each core if Hyper-threading is enabled\n");
	else
		printf("Using all the threads from each core if Hyper-threading is enabled\n");
	printf("Using traffic with the following read-write ratios\n");

	if (k_provided) {
		parse_user_cpulist();
	}
	else if (m_provided){
		for (i = 0; i < os_cpu_count; i++) {
			numa_array[i] = i;
		}
	}
	else {
		int j=0;// j is used as index for cpu_list; since the IDs of the 2 threads of a core can be consecutive, we need this
		numcpus = 0;//os_cpu_count;
		//printf("***********************************\nNUMCPUS IS %d\n************************\n",numcpus);
		for (i = 0; i < os_cpu_count; i++) {
			if(use_1tpc_bandwidth==0){
					//printf("regular\n");
					cpu_list[i] = os_map[i].os_id;
					numa_array[i] = os_map[i].os_id;
					numcpus++;
			}
			else{
				if(os_map[i].smt_id==0){
					//printf("1tpc\n");
					cpu_list[j] = os_map[i].os_id;
					numa_array[j] = os_map[i].os_id;
					numcpus++; j++;
				}
			}
		}
			/*
			if(print_verbose1){
						printf("cpu_list[%d]=%d, numa_array[%d]=%d\n",cpu_list[i],numa_array[i]);
			         		}
					*/
	}
	// if -m flag is there, then initialize everything for local traffic
	// cpu_list is already initialized when -m flag was parsed in parse() function

	DeleteAllSCMFiles();

	printf("ALL Reads        :\t");
	StartLoading(type);
	RDONLY_load = 0;
#ifdef INTERNAL
	/* This code is being commented out from internal version as well
	printf("\n");

	DeleteAllSCMFiles();

	printf("4:1 Reads-Writes :\t");
	rw_mix = 12;
	change_traffic(rw_mix);
	StartLoading(type);
	*/
#endif
	printf("\n");

	DeleteAllSCMFiles();

	printf("3:1 Reads-Writes :\t");
	rw_mix = 3;
	change_traffic(rw_mix);
	StartLoading(type);
	printf("\n");

	DeleteAllSCMFiles();

	printf("2:1 Reads-Writes :\t");
	rw_mix = 2;
	change_traffic(rw_mix);
	StartLoading(type);
	printf("\n");

	DeleteAllSCMFiles();

	printf("1:1 Reads-Writes :\t");
	rw_mix = 5;
	change_traffic(rw_mix);
	StartLoading(type);
	printf("\n");

	DeleteAllSCMFiles();

	printf("Stream-triad like:\t");
	rw_mix = 10;
	change_traffic(rw_mix);
	StartLoading(type);
#ifdef INTERNAL
	printf("\n");

	DeleteAllSCMFiles();

	printf("All NT writes    :\t");
	rw_mix = 6;
	change_traffic(rw_mix);
	StartLoading(type);
	printf("\n");

	DeleteAllSCMFiles();

	printf("1:1 Read-NT write:\t");
	rw_mix = 8;
	change_traffic(rw_mix);
	StartLoading(type);
#endif
	printf("\n\n");

	DeleteAllSCMFiles();
}
