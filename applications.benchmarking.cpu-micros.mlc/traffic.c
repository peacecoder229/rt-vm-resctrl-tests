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
#include <stdio.h>
#include <stdlib.h>
#endif


#include "types.h"
#include "common.h"

#if defined(LINUX) || defined(__APPLE__)
#define COUNTER "b"
#define arrayPOSITION "d"
#define ENDP "d"
#define baseADDRESS "c"
#define LocalADDRESS "c"
#define RemoteADDRESS "D"
#define WriteADDRESS "S"
#else
#define COUNTER ebx
#define arrayPOSITION rdx
#define ENDP rdx
#define baseADDRESS rcx
#define LocalADDRESS rcx
#define RemoteADDRESS rdi
#define WriteADDRESS rsi
#endif

char* do_independent_load_rdonly_64B(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx256_64B(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx512_64B(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_64B_verify_data_avx512(char* local_buf,  char* endp, int delay_t, char*new_chksum_value);
char* do_independent_load_rdonly_64B_verify_data_avx2(char* local_buf,  char* endp, int delay_t, char*new_chksum_value);
char* do_independent_load_rdonly_64B_verify_data_sse2(char* local_buf,  char* endp, int delay_t, char*new_chksum_value);

char* do_independent_load_rwlocal_2R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_3R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_50R_50W_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_50R_50W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_50R_50W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_random_load_rdonly(char* local_buf,  __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rdonly_avx512(char* local_buf,  __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rdonly_avx512_128B_block(char* local_buf,  __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rdonly_avx512_256B_block(char* local_buf,  __int64 buf_len, char* endp, int delay_t);

char* do_independent_load_rdonly(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx256(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx512(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx512_bsz1(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx512_bsz2(char* local_buf,  char* endp, int delay_t);
char* do_independent_load_rdonly_avx512_bsz4(char* local_buf,  char* endp, int delay_t);

char* do_independent_load_rwlocal_80R_20W(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_80R_20W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_80R_20W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_5R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_5R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_5R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_6R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_6R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_6R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_7R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_7R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_7R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_8R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_8R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_8R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_9R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_8R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
//char* do_independent_load_rwlocal_8R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
	
char* do_independent_load_rwlocal_3R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_random_load_rwlocal_2R_1W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_2R_1W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_2R_1W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_2R_1W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_60R_40W(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_60R_40W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_60R_40W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_random_load_rwlocal_50R_50W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_50R_50W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_50R_50W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_50R_50W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t);

char* do_independent_load_rwlocal_50R_50W(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_50R_50W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_50R_50W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_random_load_rwlocal_100W(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_random_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_random_load_rwlocal_100W_avx512_128B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_random_load_rwlocal_100W_avx512_256B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_load_rwlocal_100W(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_load_rwlocal_100W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);
char* do_independent_load_rwlocal_100W_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio);

char* do_independent_load_rwlocal_1R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_1R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_1R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_random_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t);
char* do_independent_random_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_3R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_3R_1W_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);

char* do_independent_load_rwlocal_100R_two_sources_1to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_100R_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_100R_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R1W_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
char* do_independent_load_rwlocal_3R1W_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R1NTW_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);
char* do_independent_load_rwlocal_2R1NTW_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t);

UINT64 compute_checksum_avx512(char *buf, __int64 buf_len, char *tmpbuf);
UINT64 compute_checksum_avx2(char *buf, __int64 buf_len, char *tmpbuf);
UINT64 compute_checksum_sse2(char *buf, __int64 buf_len, char *tmpbuf);
#ifndef __APPLE__
char* do_independent_load_rw_all_remote(char* remote_buf, char* write_buf , char* endp, int delay_t);
char* do_independent_load_NUMA1(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);
char* do_independent_load_NUMA2(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);
char* do_independent_load_NUMA3_rdonly(char* local_buf, char* remote_buf, char* endp, int delay_t);
char* do_independent_load_NUMA3(char* local_buf, char* remote_buf, char* write_buf, char* endp, int delay_t);
#endif

__int64* do_dependent_read(__int64* a);


extern long volatile start_load, stop_load;
#define FLAG_CHK_CNT	10000L
#ifdef LINUX
UINT64 compute_checksum_from_64B(char *tmpbuf)
{
	UINT64 chksum;
	
	asm (
		"xor %2, %2\n\t"
		"xor (%1), %2\n\t"
		"xor 0x8(%1), %2\n\t"
		"xor 0x10(%1), %2\n\t"
		"xor 0x18(%1), %2\n\t"
		"xor 0x20(%1), %2\n\t"
		"xor 0x28(%1), %2\n\t"
		"xor 0x30(%1), %2\n\t"
		"xor 0x38(%1), %2\n\t"
		"mov %2, %0\n\t"
	:"=a"(chksum)	
	: "r"(tmpbuf), "r"(0LL)
	);
	return chksum;
}

UINT64 compute_checksum_avx512(char *buf, __int64 buf_len, char *tmpbuf) 
{
	UINT64 chksum;
	asm (
		"xor %2, %2\n\t"
		"vpxord %%zmm1, %%zmm1, %%zmm1 \n\t"
		"loop_chk_avx512: \n\t"
		"vpxord (%1), %%zmm1, %%zmm1 \n\t"
		"vpxord 0x40(%1), %%zmm1, %%zmm1 \n\t"
		"vpxord 0x80(%1), %%zmm1, %%zmm1 \n\t"
		"vpxord 0xc0(%1), %%zmm1, %%zmm1 \n\t"
		"add $0x100, %1 \n\t"
		"add $0x100, %2 \n\t"
		"cmp %4, %2\n\t"
		"jl loop_chk_avx512 \n\t"
		"xor %2, %2\n\t"
		"vmovdqa64 %%zmm1, (%3)\n\t"
		"xor (%3), %2\n\t"
		"xor 0x8(%3), %2\n\t"
		"xor 0x10(%3), %2\n\t"
		"xor 0x18(%3), %2\n\t"
		"xor 0x20(%3), %2\n\t"
		"xor 0x28(%3), %2\n\t"
		"xor 0x30(%3), %2\n\t"
		"xor 0x38(%3), %2\n\t"
		"mov %2, %0\n\t"
	:"=a"(chksum)	
	: "r"(buf), "r"(0LL), "r"(tmpbuf), "r"(buf_len)
	);
	return chksum;
}

UINT64 compute_checksum_avx2(char *buf, __int64 buf_len, char *tmpbuf) 
{
	UINT64 chksum;
	asm (
		"xor %2, %2\n\t"
		"vpxord %%ymm1, %%ymm1, %%ymm1 \n\t"
		"loop_chk_avx2: \n\t"
		"vpxord (%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0x20(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0x40(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0x60(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0x80(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0xa0(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0xc0(%1), %%ymm1, %%ymm1 \n\t"
		"vpxord 0xe0(%1), %%ymm1, %%ymm1 \n\t"
		"add $0x100, %1 \n\t"
		"add $0x100, %2 \n\t"
		"cmp %4, %2\n\t"
		"jl loop_chk_avx2 \n\t"
		"xor %2, %2\n\t"
		"vmovdqa %%ymm1, (%3)\n\t"
		"xor (%3), %2\n\t"
		"xor 0x8(%3), %2\n\t"
		"xor 0x10(%3), %2\n\t"
		"xor 0x18(%3), %2\n\t"
		"mov %2, %0\n\t"
	:"=a"(chksum)	
	: "r"(buf), "r"(0LL), "r"(tmpbuf), "r"(buf_len)
	);
	return chksum;
}

UINT64 compute_checksum_sse2(char *buf, __int64 buf_len, char *tmpbuf)
{
	UINT64 chksum;
	asm(
		"xor %3, %3\n\t"
		"pxor %%xmm1, %%xmm1 \n\t"
		
		"continue_chksum_sse2: \n\t"
		"pxor (%1), %%xmm1 \n\t"
		"pxor 0x10(%1), %%xmm1 \n\t"
		"pxor 0x20(%1), %%xmm1 \n\t"
		"pxor 0x30(%1), %%xmm1 \n\t"
		"pxor 0x40(%1), %%xmm1 \n\t"
		"pxor 0x50(%1), %%xmm1 \n\t"
		"pxor 0x60(%1), %%xmm1 \n\t"
		"pxor 0x70(%1), %%xmm1 \n\t"
		"pxor 0x80(%1), %%xmm1 \n\t"
		"pxor 0x90(%1), %%xmm1 \n\t"
		"pxor 0xa0(%1), %%xmm1 \n\t"
		"pxor 0xb0(%1), %%xmm1 \n\t"
		"pxor 0xc0(%1), %%xmm1 \n\t"
		"pxor 0xd0(%1), %%xmm1 \n\t"
		"pxor 0xe0(%1), %%xmm1 \n\t"
		"pxor 0xf0(%1), %%xmm1 \n\t"

		"add $0x100, %1\n\t"
		"add $0x100, %3\n\t"
	
		"cmp %2, %3\n\t"
		"jl continue_chksum_sse2\n\t"
		"movdqa %%xmm1, (%4)\n\t"

		"xor %3, %3\n\t"
		"xor (%4), %3\n\t"
		"xor 0x8(%4), %3\n\t"
		"mov %3, %0\n\t"
	    :"=a"(chksum)
		:"r"(buf),"r"(buf_len),"r"(0LL),"r"(tmpbuf):);
		return chksum;
}
#endif


char* do_independent_load_rdonly_64B(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;

#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor r11, r11
		xor COUNTER, COUNTER

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz
		
		LOOPS:
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+40h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+80h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+0c0h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+100h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+140h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+180h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+1c0h]
		add LocalADDRESS, 200h

	cont11:
		xor rbx, rbx

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
	exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
        
    "LOOPSf18b:\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x40(%2),%%xmm0\n\t"
	    "movdqa 0x80(%2),%%xmm0\n\t"
	    "movdqa 0x0c0(%2),%%xmm0\n\t"
	    "movdqa 0x100(%2),%%xmm0\n\t"
	    "movdqa 0x140(%2),%%xmm0\n\t"
	    "movdqa 0x180(%2),%%xmm0\n\t"
	    "movdqa 0x1c0(%2),%%xmm0\n\t"

	    "add $0x200,%2\n\t"
  
	"FLAG4f18b:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18b:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18b\n\t"
        
	"STOP_DELAYf18b:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18b\n\t"
	"EXIT4f18b:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}
#ifdef LINUX
char* do_independent_load_rdonly_64B_verify_data_avx512(char* local_buf,  char* endp, int delay_t, char *chksum)
{
	__int64 lsz = (__int64)LineSize;

	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
		"vpxord %%zmm1, %%zmm1, %%zmm1\n\t"
        
    "LOOPSf18b_chk:\n\t"
	    "vpxord (%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x40(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x80(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0xc0(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x100(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x140(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x180(%2),%%zmm1,%%zmm1\n\t"
	    "vpxord 0x1c0(%2),%%zmm1,%%zmm1\n\t"

	    "add $0x200,%2\n\t"
  
	"FLAG4f18b_chk:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18b_chk:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18b_chk\n\t"
        
	"STOP_DELAYf18b_chk:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18b_chk\n\t"
	"EXIT4f18b_chk:\n\t"
		"mov %2, %0\n\t"	// set the return value
		"vmovdqa64 %%zmm1, (%9)\n\t"
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"r"(chksum):"r9","r11");
    return ret_ptr;
}


char* do_independent_load_rdonly_64B_verify_data_avx2(char* local_buf,  char* endp, int delay_t, char *chksum)
{
	__int64 lsz = (__int64)LineSize;

	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
		"vpxord %%ymm1, %%ymm1, %%ymm1\n\t"
        
    "LOOPSf18b_chk_avx2:\n\t"
	    "vpxord (%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x20(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x40(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x60(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x80(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0xa0(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0xc0(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0xe0(%2),%%ymm1,%%ymm1\n\t"

	    "vpxord 0x100(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x120(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x140(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x160(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x180(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x1a0(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x1c0(%2),%%ymm1,%%ymm1\n\t"
	    "vpxord 0x1e0(%2),%%ymm1,%%ymm1\n\t"

	    "add $0x200,%2\n\t"
  
	"FLAG4f18b_chk_avx2:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18b_chk_avx2:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18b_chk_avx2\n\t"
        
	"STOP_DELAYf18b_chk_avx2:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18b_chk_avx2\n\t"
	"EXIT4f18b_chk_avx2:\n\t"
		"vmovdqa %%ymm1, (%9)\n\t"
		"vpxord %%ymm1, %%ymm1, %%ymm1\n\t" // move 0 to other half of the buffer
		"vmovdqa %%ymm1, 0x20(%9)\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"r"(chksum):"r9","r11");
    return ret_ptr;
}

char* do_independent_load_rdonly_64B_verify_data_sse2(char* local_buf,  char* endp, int delay_t, char *chksum)
{
	__int64 lsz = (__int64)LineSize;

	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
		"pxor %%xmm1, %%xmm1\n\t"
        
    "LOOPSf18b_chk_sse2:\n\t"
	    "pxor (%2),%%xmm1\n\t"
	    "pxor 0x10(%2),%%xmm1\n\t"
	    "pxor 0x20(%2),%%xmm1\n\t"
	    "pxor 0x30(%2),%%xmm1\n\t"
	    "pxor 0x40(%2),%%xmm1\n\t"
	    "pxor 0x50(%2),%%xmm1\n\t"
	    "pxor 0x60(%2),%%xmm1\n\t"
	    "pxor 0x70(%2),%%xmm1\n\t"
	    "pxor 0x80(%2),%%xmm1\n\t"
	    "pxor 0x90(%2),%%xmm1\n\t"
	    "pxor 0xa0(%2),%%xmm1\n\t"
	    "pxor 0xb0(%2),%%xmm1\n\t"
	    "pxor 0xc0(%2),%%xmm1\n\t"
	    "pxor 0xd0(%2),%%xmm1\n\t"
	    "pxor 0xe0(%2),%%xmm1\n\t"
	    "pxor 0xf0(%2),%%xmm1\n\t"

	    "add $0x100,%2\n\t"
  
	"FLAG4f18b_chk_sse2:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18b_chk_sse2:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18b_chk_sse2\n\t"
        
	"STOP_DELAYf18b_chk_sse2:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18b_chk_sse2\n\t"
	"EXIT4f18b_chk_sse2:\n\t"
		"movdqa %%xmm1, (%9)\n\t"
		"pxor %%xmm1, %%xmm1\n\t"
		"movdqa %%xmm1, 0x10(%9)\n\t"
		"movdqa %%xmm1, 0x20(%9)\n\t"
		"movdqa %%xmm1, 0x30(%9)\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"r"(chksum):"r9","r11");
    return ret_ptr;
}
#endif

char* do_independent_load_rdonly_avx256_64B(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;

#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor r11, r11
		xor COUNTER, COUNTER

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz
		
		LOOPS:
		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+40h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+80h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+0c0h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+100h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+140h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+180h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+1c0h]

		add LocalADDRESS, 200h

	cont11:
		xor rbx, rbx

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
	exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
        
    "LOOPSf18c:\n\t"

		"vmovdqa (%2),%%ymm0\n\t"
	    "vmovdqa 0x40(%2),%%ymm0\n\t"
	    "vmovdqa 0x80(%2),%%ymm0\n\t"
	    "vmovdqa 0x0c0(%2),%%ymm0\n\t"
	    "vmovdqa 0x100(%2),%%ymm0\n\t"
	    "vmovdqa 0x140(%2),%%ymm0\n\t"
	    "vmovdqa 0x180(%2),%%ymm0\n\t"
	    "vmovdqa 0x1c0(%2),%%ymm0\n\t"
	
	    "add $0x200,%2\n\t"
  
	"FLAG4f18c:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18c:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18c\n\t"
        
	"STOP_DELAYf18c:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18c\n\t"
	"EXIT4f18c:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rdonly_avx512_64B(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;

#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor r11, r11
		xor COUNTER, COUNTER

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz
		
		LOOPS:
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+40h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+80h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+0c0h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+100h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+140h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+180h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+1c0h]

		add LocalADDRESS, 200h

	cont11:
		xor rbx, rbx

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
	exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
        
    "LOOPSf18d:\n\t"

		"vmovdqa64 (%2),%%zmm0\n\t"
	    "vmovdqa64 0x40(%2),%%zmm0\n\t"
	    "vmovdqa64 0x80(%2),%%zmm0\n\t"
	    "vmovdqa64 0x0c0(%2),%%zmm0\n\t"
	    "vmovdqa64 0x100(%2),%%zmm0\n\t"
	    "vmovdqa64 0x140(%2),%%zmm0\n\t"
	    "vmovdqa64 0x180(%2),%%zmm0\n\t"
	    "vmovdqa64 0x1c0(%2),%%zmm0\n\t"
	
	    "add $0x200,%2\n\t"
  
	"FLAG4f18d:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18d:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18d\n\t"
        
	"STOP_DELAYf18d:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18d\n\t"
	"EXIT4f18d:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}
#endif

char* do_independent_load_rwlocal_2R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmmword ptr [WriteADDRESS], xmm0

		movdqa xmm1, xmmword ptr [LocalADDRESS+40h]
		movdqa xmmword ptr [WriteADDRESS+40h], xmm0

		movdqa xmm1, xmmword ptr [LocalADDRESS+80h]
		movdqa xmmword ptr [WriteADDRESS+80h], xmm0

		movdqa xmm1, xmmword ptr [LocalADDRESS+0c0h]
		movdqa xmmword ptr [WriteADDRESS+0c0h], xmm0

		add LocalADDRESS, 100h
		add WriteADDRESS, 100h

    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6q1:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa %%xmm0,(%4)\n\t"

	    "movdqa 0x40(%2),%%xmm1\n\t"
	    "movdqa %%xmm0,0x40(%4)\n\t"

	    "movdqa 0x80(%2),%%xmm1\n\t"
	    "movdqa %%xmm0,0x80(%4)\n\t"

	    "movdqa 0xc0(%2),%%xmm1\n\t"
	    "movdqa %%xmm0,0xc0(%4)\n\t"

	    "add $0x100,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f6q1:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6q1:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6q1\n\t"
        
	    "STOP_DELAYf6q1:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6q1\n\t"
        
	"EXIT4f6q1:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
		
     return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymmword ptr [WriteADDRESS], ymm0

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+40h]
		vmovdqa ymmword ptr [WriteADDRESS+40h], ymm0

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+80h]
		vmovdqa ymmword ptr [WriteADDRESS+80h], ymm0

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+0c0h]
		vmovdqa ymmword ptr [WriteADDRESS+0c0h], ymm0

		add LocalADDRESS, 100h
		add WriteADDRESS, 100h

    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6q2:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"

	    "vmovdqa 0x40(%2),%%ymm1\n\t"
	    "vmovdqa %%ymm0,0x40(%4)\n\t"

	    "vmovdqa 0x80(%2),%%ymm1\n\t"
	    "vmovdqa %%ymm0,0x80(%4)\n\t"

	    "vmovdqa 0xc0(%2),%%ymm1\n\t"
	    "vmovdqa %%ymm0,0xc0(%4)\n\t"

	    "add $0x100,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f6q2:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6q2:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6q2\n\t"
        
	    "STOP_DELAYf6q2:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6q2\n\t"
        
	"EXIT4f6q2:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
		
     return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_2R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+40h]
		vmovdqa64 zmmword ptr [WriteADDRESS+40h], zmm0

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+80h]
		vmovdqa64 zmmword ptr [WriteADDRESS+80h], zmm0

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+0c0h]
		vmovdqa64 zmmword ptr [WriteADDRESS+0c0h], zmm0

		add LocalADDRESS, 100h
		add WriteADDRESS, 100h

    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6q3:\n\t"
	    // Unrolled 2 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"

	    "vmovdqa64 0x40(%2),%%zmm1\n\t"
	    "vmovdqa64 %%zmm0,0x40(%4)\n\t"

	    "vmovdqa64 0x80(%2),%%zmm1\n\t"
	    "vmovdqa64 %%zmm0,0x80(%4)\n\t"

	    "vmovdqa64 0xc0(%2),%%zmm1\n\t"
	    "vmovdqa64 %%zmm0,0xc0(%4)\n\t"

	    "add $0x100,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f6q3:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6q3:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6q3\n\t"
        
	    "STOP_DELAYf6q3:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6q3\n\t"
        
	"EXIT4f6q3:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
		
     return ret_ptr;
