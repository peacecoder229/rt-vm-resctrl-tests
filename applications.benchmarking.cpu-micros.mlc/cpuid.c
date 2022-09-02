

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
#include <minwindef.h>
#include <stdio.h>
#endif


#include "types.h"
#include "cpuid.h"
#include "common.h"

int is_skylake();
int is_icelake();
int is_sapphire_rapids();
int is_alderlake();
int is_meteorlake();
int is_raptorlake();
int is_emerald_rapids();
int is_granite_rapids();
int is_amd_processor();

void cpuid(unsigned int  eax_in, unsigned int ecx_in, cpuid_info_t* info)
{
#if defined(LINUX) || defined(__APPLE__)
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
	asm(
	    "mov %4, %%eax;"
	    "mov %5, %%ecx;"
	    "cpuid;"
	    "mov %%eax, %0;"
	    "mov %%ebx, %1;"
	    "mov %%ecx, %2;"
	    "mov %%edx, %3;"
	    : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx) // output operands
	    : "r"(eax_in), "r"(ecx_in)                   // input operands
	    : "%eax", "%ebx", "%ecx", "%edx"             // clobbered registers
	);
	info->eax = eax;
	info->ebx = ebx;
	info->ecx = ecx;
	info->edx = edx;
#else
	int cpuInfo[4];
	__cpuidex(cpuInfo, eax_in, ecx_in);
	info->eax = cpuInfo[0];
	info->ebx = cpuInfo[1];
	info->ecx = cpuInfo[2];
	info->edx = cpuInfo[3];
#endif
}

void check_processor_supported()
{
	UINT64 processor_signature, cpu_family_id=0, cpu_model_id=0;
	cpuid_info_t info;
	int id;

	if (is_amd_processor()) { // 
		unsigned int base_family=0, extended_family=0;
		AMD_cpu = 0;
		cpuid(1,0,&info); // eax=1
		id = info.eax;
		base_family = (id >> 8) & 0xf;
		if (base_family == 0xf) {
			extended_family = (id >> 20) & 0xff;
			cpu_family_id = base_family + extended_family;
			if (cpu_family_id >= 0x17) { // not supporting anything older than Ryzen - family 0x17
				AMD_cpu = 1;
				if (print_verbose1) printf("AMD processor detected, family_id=0x%llx", cpu_family_id);
				if (cpu_family_id >= 0x19)
					AMD_zen3=1; // zen3 architecture detected
			}
			else { app_exit(EACCES, "Running on unsupported processor... Bye.\n"); } 

		}
	}
	else { 
		processor_signature = get_processor_signature();
		cpu_family_id = ((processor_signature >> 8) & 0xf) | ((processor_signature & 0xf00000) >> 16);
		cpu_model_id = ((processor_signature & 0xf0) >> 4) | (processor_signature & 0xf0000) >> 12;
	}

	if (print_verbose1) 
		printf("Running on processor type: family 0x%llx, model 0x%llx\n", cpu_family_id, cpu_model_id);

	// Decided to not check for processor type anymore. This way MLC can work on any new Intel processor especially for all the client parts
/*	if ((cpu_family_id != 6) ||
	        !((cpu_model_id == 0x2E)    // Nehalem-EX
	          || (cpu_model_id == 0x1A)   // Nehalem-EP, Bloomfield
	          || (cpu_model_id == 0x1E)   // Clarksfield, Lynnfield, Jasper Forest
	          || (cpu_model_id == 0x1F)   // Nehalem
	          || (cpu_model_id == 0x25)   // Arrandale, Clarksdale
	          || (cpu_model_id == 0x2A)   // Sandy Bridge
	          || (cpu_model_id == 0x2C)   // Gulftown, Westmere-EP
	          || (cpu_model_id == 0x2D)   // Sandy Bridge-EP, -EN, -E
	          || (cpu_model_id == 0x2F)   // Westmere-EX
	          || (cpu_model_id == 0x3A)   // Ivy Bridge
	          || (cpu_model_id == 0x3C)   // Haswell
	          || (cpu_model_id == 0x3D)   // Broadwell core-M and desktop
	          || (cpu_model_id == 0x3E)   // Ivy Town
	          || (cpu_model_id == 0x3F)   // Haswell server
	          || (cpu_model_id == 0x45)   // Haswell ULT
	          || (cpu_model_id == 0x46)   // Haswell ULT
	          || (cpu_model_id == 0x47)   // E3-1200 v4 based on BDW
	          || (cpu_model_id == 0x4E)   // Skylake client
	          || (cpu_model_id == 0x4F)   // broadwell server
	          || (cpu_model_id == 0x55)   // Skylake server
	          || (cpu_model_id == 0x56)   // Broadwell Xeon D
	          || (cpu_model_id == 0x57)   // Xeon Phi : KNL
	          || (cpu_model_id == 0x85)   // Xeon Phi : KNM
	          || (cpu_model_id == 0x5C)   // Broxton-0, -P
	          || (cpu_model_id == 0x5E)   // Skylake client
	          || (cpu_model_id == 0x5F)   // Denverton
	          || (cpu_model_id == 0x7A)   // Gemini Lake
	          || (cpu_model_id == 0x9E)   // Kaby lake
			  || (cpu_model_id == 0x8E)   // Kaby lake - not sure it is 0x8e or 0x9e
	          || (cpu_model_id == 0x6A)   // Icelake server
	          || (cpu_model_id == 0x8F)   // SapphireRapids SPR
	          || (cpu_model_id == 0x86)   // SnowRidge SNR
	          || (cpu_model_id == 0x9A)   // Alder lake-P
	          || (cpu_model_id == 0x97)   // Alder lake-S
	          || (cpu_model_id == 0xAA)   // Meteor lake-P
	          || (cpu_model_id == 0xAB)   // Meteor lake-N/-U
	          || (cpu_model_id == 0xAC)   // Meteor lake-S/-H
	          || (cpu_model_id == 0xBF)   // Alder lake as RPL desktop
	          || (cpu_model_id == 0xB7)   // Raptor Lake desktop
	          || (cpu_model_id == 0xBA)   // Raptor Lake mobile
	          || (cpu_model_id == 0xCF)   // Emerald rapids
	          || (cpu_model_id == 0xAD)   // Granite rapids
				)) {
		printf("Running on unsupported processor type: family 0x%lx, model 0x%lx\n", (unsigned long)cpu_family_id, (unsigned long)cpu_model_id);
		app_exit(EACCES, "Exiting... Bye.\n");
	}
*/
}

