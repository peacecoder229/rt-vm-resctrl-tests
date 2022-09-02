
// (C) 2012-2014 Intel Corporation
//
// 

/* The following functions parse_apic_id, bind_context, bind_cpu, cpuid, get_processor_signature, get_processor_topology are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

 */
//#define LINUX
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
#include <ctype.h>
#include <regex.h>

#else // Windows specific
#include <windows.h>
#include <string.h>
#include <minwindef.h>
#include <sysinfoapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#endif

#include "types.h"
#include "common.h"
#include "topology.h"

#if !defined(LINUX) && !defined(__APPLE__)
#define strtok_r(a,b,c) strtok_s(a,b,c) 
#define strcasecmp(a,b) stricmp(a,b) 
#define strncpy(dest,src,len) strcpy_s(dest,len,src)
#endif

int traffic_pattern[MAX_THREADS];
int randombw[MAX_THREADS];
int buf_sizes[MAX_THREADS];
int is_persistent[MAX_THREADS];
int is_persistent_2[MAX_THREADS];
int remote_source[MAX_THREADS];
int remote_source_2[MAX_THREADS];
int per_thread_delay[MAX_THREADS];
int address_stream_mix_ratio[MAX_THREADS];
char* sxp_path[MAX_THREADS];
char* sxp_path_2[MAX_THREADS];
int hwrand_available=0;

char core_range[128];
char* pch;
int first=-1, last=-1;
// Following fields need to be sized larger so invalid values can be parsed without overwriting other fields
char per_thread_pattern[32]="";
char ddr_type[32]="";
char ddr_type_2[32]="";
char per_thread_random[32]="";
char ddrt[32]="pmem";
char rand1[32]="rand";
char where_to_alloc[256]="";
char where_to_alloc_2[256]="";
char address_mix_ratio[32]="";
char delay_value[32]="";

int i;

int chk_if_duplicate(int *rand_array, int value, int max_size);
int generate_random_number();
void check_for_supported_traffic_types_for_N_option();
void check_for_supported_random_traffic_types();
void check_valid_address_stream_ratios(int ratio);

// parse input -m parameter and figure out the cpu masks and populate cpu_list structure
int check_if_valid_hex_string(char *str)
{
	char *s;

	if (str == NULL) return 0;
	s = str;
	if (s[0] == '0' && s[1] != 0 && ( s[1] == 'x' || s[1] == 'X')) {// 0x precedes the hex string {
		// now skip the first two characters
		s++; s++;
	}
	while (*s != 0) {
		if (!((*s >='0' && *s <= '9') || (*s >='a' && *s <= 'f') || (*s >='A' && *s <= 'F')))
			return 0;
		s++;
	}
	return 1;
	
}
void parse(char* argp)
{
    char tmpbuf[128];
    int k, idx, i;
    UINT64 mask;
	int count_mask_cpus_tmp;

    strcpy_s(tmpbuf, sizeof(tmpbuf), argp);
	if (!check_if_valid_hex_string(tmpbuf)) {
		printf ("Invalid hex string %s in -m parameter value\n", tmpbuf);
		app_exit(EINVAL, NULL);
	}
	tmpbuf[sizeof(tmpbuf)-1]=0; // make sure to terminate just in case the input is larger
    k = strnlen_s(tmpbuf, sizeof(tmpbuf));
    idx = 0;
	count_mask_cpus_tmp=0;
    while (k >= 8) {
        mask = strtoul(&tmpbuf[k - 8], NULL, 16);
        tmpbuf[k - 8] = 0;

        for (i = 0; i < 32; i++, idx++) {
            if (mask & 1) {
                cpu_list[numcpus++] = idx;
        		count_mask_cpus_tmp++;        
				//printf("cpu_list[%d]=%d\n",numcpus-1,idx);
            }

            mask >>= 1;
        }

        k -= 8;
    }

    mask = strtoul(tmpbuf, NULL, 16);

    for (i = 0; i < 32; i++, idx++) {
        if (mask & 1) {
            cpu_list[numcpus++] = idx;
            //printf("cpu_list[%d]=%d\n",numcpus-1,idx);
			count_mask_cpus_tmp++;
        }

        mask >>= 1;
    }

    if (count_mask_cpus_tmp < 7) {
        mask_cpus_less_than_7 = 1;
    }

    //printf("numcpus is %d and mask_cpus_less_than_7 is %d\n",numcpus,mask_cpus_less_than_7);
#if 0
    printf("cpu_list is set of logical cores on which we are going to run throughput threads\n");

    for (i = 0; i < numcpus; i++) {
        printf("cpu_list[%d]=%d\n", i, cpu_list[i]);
    }

#endif
}


int is_number(char *c)
{
	int is_number_true=1;
	if (*c ==0) // null string - so return failure
		return 0;
    while(*c) {
		if(isdigit(*c++)==0) {
			is_number_true=0;
			break;
		}
	}
    return is_number_true;
}


// The next 3 routines are used for initializing the buffers used by loadlat   with patterns so memory will be written with those
int read_pattern_file(char* filename)
{
    FILE* fp;
    int patidx = 0, len;
    fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("ERROR - Unable to open the pattern input file %s. Exiting..\n", filename);
		app_exit(EACCES, NULL);
    }

    while (patidx < MAX_NUM_PATTERN_LINES && fp!= NULL) { // We accept only a max number of pattern lines
        if (fgets(pbuf[patidx], PATTERN_LINE_LENGTH, fp) == NULL) { // read one line at a time
             break;
        }

        len = strnlen_s(pbuf[patidx], PATTERN_LINE_LENGTH);

        if (len == 1 && pbuf[patidx][0] == '\n') {
            continue;    // ignore blank lines
        }

        if (len > 1 && pbuf[patidx][len - 1] == '\n') {
            pbuf[patidx][--len] = 0;    // adjust for new line at the end
        }

        // Pattern file is expected to have exactly 128 characters - 64 bytes of cache line represented in HEX without 0x in front
        if (len != 128) {
            printf("ERROR - The pattern file should have exactly 128 characters in each line (you have %d). Exiting.\n", len);
            app_exit(EINVAL, NULL);
        }

        patidx++;
    }

    pattern_lines_present = patidx; // actual number of patterns present
	if (fp!=NULL) fclose(fp);
    return 1;
}

