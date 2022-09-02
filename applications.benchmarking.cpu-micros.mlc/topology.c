#if defined(LINUX) || defined(__APPLE__)
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sched.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <glob.h>
#include <regex.h>
#include <string.h>

#else
#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#include "types.h"
#include "cpuid.h"
#include "topology.h"
#include "common.h"

#ifndef LINUX
BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity);
void BindToCpu(int LinearProcNumber);
#endif

// Define my own CPU_SET macros so we can workarond the max CPU_SETSIZE limit of 1024
// Systems with large number of CPUs like > 1024 cpus require this fix
// So, i had to duplicate many of the macros here (with fix) from /usr/include/bits/sched.h

/* Type for array elements in 'cpu_set'.  */
typedef unsigned long int __my_cpu_mask;

# define __MY_CPU_SETSIZE  8192
# define __MY_NCPUBITS     (8 * sizeof (__my_cpu_mask))

/* Basic access functions.  */
# define __MY_CPUELT(cpu)  ((cpu) / __MY_NCPUBITS)
# define __MY_CPUMASK(cpu) ((__my_cpu_mask) 1 << ((cpu) % __MY_NCPUBITS))

/* Data structure to describe CPU mask.  */
typedef struct
{
  __my_cpu_mask __my_bits[__MY_CPU_SETSIZE / __MY_NCPUBITS];
} my_cpu_set_t;

/* Access functions for CPU masks.  */
# define __MY_CPU_ZERO(cpusetp) \
  do {                                                                        \
    unsigned int __i;                                                         \
    my_cpu_set_t *__arr = (cpusetp);                                             \
    for (__i = 0; __i < sizeof (my_cpu_set_t) / sizeof (__my_cpu_mask); ++__i)      \
      __arr->__my_bits[__i] = 0;                                                 \
  } while (0)
# define __MY_CPU_SET(cpu, cpusetp) \
  ((cpusetp)->__my_bits[__MY_CPUELT (cpu)] |= __MY_CPUMASK (cpu))
# define __MY_CPU_CLR(cpu, cpusetp) \
  ((cpusetp)->__my_bits[__MY_CPUELT (cpu)] &= ~__MY_CPUMASK (cpu))
# define __MY_CPU_ISSET(cpu, cpusetp) \
  (((cpusetp)->__my_bits[__MY_CPUELT (cpu)] & __MY_CPUMASK (cpu)) != 0)


APIC_ID_t* os_map;

int n_sockets = 0;
int numa_node_access_enabled=0;
int no_2_cores_per_socket=0;

int** numanode_cpu_map_reserved;
int** numanode_cpu_map;
int* numanode_cpu_map_ctr;
int* numanode_cpu_map_1tpc_ctr;
int** numanode_cpu_map_1tpc;

// windows specific routines for pinning threads to specific cores
#if !defined(LINUX) && !defined(__APPLE__)
typedef DWORD_PTR(WINAPI* LPFN_SetThreadGroupAffinity)(HANDLE, GROUP_AFFINITY*, GROUP_AFFINITY*);
typedef DWORD_PTR(WINAPI* LPFN_GetThreadGroupAffinity)(HANDLE, GROUP_AFFINITY*);
typedef WORD(WINAPI* LPFN_GetActiveProcessorGroupCount)(VOID);
typedef DWORD (WINAPI* LPFN_GetActiveProcessorCount)(WORD);
static LPFN_GetThreadGroupAffinity pGTGA;
static LPFN_SetThreadGroupAffinity pSTGA;
static LPFN_GetActiveProcessorGroupCount pGAPGC;
static LPFN_GetActiveProcessorCount pGAPC;
static int winver_identified = 0;
static BOOL windows_supports_more_than_64_cpus = TRUE;

#endif

int parse_numanodes();
void parse_cpumap(char *buf, int nodeid);
int build_pkg_map();
int build_topology_no_cpuid_b();
int build_AMD_topology();

//
// static functions
//

static void parse_apic_id(cpuid_info_t info_l0, cpuid_info_t info_l1, APIC_ID_t* my_id)
{
	unsigned int smt_mask_width, smt_mask, core_mask_width, core_mask, pkg_mask;
	// Get the SMT ID
	smt_mask_width = info_l0.eax & 0x1f;
	smt_mask = ~((-1) << smt_mask_width);
	my_id->smt_id = info_l0.edx & smt_mask;
	// Get the core ID
	core_mask_width = info_l1.eax & 0x1f;
	core_mask = (~((-1) << core_mask_width)) ^ smt_mask;
	my_id->core_id = (info_l1.edx & core_mask) >> smt_mask_width;
	// Get the package ID
	pkg_mask = (-1) << core_mask_width;
	my_id->pkg_id = (info_l1.edx & pkg_mask) >> core_mask_width;
}

