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
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <IOKit/IOKitLib.h>
#include "MLCDriverClientInterface.h"
#endif

#if !defined(LINUX) && !defined(__APPLE__)
#define strtok_r(a,b,c) strtok_s(a,b,c) 
#define strcasecmp(a,b) stricmp(a,b) 
#define strncpy(dest,src,len) strcpy_s(dest,len,src)
#endif

#include "types.h"
#include "msr.h"
#include "cpuid.h"
#include "topology.h"
//#include "version.h"
#include "exit.h"
#include "winlp.h"
#include "common.h"
#define MY_ERROR -1

#define INTEL_MLC_VERSION "Modified - Rohan/Ripan for Different Ratios"

#if !defined(LINUX) && !defined(__APPLE__)
void InitTopologyHelper();
BOOL LinearProcNumToNumaNodeGroupAffinity(WORD LinearProcNumber, NUMA_NODE_GROUP_AFFINITY *nng_affinity);
void BindToCpu(int LinearProcNumber);
BOOL GetLogicalProcInfo(WORD LinearProcNumber, APIC_ID_t * lp);
void DeleteAllSCMFiles(void);
#endif

int buffer_size = DEFAULT_BUF_SIZE_LATENCY_THREAD;

int time_in_seconds = DEFAULT_SECONDS;
int	LineSize = LINE;
int LineSize_LatThd = LINE;
int max_rand_size = MAX_RAND_SIZE;
int max_rand_size_bw_gen = MAX_RAND_SIZE_BW_GEN;

int delay_time = 0,  num_threads = 1;
int remote_mem_alloc = 0;
int serialize_mem_initialization = 0;
int share_buffers_across_all_threads = 0;
int allow_overrides = 0;
int peak_bandwidth_mode = 0;
int max_bandwidth_mode = 0;
int bandwidth_matrix_mode = 0;
int loaded_latency_mode = 0;
int latency_matrix_mode = 0;
int idle_latency_mode = 0;
int c2c_latency_mode = 0;
int membw_scan_mode = 0;
int dont_change_pref = 0;
int any_arg_specified;
int mask_cpus_less_than_7 = 0;
int atleast_7_threads_used = 1;
int ht_enabled = 1;
int ht_of_latency_thread_run_cpuid = -1;
int volatile stop_busyloop_threads = 0;
int numa_bal_file_modified = 0;
int numa_bal_file_savedvalue = -1;
int numa_scan_file_modified = 0;
int numa_scan_file_savedvalue = -1;
int numa_file_value = 0;
int numa_scan_rate = 0;
int os_cpu_count;
int matrix_mode = 1;
int num_nodes = 0;
int rfo_c2c=0;
int partial_cache_line_access = -1;
int hugepages_not_enabled=0;
int cpu_supports_avx512=0;
int cpu_supports_avx2=0;
int num_numa_nodes=0;
UINT64 chksum_value=0;


int** pkg_map;
int* pkg_map_ctr;
int** pkg_map_1tpc;
int* pkg_map_1tpc_ctr;
int** pkg_map_reserved;

#ifdef __APPLE__
io_connect_t myConnect;
#endif

int dont_force_random=0;
int specify_mem_by_node = 0;
int need_to_restore_pref = 0;
int status;
int Init_Set = 0;
int latency_thread_run_cpuid = 0;
int latency_core = 0;
int err1 = 0;
int latency_thread_alloc_cpuid = 0;
int latency_thread_alloc_cpuid_specified = 0;
int measure_latency = 1;
int genfile_provided = 0;
int	perthreadfile_provided = 0;
int NUMA_configured = 0;
int RDONLY_load = 0;
int lp_enabled = 0;
int lp_1GB_enabled = 0;
int *rand_array;
int *thruput_array;
int *thruput_array_128B;
int *thruput_array_256B;
int Random_Access = 0;
int random_access_bw_thread = 0;
int rw_mix = 1;
int thruput_only = 0;
int ntw_partial = 0;
int print_per_thread_throughput = 0;
int no_affinity = 0;
int atleast_one_param_seen = 0;
__int64 buf_len;
int CPU_Affinity = 0;
int CPU_Affinity_user_selected = -1;
int Init_Affinity = 0;
int memory_node = 0;
int a_provided = 0;
int A_provided = 0;
int b_provided = 0;
int B_provided = 0;
int c_provided = 0;
int C_provided = 0; 
int d_provided = 0;
int D_provided = 0;
int e_provided = 0;
int E_provided = 0; 
int f_provided = 0; 
int F_provided = 0; 
int g_provided = 0;
int G_provided = 0;
int h_provided = 0;
int H_provided = 0;
int i_provided = 0;
int I_provided = 0;
int j_provided = 0;
int J_provided = 0;
int k_provided = 0;
int K_provided = 0;
int l_provided = 0;
int L_provided = 0;
int m_provided = 0;
int	M_provided = 0;
int n_provided = 0;
int N_provided = 0;
int o_provided = 0;
int p_provided = 0;
int P_provided = 0;
int Q_provided = 0;
int q_provided = 0;
int r_provided = 0;
int R_provided = 0;
int S_provided = 0;
int s_provided = 0;
int t_provided = 0;
int T_provided = 0;
int u_provided = 0;
int U_provided = 0;
int v_provided = 0;
int w_provided = 0; 
int W_provided = 0;
int O_provided = 0;
int x_provided = 0;
int X_provided = 0; 
int Y_provided = 0;
int z_provided = 0;
int Z_provided = 0;
long num_iterations = 0;
int one_iteration_only = 0;
int print_verbose1 = 0;
int use_1tpc_bandwidth = 0;
int matrix_mode1 = 0;
int total_cpus_present;
int Numa_file_provided = 0;
int tsc_freq=0;
int zero_out_memory = 0;
int use_avx256 = 0;
int use_avx512 = 0;
int flush_cache = 1;
int pattern_len = 128;
int pattern_lines_present = 0;
UINT64 cval;
double NsecClk;
UINT64 Run_Time;
FILE* OF, *fp;
int rfos_converted_to_itom=0;
int numcpus = 0;
int allcore_latency=0;
int linesize_powerof2bits=0;
int AMD_cpu=0;
int AMD_zen3=0;
int per_thread_delay_specified=0;
int force_clflush_all=0;
int force_clflush_pmem=0;
int burst_size=0;
int pmem_alloc_requested=0;
int verify_data_on_read=0;
int numa_array[MAX_THREADS] = {7, 6, 5, 4, 3, 2, 1, 0};
char pbuf[MAX_NUM_PATTERN_LINES][200]; // 128 bytes per line is sufficient; but extra padding to ensure proper handling of incorrect input file
char pcpu_str[80];
char perthreadfile[1024];
int pattern_4bytes[MAX_NUM_PATTERN_LINES * 16]; // 16 4-byte lines in each cacheline
int sockets[MAX_SOCKETS]; // support up to 256 socket platform
int delay_array[4096] = { 0, 2, 8, 15, 50, 100, 200, 300, 400, 500, 700, 1000, 1300, 1700, 2500, 3500, 5000, 9000, 20000 , -1};
char user_cpulist[1024]; // cpus defined on command line as part of -k flag

#if !defined(LINUX) && !defined(__APPLE__)
// definition of SetPrivilege function
HANDLE process;
HANDLE hToken;
#endif

uint64_t* saved_msr_values;
uint64_t* disable_pref_msr_values;
uint64_t* enable_pref_msr_values;
uint64_t* buffer;
uint64_t* saved_temp;
uint64_t* enable_temp;
uint64_t* disable_temp;
uint64_t disable_pref_msr_mask;
#if defined(LINUX) || defined(__APPLE__)
struct __smi_struct smi_data;

UINT64 start_smi_count=0, end_smi_count=0;
pthread_mutex_t AllocMutex;
pthread_mutex_t getVirtAddrMutex;
pthread_cond_t AllocCond;
pthread_mutex_t g_ExitMutex;
pthread_cond_t g_ExitCond;
pthread_mutex_t serialize_buf_alloc_mutex;
#define TRUE 1
#define FALSE 0
#else
HANDLE g_hExitEvent = NULL;						// Global exit event handle
HANDLE AllocEvent = NULL;
CRITICAL_SECTION cs;
#endif

extern  int** pkg_map1;
/*
int delay_array[4096] = { 0, 1, 2, 4, 8, 10, 15, 20, 25, 30, 40, 50, 60, 70, 90, 100, 125, 150,
175, 200, 250, 300, 350, 400, 500, 600, 700, 800, 900, 1000, 1200, 1300, 1500, 1600, 1800,
2000, 2200, 2500, 3000, 3500, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 12000, 14000, 16000, 18000, 25000, -1};
*/
int cpu_list[MAX_THREADS + 1];
int pattern_file_provided;
int hittest=0;
__int64 loopsize=2000; 
extern int extra_bytes;
extern int extra_reader_thread_bytes;
extern int Write_Affinity;
extern int third_thread;
extern int third_thread;

char pattern_file_name[512];
char genfile[1024]; // -g file input with injection delays
cache_info_t cache_info[5]; //When will we have more than 5 level of cache hierarchy?


// function prototypes
void Runtime_exit(const char* error);
void exit_handler(int);
void windows_exit_handler(int);
void save_and_enable_pref();
void save_and_disable_pref();
void Calibrate(UINT64*  ClksPerSec);
void parse(char* argp);
void Calibrate(UINT64*	ClksPerSec);
void AllocateMemoryForThruputThreads(VOID);
int read_pattern_file(char* filename);
void populate_cache_patterns();
void parse_perthreadfile(char* perthreadfile);
void touch_buffer(char* buf, __int64 len);
int chk_full();
extern int is_number(char *c);
int can_modify_hw_prefetcher();
int can_modify_AMP_prefetcher(); 

extern int membw_scan(int cpu_node, int memory_node, int num_threads, int bucket_granularity, int num_iterations);