// Get a bunch of random numbers
void get_rand(int *rand_array, int max_size)
{
	int r, idx, cnt = 0;
	char *dup_array;
	int rand_size;
	
	// dup_array is used to detect whether a number from rand() is a duplicate; 1 indicates seen already
	if (max_size <=0) return;
	dup_array = (char*)malloc(max_size+1);
	assert(dup_array);
	memset(dup_array, 0, max_size+1);
	memset(rand_array, 0, sizeof(int)*max_size);
	rand_size = max_size;
	idx = 0;

	for (;;) {
		for (;;) {
#if defined(LINUX) || defined(__APPLE__)
			r = rand() % rand_size;
#else
			r = generate_random_number() % rand_size;
#endif
			if (r!=0 && dup_array[r] == 0) { // should not be zero and not used already
				break;
			}
		}

		if (rand_array[idx] == 0 && idx != r) { // if entry not used and value won't point to itself, then go ahead
			rand_array[idx] = r;
			idx = r;
			dup_array[r]=1;
			cnt++;
		}

		if (cnt >= max_size-1) { // if you reached all but one, then bail out
			rand_array[idx] = max_size;
			break;
		}
	}

	free(dup_array);
 }

void get_rand_bw_thread(int *thruput_array, int max_size)
{
    int i;
	
	if (max_size <=0) return;
	get_rand(thruput_array, max_size);
    for (i = 0; i < max_size; i++) {
		thruput_array[i] -= 1; // since index starts with 1, reduce 1 to make it zero based
		thruput_array[i] *= LineSize;
    }
}

void get_rand_bw_thread_256B(int *thruput_array, int max_size)
{
    int i;
	
	if (max_size <=0) return;
	get_rand(thruput_array, max_size);
    for (i = 0; i < max_size; i++) {
		thruput_array[i] -= 1; // since index starts with 1, reduce 1 to make it zero based
		thruput_array[i] *= (4*LineSize); // multiply by 4 as we are accessing 256 bytes at a time
    }
}

void get_rand_bw_thread_128B(int *thruput_array, int max_size)
{
    int i;
	
	if (max_size <=0) return;
	get_rand(thruput_array, max_size);
    for (i = 0; i < max_size; i++) {
		thruput_array[i] -= 1; // since index starts with 1, reduce 1 to make it zero based
		thruput_array[i] *= (2*LineSize); // multiply by 2 as we are accessing 128 bytes at a time
    }
}

#if defined(LINUX) || defined(__APPLE__)
void Calibrate(UINT64*  ClksPerSec)
{
    UINT64  diff_tsc, clkdiff, start_tsc, end_tsc;
	struct timespec start, stop;
	
 	if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
		printf("error in clock_gettime\n");
		app_exit(EPERM, "clock_getime");
	}
    do {
        start_tsc = THE_TSC;
		clock_gettime(CLOCK_REALTIME, &start);
        clock_gettime(CLOCK_REALTIME, &stop);

		clkdiff = (stop.tv_sec - start.tv_sec) * 1000000000LL + (stop.tv_nsec - start.tv_nsec);
        while ( clkdiff  < 500000000LL) {
            clock_gettime(CLOCK_REALTIME, &stop);
			clkdiff = (stop.tv_sec - start.tv_sec) * 1000000000LL + (stop.tv_nsec - start.tv_nsec);
        }

        end_tsc = THE_TSC;
    } while (clkdiff < 1000);

    diff_tsc = end_tsc - start_tsc;
    *ClksPerSec = ((double) diff_tsc*1000000000LL) / (double)clkdiff;
    NsecClk = (double)1000000000 / (double)(__int64) *ClksPerSec;
}

#else
void Calibrate(UINT64*  ClksPerSec)
{
    UINT64  start;
    UINT64  end;
    UINT64  diff;
    unsigned long       starttick, endtick;
    unsigned long       tickdiff;
    endtick = GetTickCount();

    while (endtick == (starttick = GetTickCount()));

    start = THE_TSC;

    while ((endtick = GetTickCount())  < (starttick + 500));

    end = THE_TSC;
    diff = end - start;
    tickdiff = endtick - starttick;
    *ClksPerSec = (diff * (UINT64)1000) / (LONGLONG)(tickdiff);
    NsecClk = (double)1000000000 / (double)(__int64) * ClksPerSec;
}
#endif

