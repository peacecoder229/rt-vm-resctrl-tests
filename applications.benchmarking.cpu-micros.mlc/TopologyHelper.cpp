
#include <Windows.h>
#include <systemtopologyapi.h>
#include <Windows.h>
#include "topology.h"
#include <iostream>
#include <iomanip>

#if 1
extern "C" void InitTopologyHelper();
extern "C" BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity);
extern "C" void BindToCpu(int LinearProcNumber);
extern "C" BOOL GetLogicalProcInfo(WORD LinearProcNumber, LOGICAL_PROC_T * lp);
extern "C" NUMA_TYPE IsNumaValid(int CheckNumaNode);
extern "C" int GetMaxNuma();
extern "C" int print_verbose1;
extern "C" void DumpProcArray();
#endif

void cpuid(unsigned int  eax_in, unsigned int ecx_in, cpuid_info_t* info)
{
	int cpuInfo[4];
	__cpuidex(cpuInfo, eax_in, ecx_in);
	info->eax = cpuInfo[0];
	info->ebx = cpuInfo[1];
	info->ecx = cpuInfo[2];
	info->edx = cpuInfo[3];
}

cpuid_info_t get_processor_topology(unsigned int level)
{
	cpuid_info_t info;
	cpuid(0xb, level, &info);
	return info;
}

void get_processor_cache_info(cache_info_t *p_cache_info)
{
	unsigned int level;
	cpuid_info_t info;
	for (level = 0; level<0xff; level++)
	{
		cpuid(0x4, level, &info);
		if ((info.eax & 0x1f) == 0) // if bit 4:0 == 0, then no more caches
			break;
		//Set cache level, size, etc.
		p_cache_info[level].cache_type = (info.eax & (0xf));
		p_cache_info[level].level = ((info.eax & (0xe0)) >> 5);
		p_cache_info[level].line_size = ((info.ebx & (0xfff))) + 1;
		p_cache_info[level].way = ((info.ebx & (~0x3fffff)) >> 22) + 1;
		p_cache_info[level].set = (info.ecx) + 1;
		p_cache_info[level].total_size = p_cache_info[level].set * p_cache_info[level].way * p_cache_info[level].line_size;
	}
}

static void parse_apic_id(LOGICAL_PROC_T* proc)
{
	unsigned int smt_mask_width, smt_mask, core_mask_width, core_mask, pkg_mask;
	
	cpuid_info_t info_l0 = get_processor_topology(0);
	cpuid_info_t info_l1 = get_processor_topology(1);
	
	// Get the SMT ID
	smt_mask_width = info_l0.eax & 0x1f;
	smt_mask = ~((-1) << smt_mask_width);
	proc->cpuid.smt_id = info_l0.edx & smt_mask;
	// Get the core ID
	core_mask_width = info_l1.eax & 0x1f;
	core_mask = (~((-1) << core_mask_width)) ^ smt_mask;
	proc->cpuid.core_id = (info_l1.edx & core_mask) >> smt_mask_width;
	// Get the package ID
	pkg_mask = (-1) << core_mask_width;
	proc->cpuid.pkg_id = (info_l1.edx & pkg_mask) >> core_mask_width;
}

class TopologyHelper
{
	BOOL						InitComplete = { false };
	LOGICAL_PROC_T*				procs = NULL;
	NUMA_NODE_GROUP_AFFINITY*	NumaNodeAraay = NULL;

	ULONG						MaxNumaNode = 0;
	DWORD						MaxActiveProcInSystem = 0;
	WORD						MaxActiveGroupCount = 0;

public:
	TopologyHelper()
	{
		//1.
		MaxActiveProcInSystem = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
		MaxActiveGroupCount = GetActiveProcessorGroupCount();

		//3. for Each Numa Node, get GROUP_AFFINIY info and store in array
		PopulateNumaArray();
		
		//2.
		PopulateProcArray();


		InitComplete = true;
	}

	~TopologyHelper()
	{

	}

