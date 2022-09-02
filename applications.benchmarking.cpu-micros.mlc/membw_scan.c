// Copyright (c) 2021, Intel Corporation
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#define _GNU_SOURCE
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
#include <sys/time.h>
#include <sys/types.h>
#include <regex.h>

typedef long int __int64;
typedef unsigned long int __uint64;

#define MAX_THREADS 128
#define MAX_1GB_PAGES 4096
#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x40000 /* arch specific */
#endif
#define MAP_HUGE_SHIFT  26
#define MAP_HUGE_MASK   0x3f
#define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)
#define MAP_HUGE_1GB    (30 << MAP_HUGE_SHIFT)
#define ONE_GB    (1LL*1024LL*1024LL*1024LL)
#define MAX_BW_BUCKETS	1000

#define PAGE_4KB	0
#define PAGE_2MB	1
#define PAGE_1GB	2

extern double NsecClk;
extern int lp_1GB_enabled;
static __uint64 ClksPerSec;

static int channel_num=0, use_rfo=0; // will exercise NT writes by default
static int num_threads=16;
static int pagemap_fd=-1;
static __int64 cnt=0, loops=0;
static unsigned int num_lines=0;
static unsigned int num_1gb_pages=0;
static __int64 len, bufsize=0;
static int volatile start_flag=0;
static int num_iterations = 10;
static int cpu_node=0, memory_node=0;
static int num_bw_buckets=0;
static int bucket_granularity=5000;
static int cpulist[512];
static int num_cpus_in_node=1;
static int total_allocated_GB=0;
static int old_gb_pages=-1;
static int root_access=1;
static int can_get_phys_addr=1;

static __int64 freemem = 0;
struct _mem_stats {
	__int64 len;
	__int64 cycles;
	char *vaddr;
	int id;
} mem_stats[MAX_THREADS];

struct _addr_allocated {
	char *vaddr;
	unsigned long paddr;
	__int64 len;
	int page_size;
	__int64 bw_mbps;
} addr_allocated[MAX_1GB_PAGES];

static int bw_bucket[MAX_BW_BUCKETS];

static __int64 tmp_idx, paddr_delta, vaddr_delta;
static pthread_t td[MAX_THREADS];
static char path[1024];


static void read_only_to_selected_lines(char **vaddr_match, __int64 lines_matched);
static void rfo_to_selected_lines(char **vaddr_match, __int64 lines_matched);
static void ntw_to_selected_lines(char **vaddr_match, __int64 lines_matched);
static void partial_ntw_to_selected_lines(char **vaddr_match, __int64 lines_matched);
static void clflush_selected_lines(char **vaddr, __int64 lines);
static unsigned long get_physaddr(void *vaddr);
static unsigned int mem_access_thread(void* p);
static char *get_4kb_pages(__int64 sz, int nodeid);

int my_strscan (char *buf, char *fmt, int* p1, int* p2) ;
int memread_lines(char *addr, __int64 len);
static void busy_loop(__int64 cycles);
int parse_cpulist_in_node(int cpu_node, int *list);
static int read_line_from_file(char file_path[], char *buf);
static void exit_handler(int s);
static void restore_original_gb_pages(int old_gb_pages);

extern int my_strscan_2_integers_with_dash (char *buf, char *fmt, int* p1, int* p2);
extern int my_strcpy_s (char *dest, int len, char *src);
extern int read_int_from_file(char file_path[], int offset);
extern void Calibrate(__uint64 *ClksPerSec);
extern __int64 freemem_onnode(int node);
extern int write_int_to_file(char file_path[], int offset, int value);
extern int BindToCpu(int cpu_num);
extern char* alloc_mem_onnode(__int64 len, int nodeid);

extern int is_number(char *c);
static inline unsigned long
rdtsc()
{
	unsigned long var;
	unsigned int hi, lo;
	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	var = ((unsigned long long int) hi << 32) | lo;
	return var;
}