#endif
}
#endif

char* do_independent_load_rwlocal_3R_1W_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value
		mov r10, lsz

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+40h]
		movdqa xmmword ptr [WriteADDRESS], xmm2

		movdqa xmm1, xmmword ptr [LocalADDRESS+80h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+0c0h]
		movdqa xmmword ptr [WriteADDRESS+40h], xmm2

		movdqa xmm1, xmmword ptr [LocalADDRESS+100h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+140h]
		movdqa xmmword ptr [WriteADDRESS+80h], xmm2

		movdqa xmm1, xmmword ptr [LocalADDRESS+180h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+1c0h]
		movdqa xmmword ptr [WriteADDRESS+0c0h], xmm2

		
		add LocalADDRESS, 200h
		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value

	    "LOOPSf44:\n\t"
	    // Unrolled 4 times
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x40(%2),%%xmm0\n\t"
	    "movdqa %%xmm1,(%4)\n\t"

	    "movdqa 0x80(%2),%%xmm0\n\t"
	    "movdqa 0xc0(%2),%%xmm0\n\t"
	    "movdqa %%xmm1,0x40(%4)\n\t"

	    "movdqa 0x100(%2),%%xmm0\n\t"
	    "movdqa 0x140(%2),%%xmm0\n\t"
	    "movdqa %%xmm1,0x80(%4)\n\t"

	    "movdqa 0x180(%2),%%xmm0\n\t"
	    "movdqa 0x1c0(%2),%%xmm0\n\t"
	    "movdqa %%xmm1,0xc0(%4)\n\t"

	    "add $0x200,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f44:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f44:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f44\n\t"
	    
        "STOP_DELAYf44:\n\t"

	    "cmp %3, %2\n\t"
	    "jl LOOPSf44\n\t"
	"EXIT4f44:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_3R_1W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value
		mov r10, lsz

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+40h]
		vmovdqa ymmword ptr [WriteADDRESS], ymm2

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+80h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+0c0h]
		vmovdqa ymmword ptr [WriteADDRESS+40h], ymm2

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+100h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+140h]
		vmovdqa ymmword ptr [WriteADDRESS+80h], ymm2

		vmovdqa ymm1, ymmword ptr [LocalADDRESS+180h]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+1c0h]
		vmovdqa ymmword ptr [WriteADDRESS+0c0h], ymm2
		
		add LocalADDRESS, 200h
		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value

	    "LOOPSf45:\n\t"
	    // Unrolled 4 times
	    "vmovdqa (%2),%%ymm0\n\t"
	    "vmovdqa 0x40(%2),%%ymm0\n\t"
	    "vmovdqa %%ymm1,(%4)\n\t"

	    "vmovdqa 0x80(%2),%%ymm0\n\t"
	    "vmovdqa 0xc0(%2),%%ymm0\n\t"
	    "vmovdqa %%ymm1,0x40(%4)\n\t"

	    "vmovdqa 0x100(%2),%%ymm0\n\t"
	    "vmovdqa 0x140(%2),%%ymm0\n\t"
	    "vmovdqa %%ymm1,0x80(%4)\n\t"

	    "vmovdqa 0x180(%2),%%ymm0\n\t"
	    "vmovdqa 0x1c0(%2),%%ymm0\n\t"
	    "vmovdqa %%ymm1,0xc0(%4)\n\t"

	    "add $0x200,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f45:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f45:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f45\n\t"
	    
        "STOP_DELAYf45:\n\t"

	    "cmp %3, %2\n\t"
	    "jl LOOPSf45\n\t"
	"EXIT4f45:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_3R_1W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value
		mov r10, lsz

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+40h]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm2

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+80h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+0c0h]
		vmovdqa64 zmmword ptr [WriteADDRESS+40h], zmm2

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+100h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+140h]
		vmovdqa64 zmmword ptr [WriteADDRESS+80h], zmm2

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+180h]
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS+1c0h]
		vmovdqa64 zmmword ptr [WriteADDRESS+0c0h], zmm2
		
		add LocalADDRESS, 200h
		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value

	    "LOOPSf46:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 (%2),%%zmm0\n\t"
	    "vmovdqa64 0x40(%2),%%zmm0\n\t"
	    "vmovdqa64 %%zmm1,(%4)\n\t"

	    "vmovdqa64 0x80(%2),%%zmm0\n\t"
	    "vmovdqa64 0xc0(%2),%%zmm0\n\t"
	    "vmovdqa64 %%zmm1,0x40(%4)\n\t"

	    "vmovdqa64 0x100(%2),%%zmm0\n\t"
	    "vmovdqa64 0x140(%2),%%zmm0\n\t"
	    "vmovdqa64 %%zmm1,0x80(%4)\n\t"

	    "vmovdqa64 0x180(%2),%%zmm0\n\t"
	    "vmovdqa64 0x1c0(%2),%%zmm0\n\t"
	    "vmovdqa64 %%zmm1,0xc0(%4)\n\t"

	    "add $0x200,%2\n\t"
	    "add $0x100,%4\n\t"

	"FLAG4f46:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f46:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f46\n\t"
	    
        "STOP_DELAYf46:\n\t"

	    "cmp %3, %2\n\t"
	    "jl LOOPSf46\n\t"
	"EXIT4f46:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#endif