void Usage(const char* error)
{
	if (error) {
		fprintf(stderr, "%s\n\n", error);
	}

	fprintf(stderr, "\nUSAGE: Default and recommended usage is to just run the binary \"./mlc\". For specific analysis, there are six modes:\n");
	fprintf(stderr, "./mlc --latency_matrix \t\t prints a matrix of local and cross-socket memory latencies\n");
	fprintf(stderr, "./mlc --bandwidth_matrix \t prints a matrix of local and cross-socket memory bandwidths\n");
	fprintf(stderr, "./mlc --peak_injection_bandwidth \t\t prints peak memory bandwidths of the platform for various read-write ratios\n");
	fprintf(stderr, "./mlc --idle_latency \t\t prints the idle memory latency of the platform\n");
	fprintf(stderr, "./mlc --loaded_latency \t\t prints the loaded memory latency of the platform\n");
	fprintf(stderr, "./mlc --c2c_latency \t\t prints the cache to cache data transfer latency of the platform\n");
#ifdef LINUX
	fprintf(stderr, "./mlc --memory_bandwidth_scan \t\t prints memory bandwidth across entire memory for each 1GB address range\n");
#endif
	fprintf(stderr, "\n./mlc [-e] [-r] [-X] [-v]\n");
	fprintf(stderr, "\t -e h/w prefetcher state will not be modified\n");
	fprintf(stderr, "\t -r random access to get latencies - needed when prefetchers can't be disabled by MLC\n");
	fprintf(stderr, "\t -X only one thread from each core used\n");
	fprintf(stderr, "\t -v verbose printout\n");
	fprintf(stderr, "\nFor custom analysis (example: getting the latency matrix with a larger buffer size), specify flags to override the defaults. Each usage mode supports the following flags:\n");
	fprintf(stderr, "./mlc --latency_matrix [-a] [-bn] [-Dn] [-e] [-ln] [-L|-h] [-tn] [-r] [-tn] [-xn] [-X]\n");
	fprintf(stderr, "./mlc --idle_latency [-a] [-bn] [-cn] [-Dn] [-e] [-in] [-jn] [-Jn] [-ln] [-L|-h] [-r] [-tn] [-xn] [-X]\n");
	fprintf(stderr, "./mlc --loaded_latency [-bn] [-B] [-cn] [-dn] [-Dn] [-e] [-g<filename>] [-in] [-I] [-jn] [-kn|-mn] [-Kn] [-ln]\n");
	fprintf(stderr, "\t[-L|-h] [-M<filename>] [-nn] [-o<filename] [-P] [-Q] [-r] [-R] [-tn] [-T] [-u] [-U] [-v] [-Wn] [-X] [-Y] [-Z]\n");
	fprintf(stderr, "./mlc --bandwidth_matrix [-bn] [-e] [-ln]  [-L|-h] [-M<filename>] [-R] [-tn] [-Wn] [-X] [-Y] [-Z]\n");
	fprintf(stderr, "./mlc --peak_injection_bandwidth [-bn] [-e] [-kn|-mn] [-ln] [-L|-h] [-M<filename>] [-tn] [-X] [-Y] [-Z]\n");
	fprintf(stderr, "./mlc --max_bandwidth [-bn] [-e] [-kn|-mn] [-ln] [-L|-h] [-M<filename>] [-tn] [-X] [-Y] [-Z]\n");
	fprintf(stderr, "./mlc --c2c_latency [-bn] [-cn] [-Cn] [-Dn] {-En] [-Gn] [-H] [-in] [-L|-h] [-r] [-Sn] [-wn] [-xn]\n");
#ifdef LINUX
	fprintf(stderr, "./mlc --memory_bandwidth_scan [-jn] [-tn] [-Sn] [-qn] [-nn]\n");
#endif
	fprintf(stderr, "\n");
	fprintf(stderr, "  -a measure idle latencies on all available cpus; if -X is also specified, then only one thread in each core will be used\n");
	fprintf(stderr, "  -b buffer size in KiB (default=100000)\n");
	fprintf(stderr, "  -B print per-thread throughput\n");
	fprintf(stderr, "  -c affinitize the latency measurement thread to core #n\n");
	fprintf(stderr, "  -C window size in KB for c2c_latency(default=2000)\n");
	fprintf(stderr, "  -d delay cycles injected between requests to memory (default-0), higher value lowers bandwidth\n");
	fprintf(stderr, "  -D random access range for latency thread (default=4096)\n");
	fprintf(stderr, "  -e do not modify the h/w prefetcher states\n");
	fprintf(stderr, "  -E read extra n KB lines in writer thread in c2c_latency(default=0)\n");
	fprintf(stderr, "  -g <input-file-name> specify input file with delays to inject, for loaded latency measurement\n");
	fprintf(stderr, "  -G buffer size in KB for extra reads in reader thread in c2c_latency\n");
	fprintf(stderr, "  -h HugePage (1GB) are to be used (on windows only for DAX file mapping w/ 1GB page)\n");
	fprintf(stderr, "  -H Test cache to cache transfer latency for clean line transfers\n");
	fprintf(stderr, "  -i initialize memory from core #n (determines where requested memory resides)\n");
	fprintf(stderr, "  -I Check for data integrity on reads - valid only for all reads in loaded_latency mode\n");
	fprintf(stderr, "  -j initialize memory from numa node #n - if specified, this overrides -i\n");
	fprintf(stderr, "  -J specify a directory in which files for mmap will be created (by default no files are created) this option is mutually exclusive with -j\n");
	fprintf(stderr, "  -k specify a list of core# (use '-' for range e.g 3-6,9-13,19,20 \n");
	fprintf(stderr, "  -K option to control partial load/store to 64B lines\n");
	fprintf(stderr, "  -l stride length in bytes (default=64)\n");
	fprintf(stderr, "  -L large pages (2MB) are to be used (assuming they have been enabled)\n");
	fprintf(stderr, "  -m mask in hex value of CPUs to run the throughput threads for bandwidth measurement. It is recommended -k option is used instead\n");
	fprintf(stderr, "\tlatency thread always runs on core 0 and this should be excluded from the mask\n");
	fprintf(stderr, "  -M <filename> initialize the memory with specific patterns \n");
#ifdef LINUX
	fprintf(stderr, "  -n random access range for random bandwidth generation. In --memory_bandwidth_scan mode\n");
	fprintf(stderr, "  \t this specifies the numa node# where the cpu threads accessing memory will be running.\n");
#else
	fprintf(stderr, "  -n random access range for random bandwidth generation\n");
#endif
	fprintf(stderr, "  -o <input-file-name> specify input file with options for per-thread allocation\n");
	fprintf(stderr, "   \t each row should contain thread_id, read/write ratio, random/seq, buffer size in KB, DDR4/pmem, remote node id\n");
	fprintf(stderr, "  -p list of the cores separated by comma, where a thread will be run to prevent the corresponding sockets from transitioning to package sleep states\n");
#ifdef LINUX
	fprintf(stderr, "  -q # of iterations of each 1GB memory region where B/W is measured. Default is 10\n");
#endif
	fprintf(stderr, "  -P CLFLUSH used to evict stores to persistent memory\n");
	fprintf(stderr, "  -Q CLFLUSH used to evict stores to any memory\n");
	fprintf(stderr, "  -r random access reads for latency thread\n");
	fprintf(stderr, "  -R read-only load generated, if this is used, -W option should NOT be used \n");
#ifdef LINUX
	fprintf(stderr, "  -S affinitize 3rd thread in c2c_latency test to core #n. In --memory_bandwidth_scan mode\n");
	fprintf(stderr, "  \t this indicates the bandwidth range for each bucket in terms of MB/sec. Default is 5000\n");
#else
	fprintf(stderr, "  -S affinitize 3rd thread in c2c_latency test to core #n\n");
#endif
#ifdef LINUX
	fprintf(stderr, "  -t time in seconds (default=2).  In --memory_bandwidth_scan mode\n");
	fprintf(stderr, "  \t this specifies the # of cpu threads that will be used to access memory (default is all threads on that numa node)\n");
#else
	fprintf(stderr, "  -t time in seconds (default=2)\n");
#endif
	fprintf(stderr, "  -T throughput threads only, no latency measurement, -m or -k option can select all CPUs including core 0\n");
	fprintf(stderr, "  -u same buffer is shared by all load generation threads in loaded_latency tests\n");	
	fprintf(stderr, "  -U random access for bandwidth generation (default: off, only sequential access)\n");
	fprintf(stderr, "  -v print verbose output\n");
	fprintf(stderr, "  -w affinitize hit/hitm writer thread to core #n\n");
	fprintf(stderr, "  -Wn where n means\n");
	fprintf(stderr, "\t2  - 2:1 read-write ratio\n");
	fprintf(stderr, "\t3  - 3:1 read-write ratio\n");
	fprintf(stderr, "\t4  - 3:2 read-write ratio\n");
	fprintf(stderr, "\t5  - 1:1 read-write ratio\n");
	fprintf(stderr, "\t6  - 0:1 read-Non Temporal Write ratio\n");
	fprintf(stderr, "\t7  - 2:1 read-Non Temporal Write ratio\n");
	fprintf(stderr, "\t8  - 1:1 read-Non Temporal Write ratio\n");
	fprintf(stderr, "\t9  - 3:1 read-Non Temporal Write ratio\n");
	fprintf(stderr, "\t10 - 2:1 read-Non Temporal Write ratio (stream triad-like)\n");
	fprintf(stderr, "\t\t Same as -W7 but the 2 reads are from 2 different buffers\n");
	fprintf(stderr, "\t11 - 3:1 read-Write ratio (stream triad-like with RFO)\n");
	fprintf(stderr, "\t\t Same as -W3 but the 2 reads are from 2 different buffers\n");
	fprintf(stderr, "\t12 - 4:1 read-Write ratio\n");
	fprintf(stderr, "\t13 - 5:1 read-Write ratio\n");
	fprintf(stderr, "\t14 - 6:1 read-Write ratio\n");
	fprintf(stderr, "\t15 - 7:1 read-Write ratio\n");
	fprintf(stderr, "\t16 - 8:1 read-Write ratio\n");
	fprintf(stderr, "\t17 - 9:1 read-Write ratio\n");
	fprintf(stderr, "\t21 - 100%% read with 2 addr streams - valid with only -o option\n");
	fprintf(stderr, "\t23 - 3:1 read-write ratio with 2 addr streams - valid with only -o option\n");
	fprintf(stderr, "\t27 - 2:1 read-NT write with 2 addr streams - valid with only -o option\n");
	fprintf(stderr, "  -x number of iterations in #n millions - if specified, -t should not be specified (-x0 is overloaded case for 1 iteration)\n");
	fprintf(stderr, "  -X use only 1 hyper-thread per core for bandwidth measurements\n");
	fprintf(stderr, "  -Y use AVX2 32-byte load/store instructions\n");
	fprintf(stderr, "  -Z use AVX-512 64-byte load/store instructions\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Note that the tool uses the following terminology:\n'core' denotes logical core\n'socket' denotes processor socket/package/node \n'memory node' denotes a distinct memory NUMA region, identifiable using numactl APIs\n");
	app_exit(EINVAL, "Invalid Argument");
}