static char *get_1gb_page(int nodeid)
{
	char *buf;
	__int64 page1g=1*1024*1024*1024LL;
	
	lp_1GB_enabled = 1;
	buf = alloc_mem_onnode (page1g, nodeid);
	if (buf == NULL) { printf("unable to allocate 1GB page .. exiting\n"); exit(1); }
	buf[0] = '1'; // just write to one byte so the entire 1GB page is physically allocated
	lp_1GB_enabled = 0;
	
	return buf;
}

static char *get_4kb_pages(__int64 sz, int nodeid)
{
	char *buf;
	__int64 i;

	buf = alloc_mem_onnode (sz, nodeid);
	if (buf == NULL) { printf("unable to allocate 1GB page .. exiting\n"); exit(1); }
	// touch the memory (one byte in each page) so they get physical page backing
	for(i=0; i < sz; i+=4096) {
		buf[i] = i;
	}
	return buf;
}

int membw_scan(int cN, int mN, int nT, int bG, int nI)
{
	char *buf, *vaddr, *base_vaddr;
	int i, status, lines_matched=0;
	__int64 cnt, paddr, lines_accessed=0, rnum;
	
	cpu_node=cN;
	num_threads=nT;
	bucket_granularity=bG;
	num_iterations=nI;	
	memory_node = mN;

	num_cpus_in_node = parse_cpulist_in_node(cpu_node, cpulist);
	// if user has not specified # of threads, use all threads available in that node
	if (num_threads == 0) num_threads = num_cpus_in_node;
	// if user specified too many threads, only use the threads on this node
	if (num_threads > num_cpus_in_node) num_threads = num_cpus_in_node; 
	
	printf("Running memory bandwidth scan using %d threads on numa node %d accessing memory on numa node %d\n",
		num_threads, cpu_node, memory_node);
	// We will assume cpus and memory are on the same node
	// File handle for VA to PA translations
	if (pagemap_fd == -1) {
		pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
	}
	// Just mmap a small buffer and check whether we can get VA->PA translation
	buf = get_4kb_pages(4096, memory_node);
	memset(buf, 0, 4*1024);
	if (get_physaddr(buf) == 0) { // check whether we can read phys addr as root access is needed
		//printf("Can't read phys address\n");
		can_get_phys_addr = 0;
	}
	// release the buffer
	munmap(buf,4096);
	
	BindToCpu(cpulist[0]); // Use the first cpu in this numa node for the main thread

	snprintf(path, sizeof(path)-1, "/sys/devices/system/node/node%d/hugepages/hugepages-1048576kB/free_hugepages", 
			memory_node);
	num_1gb_pages = read_int_from_file(path, 0);
	if (num_1gb_pages == -1) {
		// don't have permission to read the file. so can't use 1GB pages
		root_access = 0;
	}
	//snprintf(path, sizeof(path)-1, "/sys/devices/system/node/node%d/hugepages/hugepages-1048576kB/nr_hugepages", memory_node);
	//old_gb_pages = read_int_from_file(path, 0);;
	
	else {
		// we need to save the old value so it can be restored on exit or cntrl-c
		snprintf(path, sizeof(path)-1, "/sys/devices/system/node/node%d/hugepages/hugepages-1048576kB/nr_hugepages", memory_node);
		old_gb_pages = read_int_from_file(path, 0);;
		// setup event handler
		if (signal(SIGINT, exit_handler) == SIG_ERR ||
		signal(SIGTERM, exit_handler) == SIG_ERR ||
		signal(SIGQUIT, exit_handler) == SIG_ERR ||
		signal(SIGTSTP, exit_handler) == SIG_ERR ||
		signal(SIGSEGV, exit_handler) == SIG_ERR) {
			printf("\nError registering signal handler!\n");
		}
		write_int_to_file(path, 0, 12000); // write a very big value (12 TB) so we can get max GB pages allocated
		// read the actual allocated pages
		num_1gb_pages = read_int_from_file(path, 0);
		printf("Reserved %d 1GB pages\n", num_1gb_pages);
	}

	if (num_1gb_pages > 0)
		printf("Now allocating %d 1GB pages. This may take several minutes..\n", num_1gb_pages);
	for (i=0; i < num_1gb_pages; i++)
	{
		char *addr;
		addr = (char*)get_1gb_page(memory_node);
		if (addr == NULL) {
			printf("failed when allocating 1GB page# %d\n", i+1);
			exit(1);
		}
		addr_allocated[total_allocated_GB].vaddr = addr;
		addr_allocated[total_allocated_GB].len = 1*ONE_GB;
		addr_allocated[total_allocated_GB].page_size = PAGE_1GB;
		addr_allocated[total_allocated_GB].paddr = get_physaddr(addr);
		//if (((i+1)%20) == 0)
		//	printf("Allocated %3d pages\r", i+1);
		total_allocated_GB++;
		//fflush(stdout);
	}
	//total_allocated_GB += num_1gb_pages;
	if (num_1gb_pages > 0) 
		printf("1GB page allocation completed\n");
	
	freemem = freemem_onnode(memory_node); //in KB
	if (freemem >= 2000LL*1000LL) // leave at least 2GB free memory
		printf("Allocating remaining %ld KB memory in 4KB pages\n", freemem);
	while (freemem >= 2000LL*1000LL) { // keep at least 2GB free after this allocation
		char *addr;
		addr = (char*)get_4kb_pages(ONE_GB, memory_node);
		if (addr == NULL) {
			printf("4KB page allocation failed at %d allocation\n", i+1);
			exit(1);
		}
		addr_allocated[total_allocated_GB].vaddr = addr;
		addr_allocated[total_allocated_GB].len = 1*ONE_GB;
		addr_allocated[total_allocated_GB].page_size = PAGE_4KB;
		// as 4KB pages are not contiguous if we allocate 1GB, no point in getting phys addr
		// but this will provide phys address of the first 4KB page in that region
		addr_allocated[total_allocated_GB].paddr = get_physaddr(addr);
		total_allocated_GB++;
		freemem = freemem_onnode(memory_node); //in KB
	}
	printf("Totally %d GB memory allocated in 4KB+1GB pages on NUMA node %d\n", total_allocated_GB, memory_node);
	printf("Measuring memory bandwidth for each of those 1GB memory regions..\n");
	Calibrate(&ClksPerSec);
	Calibrate(&ClksPerSec);

	int pg;
	// compute per thread allocationm of the 1GB buffer
	
	__int64 per_thread_len, num_1KB_blocks;
	num_1KB_blocks = (1024LL*1024LL) / num_threads;
	per_thread_len = num_1KB_blocks * 1024LL;
	//printf("pre_thd blk=%ld, len=%ld, delta=%ld\n", num_1KB_blocks, per_thread_len, ONE_GB - (per_thread_len*num_threads));
	for (pg=0; pg < total_allocated_GB; pg++) {
		__int64 total_cycles=0, total_bytes=0, avg_cycles, mbps;
		start_flag = 0;
		for (i=0; i < num_threads; i++) {
			__int64 len = addr_allocated[pg].len;
			char *addr = addr_allocated[pg].vaddr;
			mem_stats[i].id = cpulist[i+1];
			mem_stats[i].len = per_thread_len; //len/num_threads;  divide equally among all threads
			mem_stats[i].vaddr = addr + i*per_thread_len; //i*(len/num_threads); // divide equally among all threads
			pthread_create(&td[i], NULL, (void*)mem_access_thread, (void*)&mem_stats[i]);
		}
		// keep this main thread looping for ~50msec to give time for other threads to launch and be ready to go off at the same time
		__int64 msec50 = (__int64)((double)50000000LL / NsecClk);
		busy_loop(msec50);
		start_flag=1;
		// wait for all threads to complete their mem allocations
		for (i=0; i < num_threads; i++) {
			pthread_join(td[i], NULL);
		}
		for (i=0; i < num_threads; i++) {
			double mbps;
			total_cycles += mem_stats[i].cycles;
			total_bytes += mem_stats[i].len;
			mbps = ((double) mem_stats[i].len / (NsecClk * (double) mem_stats[i].cycles/(double)1000000000LL))/1000000LL;
			//printf("len=%ld, cycles=%ld, mbps = %.2f\n", mem_stats[i].len, mem_stats[i].cycles, mbps);
		}
		total_bytes = num_iterations * total_bytes;
		avg_cycles = total_cycles / num_threads;
		mbps = ((double)total_bytes / (NsecClk * (double) avg_cycles/(double)1000000000LL))/1000000LL;
		addr_allocated[pg].bw_mbps = mbps;
		//printf("page# %d w/ addr %d @ %ld MBps\n", pg, (int)(addr_allocated[pg].paddr >> 30), mbps);
		
	}
	printf("\nHistogram report of BW in MB/sec across each 1GB region on NUMA node %d\n", memory_node);
	printf("BW_range(MB/sec)\t#_of_1GB_regions\n");
	printf("----------------\t----------------\n");
	// Populate the b/w buckets
	for (pg=0; pg < total_allocated_GB; pg++) {
		bw_bucket[addr_allocated[pg].bw_mbps/bucket_granularity]++;
	}
	for (i=0; i < MAX_BW_BUCKETS; i++) {
		if (bw_bucket[i] != 0) {
			printf("[%d-%d]\t%d\n", (i*bucket_granularity), ((i+1)*bucket_granularity)-1, bw_bucket[i]);
		}
	}
	if (!can_get_phys_addr) {
		printf("\nVirtual to Physical address translation is not available possibly due to no root access privilige\n");
		printf("Values in phys_addr column below are not valid because of that\n");
	}
	if (num_1gb_pages) {
		printf("\nDetailed BW report for each 1GB region allocated as contiguous 1GB page on NUMA node %d\n", memory_node);
		printf("phys_addr\tGBaligned_page#\tMB/sec\n");
		printf("---------\t---------------\t------\n");
		for (pg=0; pg < total_allocated_GB; pg++) {
			if (addr_allocated[pg].page_size == PAGE_1GB)
				printf("0x%lx\t%d\t%ld\n",(__uint64)addr_allocated[pg].paddr, (int)(addr_allocated[pg].paddr >> 30), addr_allocated[pg].bw_mbps);
		}
	}
	
	if ((total_allocated_GB - num_1gb_pages) > 0) {// 4KB pages were allocated
		printf("\nDetailed BW report for each 1GB region allocated as 4KB page on NUMA node %d\n", memory_node);
		printf("phys_addr\tMB/sec\n");
		printf("---------\t------\n");
		for (pg=0; pg < total_allocated_GB; pg++) {
		if (addr_allocated[pg].page_size == PAGE_4KB)
			printf("0x%lx\t%ld\n",(__uint64)addr_allocated[pg].paddr, addr_allocated[pg].bw_mbps);
		}
	}
	if (old_gb_pages >=0 && root_access)
		restore_original_gb_pages(old_gb_pages);
	return 0;
}

