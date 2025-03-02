/*   SPDX-License-Identifier: BSD-3-Clause
 *   Copyright (C) 2020 Intel Corporation.
 *   All rights reserved.
 */

/** \file
 * Acceleration Framework
 */

#ifndef SPDK_ACCEL_H
#define SPDK_ACCEL_H

#include "spdk/stdinc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Flags for accel operations */
#define ACCEL_FLAG_PERSISTENT (1 << 0)

enum accel_opcode {
	ACCEL_OPC_COPY			= 0,
	ACCEL_OPC_FILL			= 1,
	ACCEL_OPC_DUALCAST		= 2,
	ACCEL_OPC_COMPARE		= 3,
	ACCEL_OPC_CRC32C		= 4,
	ACCEL_OPC_COPY_CRC32C		= 5,
	ACCEL_OPC_COMPRESS		= 6,
	ACCEL_OPC_DECOMPRESS		= 7,
	ACCEL_OPC_LAST			= 8,
};

/**
 * Acceleration operation callback.
 *
 * \param cb_arg Callback argument specified in the spdk_accel_submit* call.
 * \param status 0 if it completed successfully, or negative errno if it failed.
 */
typedef void (*spdk_accel_completion_cb)(void *cb_arg, int status);

/**
 * Acceleration framework finish callback.
 *
 * \param cb_arg Callback argument.
 */
typedef void (*spdk_accel_fini_cb)(void *cb_arg);

/**
 * Initialize the acceleration framework.
 *
 * \return 0 on success.
 */
int spdk_accel_initialize(void);

/**
 * Close the acceleration framework.
 *
 * \param cb_fn Called when the close operation completes.
 * \param cb_arg Argument passed to the callback function.
 */
void spdk_accel_finish(spdk_accel_fini_cb cb_fn, void *cb_arg);

/**
 * Get an I/O channel for the acceleration framework.
 *
 * This I/O channel is used to submit requests.
 *
 * \return a pointer to the I/O channel on success, or NULL on failure.
 */
struct spdk_io_channel *spdk_accel_get_io_channel(void);

/**
 * Submit a copy request.
 *
 * \param ch I/O channel associated with this call.
 * \param dst Destination to copy to.
 * \param src Source to copy from.
 * \param nbytes Length in bytes to copy.
 * \param flags Accel framework flags for operations.
 * \param cb_fn Called when this copy operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_copy(struct spdk_io_channel *ch, void *dst, void *src, uint64_t nbytes,
			   int flags, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a dual cast copy request.
 *
 * \param ch I/O channel associated with this call.
 * \param dst1 First destination to copy to (must be 4K aligned).
 * \param dst2 Second destination to copy to (must be 4K aligned).
 * \param src Source to copy from.
 * \param nbytes Length in bytes to copy.
 * \param flags Accel framework flags for operations.
 * \param cb_fn Called when this copy operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_dualcast(struct spdk_io_channel *ch, void *dst1, void *dst2, void *src,
			       uint64_t nbytes, int flags, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a compare request.
 *
 * \param ch I/O channel associated with this call.
 * \param src1 First location to perform compare on.
 * \param src2 Second location to perform compare on.
 * \param nbytes Length in bytes to compare.
 * \param cb_fn Called when this compare operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, any other value means there was a miscompare.
 */