	/*
	Purpose: This method converts seqential proc number to NUMA_NODE_GROUP_AFFINITY
	NUMA_NODE_GROUP_AFFINITY contains NumaNodeNumber & GROUP_AFFINITY

	Ex:
	Let say 2S BDX System with 18Cores/Socket
	Total cores 36 Cores
	Total Thread 72 Threads

	Valid LinearProc Number is 0-71

	If input LinearProcNumber is 38, This function will return NumaNodeNumber of 1 & GROUP_AFFINITY for Third thread in NUMA Node 1
	*/
	BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity)
	{
		BOOL result = FALSE;
		static int print_verbose1_once = 0;

		if (LinearProcNumber < MaxActiveProcInSystem)
		{
			nng_affinity->GroupMasks[0].Group = procs[LinearProcNumber].affinity.Group;
			nng_affinity->GroupMasks[0].Mask = procs[LinearProcNumber].affinity.Mask;
			nng_affinity->NodeNumber = procs[LinearProcNumber].node;

			if (nng_affinity->NodeNumber < MaxNumaNode)
			{
				nng_affinity->AvailableMemory = NumaNodeAraay[nng_affinity->NodeNumber].AvailableMemory;
			}
			else
			{
				nng_affinity->AvailableMemory = 0;
			}

			result = TRUE;
		}

		if ((print_verbose1_once == 0) && print_verbose1)
		{
			print_verbose1_once = 1;
			DumpProcArray();
		}
		return result;
	}

	/*Bind the current thread to specified LogicalProcNumber */
	void BindToCpu(int LinearProcNumber)
	{
		NUMA_NODE_GROUP_AFFINITY nng_affinity = { 0 };

		if (FALSE == LinearProcNumToNumaNodeGroupAffinity(LinearProcNumber, &nng_affinity))
		{
			//std::cout << "BindToCpu Failed because GetLogicalProcInfo failed for proc number: " << LinearProcNumber << endl;
		}

		if (FALSE == SetThreadGroupAffinity(GetCurrentThread(), &nng_affinity.GroupMasks[0], NULL))
		{
			//std::cout << "BindToCpu(" << LinearProcNumber << ")Failed because SetThreadGroupAffinity failed Err: " << GetLastError() << endl;
		}

		Sleep(0);
	}

	/*
	Purpose: This method converts seqential proc number to APIC_ID_t which contains core_info/smt_infp/cache_info/numa_node_info
	NUMA_NODE_GROUP_AFFINITY contains NumaNodeNumber & GROUP_AFFINITY

	Ex:
	Let say 2S BDX System with 18Cores/Socket
	Total cores 36 Cores
	Total Thread 72 Threads

	Valid LinearProc Number is 0-71

	If input LinearProcNumber is 38, This function will return APIC_ID_t which contains all topological info
	*/
	BOOL GetLogicalProcInfo(WORD LinearProcNumber, LOGICAL_PROC_T * lp)
	{
		if (LinearProcNumber < MaxActiveProcInSystem)
		{
			memcpy_s(lp, sizeof(LOGICAL_PROC_T), &procs[LinearProcNumber], sizeof(LOGICAL_PROC_T));
			return TRUE;
		}

		return FALSE;
	}


	/*
	Purpose:

	output:
	
	*/

	// NUMA_TYPE IsNumaValid(int CheckNumaNode)
	// {
	// 	if((CheckNumaNode > MaxNumaNode) && (CheckNumaNode < 0))
	// 	{
	// 		return INVALID_NODE_E;
	// 	}

	// 	if ((0 == NumaNodeAraay[CheckNumaNode].affinity.Mask) && (0 == NumaNodeAraay[CheckNumaNode].AvailableMemory))
	// 	{
	// 		return DUMMY_NODE_E;
	// 	}

	// 	if (0 == NumaNodeAraay[CheckNumaNode].affinity.Mask )
	// 	{
	// 		return CPU_MEM_NODE_E;
	// 	}

	// 	if (0 == NumaNodeAraay[CheckNumaNode].AvailableMemory)
	// 	{
	// 		return CPU_ONLY_NODE_E;
	// 	}

	// 	return CPU_MEM_NODE_E;
	// }

	int GetMaxNuma()
	{
		return (int) MaxNumaNode;
	}

	void DumpProcArray()
	{
		NUMA_NODE_GROUP_AFFINITY nng = { 0 };
		
		while(!InitComplete)
		{ }

		std::cout << "Id\t\tGroup\tNuma\tLTIdx\tCoreIdx\tNumaIdx\tLLCIdx\tAlloc\tPark\tRT\tSchd\tKGROUP" << std::endl;
		for (int i = 0; i < MaxActiveProcInSystem;i++) {
			
			LinearProcNumToNumaNodeGroupAffinity(i, &nng);

			std::cout << std::hex
				<< "0x"
				<< procs[i].pCSI->CpuSet.Id
				<< "\t\t0x"
				<< procs[i].pCSI->CpuSet.Group
				<< "\t0x"
				<< int(procs[i].node)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.LogicalProcessorIndex)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.CoreIndex)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.NumaNodeIndex)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.LastLevelCacheIndex)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.Allocated)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.Parked)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.RealTime)
				<< "\t0x"
				<< int(procs[i].pCSI->CpuSet.SchedulingClass)
				<< "\t0x"
				<< std::setfill('0') << std::setw(2) << nng.GroupMasks[0].Group
				<< "\t0x"
				<< std::setfill('0') << std::setw(16) << nng.GroupMasks[0].Mask
				<< std::endl;
		}
	}