void print_params()
{
    printf("buffer_size=%d\n stride=%d\n rw_mix=%d\n", buffer_size, LineSize, rw_mix);
}

int atoi_check(char *buf)
{
	int num=0;
	
	if (buf == NULL) return 0;
	num = atoi(buf);
	if (num > 0x7fffffff - 1) return 0;
	else return num;
	return num;
	
}

#if defined(__APPLE__)
/**
 * InitDriver
 * Connect to the driver and such
 */
void InitDriver()
{
    io_service_t service;
    
    // Get our driver from the I/O Registry.
    service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching(kMLCDriverClassName));
    if (service) {
        
        // This call will cause the user client to be instantiated. It returns an io_connect_t handle
        // that is used for all subsequent calls to the user client.
        kern_return_t kernResult = IOServiceOpen(service, mach_task_self(), 0, &myConnect);
        
        if (kernResult != KERN_SUCCESS) {
            fprintf(stderr, "[error] IOServiceOpen returned 0x%08x\n", kernResult);
        }
        /* else {
         // This is an example of calling our user client's openUserClient method.
         kernResult = MyOpenUserClient(myConnect);
         
         if (kernResult == KERN_SUCCESS) {
         printf("MyOpenUserClient was successful.\n\n");
         }
         else {
         fprintf(stderr, "MyOpenUserClient returned 0x%08x.\n\n", kernResult);
         }
         }*/
    }
    
    // Couldn't find driver
    else {
        fprintf(stderr, "[error] No matching drivers found \"%s\".\n", kMLCDriverClassName);
    }
    
    printf("Successfully initialized the MLC driver for Mac OS X\n");
}
#endif
__int64 AdvancedAtoI(char* input, NumberFormatT *pFmt)
{
	char a2i_buf[32] = { 0 };
	__int64 ret_val = 0;
	int count=0, input_len=0;

	input_len = strnlen_s(input, sizeof(a2i_buf)-1);

	if (input_len < sizeof(a2i_buf))
		strcpy_s(a2i_buf, input_len + 1, input); // +1 to copy the NULL character

	NumberFormatT fmt = DecimalE;

	for (count = 0; count < input_len && input_len < sizeof(a2i_buf); count++)
	{
		if (a2i_buf[count] > '9')
		{
			if (a2i_buf[count] == 'G' || a2i_buf[count] == 'g')
			{

				a2i_buf[count] = 0;	//teriminate the string
				fmt = GigE;
				break;
			}

			if (a2i_buf[count] == 'M' || a2i_buf[count] == 'm')
			{
				a2i_buf[count] = 0;	//teriminate the string
				fmt = MegE;
				break;
			}

			if (a2i_buf[count] == 'K' || a2i_buf[count] == 'k')
			{
				a2i_buf[count] = 0;	//teriminate the string
				fmt = KiloE;
				break;
			}

			if (a2i_buf[count] == ' ')
			{
				a2i_buf[count] = 0;	//teriminate the string if you find space
				fmt = DecimalE;
				break;
			}

			break;
		}
	}

	if (!is_number(a2i_buf)) {
		printf("Parameter value %s is not number\n", input);
		app_exit(EINVAL, NULL);
	}

	ret_val = (__int64)atoi_check(a2i_buf);
	switch (fmt)
	{
	case GigE:
		ret_val *= 1024LL;
	case MegE:
		ret_val *= 1024LL;
	case KiloE:
		ret_val *= 1024LL;
	case DecimalE:
	default:
		break;
	}

	if (NULL != pFmt)
	{
		*pFmt = fmt;
	}

	return ret_val;
}

int AdvancedAtoIinKB(char* input)
{
	NumberFormatT Fmt=0;

	__int64 bsize = AdvancedAtoI(input, &Fmt);
	if (Fmt != DefaultE)
	{
		bsize /= 1024;		// old code assunes buffer size is in KB. So revert back to KB
	}
	if (bsize <= 0) {
		Usage("Size must be greater than 0!\n");
	}

	return (int)bsize;
}

