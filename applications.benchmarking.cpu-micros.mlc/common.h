// (C) 2012-2014 Intel Corporation
// // //
// // // Some common #defines and declarations to be included here
// //
//

#ifndef COMMON_INC
#define COMMON_INC

#include "cpuid.h"

#define LINE 64
#define AMD_LINE_STRIDE_SIZE 128
#define MAX_THREADS 4096
#define MAX_SOCKETS	256
#define MAX_THREADS_PER_SOCKET	512
#define DUMMY_THREAD_CORE_OFFSET 0
#define LATENCY_THREAD_CORE_OFFSET 1
#define ITERATIONS 10000000LL
#define DEFAULT_SECONDS 3
#define SOCKET_NOT_PRESENT 0
#define SOCKET_PRESENT 1
#define MAX_NUM_PATTERN_LINES      8
#define PREFETCH_MSR 0x1a4
#define PREFETCH_MSR_ZEN3 0xc0000108

// Prefetch disable bits
#define DISABLE_PREF_SPR 0x2F	 
#define DISABLE_PREF_ADL 0x2F	
#define DISABLE_PREF_RPL 0x2F	
#define DISABLE_PREF_MTL 0x6F	
#define DISABLE_PREF_CORE 0xF	
#define DISABLE_PREF_ZEN3 0x2F	
#define DISABLE_PREF_ATOM 5		// bit 0 & 2
#define DISABLE_PREF_XEON_PHI 3 //bit 0 & 1

// We independently define bits for enable and disable prefetchers. In many cases
// we may leave certain prefetcher disabled but they should never be enabled for b/w measurement
// unless it is already enabled
// e.g would be like L3 page prefetcher. We may want to leave that disabled for b/w if it is already so

// Prefetch Enable bits
#define ENABLE_PREF_SPR 0x2F	 
#define ENABLE_PREF_ADL 0x2F	
#define ENABLE_PREF_RPL 0x2F	
#define ENABLE_PREF_MTL 0x2F	
#define ENABLE_PREF_CORE 0xF	
#define ENABLE_PREF_ZEN3 0x2F	
#define ENABLE_PREF_ATOM 5		// bit 0 & 2
#define ENABLE_PREF_XEON_PHI 3 //bit 0 & 1


#define MY_ERROR -1
#define PROCMAXLEN 2048
#define MAX_RAND_SIZE 4096
#define MAX_RAND_SIZE_BW_GEN 4096
#define SMI_COUNTS_MSR 0x34
#define PATTERN_LINE_LENGTH 200
// default buffer sizes
#define DEFAULT_BUF_SIZE_LATENCY_THREAD (200LL*1024LL)
// Buffer size for latency thread on SKX -needs to be larger to beat DBP caching in L3 - 2GB
#define NI_CACHE_BUF_SIZE_LATENCY_THREAD (2000LL*1024LL)

// we need to have lower value for bw generation thread as we may have only
// limited memory on a system
#define DEFAULT_BUF_SIZE_BANDWIDTH_THREAD (100LL*1024LL)

// source location types
#define LOCAL_MEM	1
#define NUMA_NODE	2
#define NV_RAM	3

// Cache levels
#define L1I_CACHE 0
#define L1D_CACHE 1
#define L2_CACHE 2
#define L3_CACHE 3

// bandwidth types
#define MAX_INJECTION_BW	0
#define MAX_BW_AT_ANY_INJECTION	1

// Variables