int spdk_accel_submit_compare(struct spdk_io_channel *ch, void *src1, void *src2, uint64_t nbytes,
			      spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a fill request.
 *
 * This operation will fill the destination buffer with the specified value.
 *
 * \param ch I/O channel associated with this call.
 * \param dst Destination to fill.
 * \param fill Constant byte to fill to the destination.
 * \param nbytes Length in bytes to fill.
 * \param flags Accel framework flags for operations.
 * \param cb_fn Called when this fill operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_fill(struct spdk_io_channel *ch, void *dst, uint8_t fill, uint64_t nbytes,
			   int flags, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a CRC-32C calculation request.
 *
 * This operation will calculate the 4 byte CRC32-C for the given data.
 *
 * \param ch I/O channel associated with this call.
 * \param crc_dst Destination to write the CRC-32C to.
 * \param src The source address for the data.
 * \param seed Four byte seed value.
 * \param nbytes Length in bytes.
 * \param cb_fn Called when this CRC-32C operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_crc32c(struct spdk_io_channel *ch, uint32_t *crc_dst, void *src,
			     uint32_t seed,
			     uint64_t nbytes, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a chained CRC-32C calculation request.
 *
 * This operation will calculate the 4 byte CRC32-C for the given data.
 *
 * \param ch I/O channel associated with this call.
 * \param crc_dst Destination to write the CRC-32C to.
 * \param iovs The io vector array which stores the src data and len.
 * \param iovcnt The size of the iov.
 * \param seed Four byte seed value.
 * \param cb_fn Called when this CRC-32C operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_crc32cv(struct spdk_io_channel *ch, uint32_t *crc_dst, struct iovec *iovs,
			      uint32_t iovcnt, uint32_t seed, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a copy with CRC-32C calculation request.
 *
 * This operation will copy data and calculate the 4 byte CRC32-C for the given data.
 *
 * \param ch I/O channel associated with this call.
 * \param dst Destination to write the data to.
 * \param src The source address for the data.
 * \param crc_dst Destination to write the CRC-32C to.
 * \param seed Four byte seed value.
 * \param nbytes Length in bytes.
 * \param flags Accel framework flags for operations.
 * \param cb_fn Called when this CRC-32C operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_copy_crc32c(struct spdk_io_channel *ch, void *dst, void *src,
				  uint32_t *crc_dst, uint32_t seed, uint64_t nbytes, int flags,
				  spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Submit a chained copy + CRC-32C calculation request.
 *
 * This operation will calculate the 4 byte CRC32-C for the given data.
 *
 * \param ch I/O channel associated with this call.
 * \param dst Destination to write the data to.
 * \param src_iovs The io vector array which stores the src data and len.
 * \param iovcnt The size of the io vectors.
 * \param crc_dst Destination to write the CRC-32C to.
 * \param seed Four byte seed value.
 * \param flags Accel framework flags for operations.
 * \param cb_fn Called when this CRC-32C operation completes.
 * \param cb_arg Callback argument.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_copy_crc32cv(struct spdk_io_channel *ch, void *dst, struct iovec *src_iovs,
				   uint32_t iovcnt, uint32_t *crc_dst, uint32_t seed,
				   int flags, spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Build and submit a memory compress request.
 *
 * This function will build the compress descriptor and submit it.
 *
 * \param ch I/O channel associated with this call
 * \param dst Destination to write the data to.
 * \param nbytes Length in bytes.
 * \param src_iovs The io vector array which stores the src data and len.
 * \param src_iovcnt The size of the src io vectors.
 * \param output_size The size of the compressed data
 * \param flags Flags, optional flags that can vary per operation.
 * \param cb_fn Callback function which will be called when the request is complete.
 * \param cb_arg Opaque value which will be passed back as the arg parameter in
 * the completion callback.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_compress(struct spdk_io_channel *ch, void *dst,
			       uint64_t nbytes, struct iovec *src_iovs,
			       size_t src_iovcnt, uint32_t *output_size, int flags,
			       spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Build and submit a memory decompress request.
 *
 * This function will build the decompress descriptor and submit it.
 *
 * \param ch I/O channel associated with this call
 * \param dst_iovs The io vector array which stores the dst data and len.
 * \param dst_iovcnt The size of the dst io vectors.
 * \param src_iovs The io vector array which stores the src data and len.
 * \param src_iovcnt The size of the src io vectors.
 * \param flags Flags, optional flags that can vary per operation.
 * \param cb_fn Callback function which will be called when the request is complete.
 * \param cb_arg Opaque value which will be passed back as the arg parameter in
 * the completion callback.
 *
 * \return 0 on success, negative errno on failure.
 */
int spdk_accel_submit_decompress(struct spdk_io_channel *ch, struct iovec *dst_iovs,
				 size_t dst_iovcnt, struct iovec *src_iovs,
				 size_t src_iovcnt, int flags,
				 spdk_accel_completion_cb cb_fn, void *cb_arg);

/**
 * Return the name of the module assigned to a specfic opcode.
 *
 * \param opcode Accel Framework Opcode enum value. Valid codes can be retrieved using
 * `accel_get_opc_assignments` or `spdk_accel_get_opc_name`.
 * \param module_name Pointer to update with module name.
 *
 * \return 0 if a valid module name was provided. -EINVAL for invalid opcode
 *  or -ENOENT no module was found at this time for the provided opcode.
 */
int spdk_accel_get_opc_module_name(enum accel_opcode opcode, const char **module_name);

/**
 * Override the assignment of an opcode to an module.
 *
 * \param opcode Accel Framework Opcode enum value. Valid codes can be retrieved using
 * `accel_get_opc_assignments` or `spdk_accel_get_opc_name`.
 * \param name Name of the module to assign. Valid module names may be retrieved
 * with `spdk_accel_get_opc_module_name`
 *
 * \return 0 if a valid opcode name was provided. -EINVAL for invalid opcode
 *  or if the framework has started (cannot change modules after startup)
 */
int spdk_accel_assign_opc(enum accel_opcode opcode, const char *name);

struct spdk_json_write_ctx;

/**
 * Write Acceleration subsystem configuration into provided JSON context.
 *
 * \param w JSON write context
 */
void spdk_accel_write_config_json(struct spdk_json_write_ctx *w);

#ifdef __cplusplus
}
#endif

#endif
