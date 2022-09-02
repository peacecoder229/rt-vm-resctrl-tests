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


#include <AvailabilityMacros.h>
#include <IOKit/IOKitLib.h>
#include "MLCDriverClientInterface.h"
#include <stdio.h>

kern_return_t MyOpenUserClient(io_connect_t connect)
{
    // This calls the openUserClient method in MLCDriverClient inside the kernel. Though not mandatory, it's good
    // practice to use open and close semantics in your driver to prevent multiple user space applications from
    // using your driver at the same time.
    
	kern_return_t	kernResult;
	
#if !defined(__LP64__)
	// Check if Mac OS X 10.5 API is available...
	if (IOConnectCallScalarMethod != NULL) {
		// ...and use it if it is.
#endif
		kernResult = IOConnectCallScalarMethod(connect, kMyUserClientOpen, NULL, 0, NULL, NULL);
#if !defined(__LP64__)
	}
	else {
		// Otherwise fall back to older API.
		kernResult = IOConnectMethodScalarIScalarO(connect, kMyUserClientOpen, 0, 0);
	}    
#endif
    
	return kernResult;
}


kern_return_t MyCloseUserClient(io_connect_t connect)
{
    // This calls the closeUserClient method in SimpleUserClient inside the kernel, which in turn closes
	// the driver.
    
	kern_return_t	kernResult;
	
#if !defined(__LP64__)
	// Check if Mac OS X 10.5 API is available...
	if (IOConnectCallScalarMethod != NULL) {
		// ...and use it if it is.
#endif
		kernResult = IOConnectCallScalarMethod(connect, kMyUserClientClose, NULL, 0, NULL, NULL);
#if !defined(__LP64__)
	}
	else {
		// Otherwise fall back to older API.
		kernResult = IOConnectMethodScalarIScalarO(connect, kMyUserClientClose, 0, 0);
	}    
#endif

    return kernResult;
}


/**
 * readMSR
 * Calls the the kReadMSR function in MLCDriverClient.cpp
 * @param       connect IN - connection object
 * @param		cpu IN - cpu number of the package to read this MSR on
 * @param		msr IN - MSR address to read
 * @param		output OUT - value of the specified MSR on the specified package
 * @return      returns kern_return_t from IOKit calls
 */
kern_return_t readMSR(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* output) {
    uint64_t input[] = { (uint64_t)cpu, (uint64_t)msr };
    uint32_t outCount = 1;
    return IOConnectCallScalarMethod(connect, kReadMSR, input, 2, output, &outCount);
}

/**
 * writeMSR
 * Calls the the kWriteMSR function in MLCDriverClient.cpp
 * @param       connect IN - connection object
 * @param		cpu IN - cpu number of the package to read this MSR on
 * @param		msr IN - MSR address to write
 * @param		value IN - value of the specified MSR on the specified package
 * @return      returns kern_return_t from IOKit calls
 */
kern_return_t writeMSR(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* value) {
    uint64_t input[] = { (uint64_t)cpu, (uint64_t)msr };
    return IOConnectCallMethod(connect, kWriteMSR, input, 2, value, sizeof(uint64_t), NULL, NULL, NULL, NULL);
}

/**
 * readMSRAll
 * Calls the the kReadMSRAll function in MLCDriverClient.cpp
 * @param       connect IN - connection object
 * @param		cpu IN - cpu number of the package to read this MSR on
 * @param		msr IN - MSR address to read
 * @param		output OUT - value of the specified MSR on the specified package
 * @return      returns kern_return_t from IOKit calls
 */
kern_return_t readMSRAll(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* output) {
    uint64_t input[] = { (uint64_t)cpu, (uint64_t)msr };
    size_t outStrctCount = sizeof(uint64_t)*cpu;
    return IOConnectCallMethod(connect, kReadMSRAll, input, 2, NULL, 0, NULL, NULL, output, &outStrctCount);
}

/**
 * writeMSRAll
 * Calls the the kWriteMSRAll function in MLCDriverClient.cpp
 * @param       connect IN - connection object
 * @param		cpu IN - cpu number of the package to read this MSR on
 * @param		msr IN - MSR address to read
 * @param		output OUT - value of the specified MSR on the specified package
 * @return      returns kern_return_t from IOKit calls
 */
kern_return_t writeMSRAll(io_connect_t connect, uint32_t cpu, uint32_t msr, uint64_t* value) {
    uint64_t input[] = { (uint64_t)cpu, (uint64_t)msr };
    return IOConnectCallMethod(connect, kWriteMSRAll, input, 2, value, sizeof(uint64_t)*cpu, NULL, NULL, NULL, NULL);
}

/**
 * bindCpu
 * Calls the the kBindCpu function in MLCDriverClient.cpp
 * @param       connect IN - connection object
 * @param		cpu IN - cpu number to bind the thread to
 * @return      returns kern_return_t from IOKit calls
 */
kern_return_t bindCpu(io_connect_t connect, uint32_t cpu) {
    uint64_t input[] = { (uint64_t)cpu };
    kern_return_t kernResult = IOConnectCallMethod(connect, kBindCpu, input, 1, NULL, 0, NULL, NULL, NULL, NULL);
    return kernResult;
}