struct _params {
	__int64 total_xfer;
	__int64 total_time;
	__int64 buf_len;
	__int64 buf_size;
	__int64 chksum_value;
	char* local_buf;
	char* local_buf2; //2nd address stream
	char* remote_buf;
	char* write_buf;
	char* write_buf2;
	char* src_location_path;
	char* src_location_path_2;
	int local_id;
	int remote_id;
	int rw_mix;
	int randombw;
	int src_location_type;
	int src_location_id;
	int src_location_type_2;
	int src_location_id_2;
	int per_thread_delay;
	int address_stream_mix_ratio;
	int use_clflush;
};
extern int peak_bandwidth_mode;
extern int bandwidth_matrix_mode;
extern int loaded_latency_mode;
extern int latency_matrix_mode;
extern int idle_latency_mode;
extern int status;
extern int Init_Set;
extern int latency_thread_run_cpuid;
extern int latency_core;
extern int err1;
extern int latency_thread_alloc_cpuid;
extern int latency_thread_alloc_cpuid_specified;
extern int measure_latency;
extern int genfile_provided;
extern int perthreadfile_provided;
extern int NUMA_configured ;
extern int RDONLY_load;
extern int *rand_array;
extern int *thruput_array;
extern int *thruput_array_128B;
extern int *thruput_array_256B;
extern int Random_Access;
extern int random_access_bw_thread;
extern int max_rand_size;
extern int max_rand_size_bw_gen;
extern int rw_mix;
extern int thruput_only;
extern int ntw_partial;
extern int print_per_thread_throughput;
extern int no_affinity;
extern int atleast_one_param_seen;
extern int CPU_Affinity;
extern int CPU_Affinity_user_selected;
extern int Init_Affinity;
extern int memory_node;
extern int os_cpu_count;
extern int print_verbose1;
extern int matrix_mode;
extern int matrix_mode1;
extern int num_nodes;
extern int num_numa_nodes;
extern int	LineSize;
extern int	LineSize_LatThd;
extern int	numcpus;
extern int	buffer_size;
extern int time_in_seconds;
extern int lp_enabled;
extern int lp_1GB_enabled;
extern int one_iteration_only;
extern long num_iterations;
extern UINT64 Run_Time;
extern UINT64 cval;
extern int need_to_restore_pref;
extern int delay_time;
extern int tsc_freq;
extern int use_1tpc_bandwidth;
extern int numa_node_access_enabled;
extern double NsecClk;
extern int volatile stop_busyloop_threads;
extern int allcore_latency;
extern int linesize_powerof2bits;
extern int force_clflush_all;
extern int force_clflush_pmem;
extern int burst_size;
extern int verify_data_on_read;
extern UINT64 check_sum_value;
extern int cpu_supports_avx512;
extern int cpu_supports_avx2;
extern int rfos_converted_to_itom;
extern int delay_array[];
extern char pcpu_str[];
extern int sockets[];
extern int cpu_list[];
extern int numa_array[];
extern cache_info_t cache_info[];
extern int** pkg_map;
extern int* pkg_map_ctr;
extern int** pkg_map_1tpc;
extern int* pkg_map_1tpc_ctr;
extern int** pkg_map_reserved;
extern int specify_mem_by_node;
extern int n_sockets;
extern char* mmap_files_dir;
extern size_t mmap_files_dir_len;
//extern __int64 buf_len;
extern int pmem_only;
extern int mask_cpus_less_than_7; 
extern char pbuf[][PATTERN_LINE_LENGTH];
extern int pattern_lines_present;
extern char perthreadfile[];
extern int traffic_pattern[];
extern int randombw[];
extern int buf_sizes[];
extern int is_persistent[];
extern int is_persistent_2[];
extern int remote_source[];
extern int remote_source_2[];
extern int per_thread_delay[];
extern char* sxp_path[];
extern char* sxp_path_2[];
extern int address_stream_mix_ratio[];
extern char user_cpulist[];
extern char genfile[], dbuf[], numafile[];
extern char pattern_file_name[];
extern int pattern_4bytes[]; // 16 4-byte lines in each cacheline

extern int** numanode_cpu_map_reserved;
extern int** numanode_cpu_map;
extern int* numanode_cpu_map_ctr;
extern int* numanode_cpu_map_1tpc_ctr;
extern int** numanode_cpu_map_1tpc;
extern long volatile start_load, stop_load;

extern uint64_t* disable_pref_msr_values;
extern uint64_t disable_pref_msr_mask;
extern uint64_t* saved_msr_values;


extern int pmem_alloc_requested;
extern int Numa_file_provided;
extern int pattern_len;
extern int pattern_lines_present;
extern int total_cpus_present;
extern int pattern_file_provided;
extern int total_cpus_present;
extern int pattern_len;
extern int use_avx256;
extern int use_avx512;
extern int remote_mem_alloc;
extern int partial_cache_line_access;
extern int volatile numThruputThreads;
extern UINT64 chksum_value;
extern int serialize_mem_initialization;
extern int share_buffers_across_all_threads;
extern FILE *OF,*fp;