#ifdef LINUX
static int bind_context(my_cpu_set_t* new_context, my_cpu_set_t* old_context)
{
	int err = 0, ret = 0;

	if (old_context != NULL) {
		err = sched_getaffinity(0, sizeof(my_cpu_set_t), (cpu_set_t*)old_context);

		if (0 != err) {
			ret = MY_ERROR;
		}
	}

	err += sched_setaffinity(0, sizeof(my_cpu_set_t), (cpu_set_t*)new_context);

	if (0 != err) {
		ret = MY_ERROR;
	}

	return ret;
}
#else
// due to the complexity of K-groups and how >64 processors are supported in Windows, we decided not to save old context
// but just re-pin the thread to cpu -0 on exit from this function

#endif

#ifdef LINUX
static int bind_cpu(unsigned int cpu, my_cpu_set_t* old_context)
{
	int err = 0;
	my_cpu_set_t cpu_context;
	__MY_CPU_ZERO(&cpu_context);
	__MY_CPU_SET(cpu, &cpu_context);
	err += bind_context(&cpu_context, old_context);
	return err;
}
#endif

int build_topology()
{
	int err = 0;
	unsigned int i;
	unsigned int max_pkg = 0;
	int cpuid_b_valid = 1;
	
	cpuid_info_t info_l0, info_l1, info;
#if defined(LINUX) || defined(__APPLE__)
	my_cpu_set_t prev_context;
#endif

    get_processor_cache_info();

	// check whether cpuid 0xb is valid
	cpuid(0xb, 0, &info);
	if (info.eax == 0 && info.ebx == 0) { // cpuid 0xb is invalid
		cpuid_b_valid = 0;
		if (print_verbose1) printf("Cpuid 0xb is invalid\n");
	}

	if (AMD_cpu && !cpuid_b_valid) return build_AMD_topology();
	else if (!cpuid_b_valid) return build_topology_no_cpuid_b(); // xeon cpus with no support for cpuid 0xb
	
	// Following code executed for both Xeon and AMD cpus which support cpuid 0xb
	
	// initialize sockets[], which marks the presence of a socket
	for (i = 0; i < MAX_SOCKETS; i++) {
		sockets[i] = SOCKET_NOT_PRESENT;
	}

	os_map = (APIC_ID_t*) malloc(os_cpu_count * sizeof(APIC_ID_t));

	for (i = 0; i < os_cpu_count && os_map; i++) {
		// bind to each os core
#ifdef LINUX
		err = bind_cpu(i, &prev_context);
#else
		BindToCpu(i);
#endif
		// obtain APIC ID information
		info_l0 = get_processor_topology(0);
		info_l1 = get_processor_topology(1);
		os_map[i].os_id = i;
		parse_apic_id(info_l0, info_l1, &os_map[i]);

		// max_pkg is the largest package id found on any core in the system
		if (os_map[i].pkg_id > max_pkg) {
			max_pkg = os_map[i].pkg_id;
		}

		//  if the current pkg_id (socket) entry has not been populated on the sockets[] matrix, populate it
		if (sockets[os_map[i].pkg_id] == 0) {
			sockets[os_map[i].pkg_id] = SOCKET_PRESENT;
			n_sockets++;
		}

#ifdef LINUX
		err = bind_context(&prev_context, NULL);
#endif

		if (print_verbose1) {
			printf("OS id: %3u Core id: %3u Socket id: %3u Hyperthread id: %3u\n", os_map[i].os_id, os_map[i].core_id, os_map[i].pkg_id, os_map[i].smt_id);
		}
	}
	num_nodes = max_pkg + 1;
	
	return build_pkg_map();

}