char* do_independent_load_rwlocal_50R_50W_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		movdqa xmmword ptr [WriteADDRESS], xmm1
		movdqa xmmword ptr [WriteADDRESS+40h], xmm1
		movdqa xmmword ptr [WriteADDRESS+80h], xmm1
		movdqa xmmword ptr [WriteADDRESS+0c0h], xmm1

		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
    "LOOPSf11a:\n\t"
	    // Unrolled 4 times
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x40(%4)\n\t"
	    "movdqa %%xmm0,0x80(%4)\n\t"
	    "movdqa %%xmm0,0x0c0(%4)\n\t"

	    "add $0x100,%4\n\t"

	"FLAG4f11a:\n\t"
	    "xor %1,%1\n\t"
       
    "LOOP4f11a:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f11a\n\t"
        
	    "STOP_DELAYf11a:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf11a\n\t"
	"EXIT4f11a:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");

    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_50R_50W_avx256_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		vmovdqa ymmword ptr [WriteADDRESS], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+40h], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+80h], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+0c0h], ymm1

		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
    "LOOPSf11b:\n\t"
	    // Unrolled 4 times
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x40(%4)\n\t"
	    "vmovdqa %%ymm0,0x80(%4)\n\t"
	    "vmovdqa %%ymm0,0x0c0(%4)\n\t"

	    "add $0x100,%4\n\t"

	"FLAG4f11b:\n\t"
	    "xor %1,%1\n\t"
       
    "LOOP4f11b:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f11b\n\t"
        
	    "STOP_DELAYf11b:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf11b\n\t"
	"EXIT4f11b:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");

    return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_50R_50W_avx512_64B(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		vmovdqa64 zmmword ptr [WriteADDRESS+40h], zmm1
		vmovdqa64 zmmword ptr [WriteADDRESS+80h], zmm1
		vmovdqa64 zmmword ptr [WriteADDRESS+0c0h], zmm1

		add WriteADDRESS, 100h

    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
    "LOOPSf11c:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "vmovdqa64 %%zmm0,0x40(%4)\n\t"
	    "vmovdqa64 %%zmm0,0x80(%4)\n\t"
	    "vmovdqa64 %%zmm0,0x0c0(%4)\n\t"

	    "add $0x100,%4\n\t"

	"FLAG4f11c:\n\t"
	    "xor %1,%1\n\t"
       
    "LOOP4f11c:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f11c\n\t"
        
	    "STOP_DELAYf11c:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf11c\n\t"
	"EXIT4f11c:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");

    return ret_ptr;
#endif
}
#endif
char* do_independent_load_NUMA1(char* local_buf, char* remote_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, remote_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz

		LOOPS:
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		add WriteADDRESS, r10
		
		movdqa xmm1, xmmword ptr [RemoteADDRESS]
		add RemoteADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		add WriteADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [RemoteADDRESS]
		add RemoteADDRESS, r10
		movdqa xmmword ptr [WriteADDRESS], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
	    "LOOPSf1:\n\t"
	    "\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"
		
	    "movdqa (%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
		
	    "movdqa %%xmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"

	    "movdqa (%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f1\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f1\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f1:\n\t"
	    "mov %1,%0\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
	    "LOOP4f1:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f1\n\t"
	    "STOP_DELAYf1:\n\t"
	    "mov %0,%1\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf1\n\t"
	"EXIT4f1:\n\t"
		"mov %2, %%rax\n\t"	// set the return value
	    ::"a"(0), COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(remote_buf), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10");
		asm (
			"mov %%rax, %0\n\t"
		:"=r"(ret_ptr)::"r9"); // needs some clobber register to be mentioned to make it compile on older gcc
    return ret_ptr;
#endif
}
char* do_independent_load_NUMA2(char* local_buf, char* remote_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, remote_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		movdqa xmmword ptr [LocalADDRESS], xmm0
		add LocalADDRESS, r10
		movdqa xmmword ptr [LocalADDRESS], xmm0
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [RemoteADDRESS]
		add RemoteADDRESS, r10
		movdqa xmm1, xmmword ptr [RemoteADDRESS]
		add RemoteADDRESS, r10
		movdqa xmmword ptr [RemoteADDRESS], xmm0
		add RemoteADDRESS, r10

		inc r11 
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"	    
		"LOOPSfN3:\n\t"
	    "\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"

	    "movdqa %%xmm0,(%2)\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa %%xmm0,(%2)\n\t"
	    "add %%r9,%2\n\t"

	    "movdqa (%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa (%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa %%xmm0,(%5)\n\t"
	    "add %%r9,%5\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4fN3\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4fN3\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4fN3:\n\t"
	    "mov %1,%0\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
	    "LOOP4fN3:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4fN3\n\t"
	    "STOP_DELAYfN3:\n\t"
	    "mov %0,%1\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSfN3\n\t"
	"EXIT4fN3:\n\t"
		"mov %2, %%rax\n\t"	// set the return value
	    ::"a"(0), COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(remote_buf), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10");
		asm (
			"mov %%rax, %0\n\t"
		:"=r"(ret_ptr)::"r9"); // needs some clobber register to be mentioned to make it compile on older gcc
    return ret_ptr;
#endif
}
char* do_independent_load_NUMA3(char* local_buf, char* remote_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, remote_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp

		LOOPS:

		mov rax, [LocalADDRESS]
		mov rax, [RemoteADDRESS]
		mov rax, [LocalADDRESS+40h]
		mov [WriteADDRESS], r10
		mov rax, [LocalADDRESS+80h]
		mov [WriteADDRESS+40h], r10

		mov rax, [RemoteADDRESS+40h]
		mov [RemoteADDRESS+80h], r10
		mov rax, [RemoteADDRESS+0c0h]

		mov rax, [LocalADDRESS+0c0h]
		mov rax, [RemoteADDRESS+100h]
		mov rax, [LocalADDRESS+100h]
		mov [WriteADDRESS+80h], r10
		mov rax, [LocalADDRESS+140h]
		mov [RemoteADDRESS+140h], r10

		mov rax, [RemoteADDRESS+180h]
		mov [RemoteADDRESS+1c0h], r10
		mov rax, [RemoteADDRESS+200h]

		add LocalADDRESS, 180h
		add RemoteADDRESS, 240h
		add WriteADDRESS, 0c0h

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:

		cmp RemoteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, RemoteADDRESS
	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
	    "LOOPSf3:\n\t"
	    "\n\t"
	    "mov (%2),%0\n\t"
	    "mov 0x40(%2),%0\n\t"
	    "mov 0x80(%2),%0\n\t"
	    "mov 0xc0(%2),%0\n\t"
	    "mov %0,0x100(%2)\n\t"
	    "mov %0,0x140(%2)\n\t"

	    "mov (%5),%0\n\t"
	    "mov 0x40(%5),%0\n\t"
	    "mov 0x80(%5),%0\n\t"
	    "mov 0xc0(%5),%0\n\t"
	    "mov %0,0x100(%5)\n\t"
	    "mov %0,0x140(%5)\n\t"

	    "\n\t"
	    "add $0x180,%2\n\t"
	    "add $0x180,%5\n\t"
	    "\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f3\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f3\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f3:\n\t"
	    "mov %1,%0\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
	    "LOOP4f3:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f3\n\t"
	    "STOP_DELAYf3:\n\t"
	    "mov %0,%1\n\t"
	    "cmp %3, %5\n\t"
	    "jl LOOPSf3\n\t"
	"EXIT4f3:\n\t"
		"mov %5, %%rax\n\t"	// set the return value
	    ::"a"(0), COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(remote_buf), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10");
		asm (
			"mov %%rax, %0\n\t"
		:"=r"(ret_ptr)::"r9"); // needs some clobber register to be mentioned to make it compile on older gcc
    return ret_ptr;
#endif
}
char* do_independent_load_NUMA3_rdonly(char* local_buf, char* remote_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, remote_buf
		mov ENDP, endp

		LOOPS:

		mov rax, [LocalADDRESS]
		mov rax, [RemoteADDRESS]
		mov rax, [LocalADDRESS+40h]
		mov rax, [LocalADDRESS+80h]
		mov rax, [RemoteADDRESS+40h]
		mov rax, [RemoteADDRESS+80h]

		mov rax, [LocalADDRESS+0c0h]
		mov rax, [RemoteADDRESS+0c0h]
		mov rax, [LocalADDRESS+100h]
		mov rax, [LocalADDRESS+140h]
		mov rax, [RemoteADDRESS+100h]
		mov rax, [RemoteADDRESS+140h]

		add LocalADDRESS, 180h
		add RemoteADDRESS, 180h

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:

		cmp RemoteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, RemoteADDRESS
	}
#else
    return NULL;
#endif
}