static void busy_loop(__int64 cycles)
{
	__int64 start, end;
	start = rdtsc();
	
	while(1) {
		end = rdtsc();
		if (end-start > cycles)
			break;
	}
}

// Each thread is accessing a portion of the 1GB region in a loop for num_iterations
static unsigned int mem_access_thread(void* p)
{
	struct _mem_stats *ms;
	char *addr;
	__int64 start, end;

	ms = (struct _mem_stats *)p;
	BindToCpu(ms->id);
	while(!start_flag);
	
	start = rdtsc();
	for (int i=0; i < num_iterations; i++) {
		memread_lines(ms->vaddr, ms->len);
	}
	end = rdtsc();
	
	ms->cycles = end-start;

	return 0;
}

int memread_lines(char *addr, __int64 len)
{
		    asm volatile (
			"xor %2, %2 \n\r"

			"loop_read%=: \n\r"
			"vmovntdqa (%0),%%ymm0 \n\r"
			"vmovntdqa 0x40(%0),%%ymm0 \n\r"
			"vmovntdqa 0x80(%0),%%ymm0 \n\r"
			"vmovntdqa 0xc0(%0),%%ymm0 \n\r"
			"vmovntdqa 0x100(%0),%%ymm0 \n\r"
			"vmovntdqa 0x140(%0),%%ymm0 \n\r"
			"vmovntdqa 0x180(%0),%%ymm0 \n\r"
			"vmovntdqa 0x1c0(%0),%%ymm0 \n\r"
			"vmovntdqa 0x200(%0),%%ymm0 \n\r"
			"vmovntdqa 0x240(%0),%%ymm0 \n\r"
			"vmovntdqa 0x280(%0),%%ymm0 \n\r"
			"vmovntdqa 0x2c0(%0),%%ymm0 \n\r"
			"vmovntdqa 0x300(%0),%%ymm0 \n\r"
			"vmovntdqa 0x340(%0),%%ymm0 \n\r"
			"vmovntdqa 0x380(%0),%%ymm0 \n\r"
			"vmovntdqa 0x3c0(%0),%%ymm0 \n\r"

			"add $0x400, %0 \n\r"
			"add $0x400, %2 \n\r"
			"cmp %1, %2 \n\r"  
			"jl loop_read%= \n\r"
						
			 :: "r"(addr),"r"(len),"r"(0L):"zmm0");
}