int build_topology_no_cpuid_b()
{
	int err = 0;
	unsigned int i;
	unsigned int max_pkg = 0, max_procs, temp, num_bits=0;
	unsigned int pkg_mask_width=0, smt_mask, core_mask_width=0, core_mask, pkg_mask;
	cpuid_info_t info;
	
#if defined(LINUX) || defined(__APPLE__)
	my_cpu_set_t prev_context;
#endif

	cpuid(0x4, 0, &info);
	// The nearest power-of-2 integer that is not smaller than (1 + EAX[31:26]) is the number of unique
	// Core_IDs reserved for addressing different processor cores in a physical package. Core ID is a subset of
	// bits of the initial APIC ID
	max_procs = ((info.eax >> 26) & 0x3f) +1 ; // max number of cpus per package
	temp = max_procs-1;
	// now find the nearest power-of-2 integer
	while (temp > 0) {
		temp >>= 1;
		core_mask_width++;
	}
	pkg_mask = (-1) << core_mask_width;

	// initialize sockets[], which marks the presence of a socket
	for (i = 0; i < MAX_SOCKETS; i++) {
		sockets[i] = SOCKET_NOT_PRESENT;
	}

	os_map = (APIC_ID_t*) malloc(os_cpu_count * sizeof(APIC_ID_t));

	for (i = 0; i < os_cpu_count && os_map; i++) {
		unsigned int apic_id; 
		// bind to each os core
#ifdef LINUX
		err = bind_cpu(i, &prev_context);
#else
		BindToCpu(i);
#endif
		// obtain APIC ID information
		cpuid(0x1, 0, &info);
		apic_id = ((info.ebx >> 24) & 0xff);
		os_map[i].os_id = i;
		os_map[i].smt_id = 0; // not clear how to identify SMT thread. So, assuming that SMT is off for all cases
		os_map[i].pkg_id = (apic_id & pkg_mask) >> core_mask_width;
		os_map[i].core_id = (apic_id & (~pkg_mask));

		// max_pkg is the largest package id found on any core in the system
		if (os_map[i].pkg_id > max_pkg) {
			max_pkg = os_map[i].pkg_id;
		}

		//  if the current pkg_id (socket) entry has not been populated on the sockets[] matrix, populate it
		if (sockets[os_map[i].pkg_id] == 0) {
			sockets[os_map[i].pkg_id] = SOCKET_PRESENT;
			n_sockets++;
		}

#ifdef LINUX
		err = bind_context(&prev_context, NULL);
#endif

		if (print_verbose1) {
			printf("OS id: %3u Core id: %3u Socket id: %3u Hyperthread id: %3u\n", os_map[i].os_id, os_map[i].core_id, os_map[i].pkg_id, os_map[i].smt_id);
		}
	}
	num_nodes = max_pkg + 1;
	
	return build_pkg_map();

}

int build_AMD_topology()
{
	cpuid_info_t info;
	unsigned int i;
	int err;
	int cores_pu, c_unitid, nodes_pp, nodeid, smt_id = 0;
	int prev_unitid = -1, prev_nodeid = -1; 
	int nodes_in_this_proc = 0, socket_id = 0;

	#if defined(LINUX) || defined(__APPLE__)
	my_cpu_set_t prev_context;
#endif
	
	// initialize sockets[], which marks the presence of a socket
	for (i = 0; i < MAX_SOCKETS; i++) {
		sockets[i] = SOCKET_NOT_PRESENT;
	}
	
	// os_cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
	// Construct an os map: os_map[APIC_ID ... APIC_ID]
	// os_map stores information about the system's cores, indexed by os_id.
	os_map = (APIC_ID_t*) malloc(os_cpu_count * sizeof(APIC_ID_t));

	for (i = 0; i < os_cpu_count && os_map; i++) {
		// bind to each os core
#ifdef LINUX
		err = bind_cpu(i, &prev_context);
#else
		BindToCpu(i);
#endif
		// currently, assume there is only one package. 
		os_map[i].os_id = i;
		// cpuid eax=0x8000001e has all the topology information
		cpuid(0x8000001e, 0, &info);
		cores_pu = ((info.ebx >> 8) & 3) + 1; // cores per compute unit
		c_unitid = info.ebx & 0xff; // compute unit ID
		nodes_pp = ((info.ecx >> 8) & 7) + 1; // Nodes per processor
		nodeid = info.ecx & 0xff; // Node ID
		if (c_unitid == prev_unitid) { // CPUs with the same compute unit ID are SMT cpus
			smt_id = 1;
		}
		else {
			smt_id = 0;
			prev_unitid = c_unitid;
		}
		if (nodeid != prev_nodeid) { // switching to the next node id
			nodes_in_this_proc++;
			if (nodes_in_this_proc > nodes_pp) { // reached max nodes per processor in this socket
				socket_id++;
				nodes_in_this_proc = 0;
			}
			prev_nodeid = nodeid;
		}
		os_map[i].pkg_id = socket_id;
		os_map[i].core_id = c_unitid;
		os_map[i].smt_id = smt_id;

		//  if the current pkg_id (socket) entry has not been populated on the sockets[] matrix, populate it
		if (sockets[os_map[i].pkg_id] == SOCKET_NOT_PRESENT) {
			sockets[os_map[i].pkg_id] = SOCKET_PRESENT;
			n_sockets++;
		}

#ifdef LINUX
		err = bind_context(&prev_context, NULL);
#endif

		if (print_verbose1) {
			printf("OS id: %3u Core id: %3u Socket id: %3u Hyperthread id: %3u\n", os_map[i].os_id, os_map[i].core_id, os_map[i].pkg_id, os_map[i].smt_id);
		}
	}
	num_nodes = n_sockets;
	
	return build_pkg_map();
	
}