char* do_independent_load_rwlocal_3R_1W_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov r10, local_buf2
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm1, ENDP
		mov r9, lsz
		LOOPS:

		movdqa xmm0, [LocalADDRESS]
		movdqa xmm0, [LocalADDRESS+10h]
		movdqa xmm0, [LocalADDRESS+20h]
		movdqa xmm0, [LocalADDRESS+30h]
		add LocalADDRESS, r9
		movdqa xmm0, [r10]
		movdqa xmm0, [r10+10h]
		movdqa xmm0, [r10+20h]
		movdqa xmm0, [r10+30h]
		add r10, r9
		movdqa [WriteADDRESS], xmm1
		movdqa [WriteADDRESS+10h], xmm1
		movdqa [WriteADDRESS+20h], xmm1
		movdqa [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r9

		movdqa xmm0, [LocalADDRESS]
		movdqa xmm0, [LocalADDRESS+10h]
		movdqa xmm0, [LocalADDRESS+20h]
		movdqa xmm0, [LocalADDRESS+30h]
		add LocalADDRESS, r9
		movdqa xmm0, [r10]
		movdqa xmm0, [r10+10h]
		movdqa xmm0, [r10+20h]
		movdqa xmm0, [r10+30h]
		add r10, r9
		movdqa [WriteADDRESS], xmm1
		movdqa [WriteADDRESS+10h], xmm1
		movdqa [WriteADDRESS+20h], xmm1
		movdqa [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    
        "mov %7,%%r9\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value
	    "LOOPSf6NT18:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm0\n\t"           //     (local_buf) -> xmm0
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%5),%%xmm0\n\t"           //     (local_buf2) -> xmm0
	    "movdqa 0x10(%5),%%xmm0\n\t"
	    "movdqa 0x20(%5),%%xmm0\n\t"
	    "movdqa 0x30(%5),%%xmm0\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "movdqa (%2),%%xmm0\n\t"           //     (local_buf) -> xmm0
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%5),%%xmm0\n\t"           //     (local_buf2) -> xmm0
	    "movdqa 0x10(%5),%%xmm0\n\t"
	    "movdqa 0x20(%5),%%xmm0\n\t"
	    "movdqa 0x30(%5),%%xmm0\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r9,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT18\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6NT18\n\t"
		"xor %%r11, %%r11\n\t"
	
    "FLAG4f6NT18:\n\t"
        "xor %1, %1\n\t"
        
	    "LOOP4f6NT18:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT18\n\t"
	    
        "STOP_DELAYf6NT18:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT18\n\t"
	"EXIT4f6NT18:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        : COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"xmm0", "r9","r11");
    
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov r10, local_buf2
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value
		mov r9, lsz
		LOOPS:

		vmovdqa ymm0, [LocalADDRESS]
		vmovdqa ymm0, [LocalADDRESS+20h]
		add LocalADDRESS, r9
		vmovdqa ymm0, [r10]
		vmovdqa ymm0, [r10+20h]
		add r10, r9
		vmovdqa [WriteADDRESS], ymm1
		vmovdqa [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r9

		vmovdqa ymm0, [LocalADDRESS]
		vmovdqa ymm0, [LocalADDRESS+20h]
		add LocalADDRESS, r9
		vmovdqa ymm0, [r10]
		vmovdqa ymm0, [r10+20h]
		add r10, r9
		vmovdqa [WriteADDRESS], ymm1
		vmovdqa [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
        
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value
        
	    "LOOPSf6NT18Y:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm0\n\t"           //     (local_buf) -> ymm0
	    "vmovdqa 0x20(%2),%%ymm0\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%5),%%ymm0\n\t"           //     (local_buf2) -> ymm0
	    "vmovdqa 0x20(%5),%%ymm0\n\t"
	    "add %%r9,%5\n\t"
	    "vmovdqa %%ymm1,(%4)\n\t"
	    "vmovdqa %%ymm1,0x20(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "vmovdqa (%2),%%ymm0\n\t"           //     (local_buf) -> ymm0
	    "vmovdqa 0x20(%2),%%ymm0\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%5),%%ymm0\n\t"           //     (local_buf2) -> ymm0
	    "vmovdqa 0x20(%5),%%ymm0\n\t"
	    "add %%r9,%5\n\t"
	    "vmovdqa %%ymm1,(%4)\n\t"
	    "vmovdqa %%ymm1,0x20(%4)\n\t"
	    "add %%r9,%4\n\t"
	
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT18Y\n\t"
        
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6NT18Y\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6NT18Y:\n\t"
	    "xor %1,%1\n\t"
        
	    "\n\t"
	    "LOOP4f6NT18Y:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT18Y\n\t"
        
	    "STOP_DELAYf6NT18Y:\n\t"

	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT18Y\n\t"
        
	"EXIT4f6NT18Y:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"xmm0", "r9", "r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_3R_1W_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov r10, local_buf2
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r9, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value
		LOOPS:

		vmovdqa64 zmm0, [LocalADDRESS]
		add LocalADDRESS, r9
		vmovdqa64 zmm0, [r10]
		add r10, r9
		vmovdqa64 [WriteADDRESS], zmm1
		add WriteADDRESS, r9

		vmovdqa64 zmm0, [LocalADDRESS]
		add LocalADDRESS, r9
		vmovdqa64 zmm0, [r10]
		add r10, r9
		vmovdqa64 [WriteADDRESS], zmm1
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
        "xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value

	    "LOOPSf6NT18Z:\n\t"
	    // Unrolled 2 times
	    "vmovdqa64 (%2),%%zmm0\n\t"     //     (local_buf) -> zmm0
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%5),%%zmm0\n\t"     //     (local_buf2) -> zmm0
	    "add %%r9,%5\n\t"
	    "vmovdqa64 %%zmm1,(%4)\n\t"      //     zmm1 -> (write_buf)
	    "add %%r9,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm0\n\t"     //     (local_buf) -> zmm0
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%5),%%zmm0\n\t"     //     (local_buf2) -> zmm0
	    "add %%r9,%5\n\t"
	    "vmovdqa64 %%zmm1,(%4)\n\t"      //     zmm1 -> (write_buf)
	    "add %%r9,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT18Z\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6NT18Z\n\t"
		"xor %%r11, %%r11\n\t"

	"FLAG4f6NT18Z:\n\t"
	    "xor %1,%1\n\t"
        
	    "LOOP4f6NT18Z:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT18Z\n\t"
	    
        "STOP_DELAYf6NT18Z:\n\t"
	   
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT18Z\n\t"
        
	"EXIT4f6NT18Z:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}

#endif

char* do_independent_load_rwlocal_3R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value
		mov r10, lsz

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmmword ptr [WriteADDRESS], xmm2
		movdqa xmmword ptr [WriteADDRESS+10h], xmm2
		movdqa xmmword ptr [WriteADDRESS+20h], xmm2
		movdqa xmmword ptr [WriteADDRESS+30h], xmm2
		add WriteADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmmword ptr [WriteADDRESS], xmm2
		movdqa xmmword ptr [WriteADDRESS+10h], xmm2
		movdqa xmmword ptr [WriteADDRESS+20h], xmm2
		movdqa xmmword ptr [WriteADDRESS+30h], xmm2
		add WriteADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmmword ptr [WriteADDRESS], xmm2
		movdqa xmmword ptr [WriteADDRESS+10h], xmm2
		movdqa xmmword ptr [WriteADDRESS+20h], xmm2
		movdqa xmmword ptr [WriteADDRESS+30h], xmm2
		add WriteADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmmword ptr [WriteADDRESS], xmm2
		movdqa xmmword ptr [WriteADDRESS+10h], xmm2
		movdqa xmmword ptr [WriteADDRESS+20h], xmm2
		movdqa xmmword ptr [WriteADDRESS+30h], xmm2
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm1\n\t"				// initialize xmm1 with some non-zero value

	    "LOOPSf4:\n\t"
	    // Unrolled 4 times
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm1,(%4)\n\t"
	    "movdqa %%xmm1,0x10(%4)\n\t"
	    "movdqa %%xmm1,0x20(%4)\n\t"
	    "movdqa %%xmm1,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f4\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f4\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f4:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f4:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f4\n\t"
	    
        "STOP_DELAYf4:\n\t"

	    "cmp %3, %2\n\t"
	    "jl LOOPSf4\n\t"
	"EXIT4f4:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_3R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add WriteADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add WriteADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add WriteADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf8:\n\t"
	    // Unrolled 4 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f8\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f8\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f8:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f8:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f8\n\t"
	    
        "STOP_DELAYf8:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf8\n\t"
	"EXIT4f8:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_3R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf4Z:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f4Z\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f4Z\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f4Z:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f4Z:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f4Z\n\t"
        
	    "STOP_DELAYf4Z:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf4Z\n\t"
	"EXIT4f4Z:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_3R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10
		
		sfence

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf4Zr:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4)\n\t"
	    "add %%r10,%4\n\t"
		
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"sfence \n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f4Zr\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f4Zr\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f4Zr:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f4Zr:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f4Zr\n\t"
        
	    "STOP_DELAYf4Zr:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf4Zr\n\t"
	"EXIT4f4Zr:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}


#endif

char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov r9, local_buf2
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [r9]
		movdqa xmm1, xmmword ptr [r9+10h]
		movdqa xmm1, xmmword ptr [r9+20h]
		movdqa xmm1, xmmword ptr [r9+30h]
		add r9, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, xmmword ptr [r9]
		movdqa xmm1, xmmword ptr [r9+10h]
		movdqa xmm1, xmmword ptr [r9+20h]
		movdqa xmm1, xmmword ptr [r9+30h]
		add r9, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6NT17:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%5),%%xmm1\n\t"
	    "movdqa 0x10(%5),%%xmm1\n\t"
	    "movdqa 0x20(%5),%%xmm1\n\t"
	    "movdqa 0x30(%5),%%xmm1\n\t"
	    "add %%r10,%5\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%5),%%xmm1\n\t"
	    "movdqa 0x10(%5),%%xmm1\n\t"
	    "movdqa 0x20(%5),%%xmm1\n\t"
	    "movdqa 0x30(%5),%%xmm1\n\t"
	    "add %%r10,%5\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT17\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6NT17\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6NT17:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6NT17:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT17\n\t"
        
	    "STOP_DELAYf6NT17:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT17\n\t"
        
	"EXIT4f6NT17:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx256(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov r9, local_buf2
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [r9]
		vmovdqa ymm1, ymmword ptr [r9+20h]
		add r9, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, ymmword ptr [r9]
		vmovdqa ymm1, ymmword ptr [r9+20h]
		add r9, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "mov %7,%%r10\n\t"
	    "LOOPSf6NT19:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT19\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6NT19\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6NT19:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6NT19:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT19\n\t"
        
	    "STOP_DELAYf6NT19:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT19\n\t"
	"EXIT4f6NT19:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_2R_1NTW_diff_addr_stream_avx512(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov r9, local_buf2
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [r9]
		add r9, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, zmmword ptr [r9]
		add r9, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"	
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6NT97:\n\t"
	    // Unrolled 2 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%5),%%zmm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%5),%%zmm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6NT97\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6NT97:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6NT97:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6NT97\n\t"
        
	    "STOP_DELAYf6NT97:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6NT97\n\t"
	"EXIT4f6NT97:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#endif