int main(int argc, char* argv[])
{
	int i;
	int numa_temp;
	int num_modes_specified=0; // count of how many '--' options are there in the command line; should support only one
	
#ifdef INTERNAL
#ifdef LINUX
	//initialize struct to track SMI counts. This is added to monitor whether we have SMIs firing in the system
	// during normal operaion as they are not good for performace
	// Currently this is supported only in Linux as the windows driver used by MLC can't read SMI MSR
	smi_data.init_tsc=0;
	smi_data.smi_init_count=0;
	smi_data.smi_end_count=0;
#endif
#endif
	
#if !defined(LINUX) && !defined(__APPLE__)
	typedef void (*SignalHandlerPointer)(int);
	SignalHandlerPointer previousHandler;
#endif
	// Disable output buffering on stdout
	setbuf(stdout, NULL);

#if !defined(LINUX) && !defined(__APPLE__)
	InitTopologyHelper();
#endif


#ifdef INTERNAL
	printf("Intel(R) Memory Latency Checker (for Internal Use Only) - %s-internal\n", INTEL_MLC_VERSION);
#else
	printf("Intel(R) Memory Latency Checker - %s\n", INTEL_MLC_VERSION);
#endif

	check_processor_supported(); // find family and model #
#if defined(LINUX)
	total_cpus_present = sysconf(_SC_NPROCESSORS_CONF);
	os_cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(__APPLE__)
    size_t len = sizeof(int);
    sysctlbyname("hw.ncpu", &total_cpus_present, &len, NULL, 0);
    sysctlbyname("hw.activecpu", &os_cpu_count, &len, NULL, 0);
#else
	os_cpu_count = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
	total_cpus_present = os_cpu_count;
#endif
	/* Initialize File Pointer to stdout, will change if user specifies file */
	// 
	OF = stdout;
	BindToCpu(0);
	// Initialize NUMA array in reverse order
	for (i=0; i < os_cpu_count && i < MAX_THREADS; i++)
		numa_array[os_cpu_count-i-1] = i;

	// check for availability of h/w random number generator
	hwrand_available =  is_hw_rdrand_enabled();

	/* Parse Args */
	for (i = 1; i < argc; i++) {
		atleast_one_param_seen = 1;
		if (argv[i][0] != '-') { // each argument should start with a '-'. Otherwise flag as error and quit
			printf("Unsupported argument %s in command line\n", argv[i]);
			app_exit(EINVAL, NULL);
		}

		/* Check options */
		switch (argv[i][1]) {
		case '?': {
			/* Help - print usage and exit */
			Usage((char*) 0);
		}

		case 'a': {
			check_no_option_value('a', argv[i][2]);
			a_provided = 1;
			allcore_latency = 1;
			break;
		}

		case 'b': {
			check_for_null_parameter('b', argv[i][2]);
			buffer_size = AdvancedAtoIinKB(&argv[i][2]);
			b_provided = 1;
			break;
		}

		case 'p': {
			p_provided = 1;
			check_for_null_parameter('p', argv[i][2]);
			strcpy_s(pcpu_str, sizeof(pcpu_str), &argv[i][2]);
			break;
		}

		case 'c': {
			/* Affinitize to CPU n */
			check_for_null_parameter('c', argv[i][2]);
			c_provided = 1;
			CPU_Affinity = AdvancedAtoI(&argv[i][2],NULL);
			CPU_Affinity_user_selected = CPU_Affinity;
			latency_thread_run_cpuid = CPU_Affinity; // relevant when used in --loaded_latency
			if (CPU_Affinity < 0 || CPU_Affinity > (os_cpu_count - 1)) {
				printf("Minimum value for core number is 0 and maximum value is %d\n", os_cpu_count);
				printf("Core number must be in this range! \n");
				app_exit(EINVAL, NULL);
			}

			break;
		}
		case 'C': {
			/* Windows size */
			check_for_null_parameter('C', argv[i][2]);
			C_provided = 1;
			loopsize = AdvancedAtoIinKB(&argv[i][2]) * 1024;

			break;
		}
		case 'B': {
			/* Print thread-level throughput */
			check_no_option_value('B', argv[i][2]);
			print_per_thread_throughput = 1;
			B_provided = 1;
			rfo_c2c=1;
			break;
		}

		case 'd': {
			/* delay time - only one could be specified; if not specified the delay_array would be used automatically*/
			check_for_null_parameter('d', argv[i][2]);
			delay_time = AdvancedAtoI(&argv[i][2],NULL);
			delay_array[0] = delay_time;
			delay_array[1] = -1;
			d_provided = 1;
			break;
		}

		case 'D': {
			/* Random number size*/
			check_for_null_parameter('D', argv[i][2]);
			max_rand_size = (int)AdvancedAtoI(&argv[i][2],NULL);
			
#if !defined(LINUX) && !defined(__APPLE__)
			// On windows rand() range is small. so, use h/w rand generator if available */
			if(!hwrand_available && max_rand_size>32767){
				printf("h/w rand not available and max_rand_size should be between 1 and 32767\n");
				app_exit(EINVAL, NULL);
			}
#endif
			D_provided = 1;
			break;
		}

		case 'F': {
			/* Flush cache disabled - specified - when specified, clflush is not used to flush out the lines before the latency measurement starts */
			check_no_option_value('F', argv[i][2]);
			F_provided = 1;
			flush_cache = 0;
			break;
		}

		case 'g': {
			/* Input file with delay values provided */
			check_for_null_parameter('g', argv[i][2]);
			genfile_provided = 1;
			g_provided = 1;
			strcpy_s(genfile, 1024, &argv[i][2]);
			break;
		}
		
		 case 'o': {
            /* Input file with per-thread values */
            check_for_null_parameter('o', argv[i][2]);
            perthreadfile_provided = 1;
            o_provided = 1;
            strcpy_s(perthreadfile,  sizeof(perthreadfile), &argv[i][2]);
            break;
        }
		case 'G': {
			/* Extra bytes specified in KB, this amount would be read so modified data gets removed from L1-D */
			check_for_null_parameter('G', argv[i][2]);
			G_provided = 1;
			extra_reader_thread_bytes = AdvancedAtoI(&argv[i][2],NULL) * 1024;
			break;
		}

		case 'H': {
			/* When specified, perform HIT test not HITM*/
			check_no_option_value('H', argv[i][2]);
			hittest = 1;
			break;
		}

#if !defined(LINUX) && !defined(__APPLE__)
		case 'h': {
			/* When specified, HugePage(1GB) hint is set for DAX memory mapping*/
			check_no_option_value('h', argv[i][2]);
			h_provided = 1;
			lp_1GB_enabled = 1;
			break;
		}
#else
		case 'h': {
			/* When specified, HugePage(1GB) hint is set for DAX memory mapping*/
			check_no_option_value('h', argv[i][2]);
			h_provided = 1;
			lp_enabled = 1;
			lp_1GB_enabled = 1;
			break;
		}
#endif

		case 'i': {
			// Initialize memory first by binding to this CPU
			check_for_null_parameter('i', argv[i][2]);
			i_provided = 1;
			Init_Affinity = AdvancedAtoI(&argv[i][2],NULL);
			latency_thread_alloc_cpuid = Init_Affinity;
			latency_thread_alloc_cpuid_specified = 1;
			if (Init_Affinity < 0 || Init_Affinity > (os_cpu_count - 1)) {
				printf("Minimum value for i is 0 and maximum value is %d\n", os_cpu_count);
				printf("Number must be in this range! \n\n");
				app_exit(EINVAL, NULL);
			}

			break;
		}

		case 'I': {
			verify_data_on_read=1;
			break;
		}

		case 'k': {
			/* Mask specfiying the CPUs on which the throughput threads have to run */
			check_for_null_parameter('k', argv[i][2]);
			strcpy_s(user_cpulist, sizeof(user_cpulist), &argv[i][2]);
			k_provided = 1;
			break;
		}
		
		case 'K': {
			/* flag to force or not partial cache line load/store */
			check_for_null_parameter('K', argv[i][2]);
			partial_cache_line_access = AdvancedAtoI(&argv[i][2],NULL);
			break;
		}
			
		case 'l': {
			/* stride size */
			check_for_null_parameter('l', argv[i][2]);
			LineSize = AdvancedAtoI(&argv[i][2],NULL);
			l_provided = 1;
			break;
		}

		case 'L': {
			check_no_option_value('L', argv[i][2]);
#ifdef __APPLE__
            printf("*** Large Pages (2M) is not supported on Mac OSX ***\n");
#else
			/* Enable large pages for all buffers allocated */
			lp_enabled = 1;
			L_provided = 1;
#endif
			break;
		}

		case 'm': {
			/* Mask specfiying the CPUs on which the throughput threads have to run */
			check_for_null_parameter('m', argv[i][2]);
			parse(&argv[i][2]);
			m_provided = 1;
			break;
		}

		case 'j': {
#ifdef __APPLE__
            printf("*** NUMA node is not supported on Mac OSX ***\n");
#else
			/* Enable memory allocation on nodes with no CPU present or
			 * specify memory allocation by numa node */
			check_for_null_parameter('j', argv[i][2]);
			specify_mem_by_node = 1;
			if (!is_number(&argv[i][2])) {
				printf("-j parameter is not a decimal number\n");
				app_exit(EINVAL, NULL);
			}
			memory_node = AdvancedAtoI(&argv[i][2],NULL);
			j_provided = 1;
#endif
			break;
		}

		case 'J':{
			J_provided = 1;
#ifdef __APPLE__
            printf("*** NUMA node is not supported on Mac OSX ***\n");
#else
            check_for_null_parameter('J', argv[i][2]);
            mmap_files_dir_len = strnlen_s(&argv[i][2], 256);
            mmap_files_dir = (char*)malloc(mmap_files_dir_len+1);
            if (mmap_files_dir) strcpy_s(mmap_files_dir, mmap_files_dir_len + 1, &argv[i][2]);
        	pmem_only = 1;

#if !defined(LINUX) && !defined(__APPLE__)
			char pmm_vol_name[128] = { 0 };
			if(mmap_files_dir && GetVolumePathName(mmap_files_dir, pmm_vol_name,sizeof(pmm_vol_name)))
			{
				LONG FileSystemFlags = 0;
				if (GetVolumeInformation(pmm_vol_name,NULL,0,NULL,NULL,&FileSystemFlags,NULL,0))
				{
					if (FileSystemFlags & 0x20000000) //FILE_DAX_VOLUME
					{
						printf("\nPMM File Map %s . DAX Mode is ENABLED FileSystemFlags %lx \n\n", mmap_files_dir, FileSystemFlags);
					}
					else
					{
						printf("\nPMM File Map %s . CAUTION DAX Mode is *DISABLED* FileSystemFlags %lx \n\n", mmap_files_dir, FileSystemFlags);
					}
				}
				else
				{
					printf("GetVolumePathName Failed for %s ErroCode %ld", mmap_files_dir,GetLastError() );
					app_exit(EINVAL, NULL);
				}
			}
			else
			{
				if (mmap_files_dir)
					printf("GetVolumeInformation Failed for %s ErroCode %ld", mmap_files_dir, GetLastError());
				app_exit(EINVAL, NULL);
			}
#endif //only for windows

#endif
			break;
		}
			
		case 'x': {
			// number of iterations in million
			check_for_null_parameter('x', argv[i][2]);
			x_provided = 1;
			num_iterations = (UINT64)AdvancedAtoI(&argv[i][2],NULL) * (UINT64)1000000;

			if (num_iterations < 0) {
				Usage("Number of iterations should be positive!\n");
			}

			if (num_iterations == 0) { // assume you want to do only one iteration measurement for directory snoop study
				one_iteration_only = 1;
			}

			break;
		}

		case 'n': {
			 
			check_for_null_parameter('n', argv[i][2]);
			max_rand_size_bw_gen= (int)AdvancedAtoI(&argv[i][2],NULL); //atoi(&argv[i][2]);
#if !defined(LINUX) && !defined(__APPLE__)
			// windows rand() function returns only between 1 and 32767
			if(!hwrand_available && max_rand_size_bw_gen>32767){
				printf("h/w rand not available and max_rand_size_bw_gen should be between 1 and 32767\n");
				app_exit(EINVAL, NULL);
				}
#endif
			n_provided = 1;
			break;
		}
	
		case 'M': { 
			//pattern File containing data that will be used for initializing the buffers
			check_for_null_parameter('M', argv[i][2]);
			pattern_file_provided = 1;
			strcpy_s(pattern_file_name, sizeof(pattern_file_name)-1, &argv[i][2]);
			M_provided = 1;
			break;
		}

		case 'P': {
			/* generate clflushopt for all stores to pmem*/
			check_no_option_value('P', argv[i][2]);
			force_clflush_pmem = 1;
			P_provided = 1;
			break;
		}

		case 'Q': {
			/* generate clflushopt for all stores to any memory including pmem*/
			check_no_option_value('Q', argv[i][2]);
			force_clflush_all = 1;
			Q_provided = 1;
			break;
		}

		case 'q': {
			/* amount of partials with non temporal stores*/
			check_for_null_parameter('q', argv[i][2]);
			ntw_partial = AdvancedAtoI(&argv[i][2],NULL);
			q_provided = 1;
			break;
		}

		case 'r': {
			/* Random access for latency thread */
			//check_no_option_value('r', argv[i][2]);
			if (argv[i][2] == '0') dont_force_random=1;
			Random_Access = 1;
			r_provided = 1;
			break;
		}

		case 'v': {
			/* Print verbose */
			check_no_option_value('v', argv[i][2]);
			print_verbose1 = 1;
			v_provided = 1;
			break;
		}

		case 'R': {
			/* Specify read-only traffic for the throughput threads */
			check_no_option_value('R', argv[i][2]);
			RDONLY_load = 1;
			R_provided = 1;
			break;
		}

		case 'S': {
			check_for_null_parameter('S', argv[i][2]);
			third_thread  = AdvancedAtoI(&argv[i][2],NULL);
			S_provided = 1;
			break;
		}

		case 't': {
			check_for_null_parameter('t', argv[i][2]);
			time_in_seconds  = AdvancedAtoI(&argv[i][2],NULL);
			t_provided = 1;
			if (time_in_seconds <= 0) {
				Usage("Time to run must be greater than or equal to zero!\n");
			}

			break;
		}

		case 'T': {
			/* Specify this flag if no latency is desired and just raw throughput is needed; this uses all threads */
			check_no_option_value('T', argv[i][2]);
			thruput_only = 1;
			T_provided = 1;
			break;
		}

		case 'u': {
			check_no_option_value('u', argv[i][2]);
			share_buffers_across_all_threads = 1;
			u_provided = 1;
			break;
		}

		case 'U': {
			check_no_option_value('U', argv[i][2]);
			random_access_bw_thread = 1;
			U_provided = 1;
			break;
		}
		case 'w': {
			/* Bind hit/hitm writer thread to this CPU*/
			check_for_null_parameter('w', argv[i][2]);
			Write_Affinity = AdvancedAtoI(&argv[i][2],NULL);
			w_provided = 1;
			break;
		}

		case 'W': {
			/* Specify different R/W mix ratios */
			check_for_null_parameter('W', argv[i][2]);
			rw_mix = AdvancedAtoI(&argv[i][2],NULL);
			W_provided = 1;
			if (!check_if_valid_W_options(rw_mix))
				Usage("Unsupported -W option!\n");
			break;
		}

		case 'X': {
			/* specify that only 1 thread per core should be used for bandwidth measurements*/
			check_no_option_value('X', argv[i][2]);
			X_provided=1;
			use_1tpc_bandwidth=1;
			//printf("X provided!\n");	
			break;
			}

		case 'Y': {
			check_no_option_value('Y', argv[i][2]);
			use_avx256 = 1;
			Y_provided = 1;
			break;
		}

		case 'z': {
			check_no_option_value('z', argv[i][2]);
			zero_out_memory = 1;
			z_provided = 1;
			break;
		}

#ifdef AVX512_SUPPORT
		case 'Z': {
			use_avx512 = 1;
			Z_provided = 1;
			if (argv[i][2] != 0) { // -Z has a parameter value present to specify burst size for 100% reads
				burst_size = atoi_check(&argv[i][2]);
			}
			break;
		}

#endif

		case 'e': {
			check_no_option_value('e', argv[i][2]);
			dont_change_pref = 1;
			break;
		}

		case 'E': {
			check_for_null_parameter('E', argv[i][2]);
			extra_bytes = AdvancedAtoI( &argv[i][2],NULL) * 1024;
            E_provided = 1;
			break;
		}

		case '-': {
			num_modes_specified++;
			if (num_modes_specified > 1) {
				printf("Only one mode with -- option can be specified\n");
				app_exit(EINVAL, NULL);
			}
			if (strcmp(&argv[i][2], "latency_matrix") == 0) {
				latency_matrix_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "idle_latency") == 0) {
				idle_latency_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "peak_injection_bandwidth") == 0) {
				peak_bandwidth_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "bandwidth_matrix") == 0) {
				bandwidth_matrix_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "loaded_latency") == 0) {
				loaded_latency_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "memory_bandwidth_scan") == 0) {
				membw_scan_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "c2c_latency") == 0) {
				c2c_latency_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "max_bandwidth") == 0) {
				max_bandwidth_mode = 1;
				matrix_mode = 0;
				break;
			}

			if (strcmp(&argv[i][2], "peak_bandwidth") == 0) {
				fprintf(stderr, "peak_bandwidth name has been changed. Use peak_injection_bandwidth instead\n");
				app_exit(EINVAL, NULL);
				break;
			}

			fprintf(stderr, "Invalid Argument:%s\n", &argv[i][0]);
			Usage((char*) 0);
			break;
		}

		default: {
			fprintf(stderr, "Invalid Argument:%s\n", &argv[i][0]);
			Usage((char*) 0);
			break;
		}
	}
	}

	// Currently, memory_bandwidth_scan invocation is more of a hack to ensure quick release.
	// Instead of processing this in the regular flow, it is made out as a special case for now
	// We are not checking for other parameters that may be getting set
	if (membw_scan_mode) {
#ifdef LINUX
		printf("Command line parameters: ");
		for (i = 1; i < argc ; i++) {
			printf("%s ", argv[i]);
		}
		printf("\n");
		// these are the default parameters that we assume
		// We need to overload -j, -t, -S and -q to set numa node#, # of threads, bucket size and
		// iterations respectively
		int nn=0, nt=0, nb=5000, ni=10, nc=0;
			if (j_provided) nn = memory_node;  
			if (t_provided) nt = time_in_seconds; // this parameter is overload as num_threads for this case
			if (S_provided) nb = third_thread;  // this parameter is overload as bucket size for this case
			if (q_provided) ni = ntw_partial;  // this parameter is overload as iterations for this case
			if (n_provided) 
				nc = max_rand_size_bw_gen;  // this parameter is overload as cpu numa node# for this case
			else 
				nc = nn; // if node# for where cpu threads should be running, assume the same as memory node
			//membw_scan(int cpu_node, int num_threads, int bucket_granularity, int num_iterations);
			membw_scan(nc, nn, nt, nb, ni);
		exit(0);
#else
		printf("This option is only supported on Linux. Exiting..\n");
		exit(1);
#endif
	}

	// check if NUMA balancing is enabled and if it is, disable it