int build_pkg_map()
{
	int err = 0, ctr;
	unsigned int i, j;
#ifndef LINUX
	// due to the complexity of K-groups and how >64 processors are supported in Windows, we decided not to save old context
	// but just re-pin the thread to cpu -0 on exit from this function
	//BindToCpu(0);
#endif
	// introduced error check
	//sockets[1]=0;
	// since the pgd ids start with zero:
	//num_nodes = max_pkg + 1;
	// if(print_verbose1){printf("Detected %d sockets\n",num_nodes);}
	// pkg_map_reserved is an array of pointers, each entry corresponding to a package/CPU node
	// each entry points to a set of 2 cores selected from the socket, to be used by MLC
	// for running dummy threads and latency thread
	pkg_map_reserved = (int**) malloc(num_nodes * sizeof(unsigned int*));
	pkg_map = (int**) malloc(num_nodes * sizeof(unsigned int*));
	pkg_map_1tpc = (int**) malloc(num_nodes * sizeof(unsigned int*));
	pkg_map_ctr = (unsigned int*) malloc(num_nodes * sizeof(unsigned int));
	pkg_map_1tpc_ctr = (unsigned int*) malloc(num_nodes * sizeof(unsigned int));
	

	if (pkg_map_reserved == NULL || pkg_map == NULL || pkg_map_1tpc == NULL || 
		pkg_map_ctr == NULL || pkg_map_1tpc_ctr == NULL) {
		app_exit(ENOMEM, "Out of memory!\n");
	}

	memset(pkg_map_reserved, 0, num_nodes * sizeof(unsigned int*));
	memset(pkg_map, 0, num_nodes * sizeof(unsigned int*));
	memset(pkg_map_1tpc, 0, num_nodes * sizeof(unsigned int*));
	memset(pkg_map_ctr, 0, num_nodes * sizeof(unsigned int));
	memset(pkg_map_1tpc_ctr, 0, num_nodes * sizeof(unsigned int));

	for (i = 0; i < num_nodes && pkg_map_reserved; i++) {
		pkg_map_reserved[i] = malloc(2 * sizeof(unsigned int));

		if (pkg_map_reserved[i] == NULL) {
			app_exit(ENOMEM, "Out of memory!\n");
		}
		memset(pkg_map_reserved[i], 0, 2 * sizeof(unsigned int));
		if (pkg_map != NULL && pkg_map_ctr != NULL && pkg_map_1tpc != NULL && pkg_map_1tpc_ctr != NULL) {
			pkg_map[i] = malloc(MAX_THREADS_PER_SOCKET * sizeof(unsigned int)); // hard-coded as we dont know how much to allocate
			pkg_map_ctr[i] = 0;
			pkg_map_1tpc[i] = malloc(MAX_THREADS_PER_SOCKET * sizeof(unsigned int)); // hardcoded for 1tpc scenario
			pkg_map_1tpc_ctr[i] = 0;
			if (pkg_map[i] == NULL || pkg_map_1tpc[i] == NULL) {
				app_exit(ENOMEM, "Out of memory!\n");
			}
			memset(pkg_map[i], 0, MAX_THREADS_PER_SOCKET * sizeof(unsigned int));
			memset(pkg_map_1tpc[i], 0, MAX_THREADS_PER_SOCKET * sizeof(unsigned int)); 
		}


	}

	ctr = 0;

	if (print_verbose1) {
		printf("Detected %d sockets\n", num_nodes);
	}

	// go through the set of CPU nodes to populate pkg_map_reserved with 2 cores per node
	// 1st core in each socket will be used to run dummy threads to keep cpus 100% used
	// 2nd core in each socket will be used to run the latency thread
	for (i = 0; i < num_nodes; i++) {
		if (sockets[i] == SOCKET_PRESENT) {
			j = 0;
			ctr = 0;

			do {
				// Look up the os_map structure until you get to an os core that belongs to the socket you are indexing
				// make sure the core has smt id 0 as HT may be turned off or on, and we dont want to pick 2 os cores that are hyperthreads together in the same core
				if (os_map[ctr].smt_id == 0 && os_map[ctr].pkg_id == i) {
					// store the core id in pkg_map_reserved
					if (pkg_map_reserved)
						pkg_map_reserved[i][j] = os_map[ctr].os_id;
					j++;
				}

				ctr++;
			} while (j < 2 && ctr < os_cpu_count); // need to make sure ctr does not exceed total cpu count
			if (j < 2) { // did not find at least 2 cores in each socket
				no_2_cores_per_socket = 1;
				//Runtime_exit("Need at least 2 cores in each socket for MLC to run. Exiting..\n");
			}
		}
	}

	for (i = 0; i < os_cpu_count; i++) {
		if (pkg_map != NULL && pkg_map_ctr != NULL && os_map!= NULL && pkg_map_1tpc != NULL && pkg_map_1tpc_ctr != NULL) {
			pkg_map[os_map[i].pkg_id][pkg_map_ctr[os_map[i].pkg_id]] = os_map[i].os_id;
			pkg_map_ctr[os_map[i].pkg_id]++;
			if (os_map[i].smt_id == 0) {
				pkg_map_1tpc[os_map[i].pkg_id][pkg_map_1tpc_ctr[os_map[i].pkg_id]] = os_map[i].os_id;
				pkg_map_1tpc_ctr[os_map[i].pkg_id]++;
			}
		}
		//printf("hyperthread of %d is %d\n",os_map[i].os_id,find_hyperthread(os_map[i].os_id));
	}
	/*
	for (i = 0; i < num_nodes; i++) {
		if (sockets[i] == SOCKET_PRESENT) {
			for (j = 0; j < pkg_map_ctr[i]; j++) {
			printf("pkg_map[%d][%d]=%d\n",i,j,pkg_map[i][j]);
		}
	}
	}
	*/
	if (print_verbose1 && matrix_mode1 && pkg_map_reserved) {
		for (i = 0; i < num_nodes; i++) {
			if (sockets[i] == SOCKET_PRESENT) {
				printf("Socket %3d: latency measurements and memory allocation to be done using core %d, socket kept active by core %d  \n", i, pkg_map_reserved[i][1], pkg_map_reserved[i][0]);
			}
		}

		printf("\n");
	}

	return err;
}
int find_hyperthread(int osid)
{
	int i;

	for (i = 0; i < os_cpu_count; i++) {
		if (os_map[i].pkg_id == os_map[osid].pkg_id) {
			if (os_map[i].core_id == os_map[osid].core_id) {
				if (os_map[i].smt_id != os_map[osid].smt_id) {
					return i;
				}
			}
		}
	}

	return -1;
}