char* do_independent_random_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	app_exit(EINVAL, "Not implemented\n");
	return NULL;
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd7:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"movdqa (%0, %%rbx), %%xmm0\n\t"  
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"vmovntdq %%ymm1, (%7, %%rbx)\n\t"
			"vmovntdq %%ymm1, 0x20(%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"vmovntdq %%ymm1, (%7, %%rbx)\n\t"
			"vmovntdq %%ymm1, 0x20(%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"vmovntdq %%ymm1, (%7, %%rbx)\n\t"
			"vmovntdq %%ymm1, 0x20(%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"vmovntdq %%ymm1, (%7, %%rbx)\n\t"
			"vmovntdq %%ymm1, 0x20(%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd7:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd7\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd7\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "m"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
	return (local_buf + i*LineSize);
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_random_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	//__int64 blk_offset;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	app_exit(EINVAL, "Random not implemented for this traffic type\n");
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd77:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"  
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovntdq %%zmm1, (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovntdq %%zmm1, (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovntdq %%zmm1, (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovntdq %%zmm1, (%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd77:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd77\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd77\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "m"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
#endif

char* do_independent_load_rwlocal_2R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"	
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// inqitialize xmm0 with some non-zero value
        
	    "LOOPSf13:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f13\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f13\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f13:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f13:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f13\n\t"
        
	    "STOP_DELAYf13:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf13\n\t"
        
	"EXIT4f13:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_2R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf14:\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f14\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f14\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f14:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f14:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f14\n\t"
        
	    "STOP_DELAYf14:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf14\n\t"
	"EXIT4f14:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_2R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf92:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f92\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f92\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f92:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f92:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f92\n\t"
        
	    "STOP_DELAYf92:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf92\n\t"
	"EXIT4f92:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R_1NTW_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		sfence
		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf92_mf:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"

		"sfence\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f92_mf\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f92_mf\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f92_mf:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f92_mf:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f92_mf\n\t"
        
	    "STOP_DELAYf92_mf:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf92_mf\n\t"
	"EXIT4f92_mf:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}


#endif

char* do_independent_load_rwlocal_3R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf29:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f29\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f29\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f29:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f29:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f29\n\t"
        
	    "STOP_DELAYf29:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf29\n\t"
	"EXIT4f29:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_1R_1NTW(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		movdqa xmm1, [LocalADDRESS]
		movdqa xmm1, [LocalADDRESS+10h]
		movdqa xmm1, [LocalADDRESS+20h]
		movdqa xmm1, [LocalADDRESS+30h]
		add LocalADDRESS, r10
		movntdq [WriteADDRESS], xmm0
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf25:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f25\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f25\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f25:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f25:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f25\n\t"
        
	    "STOP_DELAYf25:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf25\n\t"
	"EXIT4f25:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_1R_1NTW_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value
		LOOPS:

		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		vmovdqa ymm1, [LocalADDRESS]
		vmovdqa ymm1, [LocalADDRESS+20h]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], ymm0
		vmovntdq [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf26:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f26\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f26\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f26:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f26:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f26\n\t"
        
	    "STOP_DELAYf26:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf26\n\t"
	"EXIT4f26:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_1R_1NTW_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value
		LOOPS:

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmm1, [LocalADDRESS]
		add LocalADDRESS, r10
		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf27:\n\t"
	    // Unrolled 2 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f27\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f27\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f27:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f27:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f27\n\t"
        
	    "STOP_DELAYf27:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf27\n\t"
	"EXIT4f27:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#endif
char* do_independent_load_rw_all_remote(char* remote_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov RemoteADDRESS, remote_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, [RemoteADDRESS]
		movdqa xmm1, [RemoteADDRESS+10h]
		movdqa xmm1, [RemoteADDRESS+20h]
		movdqa xmm1, [RemoteADDRESS+30h]
		add RemoteADDRESS, r10

		movdqa xmm1, [RemoteADDRESS]
		movdqa xmm1, [RemoteADDRESS+10h]
		movdqa xmm1, [RemoteADDRESS+20h]
		movdqa xmm1, [RemoteADDRESS+30h]
		add RemoteADDRESS, r10

		movdqa [WriteADDRESS], xmm0
		movdqa [WriteADDRESS+10h], xmm0
		movdqa [WriteADDRESS+20h], xmm0
		movdqa [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp RemoteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, RemoteADDRESS
	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value

	    "LOOPSf5:\n\t"
	    "movdqa (%5),%%xmm1\n\t"
	    "movdqa 0x10(%5),%%xmm1\n\t"
	    "movdqa 0x20(%5),%%xmm1\n\t"
	    "movdqa 0x30(%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa (%5),%%xmm1\n\t"
	    "movdqa 0x10(%5),%%xmm1\n\t"
	    "movdqa 0x20(%5),%%xmm1\n\t"
	    "movdqa 0x30(%5),%%xmm1\n\t"
	    "add %%r9,%5\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r9,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f5\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f5\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f5:\n\t"
		
	    "mov %1,%0\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
	    "LOOP4f5:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f5\n\t"
	    "STOP_DELAYf5:\n\t"
	    "mov %0,%1\n\t"
	    "cmp %3, %5\n\t"
	    "jl LOOPSf5\n\t"
	"EXIT4f5:\n\t"
		"mov %5, %%rax\n\t"	// set the return value
	    ::"a"(0), COUNTER(0), LocalADDRESS(0), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(remote_buf), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10","r11");
		asm (
			"mov %%rax, %0\n\t"
		:"=r"(ret_ptr)::"r9"); // needs some clobber register to be mentioned to make it compile on older gcc
		return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9
		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r9

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9
		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
 		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6:\n\t"
	    // Unrolled 2 times
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r9,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6\n\t"
        
	    "STOP_DELAYf6:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf6\n\t"
        
	"EXIT4f6:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
		
     return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R_1W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm2, ENDP  ; Initialize xmm2 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		vmovdqa ymmword ptr [WriteADDRESS], ymm2
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm2
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf7:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f7\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f7\n\t"
		"xor %%r11, %%r11\n\t"

		"FLAG4f7:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f7:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f7\n\t"
        
	    "STOP_DELAYf7:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf7\n\t"
        
	"EXIT4f7:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");

    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_2R_1W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER
		
		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
		"mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf61:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f61\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f61\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f61:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f61:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f61\n\t"
        
	    "STOP_DELAYf61:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf61\n\t"
	"EXIT4f61:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    
     return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		clflushopt [WriteADDRESS]
		add LocalADDRESS, r10
		add WriteADDRESS, r10

		sfence
		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER
		
		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
		"mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf61f:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"
		
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"
		
		"sfence \n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f61f\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f61f\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f61f:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f61f:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f61f\n\t"
        
	    "STOP_DELAYf61f:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf61f\n\t"
	"EXIT4f61f:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    
     return ret_ptr;
#endif
}

#endif


char* do_independent_load_rwlocal_80R_20W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf34:\n\t"
	    // 3 reads and a write - No unrolling
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f34\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f34\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f34:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f34:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f34\n\t"
        
	    "STOP_DELAYf34:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf34\n\t"
	"EXIT4f34:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_80R_20W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm0
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf35:\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f35\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f35\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f35:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f35:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f35\n\t"
        
	    "STOP_DELAYf35:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf35\n\t"
	"EXIT4f35:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_80R_20W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf36:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f36\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f36\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f36:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f36:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f36\n\t"
        
	    "STOP_DELAYf36:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf36\n\t"
        
	"EXIT4f36:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

#endif

char* do_independent_load_rwlocal_5R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf51:\n\t"
	    // 3 reads and a write - No unrolling
	     "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f51\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f51\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f51:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f51:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f51\n\t"
        
	    "STOP_DELAYf51:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf51\n\t"
	"EXIT4f51:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_6R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf16:\n\t"
	    // 3 reads and a write - No unrolling
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f16\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f16\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f16:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f16:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f16\n\t"
        
	    "STOP_DELAYf16:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf16\n\t"
	"EXIT4f16:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_7R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf71:\n\t"
	    // 3 reads and a write - No unrolling
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f71\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f71\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f71:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f71:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f71\n\t"
        
	    "STOP_DELAYf71:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf71\n\t"
	"EXIT4f71:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}


char* do_independent_load_rwlocal_8R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf81:\n\t"
	    // 3 reads and a write - No unrolling
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f81\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f81\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f81:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f81:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f81\n\t"
        
	    "STOP_DELAYf81:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf81\n\t"
	"EXIT4f81:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_9R_1W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, LocalADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf91:\n\t"
	    // 3 reads and a write - No unrolling
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f91\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f91\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f91:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f91:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f91\n\t"
        
	    "STOP_DELAYf91:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf91\n\t"
	"EXIT4f91:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
	
    return ret_ptr;
#endif
}




char* do_independent_load_rwlocal_60R_40W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm0
		movdqa xmmword ptr [WriteADDRESS+10h], xmm0
		movdqa xmmword ptr [WriteADDRESS+20h], xmm0
		movdqa xmmword ptr [WriteADDRESS+30h], xmm0
		add WriteADDRESS, r10


		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf37:\n\t"
	    "\n\t"
	    "movdqa (%2),%%xmm1\n\t"
	    "movdqa 0x10(%2),%%xmm1\n\t"
	    "movdqa 0x20(%2),%%xmm1\n\t"
	    "movdqa 0x30(%2),%%xmm1\n\t"
	    "add %%r10,%2\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f37\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f37\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f37:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f37:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f37\n\t"
        
	    "STOP_DELAYf37:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf37\n\t"
        
	"EXIT4f37:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_60R_40W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm0
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm0
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf38:\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f38\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f38\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f38:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f38:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f38\n\t"
        
	    "STOP_DELAYf38:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf38\n\t"
        
	"EXIT4f38:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_60R_40W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm0
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf39:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f39\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f39\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f39:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f39:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f39\n\t"
        
	    "STOP_DELAYf39:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf39\n\t"
        
	"EXIT4f39:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

#endif
char* do_independent_load_rwlocal_50R_50W(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11
		
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		movdqa xmmword ptr [WriteADDRESS], xmm1
		movdqa xmmword ptr [WriteADDRESS+10h], xmm1
		movdqa xmmword ptr [WriteADDRESS+20h], xmm1
		movdqa xmmword ptr [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm1
		movdqa xmmword ptr [WriteADDRESS+10h], xmm1
		movdqa xmmword ptr [WriteADDRESS+20h], xmm1
		movdqa xmmword ptr [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm1
		movdqa xmmword ptr [WriteADDRESS+10h], xmm1
		movdqa xmmword ptr [WriteADDRESS+20h], xmm1
		movdqa xmmword ptr [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r10

		movdqa xmmword ptr [WriteADDRESS], xmm1
		movdqa xmmword ptr [WriteADDRESS+10h], xmm1
		movdqa xmmword ptr [WriteADDRESS+20h], xmm1
		movdqa xmmword ptr [WriteADDRESS+30h], xmm1
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf11:\n\t"
	    // Unrolled 4 times
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "movdqa %%xmm0,(%4)\n\t"
	    "movdqa %%xmm0,0x10(%4)\n\t"
	    "movdqa %%xmm0,0x20(%4)\n\t"
	    "movdqa %%xmm0,0x30(%4)\n\t"
	    "add %%r10,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f11\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f11\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f11:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f11:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f11\n\t"
        
	    "STOP_DELAYf11:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf11\n\t"
	"EXIT4f11:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");

    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_50R_50W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		vmovdqa ymmword ptr [WriteADDRESS], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r10

		vmovdqa ymmword ptr [WriteADDRESS], ymm1
		vmovdqa ymmword ptr [WriteADDRESS+20h], ymm1
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value

	    "LOOPSf12:\n\t"
	    // Unrolled 4 times
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa %%ymm0,(%4)\n\t"
	    "vmovdqa %%ymm0,0x20(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f12\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f12\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f12:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f12:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f12\n\t"
        
	    "STOP_DELAYf12:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf12\n\t"
        
	"EXIT4f12:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_50R_50W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		add WriteADDRESS, r10

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
    exit_now:
		mov rax, WriteADDRESS
		jl LOOPS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf62:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
	    "vmovdqa64 %%zmm0,(%4)\n\t"
	    "add %%r10,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62\n\t"
        
	    "STOP_DELAYf62:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf62\n\t"
	"EXIT4f62:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
		
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, lsz
		movq xmm1, ENDP  ; Initialize xmm1 with any non zero value

		LOOPS:

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		vmovdqa64 zmmword ptr [WriteADDRESS], zmm1
		clflushopt [WriteADDRESS]
		add WriteADDRESS, r10

		sfence
		
		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
    exit_now:
		mov rax, WriteADDRESS
		jl LOOPS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf62f:\n\t"
	    // Unrolled 4 times
	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa64 %%zmm0,(%4)\n\t"
		"clflushopt (%4) \n\t"
	    "add %%r10,%4\n\t"
		
		"sfence\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62f\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62f\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62f:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62f:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62f\n\t"
        
	    "STOP_DELAYf62f:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf62f\n\t"
	"EXIT4f62f:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
		
    return ret_ptr;
#endif
}

#endif
char* do_independent_load_rwlocal_100W(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value

		mov r10, partial_ratio
		LOOPS:

		movntdq [WriteADDRESS], xmm0
		dec r10
		jnz	CONTINUE
		mov r10, partial_ratio
		jmp END_OF_64B_LINE
		CONTINUE:
		movntdq [WriteADDRESS+10h], xmm0
		movntdq [WriteADDRESS+20h], xmm0
		movntdq [WriteADDRESS+30h], xmm0

		END_OF_64B_LINE:
		add WriteADDRESS, 40h

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %8,%%r9\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf9:\n\t"
	    "movntdq %%xmm0,(%4)\n\t"
	    "dec %%r10\n\t"
	    "jnz CONTINUE9\n\t"
	    "mov %7,%%r10\n\t"
	    "jmp END_OF_64B_LINE9\n\t"
	    "CONTINUE9:\n\t"
	    "movntdq %%xmm0,0x10(%4)\n\t"
	    "movntdq %%xmm0,0x20(%4)\n\t"
	    "movntdq %%xmm0,0x30(%4)\n\t"
	    "END_OF_64B_LINE9:\n\t"
	    "add %%r9,%4\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %9, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f9\n\t"
		"cmpl $0, %10\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f9\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f9:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f9:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f9\n\t"
        
	    "STOP_DELAYf9:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf9\n\t"
        
	"EXIT4f9:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(partial_ratio), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_100W_avx256(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio)
{
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp
		mov r10, partial_ratio
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value
		LOOPS:

		vmovntdq [WriteADDRESS], ymm0
		dec r10
		jnz	CONTINUE
		mov r10, partial_ratio
		jmp END_OF_64B_LINE
		CONTINUE:
		vmovntdq [WriteADDRESS+20h], ymm0

		END_OF_64B_LINE:
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %8,%%r9\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf10:\n\t"
	    "vmovntdq %%ymm0,(%4)\n\t"
	    "dec %%r10\n\t"
	    "jnz CONTINUE10\n\t"
	    "mov %7,%%r10\n\t"
	    "jmp END_OF_64B_LINE10\n\t"
	    "CONTINUE10:\n\t"
	    "vmovntdq %%ymm0,0x20(%4)\n\t"
	    "END_OF_64B_LINE10:\n\t"
	    "add %%r9,%4\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %9, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f10\n\t"
		"cmpl $0, %10\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f10\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f10:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f10:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f10\n\t"
        
	    "STOP_DELAYf10:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf10\n\t"
        
	"EXIT4f10:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(partial_ratio), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10","r11");
    return ret_ptr;
#endif
}

#ifdef AVX512_SUPPORT
char* do_independent_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio)
{
// Partial here does not make sense and will be ignored as we write the entire 64 byte line with one instruction
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp

		mov r10, partial_ratio
		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value
		LOOPS:

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %8,%%r9\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf63:\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"

        
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %9, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f63\n\t"
		"cmp $0, %10\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f63\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f63:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f63:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f63\n\t"
        
	    "STOP_DELAYf63:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf63\n\t"
	"EXIT4f63:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(partial_ratio), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rwlocal_100W_avx512_mfence(char* local_buf, char* write_buf , char* endp, int delay_t, __int64 partial_ratio)
{
// Partial here does not make sense and will be ignored as we write the entire 64 byte line with one instruction
	__int64 lsz = (__int64) LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor COUNTER, COUNTER
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov WriteADDRESS, write_buf
		mov ENDP, endp

		mov r9, lsz
		movq xmm0, ENDP  ; Initialize xmm0 with any non zero value
		LOOPS:

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		vmovntdq [WriteADDRESS], zmm0
		add WriteADDRESS, r9

		mfence
		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
    cont11:
		xor COUNTER, COUNTER

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp WriteADDRESS, ENDP
		jl LOOPS
    exit_now:
		mov rax, WriteADDRESS
	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %8,%%r9\n\t"
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
        
	    "LOOPSf6399:\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"

	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"
	    "vmovntdq %%zmm0,(%4)\n\t"
	    "add %%r9,%4\n\t"

		"mfence\n\t"
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %9, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f6399\n\t"
		"cmp $0, %10\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f6399\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f6399:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f6399:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f6399\n\t"
        
	    "STOP_DELAYf6399:\n\t"
	    "cmp %3, %4\n\t"
	    "jl LOOPSf6399\n\t"
	"EXIT4f6399:\n\t"
		"mov %4, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(0), "r"(delay_t), "m"(partial_ratio), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r10","r11");
    return ret_ptr;
#endif
}

#endif

char* do_independent_random_load_rdonly(char* local_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, local_buf
			add rax, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
		WINL01:
			mov ebx, [rsi+r8*1]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+4]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+8]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+0ch]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+10h]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+14h]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+18h]
			movdqa xmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+1ch]
			movdqa xmm0, [rax+rbx*1]
		
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd1:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd1:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd1\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd1\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}

