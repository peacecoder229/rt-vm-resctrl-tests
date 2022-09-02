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


#ifndef CPUID_INC
#define CPUID_INC

typedef struct cpuid_info_t {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
} cpuid_info_t;

typedef struct cache_info_t {
	unsigned int level;
	unsigned int set;
	unsigned int way;
	unsigned int cache_type;
	unsigned int line_size;
	unsigned int total_size;
} cache_info_t;


void cpuid(unsigned int  eax_in, unsigned int ecx_in, cpuid_info_t* info);
unsigned int get_processor_signature();
cpuid_info_t get_processor_topology(unsigned int level);
void check_procssor_supported();
unsigned short get_processor_model_id();
void get_processor_cache_info();
char is_atom();
char is_xeon_phi();

#endif
