// (C) 2012-2014 Intel Corporation
//
// - initial version extracted from topology (Thomas Willhalm)

/* The following functions parse_apic_id, bind_context, bind_cpu, cpuid, get_processor_signature, get_processor_topology are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

 */


#ifndef TOPOLOGY_INC
#define TOPOLOGY_INC

#if defined(LINUX) || defined(__APPLE__)

typedef struct APIC_ID_t {
	unsigned int smt_id;
	unsigned int core_id;
	unsigned int pkg_id;
	unsigned int os_id;
	unsigned int numa_node_id;
} APIC_ID_t;

#else

#include "cpuid.h"

typedef struct NUMA_NODE_GROUP_AFFINITY
{
	ULONG				NodeNumber;
	WORD   				GroupCount;
	GROUP_AFFINITY		GroupMasks[8];
	ULONGLONG AvailableMemory;
}NUMA_NODE_GROUP_AFFINITY;

typedef enum NUMA_TYPE
{
	INVALID_NODE_E	= -4,
	DUMMY_NODE_E	= -3,
	MEM_ONLY_NODE_E = -2,
	CPU_ONLY_NODE_E = -1,
	CPU_MEM_NODE_E	=  0
}NUMA_TYPE;

typedef struct APIC_ID_t {
	unsigned int smt_id;
	unsigned int core_id;
	unsigned int pkg_id;
	unsigned int os_id;
	unsigned int numa_node_id;

	NUMA_NODE_GROUP_AFFINITY numa_info;
	cache_info_t cache_info[5];
} APIC_ID_t;

typedef struct LOGICAL_PROC_T {
	PSYSTEM_CPU_SET_INFORMATION pCSI;
	PROCESSOR_NUMBER            num;
	ULONG                       node;
	GROUP_AFFINITY              affinity;

	struct {
		unsigned int smt_id;
		unsigned int core_id;
		unsigned int pkg_id;
		unsigned int os_id;
		unsigned int numa_node_id;
	}cpuid;
	cache_info_t cache_info[5];

}LOGICAL_PROC_T;
#endif

extern APIC_ID_t* os_map;

int build_topology();
int find_hyperthread(int osid);


#if !defined(LINUX) && !defined(__APPLE__)
BOOL getCurrentAffinity(GROUP_AFFINITY* pga);
BOOL setCurrentAffinity(GROUP_AFFINITY ga);
#else
int BindToCpu(int cpuid);
#endif


#endif
