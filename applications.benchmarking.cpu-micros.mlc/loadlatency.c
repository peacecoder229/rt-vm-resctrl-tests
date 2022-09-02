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
#include <stdlib.h>
#include <stdio.h>
#endif

#include "types.h"
#include "topology.h"
#include "common.h"

extern int are_these_threads_on_same_core(int osid, int otherid);

void measure_loaded_latencies()
{
	FILE* fp; // local file pointer
	int i, j;

	// assume defaults
	if (R_provided == 0 && W_provided == 0 && o_provided == 0) {
		RDONLY_load = 1;
	}

	printf("\n");
	printf("Measuring Loaded Latencies for the system\n");
	if (use_1tpc_bandwidth != 0)
		printf("Using only one thread from each core if Hyper-threading is enabled\n");
	else
		printf("Using all the threads from each core if Hyper-threading is enabled\n");

	if (RDONLY_load) {
		printf("Using Read-only traffic type\n");
	}

	matrix_mode = 0;

	if (c_provided == 0) {
		latency_thread_run_cpuid = 0;
	}

	if (T_provided == 0) {
		thruput_only = 0;
	}

	if (t_provided == 0) {
		time_in_seconds = 2;
	}

	// if there is no mask provided by the user, come up with a mask based on topology
	if (!m_provided && !k_provided && !o_provided) {
		if (print_verbose1)	printf("No m/k/o flags provided - Using the following CPU and NUMA configuration:\n");
		j = 0;
		numcpus = 0;// reset and count again in following loop;
		j=0;
		for (i = 0; i < os_cpu_count; i++) {
			// Need to exclude all threads on the same physical core as the latency thread - not an issue if -T is provided
            if ((i != latency_thread_run_cpuid && !are_these_threads_on_same_core(i, latency_thread_run_cpuid)) || T_provided) {
				if(use_1tpc_bandwidth==0){
					//printf("regular\n");
					cpu_list[j] = os_map[i].os_id;
					numa_array[j] = os_map[i].os_id;
					numcpus++;
					if (print_verbose1) printf("cpu_list[%d]=%d, numa_array[%d]=%d\n", j, cpu_list[j], j, numa_array[j]);
					j++;
				}
				else{ // Use only one thread from each physical core
					if(os_map[i].smt_id==0){
						//printf("1tpc\n");
						cpu_list[j] = os_map[i].os_id;
						numa_array[j] = os_map[i].os_id;
						numcpus++;
						if (print_verbose1) printf("cpu_list[%d]=%d, numa_array[%d]=%d\n", j, cpu_list[j], j, numa_array[j]);
						j++;
					}
				}
				//if (print_verbose1)	printf("cpu_list[%d]=%d, numa_array[%d]=%d\n", j, cpu_list[j], j, numa_array[j]);
				//j++;
			}
		}
	} else if (k_provided) {
		parse_user_cpulist();
	}
    latency_core = latency_thread_run_cpuid;

	if (RDONLY_load == 0 && rw_mix == 0) {
		rw_mix = 3;   // 75% read + 25% write is assumed
	}

	if (RDONLY_load != 0 && rw_mix >1 && !o_provided) {
		Usage("Both -R and -W option cannot be specified\n");
	}

	if (LineSize > 64) {
		if (NUMA_configured != 0) {
			Usage("Stride size > 64 bytes is not supported for this traffic type\n");
		}
	}

	if (use_avx256) {
		if (NUMA_configured != 0 || (rw_mix == 9)) {
			Usage("-A or -Y option not available for this traffic type\n");
		}
	}

	if (lp_enabled) {
		printf("Using large pages for buffers\n");
	}

	// if there is a delay file provided, use this instead of the default delay array (overwrite the dfault)
	if (genfile_provided) {
		int cnt = 0;
		fp = fopen(genfile, "r");

		if (fp == NULL) {
			Usage("Unable to open the input gen file\n");
		}

		while (fp!=NULL) {
			if (fgets(dbuf, 80, fp) == NULL) {
				delay_array[cnt] = -1;
				fclose(fp);
				break;
			}

			delay_array[cnt++] = atoi(dbuf);
		}
	}
	if(N_provided && !o_provided){
		Usage("Specifying -N requires -o option!");	
	}

	// Initialize buffer with patterns
	if (pattern_file_provided) {
		read_pattern_file(pattern_file_name);
		populate_cache_patterns();
	}

	// set numa file to provided to enable correct functionality in StartLoading
	Numa_file_provided = 1;
	StartLoading(MAX_INJECTION_BW);
}