int are_these_threads_on_same_core(int osid, int otherid)
{
	if ( (os_map[otherid].pkg_id == os_map[osid].pkg_id) && (os_map[otherid].core_id == os_map[osid].core_id)) 
		return 1;
	return 0;
}


// Affinitize the thread to the CPU specified

#if defined(LINUX)
int BindToCpu(int cpu_num)
{
	long status;
	my_cpu_set_t cs;
	__MY_CPU_ZERO(&cs);
	__MY_CPU_SET(cpu_num, &cs);
	status = sched_setaffinity(0, sizeof(cs), (cpu_set_t*)&cs);

	if (status < 0) {
		printf("Error: unable to bind thread to core %d\n", cpu_num);
		app_exit(EPERM, NULL);
	}
	return 1;
}
#elif defined(__APPLE__)
int BindToCpu(int cpu_num)
{
#if defined(INTERNAL)
    //printf("BindToCpu = %d\n", cpu_num);
    kern_return_t kernResult = bindCpu(myConnect, cpu_num);
    if (kernResult != KERN_SUCCESS) {
        fprintf(stderr, "[error] bindCpu returned kernResult 0x%08x.\n", kernResult);
        app_exit(EPERM, NULL);
        return 0;
    }
#else
    //printf("*** Thread Bind is not supported for Mac OS X for public version ***\n");
#endif
    return 1;
}
#endif