#ifdef LINUX

	if (check_file_exists(numa_bal_path, "rw") == 0) {
		//printf("%s does not exist\n",numa_bal_path);
		if (check_file_exists(numa_scan_path, "rw") == 0) {
			//printf("%s does not exist\n",numa_scan_path);
		} else {
			//printf("%s exists\n",numa_scan_path);
			numa_temp = read_int_from_file(numa_scan_path, 0);
			//printf("value read from %s is %d\n",numa_scan_path,numa_temp);
			numa_scan_file_savedvalue = numa_temp;
			write_int_to_file(numa_scan_path, 0, 3600000);
			//printf("written 3600000 to %s\n",numa_scan_path);
			numa_scan_file_modified = 1;
			numa_temp = read_int_from_file(numa_scan_path, 0);
			//printf("check after write:value read from %s is %d\n",numa_scan_path,numa_temp);
		}
	} else {
		//printf("%s exists\n",numa_bal_path);
		numa_temp = read_int_from_file(numa_bal_path, 0);
		//printf("value read from %s is %d\n",numa_bal_path,numa_temp);
		numa_bal_file_savedvalue = numa_temp;

		if (numa_temp != 0) {
			write_int_to_file(numa_bal_path, 0, 0);
			//printf("written 0 to %s\n",numa_bal_path);
			numa_bal_file_modified = 1;
			numa_temp = read_int_from_file(numa_bal_path, 0);
			//printf("check after write:value read from %s is %d\n",numa_bal_path,numa_temp);
		}
	}

#endif

#if defined(LINUX)

	// register exit handler for the following signals
	if (signal(SIGINT, exit_handler) == SIG_ERR ||
	        signal(SIGTERM, exit_handler) == SIG_ERR ||
	        signal(SIGQUIT, exit_handler) == SIG_ERR ||
	        signal(SIGTSTP, exit_handler) == SIG_ERR ||
	        signal(SIGSEGV, exit_handler) == SIG_ERR) {
		printf("\nError registering signal handler!\n");
	}
#elif defined(__APPLE__)
    // register exit handler for the following signals
    if (signal(SIGINT, exit_handler) == SIG_ERR ||
        signal(SIGTERM, exit_handler) == SIG_ERR ||
        signal(SIGQUIT, exit_handler) == SIG_ERR ||
        signal(SIGTSTP, exit_handler) == SIG_ERR ||
        signal(SIGSEGV, exit_handler) == SIG_ERR) {
        printf("\nError registering signal handler!\n");
    }
    
    // init the Mac OS X driver
    InitDriver();
    
#else
	previousHandler = signal(SIGSEGV, windows_exit_handler);
	previousHandler = signal(SIGABRT, windows_exit_handler);
	previousHandler = signal(SIGILL, windows_exit_handler);
	previousHandler = signal(SIGINT, windows_exit_handler);
	previousHandler = signal(SIGTERM, windows_exit_handler);
#endif

#ifdef AVX512_SUPPORT
	if (!use_avx512 && (force_clflush_all || force_clflush_pmem)) { 
		fprintf(stderr, "-P or -Q option requires -Z option to be specified as well\n");
		app_exit(EINVAL, NULL);		
	}
#else
	if (force_clflush_all || force_clflush_pmem) { 
		fprintf(stderr, "-P or -Q option is supported only in AVX512 version of the binary\n");
		app_exit(EINVAL, NULL);		
	}
#endif
	cpu_supports_avx512 = is_avx512_enabled();
	cpu_supports_avx2 = is_avx_enabled();
	// LineSize_LatThd is for the stride size used by the latency measuring thread. If prefetchers
	// can't be disabled, then it is important to do 128 byte stride. We will override this
	// LineSize_LatThd later if we determine prefetchers can't be disabled
	LineSize_LatThd = LineSize; 
	if (verify_data_on_read) { 
#ifdef LINUX
		if (rw_mix == 1 && LineSize == 64 && !random_access_bw_thread && loaded_latency_mode) {
			printf("*** Data integrity check enabled *****\n");
		}
		else {
			printf("Data integrity check is not supported with the specified options\n");
			printf("It is supported only for 100%% reads with sequential 64 byte stride in loaded_latency mode\n");
			app_exit(EINVAL, NULL);
		}
#else
		app_exit(EINVAL, "Data integrity check not supported on Windows\n");
#endif
	}
	// check for AVX presence
	if ( (A_provided || Y_provided) && !cpu_supports_avx2)
	{	
		fprintf(stderr, "AVX instructions not supported by this processor\n");
		app_exit(EINVAL, NULL);		
	}	
	if ( Z_provided && !cpu_supports_avx512)
	{	
		fprintf(stderr, "AVX-512 instructions not supported by this processor\n");
		app_exit(EINVAL, NULL);		
	}	

#if 0 // vv
	// Add special code (rather hack) to handle -e and -v as the only options on the command line
	if (argc == 2 && (dont_change_pref || print_verbose1 || use_1tpc_bandwidth)) {
		// now pretend that no parameter was input so rest of the code can be happy
		atleast_one_param_seen = 0;
	}
