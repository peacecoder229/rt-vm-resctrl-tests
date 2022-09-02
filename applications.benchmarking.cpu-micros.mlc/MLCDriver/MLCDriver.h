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
#include "UserKernelShared.h"

extern "C" {
    /* from sys/osfmk/i386/mp.c */
    
    extern void mp_rendezvous(void (*setup_func)(void *),
                              void (*action_func)(void *),
                              void (*teardown_func)(void *),
                              void *arg);
    
    extern int cpu_number(void);
    
    extern void mp_rendezvous_no_intrs(void (*action_func)(void *),
                                       void *arg);
}

typedef struct {
    int msr;
    int cpu;
    uint32_t val0;
    uint32_t val1;
} cpu_msr_t;


struct MySampleStruct;

class MLCDriverClassName : public IOService
{
    // Declare the metaclass information that is used for runtime type checking of I/O Kit objects.
    // Note that the class name is different when targeting Mac OS X 10.4 because support for that
    // version has to be built as a separate kext. This is because the KPIs for 64-bit user processes
    // to access user clients only exist on Mac OS X 10.5 and later.
    

    OSDeclareDefaultStructors(com_intel_driver_MLCDriver)

    
public:
    // IOService methods
    virtual bool init(OSDictionary* dictionary = 0);
    virtual void free(void);
    
    virtual IOService* probe(IOService* provider, SInt32* score);
    
    virtual bool start(IOService* provider);
    virtual void stop(IOService* provider);
    
    virtual bool handleOpen(IOService* forClient, IOOptionBits opts, void* args);
    virtual bool handleIsOpen(const IOService* forClient) const;
    virtual void handleClose(IOService* forClient, IOOptionBits opts);

    virtual bool willTerminate(IOService* provider, IOOptionBits options);
    virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
    
    virtual bool terminate(IOOptionBits options = 0);
    virtual bool finalize(IOOptionBits options);

    // methods
    virtual IOReturn readMSR(uint32_t cpu, uint32_t msr, uint64_t* output);
    virtual IOReturn readMSRAll(uint32_t cpu, uint32_t msr, uint64_t* output);
    virtual IOReturn writeMSR(uint32_t cpu, uint32_t msr, uint64_t* input);
    virtual IOReturn writeMSRAll(uint32_t cpu, uint32_t msr, uint64_t* input);
    virtual IOReturn bindCpu(uint32_t cpu);
};