private:
	BOOL GetNumaNodeNumber(GROUP_AFFINITY * mask, ULONG * NodeNumber)
	{
		for(int i = 0; i <= MaxNumaNode ; i++)
		{
			for(int j = 0; j < NumaNodeAraay[i].GroupCount ;j++)
			{
				if ( (NumaNodeAraay[i].GroupMasks[j].Group == mask->Group) && (NumaNodeAraay[i].GroupMasks[j].Mask & mask->Mask))
				{
					*NodeNumber = NumaNodeAraay[i].NodeNumber;
					return true;
				}
			}
		}
		return false;
	}

void PopulateProcArray()
	{

		ULONG bufflen, bufflen_ret = 0;
		PSYSTEM_CPU_SET_INFORMATION cpuInfo = NULL;

		procs = new LOGICAL_PROC_T[GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS)];

		if (!GetSystemCpuSetInformation(NULL, 0, &bufflen, NULL, 0))
		{
			cpuInfo = (PSYSTEM_CPU_SET_INFORMATION)new char[bufflen];
			memset(cpuInfo, 0, bufflen);

			if (!GetSystemCpuSetInformation(cpuInfo, bufflen, &bufflen_ret, NULL, 0))
			{
				std::cout << "GetSystemCpuSetInformation failed..." << GetLastError() << std::endl;
				exit(-1);
			}

			ULONG current = 0;
			
			int count = 0;
			while ((current < bufflen) & (0 == cpuInfo->Type))
			{
				if(cpuInfo->Type == CpuSetInformation)
				{
					procs[count].pCSI = cpuInfo;
					procs[count].num.Group = procs[count].pCSI->CpuSet.Group;
					procs[count].num.Number = procs[count].pCSI->CpuSet.LogicalProcessorIndex;
					procs[count].num.Reserved = 0;

					memset(&procs[count].affinity, 0, sizeof(GROUP_AFFINITY));
					procs[count].affinity.Group = procs[count].pCSI->CpuSet.Group;
					procs[count].affinity.Mask = ( (0==procs[count].num.Number) ? 1LL : (1LL << procs[count].num.Number) ) ;
					if (!GetNumaNodeNumber(&procs[count].affinity , &procs[count].node))
					{
						std::cout << "GetNumaNodeNumber failed..."  << " ProcCount: " << count << " Group: " << procs[count].num.Group << "  ProcIndex: " << int(procs[count].num.Number) << " Node Index: " << int(procs[count].pCSI->CpuSet.NumaNodeIndex)  << " Exiting...." << std::endl;
						exit(-1);
					}

					//  if (!GetNumaProcessorNodeEx(&procs[count].num, &procs[count].node)) {
					// 	std::cout << "GetNumaProcessorNodeEx failed..." << GetLastError() << " ProcCount: " << count << " Group: " << procs[count].num.Group << "  ProcIndex: " << int(procs[count].num.Number) << " Node Index: " << int(procs[count].pCSI->CpuSet.NumaNodeIndex)  << " Exiting...." << std::endl;
					//  	exit(-1);
					//  }

					//std::cout << "PopulateProcArray() ..." << "ProcCount: " << count << " Group: " << procs[count].num.Group << "  ProcIndex: " << int(procs[count].num.Number) << " Node: " << int(procs[count].node) << std::endl;
					count++;
				}

				current += cpuInfo->Size;
				cpuInfo = (PSYSTEM_CPU_SET_INFORMATION) (((byte *)cpuInfo)+cpuInfo->Size);
			}
		}
		else
		{
			std::cout << "How did I endup here :-( " << GetLastError() << std::endl;
			exit(-1);
		}

		//For Each logical processor, get core_id, smt_id, pkg_id & cache_info
		for (DWORD i = 0; i < MaxActiveProcInSystem; i++)
		{
			//5a. first switch to specified Logical Proc so that CPUID instruction can be issued to that logical Proc
			BindToCpu(i);

			//5b. obtain cache_info_t
			get_processor_cache_info(procs[i].cache_info);


			//5c. obtain APIC ID information
			parse_apic_id(&procs[i]);
		}
	}

	void PopulateNumaArray() 
	{
		DWORD buffsize = 0;
		char *  buffer = NULL;

		GetLogicalProcessorInformationEx(RelationNumaNodeEx, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &buffsize);
		buffer = new char[buffsize];
		if (NULL == buffer)
		{
			std::cout << "Buffer alloaction of  " << buffsize << " bytes and error code " << GetLastError() << std::endl;
			exit(-1);
		}
		memset(buffer, 0, buffsize);
		if(!GetLogicalProcessorInformationEx(RelationNumaNodeEx, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &buffsize))
		{
			buffsize = 0;

			delete buffer;
			buffer = NULL;

			GetLogicalProcessorInformationEx(RelationNumaNode, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &buffsize);
			buffer = new char[buffsize];
			if (NULL == buffer)
			{
				std::cout << "Buffer alloaction of  " << buffsize << " bytes and error code " << GetLastError() << std::endl;
				exit(-1);
			}		
	
			if(!GetLogicalProcessorInformationEx(RelationNumaNode, (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer, &buffsize))
			{
				std::cout << "GetLogicalProcessorInformationEx failed...." << GetLastError() << std::endl;
				exit(-1);				
			}
		}

		GetNumaHighestNodeNumber(&MaxNumaNode);
		NumaNodeAraay = new NUMA_NODE_GROUP_AFFINITY[MaxNumaNode + 8];
		memset(NumaNodeAraay, 0, sizeof(NUMA_NODE_GROUP_AFFINITY)*(MaxNumaNode + 8));
	
		USHORT i = 0; 
		while(buffsize)
		{
			PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pLogicalProcInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)buffer;
			switch (pLogicalProcInfo->Relationship)
			{
				case RelationNumaNode:
				case RelationNumaNodeEx:
					if(pLogicalProcInfo->NumaNode.GroupCount == 0)
					{
						pLogicalProcInfo->NumaNode.GroupCount = 1;
					}
					GetNumaAvailableMemoryNode(i, &NumaNodeAraay[i].AvailableMemory);
					NumaNodeAraay[i].NodeNumber = pLogicalProcInfo->NumaNode.NodeNumber;
					NumaNodeAraay[i].GroupCount = pLogicalProcInfo->NumaNode.GroupCount;

					for(int j = 0; j < pLogicalProcInfo->NumaNode.GroupCount; j++)
					{
						NumaNodeAraay[i].GroupMasks[j].Group = pLogicalProcInfo->NumaNode.GroupMasks[j].Group;
						NumaNodeAraay[i].GroupMasks[j].Mask = pLogicalProcInfo->NumaNode.GroupMasks[j].Mask;
					}
					break;
			}
			buffer += pLogicalProcInfo->Size;
			buffsize -= pLogicalProcInfo->Size;
			i++;
		}
	}
};