#endif

	if (U_provided && !(rw_mix == 1 || rw_mix == 2 || rw_mix == 5 || rw_mix == 6 || rw_mix == 7)) {
		fprintf(stderr, "Random access is supported only for traffic types R, W2, W5 and W6\n");
		app_exit(EINVAL, NULL);		
	}
	
	
	// check if LineSize is power of 2. If so, we can avoid divide and use bit shift later
	linesize_powerof2bits = powerof2bits(LineSize);
	
	// if the user specifies nothing, set the matrix mode flag
	if (!matrix_mode) {
		printf("Command line parameters: ");

		for (i = 1; i < argc ; i++) {
			printf("%s ", argv[i]);
		}

		printf("\n");
	}

#if 0 //vv
	// if the user specifies flags without specifying a mode, we don't know which mode to run and give an error
	if (matrix_mode == 0 && peak_bandwidth_mode == 0 && max_bandwidth_mode == 0 && bandwidth_matrix_mode == 0 && latency_matrix_mode == 0 && idle_latency_mode == 0 && loaded_latency_mode == 0 && c2c_latency_mode == 0) {
		fprintf(stderr, "Override flags specified without any usage mode! Exiting...\n");
		Usage((char*) 0);
	}
#endif
	
	if (X_provided && (m_provided || o_provided || k_provided)) {
		fprintf(stderr, "-X can't be specified if -m or -o or -k is also specified! Exiting...\n");
		Usage((char*) 0);
	}

	if (m_provided && k_provided) {
		fprintf(stderr, "Only either -m or -k option can be specified.  Exiting...\n");
		Usage((char*) 0);
	}

	if (o_provided && k_provided) {
		fprintf(stderr, "Only either -o or -k option can be specified.  Exiting...\n");
		Usage((char*) 0);
	}

	if (m_provided && o_provided) {
		fprintf(stderr, "Only either -o or -m option can be specified.  Exiting...\n");
		Usage((char*) 0);
	}

	if (m_provided && j_provided) {
		fprintf(stderr, "Only either -m or -j option can be specified.  Exiting...\n");
		Usage((char*) 0);
	}
	
	if (n_provided && !U_provided) {
		fprintf(stderr, "-U option needs to be specified if -n is present.  Exiting...\n");
		Usage((char*) 0);
	}

	
	// build the topology for the program
	build_topology();
	// Find out how many numa nodes are present - some nodes may not have any cpu resources but just memory
	num_numa_nodes = get_num_numa_nodes();

#ifdef LINUX
	pthread_mutex_init(&getVirtAddrMutex, NULL);
#endif
	build_numa_topology();

    // Initialize the defaults in case perthreadfile is not provided
	initialize_perthread_defaults();

#if !defined(LINUX) && !defined(__APPLE__)

	if (lp_enabled) { // need to enable large page support in windows
		if (!enable_windows_large_pages()) {
			printf("Unable to enable large page support - using 4k pages only\n");
			lp_enabled = 0;
		}
	}

#endif

#ifdef LINUX
	if (lp_1GB_enabled && read_int_from_file("/sys/kernel/mm/hugepages/hugepages-1048576kB/free_hugepages", 0) <=0) {
		printf("\n*****   1GB Hugepages need to be allocated using /sys/kernel/mm/hugepages/hugepages-1048576kB/nr_hugepages *****\n");
		app_exit(EINVAL, NULL);
	}		
#endif

	// if both memory by node and executing test on persistent memory are specified
    if( specify_mem_by_node && mmap_files_dir ){
        fprintf(stderr, "Using -j and -J together is not supported! Exiting...\n");
        Usage((char*) 0);
    }

	if (matrix_mode == 0) {
		if (!b_provided) {
			if (idle_latency_mode || latency_matrix_mode) {
				if (AMD_cpu)
					buffer_size = 600*1024; //buffer size is in KB 
				else
					buffer_size = get_buf_size_for_latency_thread();
			}
			else if (c2c_latency_mode) {
				if (AMD_cpu)
					buffer_size = 600*1024; //buffer size is in KB 
				else
					buffer_size = (int) (cache_info[L3_CACHE].total_size * 3)/1024; //For local socket test, we need to make buffer much bigger than LLC, so it pulls fresh data from memory everytime.
			}
			else
			{
				if (is_non_inclusive_cache_with_dbp_enabled())
					buffer_size = get_fraction_of_3GB_buf_size_per_numa_node(0);
				else
					buffer_size = DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
			}
		}

		if (!o_provided && !c2c_latency_mode) {
			if (idle_latency_mode || latency_matrix_mode)
				printf("\nUsing buffer size of %.3fMiB\n", (float)((float)buffer_size / 1024));
			else
				printf("\nUsing buffer size of %.3fMiB/thread for reads and an additional %.3fMiB/thread for writes\n",
						(float)((float)buffer_size / 1024), (float)((float)buffer_size / 1024));
		}
		// For latency matrix mode, we need to do only one iteration on SKX and beyond to avoid directory penalty
		// Also make sure that this is done only if -x and -t parameters are not already provided
		if (latency_matrix_mode && is_non_inclusive_cache_with_dbp_enabled() && !x_provided && !t_provided)
			one_iteration_only = 1;

	}

	// check if HT is enabled
	if (find_hyperthread(0) == -1) {
		//printf("HT disabled!\n");
		ht_enabled = 0;
	}

	if(tsc_freq) {
		NsecClk = (double)1000/(double)tsc_freq;
	}else {
		Calibrate(&cval);
		Calibrate(&cval);
	}
	if (AMD_cpu) {
			// For proper latency measurements, we need to have large pages enabled. Otherwise
			// TLB miss penalties would inflate the true latencies
#if defined(LINUX) || defined(__APPLE__)
			if (read_int_from_file("/proc/sys/vm/nr_hugepages", 0) <=0) {
				printf("\n*************   Hugepages need to be allocated using /proc/sys/vm/nr_hugepages ******\n");
				printf("*************   Without large pages, the latencies are not accurate\n");
				printf("*************   We need at least 500 2M pages per numa node\n");
				printf("*************   Execute the following and re-run\n");
				printf("*************   echo 4000 > /proc/sys/vm/nr_hugepages\n");
				hugepages_not_enabled=1;
				app_exit(EINVAL, NULL);
			}
#endif
			if (!AMD_zen3)dont_change_pref=1; // we have no way to disable prefetchers prior to zen3
			if (dont_force_random) Random_Access = 0;
			else Random_Access = 1; // enable random access as prefetchers can't be disabled
			if (!D_provided) max_rand_size=819200; // need randomization over large region to beat Rome prefetcher
	}
	
	if (dont_change_pref == 0 && !can_modify_hw_prefetcher()) {
		// We can't modify the prefetchers. So, we need to enable 'random' for latency measurements
#if !defined(LINUX) && !defined(__APPLE__)
		printf("*** Unable to access mlcdrv.sys to modify prefetchers\n");
#else
		printf("*** Unable to modify prefetchers (try executing 'modprobe msr')\n");
#endif
		printf("*** So, enabling random access for latency measurements\n");

		Random_Access = 1;
		dont_change_pref = 1;
	}

	// if we do random access and there is no explicit -l option specified in the command line
	// then set the LineSize_LatThd to 128 bytes
	if (Random_Access == 1 && !l_provided) {
		LineSize_LatThd = 128;
	}
	// compute the time for the program to run
	Run_Time = (UINT64)time_in_seconds * (UINT64)((double)1000000000 / (NsecClk));

	if (print_verbose1) {
		printf("Test running on %.2f MHZ processor(s)\n", (float)1000.0 / NsecClk);
	}

	if (Random_Access || perthreadfile_provided) {
		srand((unsigned)time(NULL));
		rand_array = (int*)malloc(max_rand_size*sizeof(int));
		get_rand(rand_array, max_rand_size);
	}

	if (random_access_bw_thread || perthreadfile_provided) {
		srand((unsigned)time(NULL));
		thruput_array = (int*)malloc(max_rand_size_bw_gen*sizeof(int));
		get_rand_bw_thread(thruput_array, max_rand_size_bw_gen);
		thruput_array_256B = (int*)malloc(max_rand_size_bw_gen*sizeof(int));
		get_rand_bw_thread_256B(thruput_array_256B, max_rand_size_bw_gen/4);
		thruput_array_128B = (int*)malloc(max_rand_size_bw_gen*sizeof(int));
		get_rand_bw_thread_128B(thruput_array_128B, max_rand_size_bw_gen/2);
	}

#if !defined(LINUX) && !defined(__APPLE__)
	process = OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_INFORMATION,
	                      FALSE, GetCurrentProcessId());
#endif


	if (share_buffers_across_all_threads) {
#if !defined(LINUX) && !defined(__APPLE__)
		InitializeCriticalSection(&cs);
#else
		pthread_mutex_init(&serialize_buf_alloc_mutex, NULL);
#endif
	}

	if (j_provided && !(memory_node >=0 && memory_node < num_numa_nodes)) {
		printf("-j option value should be between 0 and max_numa_nodes-1\n");
		app_exit(EINVAL, NULL);
	}

#ifdef INTERNAL
#ifdef LINUX
	// this code is added to track the number of SMIs that occur while MLC is running
	// We added this to flag the user of system issues if we see SMIs during the usage of the system
	// as SMIs during normal operation are not considered good for performance
	smi_data.init_tsc = THE_TSC;
	read_msr_without_fail(SMI_COUNTS_MSR, &smi_data.smi_init_count, 0);
#endif
#endif

// Latency matrix mode specified by the user
	if (latency_matrix_mode) {
		// check for conditions not supported by latency_matrix --
		if (AMD_cpu) {
			if (!hugepages_not_enabled) lp_enabled=1;
		}
		if ( c_provided == 1 || C_provided == 1 || A_provided == 1 || B_provided == 1 || d_provided == 1 ||  
			g_provided == 1 || i_provided == 1 || I_provided == 1 || k_provided == 1 || 
			m_provided == 1 || M_provided == 1 || n_provided == 1 || N_provided == 1 || o_provided == 1 || 
			p_provided == 1 || P_provided == 1 || q_provided == 1 || Q_provided == 1 || j_provided == 1 || R_provided == 1 ||  
			s_provided == 1 || T_provided == 1 || u_provided == 1 || W_provided == 1 ||  
			z_provided == 1 || O_provided == 1 ) {
			fprintf(stderr, "Unsupported Argument for latency_matrix! \n");
			Usage(0);
		}

		// check and warn for ambiguous input
		if (x_provided && t_provided) {
			fprintf(stderr, "Both -x and -t have been provided- defaulting to use -x \n");
		}

		save_and_disable_pref();
		// measure latencies
		latency();
		// Runtime_exit restores all program state
		app_exit(0, NULL); // normal exit
	}