unsigned int get_processor_signature()
{
	cpuid_info_t info;
	cpuid(0x1, 0, &info);
	return info.eax;
}

cpuid_info_t get_processor_topology(unsigned int level)
{
	cpuid_info_t info;
	cpuid(0xb, level, &info);
	return info;
}

void get_AMD_processor_cache_info()
{
	cpuid_info_t info;
	
	cache_info[L1I_CACHE].total_size = 0; // Since this is not being used in this test, we will set this to zero
	
	cpuid(0x80000005,0,&info); // L1D
	cache_info[L1D_CACHE].total_size = ((info.ecx >> 24) & 0xff) * 1024;

	cpuid(0x80000006,0,&info); // L2
	cache_info[L2_CACHE].total_size = ((info.ecx >> 16) & 0xffff) * 1024;

	cpuid(0x80000006,0,&info); //L3
	cache_info[L3_CACHE].total_size = ((info.edx >> 18) & 0x3fff) * 512 * 1024; // in 512KB units
	
}

void get_processor_cache_info()
{
	unsigned int level;
	cpuid_info_t info;
	
	if (AMD_cpu) {
		get_AMD_processor_cache_info();
	}
	else {
		for(level=0;level<0xff;level++)
		{
			cpuid(0x4, level, &info);
			if((info.eax & 0x1f) == 0) // if bit 4:0 == 0, then no more caches
				break;
			//Set cache level, size, etc.
			if (level < 5) {
				cache_info[level].cache_type = (info.eax & (0xf));
				cache_info[level].level = ((info.eax & (0xe0)) >> 5);
				cache_info[level].line_size = ((info.ebx & (0xfff))) + 1;
				cache_info[level].way = ((info.ebx & (~0x3fffff)) >> 22) + 1;
				cache_info[level].set = (info.ecx) + 1;
				cache_info[level].total_size = cache_info[level].set * cache_info[level].way * cache_info[level].line_size;
			}
		}
	}
}

unsigned short get_processor_model_id()
{
	UINT64 processor_signature;
	unsigned short cpu_model_id;
	processor_signature = get_processor_signature();
	//printf("Processor signature is %x\n",processor_signature);
	// the following line are from Intel Performance Counter Monitor
	cpu_model_id = ((processor_signature & 0xf0) >> 4) | (processor_signature & 0xf0000) >> 12;
	return cpu_model_id;
}
int is_avx_enabled()
{
	cpuid_info_t info;
	cpuid(1,0,&info);
	if ((info.ecx & (1<<28)) != 0) return 1;
	return 0;
} 