#if 0
int BindToCpu(int cpuid)
{
	static int groupCnt;
	static int groupArray[32];
	int idx, i, groupId=0, groupCpuid=0;
	GROUP_AFFINITY ga, oldga;
	UINT64 affinity;
	DWORD_PTR st;
	int success = 0;
	affinity = 1;
	//printf("bindto cpu %d\n", cpuid);

	if (!winver_identified) {
		pGAPGC = (LPFN_GetActiveProcessorGroupCount) GetProcAddress(
		             GetModuleHandle(TEXT("kernel32.dll")),
		             "GetActiveProcessorGroupCount");
		winver_identified = 1;

		if (NULL != pGAPGC) {
			groupCnt = pGAPGC();

			if (groupCnt > 1) {
				// we can assume that this system supports > 64 cpus
				windows_supports_more_than_64_cpus = TRUE;
				pGAPC = (LPFN_GetActiveProcessorCount) GetProcAddress(
				            GetModuleHandle(TEXT("kernel32.dll")),
				            "GetActiveProcessorCount");

				if (NULL != pGAPC) {
					for (i = 0; i < groupCnt; i++) {
						groupArray[i] = pGAPC(i);

						if (i != 0) {
							groupArray[i] += groupArray[i - 1];
						}
					}
				}

				//printf("Grouparray %d, %d, %d\n", groupArray[0], groupArray[1], groupArray[2]);
				pSTGA = (LPFN_SetThreadGroupAffinity) GetProcAddress(
				            GetModuleHandle(TEXT("kernel32.dll")),
				            "SetThreadGroupAffinity");
				pGTGA = (LPFN_GetThreadGroupAffinity) GetProcAddress(
				            GetModuleHandle(TEXT("kernel32.dll")),
				            "GetThreadGroupAffinity");
			}
		}

		//printf("groupa values %d, %d, %d\n", groupArray[0],groupArray[1],groupArray[2]);
	}

	if (windows_supports_more_than_64_cpus) {
		idx = cpuid + 1;

		for (i = 0; i < groupCnt; i++) {
			if (idx <= groupArray[i]) {
				groupId = i;

				if (i > 0) {
					groupCpuid = cpuid - groupArray[i - 1];
				} else {
					groupCpuid = cpuid;
				}

				break;
			}
		}

		if (groupArray[i] == 0) {
			printf("wrong cpuid %d, grouparray id %d\n", cpuid, i);
		} else {
			affinity <<= groupCpuid;
			ga.Mask = affinity;
			ga.Group = groupId;
			ga.Reserved[0] = 0;
			ga.Reserved[1] = 0;
			ga.Reserved[2] = 0; // reserved fields must be set to zero
			//printf("mask=%I64x, id=%d\n", ga.Mask, ga.Group);
			st = pSTGA(GetCurrentThread(), &ga, &oldga);

			if (!st) {
				printf("win7 --Unable to bind to cpu %d; Getlasterror_in_decimal=%ld; status=%I64d\n", cpuid, GetLastError(), st);
			} else {
				success = 1;
			}
		}
	} else {
		// regular method of binding threads to be followed
		affinity = 1;
		affinity <<= cpuid;
		st = SetThreadAffinityMask(GetCurrentThread(), affinity);

		if (!st && GetLastError() != 0) {
			printf("Unable to bind to cpu %d; Getlasterror_in_decimal=%ld; status=%I64d\n", cpuid, GetLastError(), st);
		} else {
			success = 1;
		}
	}

	return success;
}
#endif

#if !defined(LINUX) && !defined(__APPLE__)
BOOL getCurrentAffinity(GROUP_AFFINITY* pga)
{
	if (windows_supports_more_than_64_cpus) {
		return (GetThreadGroupAffinity(GetCurrentThread(), pga));
	} else {
		pga->Group = 0;
		// there is no GetThreadAffinityMask, instead use Set
		pga->Mask = SetThreadAffinityMask(GetCurrentThread(), 1);
		return TRUE;
	}
}

BOOL setCurrentAffinity(GROUP_AFFINITY ga)
{
	GROUP_AFFINITY oldga = {0};

	if (windows_supports_more_than_64_cpus) {
		return SetThreadGroupAffinity(GetCurrentThread(), &ga, &oldga);
	} else {
		SetThreadAffinityMask(GetCurrentThread(), ga.Mask);
	}

	return TRUE;
}
#endif

#ifdef LINUX
int parse_numanodes()
{
	int i;
	int nodeid, numa_nodes;
	char cpumap_path[1024];
	FILE *fp;
	// 2048 length buffer required to support really large number of cores like > 3k
	char buf[2048];
	
	glob_t g;
	g.gl_offs=MAX_SOCKETS; // reserve 256 nodes
	glob("/sys/devices/system/node/node*", 0, NULL, &g); 
	numa_nodes = g.gl_pathc; 
	if (print_verbose1) 
		printf("parse_numanodes(): num numa nodes = %d\n",  numa_nodes);
	for (i=0; i < g.gl_pathc; i++)
	{
		//printf("path=%s\n", g.gl_pathv[i]);
		sscanf (g.gl_pathv[i], "/sys/devices/system/node/node%d", &nodeid);
		strcpy_s(cpumap_path, sizeof(cpumap_path)-1, g.gl_pathv[i]);
		strcat_s(cpumap_path, sizeof(cpumap_path)-1, "/cpumap");
		fp = fopen(cpumap_path, "r");
		if (fp == NULL) { globfree(&g); return -1;}
		if (fgets(buf, sizeof(buf), fp) == NULL) { globfree(&g); fclose(fp); return -1;}
		parse_cpumap(buf,nodeid);
		fclose(fp);
	}
	globfree (&g);
	if (print_verbose1) printf("parse_numanodes() completed - %d nodes present\n", numa_nodes);
	return numa_nodes;
}