// Idle latency mode specified by the user
	if (idle_latency_mode) {
		// check for conditions not supported by idle_latency
		if (a_provided == 1 || A_provided == 1 || B_provided == 1 || d_provided == 1 ||
		    g_provided == 1 || I_provided == 1 || k_provided == 1 || P_provided == 1 || Q_provided == 1 ||
		    m_provided == 1 || M_provided == 1 || n_provided == 1 || N_provided == 1 || 
		    q_provided == 1 || o_provided == 1 || R_provided == 1 || s_provided == 1 ||
		    T_provided == 1 || u_provided == 1 || W_provided == 1 || X_provided == 1 || 
			z_provided == 1 || O_provided == 1 ) {
			fprintf(stderr, "Unsupported Argument for idle_latency! \n");
			Usage(0);
		}

		// check and warn for ambiguous input
		if (x_provided && t_provided) {
			fprintf(stderr, "Both -x and -t have been provided- defaulting to use -x \n");
		}

		if (i_provided && j_provided) {
			fprintf(stderr, "Both -i and -j have been provided- defaulting to use -j \n");
		}

		// if -c option is not specified, then we will use core 0 for latency measurements. This means dummy thread should not run
		// on cpu0. So, set CPU_Affinity_user_selected to 0
		if (!c_provided) {
			CPU_Affinity_user_selected = CPU_Affinity;
		}

		save_and_disable_pref();
		// measure latencies
		latency();
		// Runtime exit restores all program state
		app_exit(0, NULL);
	}
	if (c2c_latency_mode) {
		// need to enable large pages so tlb miss latencies are not impacting the performance
		if (AMD_cpu && !L_provided && !hugepages_not_enabled) lp_enabled=1;
		
		// check for conditions not supported by c2c_latency
		if(a_provided == 1 || A_provided == 1 || 
				d_provided == 1 || 
				e_provided == 1 || g_provided == 1 || I_provided == 1 ||
				j_provided == 1 || k_provided == 1 || 
				m_provided == 1 || M_provided == 1 || n_provided == 1 ||
				N_provided == 1 || p_provided == 1 || P_provided == 1 || Q_provided == 1 ||
				q_provided == 1 || R_provided == 1 || s_provided == 1 ||
				t_provided == 1 || T_provided == 1 || u_provided == 1 ||
				W_provided == 1 || O_provided == 1 || X_provided == 1 || 
				Y_provided == 1 || z_provided == 1 || Z_provided == 1) {
			fprintf(stderr, "Unsupported Argument for c2c_latency! \n");
			Usage(0);
		}

		if(print_verbose1)
			printf("In c2c_latency_mode\n");

		save_and_disable_pref();
		// measure hit/hitm latency
		if(!b_provided && !c_provided && !C_provided && !H_provided && !i_provided && !r_provided && !w_provided && !x_provided && !S_provided)
			c2c_latency_main(1); //Matrix mode
		else
			c2c_latency_main(0); //At least one user parameter was specified, run manual mode.

		// Runtime exit restores all program state
		app_exit(0, NULL);
	}

// Peak injection bandwidth mode specified by the user
	if (peak_bandwidth_mode || max_bandwidth_mode) {
		// check for conditions not supported by peak_bandwidth
		if (a_provided == 1 || B_provided == 1 || d_provided == 1 || D_provided == 1 || g_provided == 1 ||
	        i_provided == 1 || I_provided == 1 || n_provided == 1 || N_provided == 1 || o_provided == 1 || p_provided == 1 ||
			P_provided == 1 || q_provided == 1 || e_provided == 1 || R_provided == 1 || r_provided == 1 || s_provided == 1 || Q_provided == 1 ||
		    T_provided == 1 || W_provided == 1 || z_provided == 1 || O_provided == 1 || j_provided == 1 || x_provided == 1) {
			fprintf(stderr, "Unsupported Argument for peak_bandwidth! \n");
			Usage(0);
		}

		 // check and warn for ambiguous input
		 if (m_provided && X_provided) {
			 fprintf(stderr, "Both -X and -m have been provided- defaulting to use -m \n");
		 }
		                              

		save_and_enable_pref();
		// measure peak injection bandwidth or max bandwidth
		if (max_bandwidth_mode)
			measure_bandwidth(MAX_BW_AT_ANY_INJECTION);
		else
			measure_bandwidth(MAX_INJECTION_BW); // peak injection bw
			
		// Runtime_exit restores all program state
		app_exit(0, NULL);
	}

// Bandwidth matrix mode specified by user
	if (bandwidth_matrix_mode) {
		// check for conditions not supported by bandwidth matrix
		if (    a_provided == 1 || B_provided == 1 || d_provided == 1 || D_provided == 1 ||
		        g_provided == 1 || i_provided == 1 || I_provided == 1 || k_provided == 1 || m_provided == 1 ||
		        n_provided == 1 || N_provided == 1 || o_provided == 1 || p_provided == 1 || P_provided == 1 ||  q_provided == 1 || Q_provided == 1 ||
		        e_provided == 1 || r_provided == 1 || s_provided == 1 || T_provided == 1 || 
		        z_provided == 1 || O_provided == 1 ||  j_provided == 1 || x_provided == 1) {
			fprintf(stderr, "Unsupported Argument for bandwidth_matrix! \n");
			Usage(0);
		}

		// check and warn for ambiguous input
         if (m_provided && X_provided) {
             fprintf(stderr, "Both -X and -m have been provided- defaulting to use -m \n");
         }


		save_and_enable_pref();
		// measure bandwidth matrix
		measure_bandwidth_matrix();
		// Runtime_exit restores all program state
		app_exit(0, NULL);
	}

// Loaded latency mode specified by user
	if (loaded_latency_mode) {
		// check for conditions not supported by loaded latency
		if (a_provided == 1 || p_provided == 1 || J_provided == 1 ||
		    s_provided == 1 || z_provided == 1 || O_provided == 1) {
			fprintf(stderr, "Unsupported Argument for loaded_latency! \n");
			Usage(0);
		}
		if (x_provided == 1 && !one_iteration_only) {
			fprintf(stderr, "Only -x0 is provided in loaded_latency mode\n");
			app_exit(EINVAL, NULL);
		}

		if(o_provided){
            parse_perthreadfile(perthreadfile);
        }

		save_and_enable_pref();

		// unless user asks not to mess with prefetchers, we disable only the latency core's prefetcher
		if (dont_change_pref == 0) {
			unsigned int prefetch_msr;
			if (AMD_zen3)		
				prefetch_msr = PREFETCH_MSR_ZEN3;
			else
				prefetch_msr = PREFETCH_MSR;
			if (!thruput_only) { // if we are measuring throughput only, then core 0 is also running the thruput thread - so don't disable prefetcher on core0
				disable_pref_msr_mask = get_hwpref_disable_mask();
				//printf("\nPrefetcher msr %x, value %d, core: %d:\n",PREFETCH_MSR,disable_pref_msr_mask,latency_thread_run_cpuid);
				write_msr(prefetch_msr, &disable_pref_msr_mask, latency_thread_run_cpuid);
			}
		}
		// measure loaded latencies
		measure_loaded_latencies();
		// Runtime_exit restores all program state
		app_exit(0, NULL);
	}

	if (matrix_mode) {
		if (AMD_cpu) {
			LineSize = AMD_LINE_STRIDE_SIZE; // 128 bytes to beat the prefetchers
			buffer_size = 600*1024; // 600 MB buffer size for memory latency tests
			if (!L_provided && !hugepages_not_enabled) lp_enabled=1;
		}
		else { // intel processors
			// use 200MB buffer size for latency measurements
			buffer_size = get_buf_size_for_latency_thread();
		}
		// For SKX server and beyond, use single iteration with large buffer size so we don't include the directory update penalty
		if (is_non_inclusive_cache_with_dbp_enabled()) {
			one_iteration_only = 1;
		}
        if(print_verbose1)
            printf("\nUsing buffer size of %.3fMiB\n", (float)((float)buffer_size / 1024));
		/*** latency matrix ***/
		// save and disable prefetchers for latency measurements
		save_and_disable_pref();
		latency_matrix_mode = 1;
		// measure latencies
		latency();
		// set stop_busy_loop flag to prevent p_threads from polling
		
		stop_busyloop_threads = 1;
		one_iteration_only = 0; // reset it
		if (AMD_cpu) { // restore the values to defaults so other tests can function properly
			LineSize=64;
			buffer_size = 200*1024; // 200 MB buffer size
			if (!L_provided && !hugepages_not_enabled) lp_enabled=0;
		}
		// give a second for the polling threads to stop
#if defined(LINUX) || defined(__APPLE__)
		sleep(1);
#else
		Sleep(1000);
#endif
		if (supports_spec_itom() && !K_provided ) {
			// On processors which supports specItoM, full 64 byte writes can avoid RFOs
			// So, we need to do onluy partial stores so we can properly account for read+write bw
			rfos_converted_to_itom = 1; // if this is set, don't do full 64 byte line stores so rfos can't be converted to ITOM
			if (print_verbose1) {
				printf("Processor supports specItoM and hence setting partial_cache_line\n");
			}
		}
		/*** peak bandwidth ***/
		// save (if not already saved) and enable prefetchers for bandwidth measurements
		save_and_enable_pref();
		// go back to default buffer size of 100MB for bandwidth measurements.
		// since this is matrix mode, possibility of user specifying a mask
		// does not arise
        buffer_size = DEFAULT_BUF_SIZE_BANDWIDTH_THREAD;
		// measure peak bandwidths

		measure_bandwidth(MAX_INJECTION_BW);
		// give a second before the next test
#if defined(LINUX) || defined(__APPLE__)
		sleep(1);
#else
		Sleep(1000);
#endif
		/*** bandwidth matrix ***/
		// use read only load for the bandwidth matrix
		rw_mix = 1;
		RDONLY_load = 1;
		change_traffic(rw_mix);
		// measure bandwidths
		measure_bandwidth_matrix();
		// give a second before the next test
#if defined(LINUX) || defined(__APPLE__)
		sleep(1);
#else
		Sleep(1000);
#endif
		/*** loaded latencies ***/
		remote_mem_alloc = 0; // todo: check if this is required or used
		// use read-only traffic type
		RDONLY_load = 1;
		//  hack to unset the peak-load-only operation (delay array[1] was set to -1)
		delay_array[1] = 2;

		// disable the prefetcher for the latency thread alone as the others are already enabled for bandwidth measurements
		if (dont_change_pref == 0) {
			write_msr(PREFETCH_MSR, &disable_pref_msr_mask, latency_thread_run_cpuid);
		}
		// measure loaded latencies
		measure_loaded_latencies();
        
        save_and_disable_pref();
        
		//measure cache to cache transfer latency
		if (AMD_cpu) {
			if (!L_provided && !hugepages_not_enabled) lp_enabled=1;
			LineSize = AMD_LINE_STRIDE_SIZE; //beat the adjacent prefetcher
			buffer_size = 600*1024;
		}
		c2c_latency_main(1);
		// Runtime_exit restores all system state
		app_exit(0, NULL);
	}