#ifdef AVX512_SUPPORT
char* do_independent_random_load_rdonly_avx512_128B_block(char* local_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	//__int64 blk_offset;
	 numEntries = sizeof(int)* max_rand_size_bw_gen/2; // divided by 2 as each entry can cover 128 Bytes
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd128:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd128:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd128\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd128\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_128B), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
char* do_independent_random_load_rdonly_avx512_256B_block(char* local_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	//__int64 blk_offset;
	 numEntries = sizeof(int)* max_rand_size_bw_gen/4; // divided by 4 as each entry can cover 256Bytes
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd256:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x40(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0x80(%0, %%rbx), %%zmm0\n\t"
			"vmovdqa64 0xc0(%0, %%rbx), %%zmm0\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd256:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd256\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd256\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_256B), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}

#endif


#ifdef AVX512_SUPPORT
char* do_independent_random_load_rdonly_avx512(char* local_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;
	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, local_buf
			add rax, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+4]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+8]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+0ch]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+10h]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+14h]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+18h]
			vmovdqa64 zmm0, [rax+rbx*1]
			mov ebx, [rsi+r8*1+1ch]
			vmovdqa64 zmm0, [rax+rbx*1]
		
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
"LOOPrnd1rr:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%zmm0\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd1rr:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd1rr\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd1rr\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}

#endif

char* do_independent_random_load_rwlocal_100W(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio)
{
     __int64 i, iterations, numEntries;
	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, write_buf
			add rax, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm0, rsi  ; Initialize xmm0 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+4]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+8]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+0ch]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+10h]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+14h]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+18h]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
			mov ebx, [rsi+r8*1+1ch]
			movntdq [rax+rbx*1], xmm0
			movntdq [rax+rbx*1+10h], xmm0
			movntdq [rax+rbx*1+20h], xmm0
			movntdq [rax+rbx*1+30h], xmm0
		
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
"LOOPrnd5:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"movntdq %%xmm0, (%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x10(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x20(%0, %%rbx)\n\t"
			"movntdq %%xmm0, 0x30(%0, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd5:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd5\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd5\n\t"
			
			::"r"(write_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}
#ifdef AVX512_SUPPORT
char* do_independent_random_load_rwlocal_100W_avx512(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio)
{
     __int64 i, iterations, numEntries;
	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, write_buf
			add rax, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm0, rsi  ; Initialize xmm0 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+4]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+8]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+0ch]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+10h]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+14h]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+18h]
			vmovntdq [rax+rbx*1], zmm0
			mov ebx, [rsi+r8*1+1ch]
			vmovntdq [rax+rbx*1], zmm0
		
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
"LOOPrnd55:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd55:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd55\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd55\n\t"
			
			::"r"(write_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_100W_avx512_128B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio)
{
     __int64 i=0, iterations, numEntries;
	//__int64 blk_offset;
	 numEntries = sizeof(int)* max_rand_size_bw_gen/2;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
"LOOPrnd46:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd46:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd46\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd46\n\t"
			
			::"r"(write_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_128B), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_100W_avx512_256B_block(char* local_buf, char* write_buf , __int64 buf_len, char* endp, int delay_t, __int64 partial_ratio)
{
     __int64 i=0, iterations, numEntries;
	//__int64 blk_offset;
	 numEntries = sizeof(int)* max_rand_size_bw_gen/4;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// TODO - add windows version
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm0\n\t"				// initialize xmm0 with some non-zero value
"LOOPrnd36:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovntdq %%zmm0, (%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x40(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0x80(%0, %%rbx)\n\t"
			"vmovntdq %%zmm0, 0xc0(%0, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd36:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd36\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd36\n\t"
			
			::"r"(write_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_256B), "r"(0UL), "r"(0), "r"(delay_t):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}
#endif

char* do_independent_random_load_rwlocal_50R_50W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+4]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+8]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+0ch]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+10h]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+14h]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+18h]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+1ch]
			movdqa [r11+rbx*1], xmm1

			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd3:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd3:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd3\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd3\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}

#ifdef AVX512_SUPPORT
char* do_independent_random_load_rwlocal_50R_50W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	// we use only 256-bit stores. As otherwise, ItoM is used and the line may not be read at all
	
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+4]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+8]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+0ch]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+10h]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+14h]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+18h]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+1ch]
			vmovdqa [r11+rbx*1], ymm1

			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd3a:\n\t"

			// we use only 256-bit stores. As otherwise, ItoM is used and the line may not be read at all
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd3a:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd3a\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd3a\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}

char* do_independent_random_load_rwlocal_50R_50W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+4]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+8]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+0ch]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+10h]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+14h]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+18h]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			mov ebx, [rsi+r8*1+1ch]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1] 

			sfence
			
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd3b:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"

			"sfence\n\t"
			
			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd3b:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd3b\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd3b\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}

char* do_independent_random_load_rwlocal_50R_50W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	// __int64 blk_offset;

	numEntries = sizeof(int)* max_rand_size_bw_gen/2;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// to be implemented for Windows
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd3b45:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"

			"sfence\n\t"
			
			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd3b45:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd3b45\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd3b45\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_128B), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_50R_50W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	// __int64 blk_offset;

	numEntries = sizeof(int)* max_rand_size_bw_gen/4;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// to be implemented for Windows
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd3b35:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"

			"sfence\n\t"
			
			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd3b35:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd3b35\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd3b35\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_256B), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (write_buf + i*LineSize);
}

#endif

char* do_independent_random_load_rwlocal_2R_1W(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, local_buf
			add rax, blk_offset
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+4]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+8]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+0ch]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+10h]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+14h]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+18h]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			mov ebx, [rsi+r8*1+1ch]
			movdqa xmm0, [rax+rbx*1]
			movdqa [r11+rbx*1], xmm1

			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd2:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"movdqa (%0, %%rbx), %%xmm0\n\t"  
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"movdqa (%0, %%rbx), %%xmm0\n\t"
			"movdqa %%xmm1, (%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd2:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd2\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd2\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
#ifdef AVX512_SUPPORT
char* do_independent_random_load_rwlocal_2R_1W_avx512(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, local_buf
			add rax, blk_offset
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+4]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+8]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+0ch]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+10h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+14h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+18h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			mov ebx, [rsi+r8*1+1ch]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1

			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd2c:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd2c:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd2c\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd2c\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_2R_1W_avx512_256B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	// __int64 blk_offset;

	 numEntries = sizeof(int)* max_rand_size_bw_gen/4; // divided by 4 as each entry can cover 256Bytes
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// to be implemented
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd32c:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x80(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0xc0(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x80(%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0xc0(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			"clflushopt 0x80(%7, %%rbx)\n\t"
			"clflushopt 0xc0(%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd32c:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd32c\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd32c\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_256B), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_2R_1W_avx512_128B_block(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i=0, iterations, numEntries;
	// __int64 blk_offset;

	 numEntries = sizeof(int)* max_rand_size_bw_gen/2; // divided by 4 as each entry can cover 256Bytes
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	// to be implemented
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd42c:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"

			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa64 0x40(%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"vmovdqa %%ymm1, 0x40(%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			"clflushopt 0x40(%7, %%rbx)\n\t"

			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd42c:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd42c\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd42c\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array_128B), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
char* do_independent_random_load_rwlocal_2R_1W_avx512_clflush(char* local_buf, char *write_buf, __int64 buf_len, char* endp, int delay_t)
{
     __int64 i, iterations, numEntries;

	 numEntries = sizeof(int)* max_rand_size_bw_gen;
	iterations = buf_len/LineSize; /* number of cachelines */

#if !defined(LINUX) && !defined(__APPLE__)
	__int64 blk_offset;
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		blk_offset = i*LineSize;
		__asm {
			xor rbx, rbx
			xor r8, r8
			mov rax, local_buf
			add rax, blk_offset
			mov r11, write_buf
			add r11, blk_offset
			mov rsi, thruput_array
			mov r9, numEntries
			movq xmm1, rsi  ; Initialize xmm1 with any non zero value
		WINL01:
			mov ebx, [rsi+r8*1]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+4]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+8]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+0ch]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+10h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+14h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+18h]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			mov ebx, [rsi+r8*1+1ch]
			vmovdqa64 zmm0, [rax+rbx*1]
			vmovdqa [r11+rbx*1], ymm1
			clflushopt [r11+rbx*1]

			sfence
			xor r10, r10
		DELAYLOOP1:
			inc r10
			cmp r10d, delay_t
			jl DELAYLOOP1
			add r8, 20h
			cmp r8, r9
			jl WINL01
		}
		if (stop_load != 0) break;
	}