TopologyHelper *tophelp = { 0 };

/*Create non-object version of the key methods (TopologyHelper class methods) for mlc integerations*/
#if 0
extern "C" void InitTopologyHelper();
extern "C" BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity);
extern "C" void BindToCpu(int LinearProcNumber);
extern "C" BOOL GetLogicalProcInfo(WORD LinearProcNumber, APIC_ID_t * lp);
#endif

void InitTopologyHelper()
{
	tophelp = new TopologyHelper;
}


BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity)
{
	if (NULL != tophelp)
	{
		return tophelp->LinearProcNumToNumaNodeGroupAffinity(LinearProcNumber, nng_affinity);
	}
	else
	{
		return FALSE;
	}
}

void BindToCpu(int LinearProcNumber)
{
	if (NULL != tophelp)
	{
		return tophelp->BindToCpu(LinearProcNumber);
	}
	else
	{
		return;
	}
}


BOOL GetLogicalProcInfo(WORD LinearProcNumber, LOGICAL_PROC_T* lp)
{
	if (NULL != tophelp)
	{
		return tophelp->GetLogicalProcInfo(LinearProcNumber, lp);
	}
	else
	{
		return FALSE;
	}
}

/*

*/
// NUMA_TYPE IsNumaValid(int CheckNumaNode)
// {
// 	return tophelp->IsNumaValid(CheckNumaNode);
// }

int GetMaxNuma()
{
	return tophelp->GetMaxNuma();
}

void DumpProcArray()
{
	tophelp->DumpProcArray();
}