#if !defined(LINUX) && !defined(__APPLE__)
	UnGrabAllMemoryFromNvRam();
#endif
	return 0; //should never get here
}


void save_and_disable_pref()
{
	int i;
	unsigned int prefetch_msr;

	if (dont_change_pref == 1) {
		return;    // user wants the prefetchers not modified
	}

	if (AMD_zen3)
		prefetch_msr = PREFETCH_MSR_ZEN3;
	else
		prefetch_msr = PREFETCH_MSR;
	
	// check if prefetchers have already been saved (meaning this is a point in the middle of the program and not the original prefetcher setting)
	// if not create array, saved_msr_values to save them, else create a temp array, saved_temp

	if (need_to_restore_pref == 0) {
		saved_msr_values = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	} else {
		saved_temp = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	}

	// todo: check if this is required
	buffer = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	// create an array to hold the values to disable prefetchers
	disable_pref_msr_values = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	disable_pref_msr_mask = get_hwpref_disable_mask();

	// copy the current prefetcher values to either the saved values array or to the temp array if they've already been saved earlier
	if (need_to_restore_pref == 0) {
		// if prefetchers have not been saved already earlier in the program
		err1 = read_msr_all(prefetch_msr, saved_msr_values); // save original prefetcher value to saved_msr_values
		saved_temp = saved_msr_values;
	} else {
		err1 = read_msr_all(prefetch_msr, saved_temp); // save original prefetcher value to saved_temp, we need this for masking
	}

	// use a temp pointer to manipulate the disable_pref_msr_values array
	disable_temp = disable_pref_msr_values;

	// mask the current values of prefetchers with the disable mask and populate disable_pref_msr_values, using the disable_temp pointer
	for (i = 0; i < os_cpu_count && disable_temp && saved_temp; i++) {
		(*disable_temp) = (uint64_t)(*saved_temp);
		//printf("after anding, disable is %x\n",*disable_temp);
		*disable_temp = *disable_temp | (uint64_t)disable_pref_msr_mask;
		//printf("after masking, disable is %x\n",*disable_temp);
		disable_temp++;
		saved_temp++;
	}

	// no matter how you came in, at the end of this function prefetchers need to be restored
	need_to_restore_pref = 1;
	// disable prefetchers
	write_msr_all(prefetch_msr, disable_pref_msr_values);
}


void save_and_enable_pref()
{
	int i;
	unsigned int prefetch_msr;

	if (dont_change_pref == 1) {
		return;    // user wants the prefetchers not modified
	}
    
	if (AMD_zen3)
		prefetch_msr = PREFETCH_MSR_ZEN3;
	else
		prefetch_msr = PREFETCH_MSR;

	// check if prefetchers have already been saved (meaning this is a point in the middle of the program and not the original prefetcher setting)
	// if not create array, saved_msr_values to save them, else create a temp array, saved_temp
	if (need_to_restore_pref == 0) {
		saved_msr_values = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	} else {
		saved_temp = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));
	}

	// create an array to hold the values to disable prefetchers
	enable_pref_msr_values = (uint64_t*)malloc(os_cpu_count * sizeof(uint64_t));

	// copy the current prefetcher values to either the saved values array or to the temp array if they've already been saved earlier
	if (need_to_restore_pref == 0) {
		// if prefetchers have not been saved already earlier in the program
		err1 = read_msr_all(prefetch_msr, saved_msr_values); // save original prefetcher value to saved_msr_values
		saved_temp = saved_msr_values;
	} else {
		err1 = read_msr_all(prefetch_msr, saved_temp); // save original prefetcher value to saved_temp, we need this for masking
	}

	// use a temp pointer to manipulate the enable_pref_msr_values array
	enable_temp = enable_pref_msr_values;

	// mask the current values of prefetchers with the enable mask and populate enable_pref_msr_values, using the enable_temp pointer
	for (i = 0; i < os_cpu_count && enable_temp && saved_temp; i++) {
		//printf("at beginning enable temp is %x\n save temp is %x\n",*enable_temp,*saved_temp);
		(*enable_temp) = (uint64_t)(*saved_temp) ; //& (uint64_t)0xfffffffffffffff0;
		//printf("after anding, enable is %x\n",*enable_temp);
		*enable_temp = *enable_temp & ~(get_hwpref_enable_mask()); //(uint64_t)enable_pref_msr_values;
		//printf("after masking, enable is %x\n",*enable_temp);
		enable_temp++;
		saved_temp++;
	}

	need_to_restore_pref = 1;
	write_msr_all(prefetch_msr, enable_pref_msr_values);
	//printf("pref should now be enabled");
}

void restore_prefetchers()
{
	// return if user did not want to modify the prefetchers - if so nothing to restore
	// return if we have not saved the prefetchers state till now
	if (dont_change_pref == 1 || need_to_restore_pref != 1) {
		return;    // user wants the prefetchers not modified
	}

	if (AMD_zen3)
		write_msr_all(PREFETCH_MSR_ZEN3, saved_msr_values);
	else
		write_msr_all(PREFETCH_MSR, saved_msr_values);
}

#if !defined(LINUX) && !defined(__APPLE__)
int read_msr(unsigned int address, UINT64* value, int latency_core);
int write_msr(unsigned int address, UINT64* value, int latency_core);
#else
int read_msr(unsigned int address, uint64_t* value, int latency_core);
int write_msr(unsigned int address, uint64_t* value, int latency_core);
#endif

int can_modify_AMP_prefetcher() 
{
	uint64_t orig_val, new_val, temp_val=0xff;
	unsigned int prefetch_msr = PREFETCH_MSR;
	int result=1;

	if (!supports_AMP_prefetcher()) return 0; // Not SPR or later processors which supports this prefefcher
	
	if (read_msr(prefetch_msr, &orig_val, 0) != 0) {
	// Now make sure that the read is actually valid and hypervisor is not faking a value
	// for that, we will write a specific value and read it back
		new_val = DISABLE_PREF_SPR;
		if (write_msr (prefetch_msr, &new_val, 0) != 0) {
			// now read it back and make sure it changed actually
			read_msr(prefetch_msr, &temp_val, 0);
			if (temp_val == new_val) {
				// confirms we can successfully modify prefetchers. Now restore the orig_val;
				write_msr (prefetch_msr, &orig_val, 0);
			}
			else result=0;
			
		}
		else { 
			if (print_verbose1)
				printf("Unable to read prefetch msr\n");
			result = 0;
		}
	}
	else result = 0;
	return result;
}

int can_modify_hw_prefetcher() 
{
	uint64_t orig_val, new_val, temp_val=0xff;
	int result=1;
	unsigned int prefetch_msr;
	
#if !defined(LINUX) && !defined(__APPLE__)
	if (AMD_cpu) return 0; // Even on Zen3, we don't support prefetch control on Windows as it requires driver modification
	if (!init_windows_msr_access()) {
		if (print_verbose1)
			printf("Error --- Unable to install the mlcdrv.sys driver\n");
		return 0;
	}
#endif	
	if (AMD_cpu && !AMD_zen3) return 0;  // prior to zen3, we could not disable prefetchers on AMD
	if (AMD_zen3)
		prefetch_msr = PREFETCH_MSR_ZEN3;
	else
		prefetch_msr = PREFETCH_MSR;
	if (read_msr(prefetch_msr, &orig_val, 0) != 0) {
		// Now make sure that the read is actually valid and hypervisor is not faking a value
		// for that, we will write a specific value and read it back
		// Now just flip bit 0 alone
		if (orig_val & 1) new_val = (orig_val & ~1);
		else new_val = (orig_val | 1);
		//new_val = (orig_val == 0 )?0x1:0;
		if (write_msr (prefetch_msr, &new_val, 0) != 0) {
			// now read it back and make sure it changed actually
			read_msr(prefetch_msr, &temp_val, 0);
			if (temp_val == new_val) {
				// confirms we can successfully modify prefetchers. Now restore the orig_val;
				write_msr (prefetch_msr, &orig_val, 0);
			}
			else result=0;
			
		}
		else { 
			if (print_verbose1)
				printf("Unable to read prefetch msr\n");
			result = 0;
		}
	
	}
	else result = 0;
	return result;
 
}