#else	 
	for (i = 0; i < iterations - max_rand_size_bw_gen; i+= max_rand_size_bw_gen) {
		asm  volatile (
			"xor %%rbx, %%rbx\n\t"
			"add %1, %0\n\t" // add i*Linesize to local_buf to get to the next block of addresses
			"add %1, %7\n\t" // add i*Linesize to write_buf to get to the next block of addresses
			"lea (%3), %%rsi\n\t" // Load base addr of thruput_array into rsi
			"movq %1, %%xmm1\n\t"				// initialize xmm with some non-zero value
"LOOPrnd2d:\n\t"
			"mov (%%rsi, %4), %%ebx\n\t" // %4 is the index into the thruput array
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"  
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x4(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
				
			"mov 0x8(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x0c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x10(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x14(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x18(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"mov 0x1c(%%rsi, %4), %%ebx\n\t"
			"vmovdqa64 (%0, %%rbx), %%xmm0\n\t"
			"vmovdqa %%ymm1, (%7, %%rbx)\n\t"
			"clflushopt (%7, %%rbx)\n\t"
			
			"sfence\n\t"
			
			"xor %5, %5\n\t"		// next 4 lines of code implementing the load injection delays
"LOOP4frnd2d:\n\t"
			"inc %5\n\t"
			"cmp %6, %5\n\t"
			"jl LOOP4frnd2d\n\t"
			
			"add $0x20, %4\n\t"	// skip the 8 thruput_array entries that are consumed
			"cmp %2, %4\n\t"	// have we exhausted all the numEntries of the thrupt_array
			"jl LOOPrnd2d\n\t"
			
			::"r"(local_buf), "r"(i*LineSize),"r"(numEntries), "r" (thruput_array), "r"(0UL), "r"(0), "r"(delay_t), "r"(write_buf):"%rbx", "%rsi");
		if (stop_load != 0) break;
	}
#endif
	return (local_buf + i*LineSize);
}
#endif

char* do_independent_load_rdonly(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;

#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor r11, r11
		xor COUNTER, COUNTER

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz
		
		LOOPS:
		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9

		movdqa xmm1, xmmword ptr [LocalADDRESS]
		movdqa xmm1, xmmword ptr [LocalADDRESS+10h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+20h]
		movdqa xmm1, xmmword ptr [LocalADDRESS+30h]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc COUNTER
		cmp COUNTER, delay_t
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
	exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
		"xor %%r11, %%r11\n\t"
        "xor %1, %1\n\t"
	    "mov %7,%%r9\n\t"
        
    "LOOPSf18:\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"
	    "movdqa (%2),%%xmm0\n\t"
	    "movdqa 0x10(%2),%%xmm0\n\t"
	    "movdqa 0x20(%2),%%xmm0\n\t"
	    "movdqa 0x30(%2),%%xmm0\n\t"
	    "add %%r9,%2\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f18\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f18\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f18:\n\t"
		"xor %1,%1\n\t"
		"\n\t"
	"LOOP4f18:\n\t"
		"inc %1            \n\t"
		"cmp %6,%1 \n\t"
		"jl LOOP4f18\n\t"
        
	"STOP_DELAYf18:\n\t"
		"cmp %3, %2\n\t"
		"jl LOOPSf18\n\t"
	"EXIT4f18:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rdonly_avx256(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor rbx, rbx
		xor r11, r11
		
		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz ;

		LOOPS:

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		vmovdqa ymm1, ymmword ptr [LocalADDRESS]
		vmovdqa ymm1, ymmword ptr [LocalADDRESS+20h]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc rbx
		cmp rbx, r8 ;
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
	exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        
	    "LOOPSf19:\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r9,%2\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f19\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f19\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f19:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f19:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f19\n\t"
        
	    "STOP_DELAYf19:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf19\n\t"
	"EXIT4f19:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}
#ifdef AVX512_SUPPORT
char* do_independent_load_rdonly_avx512_bsz1(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor rbx, rbx
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz ;

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc rbx
		cmp rbx, r8 ;
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
		exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        
	    "LOOPSf641:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f641\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f641\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f641:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f641:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f64\n\t"
        
	    "STOP_DELAYf641:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf641\n\t"
	"EXIT4f641:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rdonly_avx512_bsz2(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor rbx, rbx
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz ;

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc rbx
		cmp rbx, r8 ;
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
		exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        
	    "LOOPSf642:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f642\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f642\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f642:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f642:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f642\n\t"
        
	    "STOP_DELAYf642:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf642\n\t"
	"EXIT4f642:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rdonly_avx512_bsz4(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor rbx, rbx
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz ;

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc rbx
		cmp rbx, r8 ;
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
		exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        
	    "LOOPSf644:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f644\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f644\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f644:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f644:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f644\n\t"
        
	    "STOP_DELAYf644:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf644\n\t"
	"EXIT4f644:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}
char* do_independent_load_rdonly_avx512(char* local_buf,  char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		xor rbx, rbx
		xor r11, r11

		mov LocalADDRESS, local_buf
		mov ENDP, endp
		mov r9, lsz ;

		LOOPS:

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		vmovdqa64 zmm1, zmmword ptr [LocalADDRESS]
		add LocalADDRESS, r9

		inc r11
		cmp r11, FLAG_CHK_CNT
		jb cont11
		cmp stop_load, 0
		jne exit_now
		xor r11, r11
	cont11:
		xor rbx, rbx

		LOOP4:
		inc rbx
		cmp rbx, r8 ;
		jl LOOP4

		STOP_DELAY:
		cmp LocalADDRESS, ENDP
		jl LOOPS
		exit_now:
		mov rax, LocalADDRESS

	}
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
	    "mov %7,%%r9\n\t"
        
	    "LOOPSf64:\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
	    "vmovdqa64 (%2),%%zmm1\n\t"
	    "add %%r9,%2\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f64\n\t"
		"cmp $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f64\n\t"
		"xor %%r11, %%r11\n\t"
        
	"FLAG4f64:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f64:\n\t"
	    "inc %1            \n\t"
	    "cmp %6,%1 \n\t"
	    "jl LOOP4f64\n\t"
        
	    "STOP_DELAYf64:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf64\n\t"
	"EXIT4f64:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(0), RemoteADDRESS(0), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r9","r11");
    return ret_ptr;
#endif
}

#endif

// Returns the location where it left off in the circular buffer so
// next iteration can continue from that location in the buffer
__int64* do_dependent_read(__int64* a)
{
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		//zero out COUNTER and initialize arrayPOSITION
		xor COUNTER, COUNTER
		mov arrayPOSITION, a

		LOOP1:
		//load value in [arrayPOSITION] into arrayPOSITION
		// basically pointer chasing code
		mov arrayPOSITION, [arrayPOSITION]
		//increment iteration counter
		inc COUNTER
		cmp COUNTER, ITERATIONS

		//quit if loop has run allotted number of times
		jb LOOP1

		STOP:
		mov rax, arrayPOSITION

	}
#else
	__int64 *ret_ptr;
	asm volatile(
	    "LOOPSf20:\n\t"
	    "mov (%4),%4\n\t"
	    "inc %2\n\t"
	    "cmp %7,%2\n\t"
	    "jb LOOPSf20\n\t"
	    "STOPf9:\n\t"
	    "mov %4,%0\n\t"
	    :"=r"(ret_ptr):"a"(0), COUNTER(0), LocalADDRESS(0), arrayPOSITION(a), WriteADDRESS(0), RemoteADDRESS(0), "i"(ITERATIONS));
	return ret_ptr;
#endif
}

// Dependent read loop that goes over the entire buffer ITERATIONS times
// Returns the location where it left off in the circular buffer so
// next iteration can continue from that location in the buffer
#if defined(LINUX) || defined(__APPLE__)
UINT64* do_specified_dependent_read(UINT64* a, UINT64 iter)
{
	UINT64 *ret_ptr;
	asm volatile(
	    "xor %1, %1\n\t"
	    "LOOP990:\n\t"
	    "mov (%2),%2\n\t"
	    "inc %1\n\t"
	    "cmp %3,%1\n\t"
	    "jb LOOP990\n\t"
	    "mov %2,%0\n\t"
	    :"=r"(ret_ptr):"b"(0LL), "d"(a), "r"(iter));
	return ret_ptr;
}
#else
UINT64* do_specified_dependent_read(UINT64* a, UINT64 iter)
{
	__asm {
		//zero out COUNTER and initialize arrayPOSITION
		xor rbx, rbx
		mov rdx, a
		mov rcx, iter

		LOOP1:
		//load value in [arrayPOSITION] into arrayPOSITION
		// basically pointer chasing code
		mov rdx, [rdx]
		//increment iteration counter
		inc rbx
		cmp rbx, rcx

		//quit if loop has run allotted number of times
		jb LOOP1

		STOP:
		mov rax, rdx

	}
}
#endif

// Dependent read loop that goes over the entire buffer only one time
#if defined(LINUX) || defined(__APPLE__)
void do_dependent_read_one_iteration_only(UINT64* a, UINT64 nLines)
{
	asm volatile(
	    "xor %0, %0\n\t"
	    "LOOP991:\n\t"
	    "mov (%1),%1\n\t"
	    "inc %0\n\t"
	    "cmp %2,%0\n\t"
	    "jb LOOP991\n\t"
	    ::"b"(0LL), "d"(a), "r"(nLines));
}
#else
void do_dependent_read_one_iteration_only(UINT64* a, UINT64 nLines)
{
	__asm {
		//zero out COUNTER and initialize arrayPOSITION
		xor rbx, rbx
		mov rdx, a
		mov r8, nLines

		LOOP2:
		//load value in [arrayPOSITION] into arrayPOSITION
		// basically pointer chasing code
		mov rdx, [rdx]
		//increment iteration counter
		inc rbx
		cmp rbx, r8

		//quit if loop has run allotted number of times
		jb LOOP2


	}
}
#endif
int * do_write( __int64 cnt, int* a, __int64 stride)
{

#if defined(LINUX) || defined(__APPLE__)
	int *ret_ptr;
	asm volatile(
		"LOOPS993:\n\t"
		"cmp %5,%1\n\t"
		"jge STOPf993\n\t"
		"inc %1\n\t"
		"mov %3, 32(%3)\n\t"
		"mov (%3), %3\n\t"
		"jmp LOOPS993\n\t"
		"STOPf993:\n\t"
		"mov %3,%0\n\t"
		:"=a"(ret_ptr)
        :COUNTER((__int64)0),LocalADDRESS(stride),arrayPOSITION(a),WriteADDRESS(stride),RemoteADDRESS(cnt));
    return ret_ptr;
#else
	__asm
	{	

	//zero out COUNTER and arrayPOSITION
		xor rbx, rbx
		mov rax, cnt

	//put base address of allocated mem in baseADDRESS
		mov rdx, a
		
LOOP2:
	//quit if loop has run allotted number of times
		cmp rbx, rax 
		jge STOP        
		
	//increment iteration counter	
		inc rbx		
		
	//load value in a[arrayPOSITION] into arrayPOSITION
		mov [rdx+32],rdx
		mov rdx, [rdx]
		
	//goto LOOP
		jmp LOOP2

STOP:		
		mov rax, rdx
	}

#endif
}