int parse_cores_by_hyphen_and_set_values(char* core_range, int num2, int num3, 
	int num4, int num5, int num6, char* pathname, int num7, int num8, int num9, char*pathname_2){
	//delimit the core_range by hyphen and set values num2-num6 to this core
	//range
	int num1;
	char *pch_t;
	char *end_t;
	char core_range_t[128]; // duplicate buffer to be modified by the second strok_r
	int first, last;
	//printf("parsing %s\n",core_range);
	strcpy_s(core_range_t, sizeof(core_range_t), core_range);
	//core_range_t[strnlen_s(core_range, sizeof(core_range)) + 1] = 0;
	
	pch_t = strtok_r(core_range_t, "-",&end_t);
	i=0;
	// first and last are used to track the range of cores in a
	// given line, in format "first-last"
    first=-1;
    last=-1;
	// extract "first"
	if (pch_t != NULL && !is_number(pch_t)) {
		printf("Invalid string %s in cpu range field\n", pch_t);
		app_exit(EINVAL, NULL);
	}
	if (pch_t != NULL)
		first = atoi_check(pch_t);
	
	i++;
	//printf("first is %d\n",atoi(pch));
	pch_t = strtok_r(NULL, "-",&end_t);
	if(pch_t != NULL){
		if (!is_number(pch_t)) {
			printf("Invalid string %s in cpu range field\n", pch_t);
			app_exit(EINVAL, NULL);
		}
		// this is the scenario where there is a hyphen, so "last"
		// exists, and we extract it
		last = atoi_check(pch_t);
		//printf("last is %d\n",atoi(pch_t));
		pch_t = strtok_r(NULL, "-",&end_t);
    }
    if(pch_t != NULL || first<0){
		// if there is a second hyphen "first" is also negative there is an error
			printf("Error in cpu range field\n");
			app_exit(EINVAL, NULL);
	}
    if(last==-1){
		// this is the scenario where there is no hyphen, so just
		// set all the arrays with the given core id (=first)
		num1=first;
		if(num1<0 || num1>os_cpu_count){
			Usage("\nInvalid core id specified\n");
		}
		if ( (num1 != latency_thread_run_cpuid) || T_provided) { // need to exclude cpu where latency thread will run
			cpu_list[numcpus++]=num1;
		}
		assert(num1 < MAX_THREADS && num1 >= 0); // make sure we don't overwrite the following buffers with large index
		traffic_pattern[num1]=num2;
		randombw[num1]=num3;
		buf_sizes[num1]=num4;
		is_persistent[num1]=num5;
		is_persistent_2[num1]=num7;
		//printf("pers for %d = %d, %d\n", num1, is_persistent[num1], is_persistent_2[num1]);
		remote_source[num1]=num6;		
		remote_source_2[num1]=num8;		
		
		sxp_path[num1] = malloc(strnlen_s(pathname, 256)+1);
		assert(sxp_path[num1] != NULL);
		strcpy_s(sxp_path[num1], strnlen_s(pathname, 256)+1, pathname);

		sxp_path_2[num1] = malloc(strnlen_s(pathname_2, 256)+1);
		assert(sxp_path_2[num1] != NULL);
		strcpy_s(sxp_path_2[num1], strnlen_s(pathname_2, 256)+1, pathname_2);
		
		address_stream_mix_ratio[num1] = num9;
		if (per_thread_delay_specified) {
			per_thread_delay[num1]=num7; // num7 contains is overloaded to delay value in this case
			delay_array[0] = num7;
			delay_array[1] = -1 ; // terminate with -1 for the next element so we won't go to next delay value and so on - it should stop with only one
		}
		// if any of the num2's are nonzero, it is not readonly load
		if(num2!=1){RDONLY_load = 0;}
		if (num2 < 20) { // address stream coming from 2 different sources
			if (is_persistent[num1])
				printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, pmem path %s",
					num1, per_thread_pattern, per_thread_random, buf_sizes[num1],ddr_type, sxp_path[num1]);
			else {
				if (remote_source[num1] == -1) // local node
					printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from local node",
						num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type);
				else
					printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from numa node %d",
						num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type, remote_source[num1]);
			}
			if (per_thread_delay_specified) 
				printf(",\tinjection delay %d\n", num7);
			else printf("\n");
		}
		else {
			if (is_persistent[num1])
				printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, pmem path %s, ",
					num1, per_thread_pattern, per_thread_random, buf_sizes[num1],ddr_type, sxp_path[num1]);
			else
				printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from numa node %d, ",
					num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type, remote_source[num1]);
					
			if (is_persistent_2[num1])
				printf("%s memory, pmem path %s\taddress mix ratio %d\n", ddr_type_2, sxp_path_2[num1], address_stream_mix_ratio[num1]);
			else
				printf("%s memory, access from numa node %d\taddress mix ratio %d\n", ddr_type_2, remote_source_2[num1], address_stream_mix_ratio[num1]);
			
		}
    } else{
		// when there is a hyphen
		if(first>last) {
			printf("In cpu range field, %d > %d\n", first, last);
			app_exit(EINVAL, NULL);
		}
		// loop from "first" until "last" populating the matrices
		assert(first >= 0 && first < MAX_THREADS);
		assert(last >= 0 && last < MAX_THREADS);
		for(i=first;i<last+1;i++){
			num1=i;
			if(num1<0 || num1>os_cpu_count){
            	Usage("\nInvalid core id specified\n");
        	}	
			if ( (i != latency_thread_run_cpuid) || T_provided) { // need to exclude cpu where latency thread will run
				cpu_list[numcpus++]=i;
			}
			traffic_pattern[i]=num2;
			randombw[i]=num3;
			buf_sizes[i]=num4;
			is_persistent[i]=num5;
			is_persistent_2[i]=num7;	
			remote_source[i]=num6;
			remote_source_2[i]=num8;		

			sxp_path[i] = malloc(strnlen_s(pathname, 256)+1);
			assert(sxp_path[i] != NULL);
			strcpy_s(sxp_path[i], strnlen_s(pathname, 256)+1, pathname);

			sxp_path_2[i] = malloc(strnlen_s(pathname_2, 256)+1);
			assert(sxp_path_2[i] != NULL);
			strcpy_s(sxp_path_2[i], strnlen_s(pathname_2, 256)+1, pathname_2);
			
			address_stream_mix_ratio[i] = num9;
			if (per_thread_delay_specified) {
				per_thread_delay[i]=num7; // num7 contains is overloaded to delay value in this case
				delay_array[0] = num7;
				delay_array[1] = -1 ; // terminate with -1 for the next element so we won't go to next delay value and so on - it should stop with only one
			}
			if (num2 < 20) { // address stream coming from 2 different sources
				if (is_persistent[num1])
					printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, pmem path %s",
						num1, per_thread_pattern, per_thread_random, buf_sizes[num1],ddr_type, sxp_path[num1]);
				else {
					if (remote_source[num1] == -1) // local node
						printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from local node",
							num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type);
					else
						printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from numa node %d",
							num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type, remote_source[num1]);
				}
				if (per_thread_delay_specified) 
					printf(",\tinjection delay %d\n", num7);
				else printf("\n");
			}
			else {
				if (is_persistent[num1])
					printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, pmem path %s, ",
						num1, per_thread_pattern, per_thread_random, buf_sizes[num1],ddr_type, sxp_path[num1]);
				else
					printf("Thread id %3d, traffic pattern %3s, %4s access, %8d KiB buffer, %s memory, access from numa node %d, ",
						num1, per_thread_pattern, per_thread_random, buf_sizes[num1], ddr_type, remote_source[num1]);
				if (is_persistent_2[num1])
					printf("%s memory, pmem path %s\taddress mix ratio %d\n", ddr_type_2, sxp_path_2[num1], address_stream_mix_ratio[num1]);
				else
					printf("%s memory, access from numa node %d\taddress mix ratio %d\n", ddr_type_2, remote_source_2[num1], address_stream_mix_ratio[num1]);
			}
			
        }
    }
    // if any of the num2's are nonzero, it is not readonly load
	if(num2!=1){RDONLY_load = 0;}
	
	return 0;
}