#ifdef __APPLE__
#include <IOKit/IOKitLib.h>
extern io_connect_t myConnect;
#endif

// Function declarations

extern void Usage(const char* error);
extern void Runtime_exit(const char* error);
extern void app_exit(unsigned int errcode, const char* error);
extern int write_int_to_file(char file_path[], int offset, int value);
extern int check_file_exists(char file_path[], char mode[]);
extern int read_int_from_file(char file_path[], int offset);
extern void check_processor_supported();
extern int latency();
extern void measure_bandwidth(int type);
extern void measure_bandwidth_matrix();
extern void measure_loaded_latencies();
extern void CreatePstateThreadsOnOtherCores(char* str);
extern void CreatePstateThreadsOnAllSockets(int socket_to_omit);
extern void do_dependent_read_one_iteration_only(UINT64* a, UINT64 nLines);
extern void c2c_latency_main(char matrix_mode);
extern void populate_cache_patterns();
extern int read_pattern_file(char* filename);
extern void change_traffic(int traffic_type);
extern void change_bandwidth(int random);
extern void change_bufsize(int buf_size);
extern char *GrabMemory(UINT64 len, int src_location_type, int nodeid, char*sxp_path);
extern char *GrabMemoryFromNumaNode(UINT64 len, int nodeid);
extern char *GrabMemoryFromNvRam(UINT64 len, char* sxp_path);
extern char *GrabMemoryFromLocal(UINT64 len, int nodeid);
extern void get_rand(int *rand_array, int max_size);
extern void get_rand_bw_thread(int *rand_array, int max_size);
extern void get_rand_bw_thread_128B(int *rand_array, int max_size);
extern void get_rand_bw_thread_256B(int *rand_array, int max_size);
extern UINT64* do_specified_dependent_read(UINT64* a, UINT64 iter);
extern int *do_write( __int64 cnt, int* a, __int64 stride);
extern void do_nops(__int64 a);
extern void write_pattern_into_buffer(char* buf, __int64 len);
extern int is_avx512_enabled();
extern int is_avx_enabled();
extern int supports_spec_itom();
extern int supports_AMP_prefetcher();
extern int is_hw_rdrand_enabled();
extern int get_num_numa_nodes();
extern char* alloc_mem_onnode(UINT64 len, int nodeid);
extern void build_numa_topology();
extern void initialize_perthread_defaults();
extern int get_socketid_of_numanode(int nodeid);
extern __int64 freemem_onnode(int node);
extern __int64 get_fraction_of_3GB_buf_size_per_numa_node(int os_core_id);
extern __int64 get_safe_buf_size(int numthd, int nodeid);
extern void parse_user_cpulist();
extern void run_all_core_latency_measurement();
extern int powerof2bits(int num);
extern int check_if_valid_W_options(int rw_mix);
extern void check_for_supported_traffic_types();
extern int check_for_valid_chars_in_cpu_range(char *s);
extern int is_non_inclusive_cache_with_dbp_enabled();
extern int get_buf_size_for_latency_thread();
extern void StartLoading(int type);
extern int read_msr_without_fail(unsigned int address, UINT64* value, int latency_core);
extern int atoi_check(char *buf);
extern int is_number(char *c);
extern void  check_for_null_parameter(char b, char arg);
extern void  check_no_option_value(char b, char arg);
extern __int64 getActualLenToAllocate(__int64 len);
extern char* GetMemory(UINT64 len);
extern int is_alderlake();

extern UINT64 compute_checksum_avx512(char *buf, __int64 buf_len, char *tmpbuf);
extern UINT64 compute_checksum_avx2(char *buf, __int64 buf_len, char *tmpbuf);
extern UINT64 compute_checksum_sse2(char *buf, __int64 buf_len, char *tmpbuf);
extern UINT64 compute_checksum_orig(char *buf, __int64 buf_len);
extern UINT64 compute_checksum_from_64B(char *tmpbuf);
extern UINT64 compute_checksum_for_64B(char *tmp);
extern UINT64 chksum_value;
extern uint64_t get_hwpref_enable_mask();
extern uint64_t get_hwpref_disable_mask();

