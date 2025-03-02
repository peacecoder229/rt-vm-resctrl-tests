// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2019 Intel Corporation. All rights reserved. */
#include <stdint.h>
#include <string.h>
#include <linux/idxd.h>
#include <immintrin.h>
#include <x86intrin.h>
#include <errno.h>

#include "common.h"
#include "util.h"
#include "cpu.h"

#pragma GCC push_options
#pragma GCC optimize("O0")
static inline void
cmpval(char *buf, uint64_t len)
{
	__m512i *b = (__m512i *)buf;
	__m512i *e = (__m512i *)(buf + len);

	for (; b < e; b += 1) {
		const void *p = b;

		_mm512_loadu_si512(p);
	}
}
#pragma GCC pop_options

static uint64_t
memcmp1_8(const uint64_t *src1, const uint64_t *src2, uint64_t len)
{
	uint64_t i = 0;

	while (i < len && src1[i] == src2[i])
		i++;
	return i;
}


void
cr_delta(char *src1, char *src2, struct delta_rec *delta, uint64_t len)
{
	uint64_t src_offset = 0;
	uint16_t d;
	uint64_t *s1, *s2;

	s1 = (uint64_t *)src1;
	s2 = (uint64_t *)src2;
	len = len/8;

	d = 0;
	while (src_offset < len) {
		uint64_t delta_offset;

		delta_offset = memcmp1_8(s1 + src_offset, s2 + src_offset, len - src_offset);
		if (delta_offset == len - src_offset)
			break;

		delta_offset += src_offset;
		delta[d].off = delta_offset;
		delta[d].val = s2[delta_offset];
		d++;
		src_offset = delta_offset + 1;
	}
}

static inline void
ap_delta(char *dst, struct delta_rec *delta, uint32_t delta_len)
{
	uint64_t *p;
	uint32_t i;

	p = (uint64_t *)dst;

	for (i = 0; i < delta_len/sizeof(*delta); i++)
		p[delta[i].off] = delta[i].val;
}

void
test_memcpy(struct tcfg_cpu *tcpu)
{
	uint32_t i, j;
	char *src, *dst, *src1, *src2;
	struct delta_rec *delta;
	uint32_t off;
	struct tcfg *tcfg = tcpu->tcfg;

	if (tcfg->bstride != 0)
		off = tcfg->bstride;
	else
		off = tcfg->blen;

	init_buffers(tcpu);

	/* Warmup TLB */
	for (i = 0; i < 3; i++) {
		src = tcpu->src;
		dst = tcpu->dst;
		src1 = tcpu->src1;
		src2 = tcpu->src2;
		delta = tcpu->delta;

		for (j = 0; j < tcfg->nb_bufs; j++) {

			switch (tcfg->op) {
			case DSA_OPCODE_MEMFILL:
			case DSA_OPCODE_CFLUSH:
				memset(dst, TEST_CHAR, tcfg->blen);
				break;

			case DSA_OPCODE_COMPVAL:
				cmpval(src, tcfg->blen);
				break;

			case DSA_OPCODE_MEMMOVE:
				memcpy(dst, src, tcfg->blen);
				break;

			case DSA_OPCODE_CR_DELTA:
				cmpval(src1, tcfg->blen);
				cmpval(src2, tcfg->blen);
				memset(delta, 0, tcfg->delta_rec_size);
				break;

			case DSA_OPCODE_AP_DELTA:
				ap_delta(dst, delta, tcfg->delta_rec_size);
				break;

			default:
				ERR("Unimplemented opcode\n");
				tcpu->err = -EINVAL;
				return;
			}

			src += off;
			dst += off;
			src1 += off;
			src2 += off;
			delta += tcfg->delta_rec_size/sizeof(*delta);
		}
	}

	do_cache_ops(tcpu);
	tcpu->cycles = 0;
	test_barrier(tcfg, 0);

	for (i = 0; i < tcfg->iter; i++) {
		uint64_t c;

		src = tcpu->src;
		dst = tcpu->dst;
		delta = tcpu->delta;
		src1 = tcpu->src1;
		src2 = tcpu->src2;

		/*
		 * If cpus > 1, then you can either measure the BW for a
                 * single buffer for a single iteration or if the number
		 * iterations is > 1 then use a working set such that for a single threaded
		 * config nb_bufs * blen exceeds the L2 size for LLC BW measurement or
		 * nb_cpus * nb_bufs * blen exceeds total L2 + LLC size for memory BW
		 * measurement. Halve the sizes for MT.
		 */
		if (tcfg->nb_cpus == 1)
			do_cache_ops(tcpu);

		for (j = 0; j < tcfg->nb_bufs; j++) {

			c = rdtsc();

			switch (tcfg->op) {
			case DSA_OPCODE_MEMMOVE:
				memcpy(dst, src, tcfg->blen);
				break;

			case DSA_OPCODE_MEMFILL:
				memset(dst, 0, tcfg->blen);
				break;

			case DSA_OPCODE_COMPVAL:
				cmpval(src, tcfg->blen);
				break;

			case DSA_OPCODE_CFLUSH:
				if (tcfg->desc_flags & IDXD_OP_FLAG_CC)
					clwb(dst, tcfg->blen);
				else
					cflush(dst, tcfg->blen);
				break;

			case DSA_OPCODE_CR_DELTA:
				cr_delta(src1, src2, delta, tcfg->blen);
				break;

			case DSA_OPCODE_AP_DELTA:
				ap_delta(dst, delta, tcfg->delta_rec_size);
				break;

			}

			tcpu->cycles += rdtsc() - c;
			src += off;
			dst += off;
			src1 += off;
			src2 += off;
			delta += tcfg->delta_rec_size/sizeof(*delta);
		}

		tcpu->curr_stat.iter++;
	}

	if (tcfg->iter)
		tcpu->cycles /= tcfg->iter;
	tcpu->err = 0;
}