void do_nops(__int64 a)
{
#if !defined(LINUX) && !defined(__APPLE__)
	__asm
	{	

	//zero out COUNTER and arrayPOSITION
		xor rbx, rbx
		
LOOPS:
	//quit if LOOPS has run allotted number of times
		cmp rbx, a
		jge STOP        
		
	//increment iteration counter	
		inc rbx		
		
	//DON'T load value in a[arrayPOSITION] into arrayPOSITION
		//mov arrayPOSITION, [baseADDRESS + arrayPOSITION * INT]
		nop
		
	//goto LOOPS
		jmp LOOPS	       

STOP:		
	}
#else
	asm volatile(
		"LOOPSf2:\n\t"
		"cmp %5,%1\n\t"
		"jge STOPf2\n\t"
		"inc %1\n\t"
		"nop\n\t"
		"jmp LOOPSf2\n\t"
		"STOPf2:\n\t"
		::"a"(0),COUNTER((__int64)0),LocalADDRESS(0),arrayPOSITION(0),WriteADDRESS(0),RemoteADDRESS(a));
#endif
}

char* do_independent_load_rwlocal_100R_two_sources_1to1_ratio(char* local_buf, char* local_buf2, char* write_buf, char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (1 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (3 * LINE / 2)]

			add LocalADDRESS, (4 * LINE / 2)
			add RemoteADDRESS, (4 * LINE / 2)

			cont11 :
		xor COUNTER, COUNTER

			LOOP4 :
		inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
		mov rax, LocalADDRESS
	}


	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
	    "LOOPSf62ST191:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST191\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST191\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST191:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST191:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST191\n\t"
        
	    "STOP_DELAYf62ST191:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST191\n\t"
	"EXIT4f62ST191:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_100R_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf, char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (1 * LINE / 2)]

			add LocalADDRESS, (6 * LINE / 2)
			add RemoteADDRESS, (2 * LINE / 2)

			cont11 :
		xor COUNTER, COUNTER

			LOOP4 :
		inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
		mov rax, LocalADDRESS
	}


	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
	    "LOOPSf62ST19:\n\t"
	    // Unrolled 2 times
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
		
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"

	
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST19\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST19\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST19:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST19:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST19\n\t"
        
	    "STOP_DELAYf62ST19:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST19\n\t"
	"EXIT4f62ST19:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_100R_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (6 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (7 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (8 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (9 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (10 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (11 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (12 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (13 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (14 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (15 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (16 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (17 * LINE / 2)]

			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[RemoteADDRESS + (1 * LINE / 2)]

			add LocalADDRESS, (18 * LINE / 2)
			add RemoteADDRESS,(2 * LINE / 2)

		cont11 :
			xor COUNTER, COUNTER

			LOOP4 :
			inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
			mov rax, LocalADDRESS
		}

	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
	    "LOOPSf62ST10:\n\t"
	    
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
		
		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST10\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST10\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST10:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST10:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST10\n\t"
        
	    "STOP_DELAYf62ST10:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST10\n\t"
	"EXIT4f62ST10:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_3R1W_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]
			vmovdqa ymmword ptr[RemoteADDRESS + (0 * LINE / 2)], ymm1
			vmovdqa ymmword ptr[RemoteADDRESS + (1 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (6 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (7 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (8 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (9 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (10 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (11 * LINE / 2)]
			vmovdqa ymmword ptr[RemoteADDRESS + (2 * LINE / 2)], ymm1
			vmovdqa ymmword ptr[RemoteADDRESS + (3 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (12 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (13 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (14 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (15 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (16 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (17 * LINE / 2)]
			vmovdqa ymmword ptr[RemoteADDRESS + (4 * LINE / 2)], ymm1
			vmovdqa ymmword ptr[RemoteADDRESS + (5 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (18 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (19 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (20 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (21 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (22 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (23 * LINE / 2)]
			vmovdqa ymmword ptr[RemoteADDRESS + (6 * LINE / 2)], ymm1
			vmovdqa ymmword ptr[RemoteADDRESS + (7 * LINE / 2)], ymm1

			add LocalADDRESS, (24 * LINE / 2)
			add RemoteADDRESS, (8 * LINE / 2)

			cont11 :
		xor COUNTER, COUNTER

			LOOP4 :
		inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
		mov rax, LocalADDRESS
	}


	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm2\n\t"				// initialize xmm2 with some non-zero value

	    "LOOPSf62ST20:\n\t"
	  
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa %%ymm2, (%10)\n\t"
	    "vmovdqa %%ymm2, 0x20(%10)\n\t"
	    "add %%r10,%10\n\t"


		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST20\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST20\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST20:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST20:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST20\n\t"
        
	    "STOP_DELAYf62ST20:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST20\n\t"
	"EXIT4f62ST20:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load),"r"(write_buf2):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_3R1W_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t) 
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (6 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (7 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (8 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (9 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (10 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (11 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (12 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (13 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (14 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (15 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (16 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (17 * LINE / 2)]

			vmovdqa ymmword ptr[RemoteADDRESS + (0 * LINE / 2)], ymm1
			vmovdqa ymmword ptr[RemoteADDRESS + (1 * LINE / 2)], ymm1

			add LocalADDRESS, (18 * LINE / 2)
			add RemoteADDRESS, (2 * LINE / 2)

			cont11 :
		xor COUNTER, COUNTER

			LOOP4 :
		inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
		mov rax, LocalADDRESS
	}


	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm2\n\t"				// initialize xmm2 with some non-zero value

	    "LOOPSf62ST21:\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa %%ymm2, (%4)\n\t"
	    "vmovdqa %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa %%ymm2, (%10)\n\t"
	    "vmovdqa %%ymm2, 0x20(%10)\n\t"
	    "add %%r10,%10\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST21\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST21\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST21:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST21:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST21\n\t"
        
	    "STOP_DELAYf62ST21:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST21\n\t"
	"EXIT4f62ST21:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load),"r"(write_buf2):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R1NTW_two_sources_3to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE/2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE/2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE/2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE/2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (0 * LINE/2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (1 * LINE/2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (6 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (7 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (2 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (3 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (8 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (9 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (10 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (11 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (4 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (5 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (12 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (13 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (14 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (15 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (6 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (7 * LINE / 2)], ymm1

			add LocalADDRESS, (16 * LINE/2)
			add RemoteADDRESS, (8 * LINE/2)

			cont11 :
		xor COUNTER, COUNTER

			LOOP4 :
		inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
		mov rax, LocalADDRESS
	}

	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm2\n\t"				// initialize xmm2 with some non-zero value

	    "LOOPSf62ST22:\n\t"
	  
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%ymm2, (%10)\n\t"
	    "vmovntdq %%ymm2, 0x20(%10)\n\t"
	    "add %%r10,%10\n\t"


		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST22\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST22\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST22:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST22:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST22\n\t"
        
	    "STOP_DELAYf62ST22:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST22\n\t"
	"EXIT4f62ST22:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load),"r"(write_buf2):"r10","r11");
    return ret_ptr;
#endif
}

char* do_independent_load_rwlocal_2R1NTW_two_sources_9to1_ratio(char* local_buf, char* local_buf2, char* write_buf , char* write_buf2 , char* endp, int delay_t)
{
	__int64 lsz = (__int64)LineSize;
#if !defined(LINUX) && !defined(__APPLE__)
	__asm {
		mov LocalADDRESS, local_buf
		mov RemoteADDRESS, local_buf2
		mov ENDP, endp

		LOOPS :
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (0 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (1 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (2 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (3 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (0 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (1 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (4 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (5 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (6 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (7 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (2 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (3 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (8 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (9 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (10 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (11 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (4 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (5 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (12 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (13 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (14 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (15 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (6 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (7 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (16 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (17 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (18 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (19 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (8 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (9 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (20 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (21 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (22 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (23 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (10 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (11 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (24 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (25 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (26 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (27 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (12 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (13 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (28 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (29 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (30 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (31 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (14 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (15 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (32 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (33 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (34 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (35 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (16 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (17 * LINE / 2)], ymm1

			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (36 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (37 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (38 * LINE / 2)]
			vmovdqa ymm1, ymmword ptr[LocalADDRESS + (39 * LINE / 2)]
			vmovntdq ymmword ptr[RemoteADDRESS + (18 * LINE / 2)], ymm1
			vmovntdq ymmword ptr[RemoteADDRESS + (19 * LINE / 2)], ymm1

			add LocalADDRESS, (40 * LINE / 2)
			add RemoteADDRESS, (20 * LINE / 2)

			cont11 :
			xor COUNTER, COUNTER

			LOOP4 :
			inc COUNTER
			cmp COUNTER, delay_t
			jl LOOP4

			cmp LocalADDRESS, ENDP
			jl LOOPS
			exit_now :
			mov rax, LocalADDRESS
	}

	return endp;
#else
	char *ret_ptr;
	asm(
        "xor %1, %1\n\t"
		"xor %%r11, %%r11\n\t"
        
	    "mov %7,%%r10\n\t"
        "movq %3, %%xmm2\n\t"				// initialize xmm2 with some non-zero value

	    "LOOPSf62ST23:\n\t"
	  
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovdqa (%2),%%ymm1\n\t"
	    "vmovdqa 0x20(%2),%%ymm1\n\t"
	    "add %%r10,%2\n\t"
	    "vmovntdq %%ymm2, (%4)\n\t"
	    "vmovntdq %%ymm2, 0x20(%4)\n\t"
	    "add %%r10,%4\n\t"

	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovdqa (%5),%%ymm1\n\t"
	    "vmovdqa 0x20(%5),%%ymm1\n\t"
	    "add %%r10,%5\n\t"
	    "vmovntdq %%ymm2, (%10)\n\t"
	    "vmovntdq %%ymm2, 0x20(%10)\n\t"
	    "add %%r10,%10\n\t"

		"inc %%r11\n\t"		// increment counter to see when to check for stop_load indication
		"cmp %8, %%r11\n\t"	// check again FLAG_CHK_CNT
		"jb FLAG4f62ST23\n\t"
		"cmpl $0, %9\n\t"	// check whether stop_load is set; if so bail out
		"jne EXIT4f62ST23\n\t"
		"xor %%r11, %%r11\n\t"
	"FLAG4f62ST23:\n\t"
	    "xor %1,%1\n\t"
	    "\n\t"
        
	    "LOOP4f62ST23:\n\t"
	    "inc %1\n\t"
	    "cmp %6,%1\n\t"
	    "jl LOOP4f62ST23\n\t"
        
	    "STOP_DELAYf62ST23:\n\t"
	    "cmp %3, %2\n\t"
	    "jl LOOPSf62ST23\n\t"
	"EXIT4f62ST23:\n\t"
		"mov %2, %0\n\t"	// set the return value
	    :"=a"(ret_ptr)
        :COUNTER(0), LocalADDRESS(local_buf), ENDP(endp), WriteADDRESS(write_buf), RemoteADDRESS(local_buf2), "r"(delay_t), "m"(lsz),"i"(FLAG_CHK_CNT),"m"(stop_load),"r"(write_buf2):"r10","r11");
    return ret_ptr;
#endif
}