void parse_cpumap(char *buf, int nodeid)
{
	int len, cpunum;
	unsigned int mask;
	int num_groups, nid;
	char *p, tmp[2048], *nexToken=NULL;

	len = strnlen_s(buf, sizeof(tmp));
	num_groups=1;
	// We use tmp buffer to perform all strtok operations as it clobbers the input
    strcpy_s(tmp, sizeof(tmp), buf);
	p = strtok_r(tmp, ",", &nexToken);
    while(strtok_r(NULL, ",", &nexToken) != NULL) 
        num_groups++;
	if (print_verbose1) {
		printf("Number of groups in cpumap file in nodeid %d=%d\n", nodeid, num_groups);
	}
	nid = (num_groups-1)*32; // 32 bits in each group
    strcpy_s(tmp, sizeof(tmp), buf);
	nexToken = NULL;
	p = strtok_r (tmp, ",", &nexToken);
	while(p!=NULL)
	{
		int bitpos=1, cnt=0;
		mask = strtol(p, NULL, 16); //sscanf(p, "%x", &mask);
		if (mask!=0) {
			bitpos=1;
			while(cnt < 32) {
				if ((mask & bitpos) != 0) {
					cpunum = nid+cnt;
					os_map[cpunum].numa_node_id = nodeid;
				}
				bitpos<<=1;
				cnt++;
			}
		}
		nid -= 32;
		p = strtok_r(NULL, ",", &nexToken);
	}
}
#else
int parse_numanodes()
{
	ULONG MaxNumaNode = 0;

	GetNumaHighestNodeNumber(&MaxNumaNode);

	if (print_verbose1) printf("parse_numanodes() completed - %ld nodes present\n", MaxNumaNode);

	return MaxNumaNode;
}

void parse_cpumap(char *buf, int nodeid)
{

}
#endif

