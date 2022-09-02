/*
 * INTEL CONFIDENTIAL
 * Copyright 2011 - 2013 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related
 * to the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors. Title to the Material remains with Intel Corporation
 * or its suppliers and licensors. The Material may contain trade secrets and
 * proprietary and confidential information of Intel Corporation and its
 * suppliers and licensors, and is protected by worldwide copyright and trade
 * secret laws and treaty provisions. No part of the Material may be used,
 * copied, reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way without Intel's prior express written
 * permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 * Unless otherwise agreed by Intel in writing, you may not remove or alter
 * this notice or any other notice embedded in Materials by Intel or Intel's
 * suppliers or licensors in any way.
 */

#include <stdint.h>
#include <stdlib.h>
#include "MLCDriver/UserKernelShared.h"

#ifdef __cplusplus
extern "C" {
#endif
    

kern_return_t MyOpenUserClient(io_connect_t connect);
kern_return_t MyCloseUserClient(io_connect_t connect);

kern_return_t readMSR(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* output);
kern_return_t readMSRAll(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* output);
kern_return_t writeMSR(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* value);
kern_return_t writeMSRAll(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* value);
kern_return_t bindCpu(io_connect_t connect, uint32_t cpu);

#ifdef __cplusplus
}
#endif