// parse the per-thread-config file
// This code can easily win the ugliest piece of code award
void parse_perthreadfile(char* perthreadfile){
	char pathname[256], pathname_2[256];
	char dbuf[1024];
	int lineno=0;
	
	fp = fopen(perthreadfile, "r");
    if (fp == NULL) {
		printf("Unable to open the input perthread file: %s\n", perthreadfile);
		Usage("Unable to open the input perthread file\n");

    }
	printf("Per-thread configuration from %s\n", perthreadfile);
	while (fp!=NULL) {
            int num2=0,num3=0,num4=0,num5=0,num6=-1,num7=0,num8=-1, num9=0; // remote node id is set to -1 if not provided
			char* end_s, bufs[32];
			if (fgets(dbuf, sizeof(dbuf), fp) == NULL) {
				fclose(fp);
                break;
            } else{
				// scan each row from the file
                int num_inputs;
				lineno++;
				if (dbuf[0] == '#' || dbuf[0] == '\n' || dbuf[0] == '\r' ) {
					// Either comment line or blank line seen; ignore this line and continue
					continue;
				}
#if defined(LINUX) || defined(__APPLE__)
				num_inputs=my_sscanf_9_strings(dbuf, core_range, sizeof(core_range)-1, per_thread_pattern, sizeof(per_thread_pattern)-1, 
						per_thread_random, sizeof(per_thread_random)-1, bufs, sizeof(bufs)-1, ddr_type, sizeof(ddr_type)-1, 
						where_to_alloc, sizeof(where_to_alloc)-1, ddr_type_2, sizeof(ddr_type_2)-1, 
						where_to_alloc_2, sizeof(where_to_alloc_2)-1, address_mix_ratio, sizeof(address_mix_ratio)-1 );
#else
				num_inputs=sscanf_s(dbuf, "%s %s %s %s %s %s %s %s %s", core_range, sizeof(core_range)-1, per_thread_pattern, sizeof(per_thread_pattern)-1, 
						per_thread_random, sizeof(per_thread_random)-1, bufs, sizeof(bufs)-1, ddr_type, sizeof(ddr_type)-1, 
						where_to_alloc, sizeof(where_to_alloc)-1, ddr_type_2, sizeof(ddr_type_2)-1, 
						where_to_alloc_2, sizeof(where_to_alloc_2)-1, address_mix_ratio, sizeof(address_mix_ratio)-1 );
#endif
				// The semantics change based on the # of fields in each line
				// 5 or 6 fields - core-range, traffic type, seq/rand, bufsize, dram/pmem, node-id (optional)
				// 7 fields (per-thread delay added) - core-range, traffic type, seq/rand, bufsize, dram/pmem, node-id, per-thread-delay
				// 9 fields (dual address streams) - core-range, traffic type, seq/rand, bufsize, dram/pmem, node-id, dram2/pmem2, path/nodeid, addr-stream-ratio
				// note that with dual address streams, we don't support per-thread injection delay
				
				if(num_inputs != 5 && num_inputs != 6 && num_inputs != 7 && num_inputs != 9) { // expects only 5, 6 , 7 or 9 input parameters
					printf("Error in %s config file - Expected only either 5, 6, 7 or 9 parameters in line# %d \n", perthreadfile, lineno);
					app_exit(EINVAL, NULL);
				}
				if (num_inputs == 5 ) where_to_alloc[0] = '\0'; // set it to NULL if remote node is not provided
				if (num_inputs == 7 ) { // A fixed injection delay value is specified					
						per_thread_delay_specified=1;
						// ddr_type_2 string is mapped to this field now
						// we will overload num7 with delay value now - Ugly..
						num7 = atoi_check(ddr_type_2);
						if (!is_number(&ddr_type_2[0])) {
							printf("Invalid parameter in 7th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
							app_exit(EINVAL, NULL);
						}
				}

				if(peak_bandwidth_mode){
					printf("Peak Bandwidth mode: traffic type, random/seq, memory source inputs will be ignored\n");	
				}
				
				if(bandwidth_matrix_mode){
					printf("Peak Bandwidth mode: memory source input will be ignored\n");			
				}


				// if the first letter is W, then the number following is the
				// W value (ex: for W3, num2=3, for everything else we assume a default of read R,
				// so num2=0
				if(per_thread_pattern[0]=='R'){num2=1;}
                else if (per_thread_pattern[0]=='W'){
					if (!is_number(&per_thread_pattern[1])) {
						printf("Invalid parameter in 2nd field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
						app_exit(EINVAL, NULL);
					}
					num2=atoi_check(&per_thread_pattern[1]);
					if (!check_if_valid_W_options(num2)) {
						printf("Invalid option in for traffic type in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
						app_exit(EINVAL, NULL);
					}
				}
                else {
					printf("Invalid parameter in 2nd field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);
				} // default
				
				if (num_inputs == 9 && num2 < 20) { // 9 parameters supported only for traffic types > 20) 
					printf ("Invalid input for this traffic type in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);					
				}
				
				if (num_inputs != 9 && num2 >= 20 && num2 <= 29) { // 9 parameters supported only for traffic types > 20) 
					printf ("Invalid number of inputs for this traffic type in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);					
				}
				
				// check if the input is "rand", then num3=1, for all else we assume default is sequential
                if (strcasecmp(per_thread_random,"rand")==0){num3=1;}
                else if (strcasecmp(per_thread_random,"seq")==0){num3=0;} // default
				else {
					printf("Only \"seq\" or \"rand\" allowed in 3rd field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);
				}
/*
				if (!is_number(bufs)) {
					printf("Buffer size in 4th field in line# %d in per-thread-config file %s is not an integer\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);
				}
*/
				num4 = AdvancedAtoIinKB(bufs);
				// we only support R,W2,W5 for random bw
				if(num3==1){ // indicates rand b/w selected
					if(!(num2==1 || num2==2 || num2==5 || num2 == 6 || num2 == 7 || 
						num2 == 31 || num2 == 32 || num2 == 35 || num2 == 36 ||
						num2 == 41 || num2 == 42 || num2 == 45 || num2 == 46 ) ){
							Usage("\nRandom bandwidth option is supported only for R, W2, W5 and W6 traffic types!\n");
						}
				}

				// if pmem(ddrt) is specified, it is pmem else it is dram, no other
				// options possible
                if (strcasecmp(ddr_type,ddrt)==0){
					num5=1;
					pmem_alloc_requested=1;
				}
				else if (strcasecmp(ddr_type,"dram")==0) num5=0; // default
				else {
					printf("Only \"dram\" or \"pmem\" allowed in 5th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
					app_exit(EINVAL, NULL);
				}
				
				if (num5 == 0) { // dram option selected
					if(is_number(&where_to_alloc[0])==1) {
						num6=atoi_check(where_to_alloc);
						if (num6 < 0 || num6 > num_numa_nodes) {
							printf("Numa node id > max_numa_nodes in 6th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
							app_exit(EINVAL, NULL);
						}
						//printf("num6 is %d\n",num6);
					}
					else if (where_to_alloc[0] != '\0'){ // it is ok if numa id is not provided
						printf("Invalid numa node id in 6th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
						app_exit(EINVAL, NULL);
					}
				}
				else { // pmem
					// check whether sxp path name is provided instead
					int pathlen = strnlen_s(where_to_alloc, 256);
					if (pathlen >= 1 ) { // assume a real path will be at least 1 character long
						strcpy_s(pathname, sizeof(pathname), where_to_alloc);
					}
					else {
						printf("Invalid pmem path name in 6th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile);
						app_exit(EINVAL, NULL);
					}
				}
				if (num_inputs == 9) {
					if (strcasecmp(ddr_type_2,ddrt)==0){num7=1;}
					else if (strcasecmp(ddr_type_2,"dram")==0) num7=0; // default
					if (num7 == 0) { // dram option selected
						if(is_number(&where_to_alloc_2[0])==1) {
							num8=atoi_check(where_to_alloc_2);
							if (num8 < 0 || num8 > num_numa_nodes) {
								printf("Numa node id > max_numa_nodes in 8th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
								app_exit(EINVAL, NULL);
							}
							//printf("num6 is %d\n",num6);
						}
						else if (where_to_alloc_2[0] != '\0'){ // it is ok if numa id is not provided
							printf("Invalid numa node id in 8th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile); 
							app_exit(EINVAL, NULL);
						}
					}
					else { // pmem
						// check whether sxp path name is provided instead
						int pathlen = strnlen_s(where_to_alloc_2, 256);
						if (pathlen >= 1) { // assume a real path will be at least 1 character long
							strcpy_s(pathname_2, sizeof(pathname_2), where_to_alloc_2);
						}
						else {
							printf("Invalid pmem path name in 6th field in line# %d in per-thread-config file %s\n", lineno, perthreadfile);
							app_exit(EINVAL, NULL);
						}
					}
					if(is_number(&address_mix_ratio[0])==1) {
						num9=atoi_check(address_mix_ratio);
						check_valid_address_stream_ratios(num9);
					}
					
				}
				pch = strtok_r(core_range, ",",&end_s);			
				//pch = strtok(NULL, ",");
    			while(pch != NULL){
        			//printf("string is %s\n",pch);			
        			parse_cores_by_hyphen_and_set_values(pch, num2, num3, num4, num5, num6, pathname, num7, num8, num9, pathname_2);
					pch = strtok_r(NULL, ",",&end_s);
    			}
				//printf("string is %s\n",pch);

				//parse_cores_by_hyphen_and_set_values(pch, num2, num3, num4, num5, num6);
		}
	}
	// Now make sure that no cpu# is specified more than once in the per-thread-config file
	for (int i=0; i < numcpus; i++) {
		int id = cpu_list[i];
		int num_present=0;
		for (int j=0; j < numcpus; j++) {
			if (cpu_list[j] == id) num_present++;
			if (num_present > 1) {
					printf("cpu# %d is specified more than once in per-thread-config file\n", id);
					app_exit(EINVAL, NULL);
			}
		}
	}
	
	if (NUMA_configured)
		check_for_supported_traffic_types_for_N_option();
		
	check_for_supported_random_traffic_types();
	//if (fp != NULL) fclose(fp);
}

void change_traffic(int traffic_type){
	int i;
	for(i=0;i<os_cpu_count;i++){
		traffic_pattern[i]=traffic_type;
	}
}

void change_bandwidth(int random){
	int i;
	for(i=0;i<os_cpu_count;i++){
		randombw[i]=random;
	}
}

void change_bufsize(int buf_size){
	int i;
	for(i=0;i<os_cpu_count;i++){
		buf_sizes[i]=buf_size;
	}
}

void initialize_perthread_defaults()
{
	// Initialize the defaults in case perthreadfile is not provided
	for(i=0;i<os_cpu_count;i++){
		traffic_pattern[i]=rw_mix;
		randombw[i]=random_access_bw_thread;
		if (b_provided)
			buf_sizes[i]=buffer_size;
		else if (is_non_inclusive_cache_with_dbp_enabled())
			buf_sizes[i]=get_fraction_of_3GB_buf_size_per_numa_node(i);
		else
			buf_sizes[i]=DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
		is_persistent[i]=0;
		remote_source[i]=-1; // -1 designates pinning to a specific core
		sxp_path[i] = NULL;
	}
}

void parse_user_cpulist()
{
	int i, num, from, to;
	char *p, *tmp;
	char buf[1024];

	strcpy_s(buf, sizeof(buf), user_cpulist);
	buf[sizeof(buf)-1]=0; // force NULL termination in case the input string is too large
	if (!check_for_valid_chars_in_cpu_range(buf)) {
		printf("Invalid characters in cpu_range in -k option\n");
		app_exit(EINVAL, NULL);
	}
	p=buf;
	numcpus=0;
	while(p) {
		tmp = p;
		p = strchr(p, ',');
		if (p != NULL)  
			*p++='\0';
		// tmp contains substr which may contain '-'
		if (strchr(tmp, '-')) { // contains '-'
#if defined(LINUX) || defined(__APPLE__)
			my_strscan_2_integers_with_dash(tmp, "%d-%d", &from, &to);
#else
			sscanf(tmp, "%d-%d", &from, &to);
#endif
			if (from > to) {
				printf("In cpu range (-k option) field, %d > %d\n", from, to);
				app_exit(EINVAL, NULL);
			}
			for (i=from; i <= to; i++) {
				if (i > os_cpu_count-1) { printf("Invalid cpu id %d on -k parameter list\n", i); app_exit(EINVAL, NULL); }
				if (i==latency_thread_run_cpuid && loaded_latency_mode && !T_provided) { printf("Exclude cpu 0 from the -k parameter list as latency thread runs on cpu0\n");app_exit(EINVAL, NULL); }
				cpu_list[numcpus] = i;
				if (specify_mem_by_node)
					remote_source[i] = memory_node;
				numcpus++;
			}
			//printf("from=%d, to=%d\n", from, to);
		} else { // single number
			num = strtol(tmp, NULL, 10); //sscanf(tmp, "%d", &num);
			cpu_list[numcpus] = num;
			if (num > os_cpu_count-1 || num < 0) { printf("Invalid cpu id %d on -k parameter list\n", num);app_exit(EINVAL, NULL); }
			if (num==latency_thread_run_cpuid && loaded_latency_mode && !T_provided) { printf("Exclude cpu 0 from the -k parameter list as latency thread runs on cpu0\n");app_exit(EINVAL, NULL); }
			if (specify_mem_by_node && num >=0 && num < MAX_THREADS)
				remote_source[num] = memory_node;
			numcpus++;
			//printf("num=%d\n", num);
		}
	}
}

int check_for_valid_chars_in_cpu_range(char *s)
{
	while (*s) {
		// check for only '-', ',' and digits 0-9
		if ( !(*s=='-' || *s==',' || (*s>='0' && *s<='9'))) return 0;
		s++;
	}
	return 1;
}

// if number == 2^n, return n; else return 0
int powerof2bits(int num)
{
	int bitstoshift=0;
	
	if (num==0) return 0; 
	while (num !=1) {
		if ( (num%2) != 0) return 0; // not a power of 2
		num /= 2; bitstoshift++;
	}
	return bitstoshift;
}

int check_if_valid_W_options(int rw_mix)
{

	// we will allow all option even in public release though these would remain undocumented
	if (rw_mix >= 2 && rw_mix <= 12) return 1;

	// single Address stream coming from 2 sources (like ddr4 and ddrt) mixed in certain ratio
	if (rw_mix == 21 || rw_mix == 23 || rw_mix == 27) return 1;

#ifdef AVX512_SUPPORT
	// 256B accesses
	if (rw_mix >=31 && rw_mix <=39) return 1;
	// 128B accesses
	if (rw_mix >=41 && rw_mix <=46) return 1;
#endif
	return 0;
}

void check_for_supported_random_traffic_types()
{
	int i, rw_mix, cpu;
	
	for (i=0; i < numcpus; i++) {
		cpu = cpu_list[i];
		rw_mix = traffic_pattern[cpu];
		
#ifdef AVX512_SUPPORT
		if (randombw[cpu] && (rw_mix >=31 && rw_mix <=39 )) return;
		if (randombw[cpu] && (rw_mix >=41 && rw_mix <=46 )) return;
#endif

		if (randombw[cpu] && (rw_mix != 1 && rw_mix != 2 && rw_mix != 5 && rw_mix != 6 && rw_mix != 7)) {
			printf("Error in per-thread-config file: random access type supported only for traffic types R, W2, W5 and W6\n");
			app_exit(EINVAL, NULL);
		}
	}

}

void check_valid_address_stream_ratios(int ratio)
{
	if (ratio != 10 && ratio != 25 && ratio != 50) {
		printf("Error in per-thread-config file: Only values of 10 or 25 are supported for address stream ratios\n");
		app_exit(EINVAL, NULL);
	}
}


void check_for_supported_traffic_types_for_N_option()
{
	int i, traffic, cpu;
	
	traffic = traffic_pattern[cpu_list[0]];
	for (i=0; i < numcpus; i++) {
		cpu = cpu_list[i];
		if (traffic != traffic_pattern[cpu]) {
			printf("When -N%d option is specified, traffic types on all cpu threads should be the same except on cpu#0\n", NUMA_configured);
			app_exit(EINVAL, NULL);
		}
	}
	switch (NUMA_configured) {
		case 2:
		case 3:
			if (traffic != 3) {
				printf("For this -N%d option, only traffic type W3 is supported\n", NUMA_configured);
				app_exit(EINVAL, NULL);
			}
			break;
		case 4:
			if (traffic != 1 && traffic != 3 && traffic != 6) {
#ifdef INTERNAL
				printf("For this -N%d option, only traffic types R, W3 and W6 are supported\n", NUMA_configured);
#else
				printf("For this -N%d option, only traffic types R, and W3 are supported\n", NUMA_configured);
#endif
				app_exit(EINVAL, NULL);
			}
			break;
	}
	
}

int generate_random_number() 
{
#if defined(LINUX) || defined(__APPLE__)
	return rand();
#else
	// in windows rand() function only returns a max of 32K. SO, we will use h.w random generator if it is available
	int number;
	if (hwrand_available) {
	__asm {
		xor rax, rax
		retry:
			rdrand eax
			jc gotone
			pause
			jmp retry
		gotone:
			and eax, 07fffffffh ; lose bit 31 so it is not a negative number
			mov number, eax
		}
		return number;
	}
	else
		return rand();
#endif
}

// Get free memory available on any numa node
// Currently only supported on Linux
__int64 freemem_onnode(int node)
{ 
#ifdef LINUX 
	long size = -1;
	FILE *fp; 
	char fname[128], str[256];
    regex_t re;
    regmatch_t pm[3];
    
	snprintf(fname, sizeof (fname), "/sys/devices/system/node/node%d/meminfo", node); 
	fp = fopen(fname, "r");
	if (fp == NULL) return -1;
    if (regcomp(&re,"MemFree:\\s+([0-9]+)\\s+kB", REG_EXTENDED | REG_ICASE) !=0) {
		printf("regcomp error\n .. Exiting\n");
		fclose(fp);
		app_exit(EINVAL, NULL);
	}

	else {
		while(fgets(str, sizeof(str), fp)) {
			if  (!regexec(&re, str, 2, &pm[0], 0)) { // match
				str[pm[1].rm_eo]=0;
				size = strtoll (&str[pm[1].rm_so], NULL, 10); // sscanf(&str[pm[1].rm_so], "%ld", &size);
				//printf("size=%ld\n", size);
			}
		}
		if (print_verbose1) printf("Free memory on node %d = %ld\n", node, size);
		fclose(fp);
	}
    return size;

#else
	// windows version is not implemented as numa_node_access is not enabled
	ULONGLONG AvailableBytes = 0;
	GetNumaAvailableMemoryNodeEx(node, &AvailableBytes);
	return AvailableBytes;
#endif
}

/**
 * For non inclusive cache, in order to get accurate loaded memory latency, we should
 * use span 100x then LLC size for that NUMA node. Approximately, that is about 3GB.
 * This is especially important for lower core counts SKUs (i.e. <16C.) If the core
 * count is less than DEFAULT_BUF_SIZE_BANDWIDTH_THREAD, then we still use
 * DEFAULT_BUF_SIZE_BANDWIDTH_THREAD.
 */
__int64 get_fraction_of_3GB_buf_size_per_numa_node(int os_core_id)
{
	int per_thread_buf_size;
	
	// If numa is not enabled, this won't work. So, check for that
	if (num_numa_nodes == 0) return DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
	int numa_id = os_map[os_core_id].numa_node_id;
	int threads = numanode_cpu_map_ctr[numa_id]; //number of cores in this numa node.
	if (threads <= 0) // node that contains only memory and not any cpu resources
		per_thread_buf_size = 3000000;
	else
		per_thread_buf_size = 3000000 / threads;
	if (print_verbose1) printf("%s: 3000000 / %d = %d\n", __func__, threads, per_thread_buf_size);
	return per_thread_buf_size > DEFAULT_BUF_SIZE_BANDWIDTH_THREAD ? per_thread_buf_size : DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
}

// Calculate the maximum buffer size that can be used
// based on # of threads that are allocating memory and available memory on the node
__int64 get_safe_buf_size(int numthd, int nodeid) 
{
	__int64 freemem, bufskb, min_bufskb;
	int per_thread_buf_size;
	
	freemem = freemem_onnode(nodeid);
	if (freemem == -1) {
		printf("Unable to read free memory on node %d\n", nodeid);
		app_exit(EACCES, NULL);
	}
	if (freemem < 0) return 0; // should not happen
	// at least leave 1GB memory free on this node and try to allocate the remaining
    // freemem is in KB unit. We are dividing by 2*threads as we will allocate 2x the buffer
    // in case both reads and writes are included. The same size buffer is allocated once for reads and once for writes
	if (freemem > 1000000UL && numthd != 0)
		bufskb = (freemem - 1000000UL) / (2*(__int64)numthd);
	else bufskb = buffer_size;
	if (b_provided) {
		if (buffer_size < bufskb) return buffer_size;
		if (print_verbose1) printf("get_safe_buf_size(): setting buf_size to %ld KB on node %d\n", (unsigned long) bufskb, nodeid);
		return bufskb;
	}
	min_bufskb = DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
	if (num_numa_nodes != 0) { // Make sure to use at least 3GB total buffer size as long as there is at least that much free memory
		if (numanode_cpu_map_ctr[nodeid] <= 0) // node that contains only memory and not any cpu resources
			per_thread_buf_size = 3000000;
		else
			per_thread_buf_size = 3000000 / numanode_cpu_map_ctr[nodeid];
		min_bufskb = (min_bufskb > per_thread_buf_size) ? min_bufskb : per_thread_buf_size; 
	}
	bufskb  = (bufskb > min_bufskb ) ? min_bufskb: bufskb;
	if (print_verbose1) printf("get_safe_buf_size(): setting buf_size to %ld KB on node %d\n", (unsigned long) bufskb, nodeid);
	//printf("bufz = %ld on nodeid=%d\n", bufskb, nodeid);
	return bufskb;
}

int get_buf_size_for_latency_thread()
{
	// For SKX with non-inclusive cache, we need to have a really large buffer so nothing gets cached in L3
	// So use 2GB as buffer size. For others, 200MB is sufficient
	//ALderlake has GLC core which has NI cache as well
	if (is_non_inclusive_cache_with_dbp_enabled() || is_alderlake())
		return NI_CACHE_BUF_SIZE_LATENCY_THREAD;
	else
		return DEFAULT_BUF_SIZE_LATENCY_THREAD;
	
}

#if defined(LINUX) || defined(__APPLE__)
int my_strcpy_s (char *dest, int len, char *src)
{
	int i;

	if (!(dest && src)) return -1;
	for (i=0;  i < len && src[i] != 0; i++) {
		dest[i] = src[i];
	}
	dest[i] = 0;
	return 0;	
		
}

int my_strnlen_s (char *buf, int maxlen)
{
	int len=0;
	
	if (buf == NULL) return 0;
	while(len < maxlen) {
		if (*buf++ != 0) len++;
		else break;
	}
	return len;
}

int my_strcat_s (char *dest, int maxlen, char *src)
{
	int destlen, srclen, i, idx=0;
	
	if (!(dest && src)) return -1;
	srclen = my_strnlen_s (src, maxlen);
	destlen = my_strnlen_s (dest, maxlen);
	for (i=destlen; i < maxlen-1 && idx < srclen; i++, idx++) {
		dest[i] = src[idx];
		if (src[idx] == 0) break;
	}
	dest[i] = 0;
	return 0;
		
}

// Handles up to 9 strings in sscanf
int my_sscanf_9_strings(char *buf, char* str1, int len1, char *str2, int len2, char *str3, int len3, 
	char *str4, int len4, char *str5, int len5, char *str6, int len6, 
	char *str7, int len7, char *str8, int len8, char *str9, int len9)
{
	char *p, *end, *temp;
	int idx;
	char *arr[10];
	int arrlen[10];
	
	arr[0] = str1;
	arr[1] = str2;
	arr[2] = str3;
	arr[3] = str4;
	arr[4] = str5;
	arr[5] = str6;
	arr[6] = str7;
	arr[7] = str8;
	arr[8] = str9;
	arrlen[0] = len1;
	arrlen[1] = len2;
	arrlen[2] = len3;
	arrlen[3] = len4;
	arrlen[4] = len5;
	arrlen[5] = len6;
	arrlen[6] = len7;
	arrlen[7] = len8;
	arrlen[8] = len9;

	temp = buf;
	if (temp == NULL) return 0;
	idx = 0;
	
	while(idx <= 8) {
		p = strtok_r(temp, " \r\n\t", &end);
		if (p == NULL || arr[idx] == NULL) 
			break;
		my_strcpy_s(arr[idx], arrlen[idx], p);
		idx++;
		temp = NULL;  //strotk_r needs 1st param to be NULL in 2nd and subsequent iterations
	
	}
	return idx;
	
}

int my_strscan_2_integers_with_dash (char *buf, char *fmt, int* p1, int* p2) 
{
	char *end, *p;
	char tmpbuf[1024];
	
	if (buf == NULL || p1 == NULL || p2 == NULL) return 0;
	my_strcpy_s (tmpbuf, sizeof(tmpbuf)-1, buf);
	
	p = strtok_r(tmpbuf, "-", &end);
	
	if (p == NULL || !is_number(p)) return 0;
	*p1 = atoi(p);
	if (end == NULL || !is_number(end)) return 0;
	*p2 = atoi(end);
	return 2;
	
}

#endif