extern int a_provided;
extern int A_provided;
extern int b_provided;
extern int B_provided;
extern int c_provided;
extern int C_provided; //TODO: Add to chcek list for all modes
extern int d_provided;
extern int D_provided;
extern int e_provided;
extern int E_provided; //TODO: Add to checklist for all modes
extern int f_provided; //TODO: Add to checklist for all modes
extern int g_provided;
extern int G_provided; //TODO: Add to checklist for all modes
extern int H_provided;
extern int i_provided;
extern int I_provided;
extern int j_provided;
extern int k_provided;
extern int K_provided;
extern int l_provided;
extern int L_provided;
extern int m_provided;
extern int M_provided;
extern int n_provided;
extern int N_provided;
extern int o_provided;
extern int p_provided;
extern int P_provided;
extern int Q_provided;
extern int q_provided;
extern int r_provided;
extern int R_provided;
extern int S_provided;
extern int s_provided;
extern int t_provided;
extern int T_provided;
extern int u_provided;
extern int U_provided;
extern int v_provided;
extern int w_provided; //TODO: Add to checklist for all modes
extern int W_provided;
extern int O_provided;
extern int x_provided;
extern int X_provided;
extern int Y_provided;
extern int z_provided;
extern int Z_provided;
extern int hwrand_available;
extern int AMD_cpu;
extern int AMD_zen3;
extern int per_thread_delay_specified;
extern int zero_out_memory;

#if defined(LINUX) || defined(__APPLE__)
extern pthread_mutex_t AllocMutex;
extern pthread_cond_t AllocCond;
extern pthread_mutex_t g_ExitMutex;
extern pthread_cond_t g_ExitCond;
#endif

#ifdef LINUX
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

// Security Development Life cycle (SDL process) bans several C functions related to string processing. 
// Windows provides safe alternative functions (with _s ending). But there is no such equivalent functions
// in C. So, we are forced to write our own.
// Since the sscanf is variable length, we are providing just the size that we care about

#define strcpy_s(a, b, c) my_strcpy_s(a, b, c)
#define strcat_s(a, b, c) my_strcat_s(a, b, c)
#define strnlen_s(a, b) my_strnlen_s(a, b)
 
extern int my_strcpy_s (char *dest, int len, char *src);
extern int my_strnlen_s (char *buf, int maxlen);
extern int my_strcat_s (char *dest, int maxlen, char *src);
extern int my_sscanf_9_strings(char *buf, char* str1, int len1, char *str2, int len2, char *str3, int len3, 
	char *str4, int len4, char *str5, int len5, char *str6, int len6, 
	char *str7, int len7, char *str8, int len8, char *str9, int len9);

extern int my_strscan_2_integers_with_dash (char *buf, char *fmt, int* p1, int* p2) ;

struct __smi_struct {
	UINT64 smi_init_count;
	UINT64 smi_end_count;
	UINT64 init_tsc;
	UINT64 end_tsc;
};
extern struct __smi_struct smi_data;

#endif

typedef enum NumberFormat {
	DefaultE = 0,
	DecimalE = 0,
	KiloE,
	MegE,
	GigE
}NumberFormatT;

__int64 AdvancedAtoI(char* input, NumberFormatT *pFmt);

int AdvancedAtoIinKB(char* input);
#if defined(LINUX) || defined(__APPLE__)
#define THE_TSC rdtsc()
#else
#define THE_TSC __rdtsc()
#endif

#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0x40000 /* arch specific */
#endif

#define MAP_HUGE_SHIFT  26
#define MAP_HUGE_MASK   0x3f
#define MAP_HUGE_2MB    (21 << MAP_HUGE_SHIFT)
#define MAP_HUGE_1GB    (30 << MAP_HUGE_SHIFT)

#if defined(LINUX) || defined(__APPLE__)
extern int BindToCpu(int cpu_num);
/*
the following rdtsc come from Lu, Hongjiu
*/
#ifdef __x86_64__
static inline unsigned long
rdtsc()
{
	unsigned long var;
	unsigned int hi, lo;
	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	var = ((unsigned long long int) hi << 32) | lo;
	return var;
}
#else
static inline unsigned long long
rdtsc()
{
	unsigned long long var;
	asm volatile("rdtsc" : "=A"(var));
	return var;
}
#endif
#endif

#endif