void build_numa_topology()
{
	int i, ctr, j;
	
	if (num_numa_nodes ==0) return;
	
	numanode_cpu_map_reserved = (int**) malloc(num_numa_nodes * sizeof(unsigned int*));
	numanode_cpu_map = (int**) malloc(num_numa_nodes * sizeof(unsigned int*));
	numanode_cpu_map_1tpc = (int**) malloc(num_numa_nodes * sizeof(unsigned int*));
	numanode_cpu_map_ctr = (unsigned int*) malloc(num_numa_nodes * sizeof(unsigned int));
	numanode_cpu_map_1tpc_ctr = (unsigned int*) malloc(num_numa_nodes * sizeof(unsigned int));

	
	if (numanode_cpu_map_reserved == NULL || numanode_cpu_map == NULL || numanode_cpu_map_1tpc == NULL || 
		numanode_cpu_map_ctr == NULL || numanode_cpu_map_1tpc_ctr == NULL ) {
		app_exit(ENOMEM, "Out of memory!\n");
	}

	memset(numanode_cpu_map_reserved, 0, num_numa_nodes * sizeof(unsigned int*));
	memset(numanode_cpu_map, 0, num_numa_nodes * sizeof(unsigned int*));
	memset(numanode_cpu_map_1tpc, 0, num_numa_nodes * sizeof(unsigned int*));
	memset(numanode_cpu_map_ctr, 0, num_numa_nodes * sizeof(unsigned int));
	memset(numanode_cpu_map_1tpc_ctr, 0, num_numa_nodes * sizeof(unsigned int));

	for (i = 0; i < num_numa_nodes; i++) {
		if (numanode_cpu_map_reserved) {
			numanode_cpu_map_reserved[i] = malloc(2 * sizeof(unsigned int));

			if (numanode_cpu_map_reserved[i] == NULL) {
				app_exit(ENOMEM, "Out of memory!\n");
			}
			memset(numanode_cpu_map_reserved[i], 0, 2 * sizeof(unsigned int));
		}
		numanode_cpu_map[i] = malloc(MAX_THREADS_PER_SOCKET * sizeof(unsigned int));
		numanode_cpu_map_1tpc[i] = malloc(MAX_THREADS_PER_SOCKET * sizeof(unsigned int));
		if (numanode_cpu_map[i] == NULL || numanode_cpu_map_1tpc[i] == NULL) {
			app_exit(ENOMEM, "Out of memory!\n");
		}
		numanode_cpu_map_ctr[i] = 0;
		numanode_cpu_map_1tpc_ctr[i] = 0;
		memset (numanode_cpu_map[i], 0, MAX_THREADS_PER_SOCKET * sizeof(unsigned int));
		memset (numanode_cpu_map_1tpc[i], 0, MAX_THREADS_PER_SOCKET * sizeof(unsigned int));
	}
#if 0	
	if (parse_numanodes() <=1) {
		if (print_verbose1) printf("No numa support - falling to socket level only\n");
		numa_node_access_enabled = 0;
		// No NUMA support. Will fall back to regular socket level breakdown only
	}
	else numa_node_access_enabled = 1;
#endif
	// if we are here, we should enable numa_node_access all the time
	parse_numanodes();
	numa_node_access_enabled = 1;
    
	ctr = 0;

	
	for (i = 0; i < os_cpu_count; i++) {
#ifndef LINUX
		//HACK...testing
		NUMA_NODE_GROUP_AFFINITY nng_affinity = { 0 };
		LinearProcNumToNumaNodeGroupAffinity(i, &nng_affinity);
		os_map[i].numa_node_id = nng_affinity.NodeNumber;
#endif
		int numa_id = os_map[i].numa_node_id;
		numanode_cpu_map[numa_id][numanode_cpu_map_ctr[numa_id]++] = os_map[i].os_id;
		if(os_map[i].smt_id==0){
			numanode_cpu_map_1tpc[numa_id][numanode_cpu_map_1tpc_ctr[numa_id]++] = os_map[i].os_id;
		}
	}
	
	if (print_verbose1) {
		printf("Detected %d numa nodes\n", num_numa_nodes);
	}

	// go through the set of CPU nodes to populate numanode_cpu_map_reserved with 2 cores per node
	// 1st core in each socket will be used to run dummy threads to keep cpus 100% used
	// 2nd core in each socket will be used to run the latency thread
	for (i = 0; i < num_numa_nodes; i++) {
		if (numanode_cpu_map_ctr && numanode_cpu_map_ctr[i]) { // at least one cpu present in this numa node
			j = 0;
			ctr = 0;

			do {
				// Look up the os_map structure until you get to an os core that belongs to the numa node you are indexing
				// make sure the core has smt id 0 as HT may be turned off or on, and we dont want to pick 2 os cores 
				// that are hyperthreads together in the same core
				if (os_map[ctr].smt_id == 0 && os_map[ctr].numa_node_id == i) {
					// store the core id in numanode_cpu_map_reserved
					if (numanode_cpu_map_reserved) numanode_cpu_map_reserved[i][j] = os_map[ctr].os_id;
					j++;
				}

				ctr++;
			} while (j < 2 && ctr < os_cpu_count);
			if (j < 2) { // did not find at least 2 cores in each numa node
				no_2_cores_per_socket = 1;
				//Runtime_exit("Need at least 2 cores in each numa node for MLC to run. Exiting..\n");
			}
		}
	}

	if (print_verbose1 && numanode_cpu_map_1tpc_ctr && numanode_cpu_map_ctr && numanode_cpu_map) {
		printf("num numa nodes=%d....\n", num_numa_nodes);
		printf("numa[numa_node][offset]=logical_cpu_id\n");
		for (i = 0; i < num_numa_nodes; i++) {
			for (j=0; j < numanode_cpu_map_ctr[i]; j++) {
				if (numanode_cpu_map) printf("numa[%d][%d]=%d\n", i, j, numanode_cpu_map[i][j]);
			}
		}

		printf("1TPC num numa nodes=%d....\n", num_numa_nodes);
		for (i = 0; i < num_numa_nodes; i++) {
			for (j=0; j < numanode_cpu_map_1tpc_ctr[i]; j++) {
				if (numanode_cpu_map_1tpc) printf("numa[%d][%d]=%d\n", i, j, numanode_cpu_map_1tpc[i][j]);
			}
		}

		printf("NODE RESERVED....\n");
		for (i = 0; i < num_numa_nodes; i++) {
			if (numanode_cpu_map_reserved)
			printf("numa_reserved[%d][%d]=%d, %d\n", i, j, 
				numanode_cpu_map_reserved[i][0], numanode_cpu_map_reserved[i][1]);
		}

		for (i = 0; i < os_cpu_count; i++) 
			printf("OS id: %3u Core id: %3u Socket id: %3u Hyperthread id: %3u numa: %3u\n", os_map[i].os_id, os_map[i].core_id,
				os_map[i].pkg_id, os_map[i].smt_id, os_map[i].numa_node_id);
		if (print_verbose1 && matrix_mode1) {
			for (i = 0; i < num_numa_nodes; i++) {
				if (numanode_cpu_map_ctr[i]) {
					if (numanode_cpu_map_reserved) printf("Socket %3d: latency measurements and memory allocation to be done using core %d, socket kept active by core %d  \n", i, numanode_cpu_map_reserved[i][1], numanode_cpu_map_reserved[i][0]);
				}
			}

			printf("\n");
		}
	}

}

int get_socketid_of_numanode(int nodeid)
{
	int i;
	for (i=0; i < os_cpu_count; i++)
		if (os_map[i].numa_node_id == nodeid) return os_map[i].pkg_id;
	return -1;
}
