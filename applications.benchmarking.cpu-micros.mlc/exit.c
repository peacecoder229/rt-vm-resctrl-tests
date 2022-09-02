#if defined(LINUX) || defined(__APPLE__)
#include<stdio.h>
#include <signal.h>
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
#include <sched.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdint.h>
#else // Windows specific
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <process.h>
#endif

#include "exit.h"
#include "types.h"
#include "common.h"

extern int numa_bal_file_modified;
extern int numa_bal_file_savedvalue;
extern int numa_scan_file_modified;
extern int numa_scan_file_savedvalue;

extern void DeleteAllSCMFiles(void);

extern void restore_prefetchers();
// extern variables

char numa_bal_path[64] = "/proc/sys/kernel/numa_balancing";
char numa_scan_path[64] = "/proc/sys/kernel/numa_balancing_scan_delay_ms";

void  check_for_null_parameter(char b, char arg)
{
	if (arg == 0) {
		fprintf(stderr, "Value missing for the -%c parameter or an unnecessary blank space between -%c and its value\n", b, b);
		fprintf(stderr, "Exiting..\n");
		app_exit(EINVAL, NULL);
	}
}

void  check_no_option_value(char b, char arg)
{
	if (arg != 0) {
		fprintf(stderr, "-%c parameter does not accept any value\n", b);
		fprintf(stderr, "Exiting..\n");
		app_exit(EINVAL, NULL);
	}
}

void app_exit(unsigned int errcode, const char* error)
{
	stop_load++; // indicate non-zero value here so all threads can stop
	if (error) {
		fprintf(stderr, "%s\n\n", error);
	}

#ifdef INTERNAL
#ifdef LINUX
	smi_data.end_tsc = THE_TSC;
	smi_data.smi_end_count = 0;
	if (read_msr_without_fail(SMI_COUNTS_MSR, &smi_data.smi_end_count, 0) != -1 && smi_data.init_tsc != 0) { // no error
		UINT64 smi_diff = smi_data.smi_end_count - smi_data.smi_init_count;
		if (smi_diff > 0) {
			UINT64 tsc_delta = smi_data.end_tsc - smi_data.init_tsc;
			printf("***********  WARNING *************\n");
			if (NsecClk != 0) { // system calibrated to find tsc frequency
				printf("%d SMI interrupts observed in the system in %f seconds\n", (int)smi_diff, (double)(tsc_delta * NsecClk)/(double)1000000000);
			}
			else {
				printf("%d SMI interrupts observed in the system in %lld cycles\n", (int)smi_diff, tsc_delta);
			}
			printf("Performance may be significantly degraded by SMI\n");
		}
	}

#endif
#endif

	restore_prefetchers();
#if defined(LINUX) || defined(__APPLE__)

	if (numa_bal_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_bal_file_savedvalue,numa_bal_path);
		write_int_to_file(numa_bal_path, 0, numa_bal_file_savedvalue);
	}

	if (numa_scan_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_scan_file_savedvalue,numa_scan_path);
		write_int_to_file(numa_scan_path, 0, numa_scan_file_savedvalue);
	}
	if( mmap_files_dir ){ free(mmap_files_dir); mmap_files_dir = NULL; }

#endif

	DeleteAllSCMFiles();

	exit(errcode);
}
void Runtime_exit(const char* error)
{
	stop_load++; // indicate non-zero value here so all threads can stop
	if (error) {
		fprintf(stderr, "%s\n\n", error);
	}

#ifdef INTERNAL
#ifdef LINUX
	smi_data.end_tsc = THE_TSC;
	smi_data.smi_end_count = 0;
	if (read_msr_without_fail(SMI_COUNTS_MSR, &smi_data.smi_end_count, 0) != -1 && smi_data.init_tsc != 0) { // no error
		UINT64 smi_diff = smi_data.smi_end_count - smi_data.smi_init_count;
		if (smi_diff > 0) {
			UINT64 tsc_delta = smi_data.end_tsc - smi_data.init_tsc;
			printf("***********  WARNING *************\n");
			if (NsecClk != 0) { // system calibrated to find tsc frequency
				printf("%d SMI interrupts observed in the system in %f seconds\n", (int)smi_diff, (double)(tsc_delta * NsecClk)/(double)1000000000);
			}
			else {
				printf("%d SMI interrupts observed in the system in %lld cycles\n", (int)smi_diff, tsc_delta);
			}
			printf("Performance may be significantly degraded by SMI\n");
		}
	}

#endif
#endif

	restore_prefetchers();
#if defined(LINUX) || defined(__APPLE__)

	if (numa_bal_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_bal_file_savedvalue,numa_bal_path);
		write_int_to_file(numa_bal_path, 0, numa_bal_file_savedvalue);
	}

	if (numa_scan_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_scan_file_savedvalue,numa_scan_path);
		write_int_to_file(numa_scan_path, 0, numa_scan_file_savedvalue);
	}
	if( mmap_files_dir ){ free(mmap_files_dir); mmap_files_dir = NULL; }

#endif

	DeleteAllSCMFiles();

	exit(0);
}

void exit_handler(int s)
{
	signal(s, SIG_IGN);
	restore_prefetchers();
#if defined(LINUX) || defined(__APPLE__)

	if (numa_bal_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_bal_file_savedvalue,numa_bal_path);
		write_int_to_file(numa_bal_path, 0, numa_bal_file_savedvalue);
		numa_bal_file_modified = 0;
	}

	if (numa_scan_file_modified == 1) {
		//printf("restoring %d to %s\n",numa_scan_file_savedvalue,numa_scan_path);
		write_int_to_file(numa_scan_path, 0, numa_scan_file_savedvalue);
		numa_scan_file_modified = 0;
	}

#endif
	app_exit(s, "\nExiting...\n");
}

void windows_exit_handler(int s)
{
	signal(s, SIG_IGN);
	restore_prefetchers();

#if 0
	switch (s)
	{

	case SIGSEGV:
		printf("\nexit_handler code SIGSEGV %d", s);
		break;

	case SIGABRT:
		printf("\nexit_handler code SIGABRT %d", s);
		break;

	case SIGILL:
		printf("\nexit_handler code SIGILL %d", s);
		break;

	case SIGINT:
		printf("\nexit_handler code SIGINT %d", s);
		break;

	case SIGTERM:
		printf("\nexit_handler code SIGTERM %d", s);
		break;

	default:
		printf("\nexit_handler code %d", s);
		break;
	}

#endif
	app_exit(s, "\nExiting...\n");
}