int is_hw_rdrand_enabled()
{
	cpuid_info_t info;
	cpuid(1,0,&info);
	if ((info.ecx & (1<<30)) != 0) return 1;
	return 0;
} 


int is_avx512_enabled()
{
	cpuid_info_t info;
	cpuid(7,0,&info);
	if ((info.ebx & (1<<16)) != 0) return 1;
	return 0;
} 
char is_atom()
{
	unsigned short cpu_model_id = get_processor_model_id();
	//We only care about Goldmont based Atom or newer
	return (cpu_model_id == 0x5C)
		|| (cpu_model_id == 0x5F);

}
char is_xeon_phi()
{
	unsigned short cpu_model_id = get_processor_model_id();
	//We only care about Goldmont based Atom or newer
	return (cpu_model_id == 0x57 || cpu_model_id == 0x85);

}

int is_non_inclusive_cache_with_dbp_enabled()
{

	return (is_skylake() || is_icelake()
	 || is_sapphire_rapids() || is_alderlake() || is_raptorlake()
	 || is_meteorlake() || is_emerald_rapids() || is_granite_rapids()
	 );
}

int supports_AMP_prefetcher()
{
	return (is_sapphire_rapids() || is_alderlake() || is_raptorlake()
	 || is_meteorlake() || is_emerald_rapids() || is_granite_rapids()
	 );
}

int supports_spec_itom()
{
	// On processors which supports specItoM, full 64 byte writes can avoid RFOs
	// So, we need to do onluy partial stores so we can properly account for read+write bw
	return (is_icelake()
	 || is_sapphire_rapids() || is_alderlake() || is_raptorlake()
	 || is_meteorlake() || is_emerald_rapids() || is_granite_rapids()
	 );
	
}
int is_skylake()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 55); 

}

int is_icelake()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0x6A); 

}

int is_sapphire_rapids()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0x8F); // SPR

}
int is_alderlake()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0x9A || cpu_model_id == 0x97 || cpu_model_id == 0xBF); // Alderlake
}

int is_meteorlake()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0xAA || cpu_model_id == 0xab || cpu_model_id == 0xac); // Meteor lake
}

int is_raptorlake()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0xB7 || cpu_model_id == 0xBA ); // Raptor lake
}

int is_emerald_rapids()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0xCF ); 
}

int is_granite_rapids()
{
	unsigned short cpu_model_id = get_processor_model_id();

	return (cpu_model_id == 0xAD ); 
}

int is_amd_processor()
{
	cpuid_info_t info;

	cpuid(0,0,&info);
	if (info.ebx == 0x68747541 && info.edx == 0x69746E65 && info.ecx == 0x444D4163)
		return 1;
	else return 0; // not an AMD cpu
}

extern int can_modify_AMP_prefetcher();
uint64_t get_hwpref_disable_mask()
{ 
	uint64_t disable_pref_msr_mask =0;
	
	if(is_atom())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_ATOM;
	else if (is_xeon_phi())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_XEON_PHI;	
	else if (AMD_zen3)
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_ZEN3;
	else if (is_alderlake())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_ADL;
	else if (is_meteorlake())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_MTL;
	else if (is_raptorlake())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_RPL;
	else if (is_sapphire_rapids())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_SPR;
	else if (is_emerald_rapids())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_RPL; // EMR uses RPC core
	else if (is_granite_rapids())
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_MTL; // GNR uses RWC core
	else
		disable_pref_msr_mask = (uint64_t)DISABLE_PREF_CORE;
	
	return disable_pref_msr_mask;

}

uint64_t get_hwpref_enable_mask()
{ 
	uint64_t enable_pref_msr_mask =0;
	
	if(is_atom())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_ATOM;
	else if (is_xeon_phi())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_XEON_PHI;	
	else if (AMD_zen3)
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_ZEN3;
	else if (is_alderlake())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_ADL;
	else if (is_meteorlake())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_MTL;
	else if (is_raptorlake())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_RPL;
	else if (is_sapphire_rapids()) 
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_SPR;
	else if (is_emerald_rapids())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_RPL; // EMR uses RPC core
	else if (is_granite_rapids())
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_MTL;
	else
		enable_pref_msr_mask = (uint64_t)ENABLE_PREF_CORE;
	return enable_pref_msr_mask;

}