// Get physical address for the given virtual address
static unsigned long get_physaddr(void *vaddr)
{
	unsigned long long addr;
	int n = pread(pagemap_fd, &addr, 8, ((unsigned long long)vaddr / 4096) * 8);
	if (n != 8)
		return 0;
	if (!(addr & (1ULL<<63)))
		return 0;
	addr &= (1ULL<<54)-1;
	addr <<= 12;
	return addr + ((unsigned long long)vaddr  & (4096-1));
}

// Read a line from the specified file
static int read_line_from_file(char file_path[], char *buf)
{
	int file_value=0;
	FILE *fp;
	
	if ((fp = fopen(file_path, "r")) == NULL)
		return -1;
	if (fgets(buf, 256, fp) == NULL) {
		fclose(fp);
		return -1;
	}
	// if new line is present, replace that with NULL
	for (int i=0; buf[i] != 0 && i < 256; i++) {
		if (buf[i] == '\n') { buf [i] = 0; break; }
	}

	fclose(fp);
	return 1;
}

// Parse the cpus specified in /sys/devices/system/node/node*/cpulist
int parse_cpulist_in_node(int node, int *cpu_list)
{
	int i, num, from, to;
	char *p, *tmp;
	char buf[256], filename[256], cpus[256];
	int numcpus=0;

	snprintf(filename, sizeof(filename)-1, "/sys/devices/system/node/node%d/cpulist", node);
	read_line_from_file(filename, cpus);
	my_strcpy_s(buf, sizeof(buf)-1, cpus);
	p=buf;
	while(p) {
		tmp = p;
		p = strchr(p, ',');
		if (p != NULL)  
			*p++='\0';
		// tmp contains substr which may contain '-'
		if (strchr(tmp, '-')) { // contains '-'
			//sscanf(tmp, "%d-%d", &from, &to);
			my_strscan_2_integers_with_dash(tmp, "%d-%d", &from, &to);
			if (from > to) {
				//printf("In cpu range (-k option) field, %d > %d\n", from, to);
				exit(0);
			}
			for (i=from; i <= to; i++) {
				//if (i > os_cpu_count-1) { printf("Invalid cpu id %d on -k parameter list\n", i); exit(0); }
				cpu_list[numcpus] = i;
				numcpus++;
			}
		} else { // single number
			sscanf(tmp, "%d", &num);
			cpu_list[numcpus] = num;
			//if (num > os_cpu_count-1 || num < 0) { printf("Invalid cpu id %d on -k parameter list\n", num);exit(0); }
			numcpus++;
		}
	}
	return numcpus;
}

static void restore_original_gb_pages(int old_gb_pages)
{
	char path[1024];
	if (old_gb_pages >= 0) {
		// need to restore original value
		snprintf(path, sizeof(path)-1, "/sys/devices/system/node/node%d/hugepages/hugepages-1048576kB/nr_hugepages", memory_node);
		write_int_to_file(path, 0, old_gb_pages); 
	}
}

static void exit_handler(int s)
{

	signal(s, SIG_IGN);
	if (old_gb_pages >= 0 && root_access) {
		restore_original_gb_pages(old_gb_pages); 
	}
	exit(1);
}

int my_strscan (char *buf, char *fmt, int* p1, int* p2) 
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

