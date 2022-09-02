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

#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
#include "MLCDriver.h"

#define MLCDriverClientClassName com_intel_driver_MLCDriverClient

class MLCDriverClientClassName : public IOUserClient
{

    OSDeclareDefaultStructors(com_intel_driver_MLCDriverClient)
    
protected:
    MLCDriverClassName*					    fProvider;
    task_t									fTask;
	bool									fCrossEndian;

    static const IOExternalMethodDispatch	sMethods[kNumberOfMethods];
      
public:
    // IOUserClient methods
    virtual void stop(IOService* provider);
    virtual bool start(IOService* provider);
    virtual bool initWithTask(task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties);
    virtual IOReturn clientClose(void);
	virtual bool willTerminate(IOService* provider, IOOptionBits options);
	virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
    virtual bool terminate(IOOptionBits options = 0);
    virtual bool finalize(IOOptionBits options);
	
protected:	

	// KPI for supporting access from both 32-bit and 64-bit user processes beginning with Mac OS X 10.5.
	virtual IOReturn externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
									IOExternalMethodDispatch* dispatch, OSObject* target, void* reference);

    // MLCDriverClient methods
	static IOReturn sOpenUserClient(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
	virtual IOReturn openUserClient(void);
	static IOReturn sCloseUserClient(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn closeUserClient(void);
    
    static IOReturn sReadMSR(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn readMSR(uint32_t inNumber1, uint32_t inNumber2, uint64_t * outStruct);
    static IOReturn sReadMSRAll(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn readMSRAll(uint32_t inNumber1, uint32_t inNumber2, uint64_t * outStruct);
    static IOReturn sWriteMSR(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn writeMSR(uint32_t inNumber1, uint32_t inNumber2, uint64_t * inStruct);
    static IOReturn sWriteMSRAll(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn writeMSRAll(uint32_t inNumber1, uint32_t inNumber2, uint64_t * inStruct);
    static IOReturn sBindCpu(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments);
    virtual IOReturn bindCpu(uint32_t inNumber);
    
};