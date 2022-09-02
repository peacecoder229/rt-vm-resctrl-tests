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


#include <IOKit/IOLib.h>
#include <IOKit/IOKitKeys.h>
#include <libkern/OSByteOrder.h>
#include "MLCDriverClient.h"


#define super IOUserClient

// Even though we are defining the convenience macro super for the superclass, you must use the actual class name
// in the OS*MetaClass macros. Note that the class name is different when supporting Mac OS X 10.4.

OSDefineMetaClassAndStructors(com_intel_driver_MLCDriverClient, IOUserClient)


// This is the technique which supports both 32-bit and 64-bit user processes starting with Mac OS X 10.5.
//
// User client method dispatch table.
//
// The user client mechanism is designed to allow calls from a user process to be dispatched to
// any IOService-based object in the kernel. Almost always this mechanism is used to dispatch calls to
// either member functions of the user client itself or of the user client's provider. The provider is
// the driver which the user client is connecting to the user process.
//
// It is recommended that calls be dispatched to the user client and not directly to the provider driver.
// This allows the user client to perform error checking on the parameters before passing them to the driver.
// It also allows the user client to do any endian-swapping of parameters in the cross-endian case.
// (See ScalarIStructI below for further discussion of this subject.)

const IOExternalMethodDispatch MLCDriverClientClassName::sMethods[kNumberOfMethods] = {
	{   // kMyUserClientOpen
		(IOExternalMethodAction) &MLCDriverClientClassName::sOpenUserClient,	// Method pointer.
		0,																		// No scalar input values.
		0,																		// No struct input value.
		0,																		// No scalar output values.
		0																		// No struct output value.
	},
	{   // kMyUserClientClose
		(IOExternalMethodAction) &MLCDriverClientClassName::sCloseUserClient,	// Method pointer.
		0,																		// No scalar input values.
		0,																		// No struct input value.
		0,																		// No scalar output values.
		0																		// No struct output value.
	},
    {   // kReadMSR
        (IOExternalMethodAction) &MLCDriverClientClassName::sReadMSR,	        // Method pointer.
        2,																		// Two scalar input values.
        0,																		// No struct input value.
        0,																		// No scalar output value.
        1																		// One struct output value.
    },
    {   // kReadMSRAll
        (IOExternalMethodAction) &MLCDriverClientClassName::sReadMSRAll,        // Method pointer.
        2,																		// Two scalar input values.
        0,																		// No struct input value.
        0,																		// No scalar output value.
        kIOUCVariableStructureSize												// variable struct output value.
    },
    {   // kWriteMSR
        (IOExternalMethodAction) &MLCDriverClientClassName::sWriteMSR,	        // Method pointer.
        2,																		// Two scalar input values.
        kIOUCVariableStructureSize,												// One struct input value.
        0,																		// No scalar output value.
        0																		// No struct output value.
    },
    {   // kWriteMSRAll
        (IOExternalMethodAction) &MLCDriverClientClassName::sWriteMSRAll,        // Method pointer.
        2,																		// Two scalar input values.
        kIOUCVariableStructureSize,												// variable struct input value.
        0,																		// No scalar output value.
        0																		// No struct output value.
    },
    {   // kBindCpu
        (IOExternalMethodAction) &MLCDriverClientClassName::sBindCpu,	        // Method pointer.
        1,																		// One scalar input values.
        0,																		// No struct input value.
        0,																		// No scalar output value.
        0																		// No struct output value.
    },
};

IOReturn MLCDriverClientClassName::externalMethod(uint32_t selector, IOExternalMethodArguments* arguments,
												   IOExternalMethodDispatch* dispatch, OSObject* target, void* reference)

{
	//IOLog("%s[%p]::%s(%d, %p, %p, %p, %p, %d)\n", getName(), this, __FUNCTION__, selector, arguments, dispatch, target, reference, kNumberOfMethods);
        
    if (selector < (uint32_t) kNumberOfMethods) {
        dispatch = (IOExternalMethodDispatch *) &sMethods[selector];
        if (!target) {
				target = this;
		}
    }
        
	IOReturn result = super::externalMethod(selector, arguments, dispatch, target, reference);
    if (result) {
        IOLog("%s[%p]::%s() -- super::externalMethod result: %x\n", getName(), this, __FUNCTION__, result);
    }
    
    return result;
}

// There are two forms of IOUserClient::initWithTask, the second of which accepts an additional OSDictionary* parameter.
// If your user client needs to modify its behavior when it's being used by a process running using Rosetta,
// you need to implement the form of initWithTask with this additional parameter.
//
// initWithTask is called as a result of the user process calling IOServiceOpen.
bool MLCDriverClientClassName::initWithTask(task_t owningTask, void* securityToken, UInt32 type, OSDictionary* properties)
{
	bool success = super::initWithTask(owningTask, securityToken, type, properties);
	
	// This IOLog must follow super::initWithTask because getName relies on the superclass initialization.
	IOLog("%s[%p]::%s(%p, %p, %ld, %p)\n", getName(), this, __FUNCTION__, owningTask, securityToken, type, properties);

	if (success) {
		// This code will do the right thing on both PowerPC- and Intel-based systems because the cross-endian
		// property will never be set on PowerPC-based Macs. 
		fCrossEndian = false;
	
		if (properties != NULL && properties->getObject(kIOUserClientCrossEndianKey)) {
			// A connection to this user client is being opened by a user process running using Rosetta.
			
			// Indicate that this user client can handle being called from cross-endian user processes by 
			// setting its IOUserClientCrossEndianCompatible property in the I/O Registry.
			if (setProperty(kIOUserClientCrossEndianCompatibleKey, kOSBooleanTrue)) {
				fCrossEndian = true;
				IOLog("%s[%p]::%s(): fCrossEndian = true\n", getName(), this, __FUNCTION__);
			}
		}
	}
	
    fTask = owningTask;
    fProvider = NULL;
        
    return success;
}

// start is called after initWithTask as a result of the user process calling IOServiceOpen.
bool MLCDriverClientClassName::start(IOService* provider)
{
    bool	success;
	
	//IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
    
    // Verify that this user client is being started with a provider that it knows
	// how to communicate with.
	fProvider = OSDynamicCast(MLCDriverClassName, provider);
    success = (fProvider != NULL);
    
    if (success) {
		// It's important not to call super::start if some previous condition
		// (like an invalid provider) would cause this function to return false. 
		// I/O Kit won't call stop on an object if its start function returned false.
		success = super::start(provider);
	}
	
    return success;
}

// We override stop only to log that it has been called to make it easier to follow the user client's lifecycle.
void MLCDriverClientClassName::stop(IOService* provider)
{
	//IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
    
    super::stop(provider);
}

// clientClose is called as a result of the user process calling IOServiceClose.
IOReturn MLCDriverClientClassName::clientClose(void)
{
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    // Defensive coding in case the user process called IOServiceClose
	// without calling closeUserClient first.
    (void) closeUserClient();
    
	// Inform the user process that this user client is no longer available. This will also cause the
	// user client instance to be destroyed.
	//
	// terminate would return false if the user process still had this user client open.
	// This should never happen in our case because this code path is only reached if the user process
	// explicitly requests closing the connection to the user client.
	bool success = terminate();
	if (!success) {
		IOLog("%s[%p]::%s(): terminate() failed.\n", getName(), this, __FUNCTION__);
	}

    // DON'T call super::clientClose, which just returns kIOReturnUnsupported.
    
    return kIOReturnSuccess;
}

// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool MLCDriverClientClassName::willTerminate(IOService* provider, IOOptionBits options)
{
	//IOLog("%s[%p]::%s(%p, %ld)\n", getName(), this, __FUNCTION__, provider, options);
	
	return super::willTerminate(provider, options);
}


// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool MLCDriverClientClassName::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
	//IOLog("%s[%p]::%s(%p, %ld, %p)\n", getName(), this, __FUNCTION__, provider, options, defer);
	
	// If all pending I/O has been terminated, close our provider. If I/O is still outstanding, set defer to true
	// and the user client will not have stop called on it.
	closeUserClient();
	*defer = false;
	
	return super::didTerminate(provider, options, defer);
}


// We override terminate only to log that it has been called to make it easier to follow the user client's lifecycle.
// Production user clients will rarely need to override terminate. Termination processing should be done in
// willTerminate or didTerminate instead.
bool MLCDriverClientClassName::terminate(IOOptionBits options)
{
    bool	success;
    
	//IOLog("%s[%p]::%s(%ld)\n", getName(), this, __FUNCTION__, options);

    success = super::terminate(options);
    
    return success;
}


// We override finalize only to log that it has been called to make it easier to follow the user client's lifecycle.
// Production user clients will rarely need to override finalize.
bool MLCDriverClientClassName::finalize(IOOptionBits options)
{
    bool	success;
    
	//IOLog("%s[%p]::%s(%ld)\n", getName(), this, __FUNCTION__, options);
    
    success = super::finalize(options);
    
    return success;
}

IOReturn MLCDriverClientClassName::sOpenUserClient(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->openUserClient();
}

IOReturn MLCDriverClientClassName::openUserClient(void)
{
    IOReturn	result = kIOReturnSuccess;
	
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    if (fProvider == NULL || isInactive()) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called openUserClient without calling IOServiceOpen first. Or, the user client could be
		// in the process of being terminated and is thus inactive.
        result = kIOReturnNotAttached;
	}
    else if (!fProvider->open(this)) {
		// The most common reason this open call will fail is because the provider is already open
		// and it doesn't support being opened by more than one client at a time.
		result = kIOReturnExclusiveAccess;
	}
        
    return result;
}

IOReturn MLCDriverClientClassName::sCloseUserClient(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments)
{
    return target->closeUserClient();
}

IOReturn MLCDriverClientClassName::closeUserClient(void)
{
    IOReturn	result = kIOReturnSuccess;
	
	IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
            
    if (fProvider == NULL) {
		// Return an error if we don't have a provider. This could happen if the user process
		// called closeUserClient without calling IOServiceOpen first. 
		result = kIOReturnNotAttached;
		IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
	}
	else if (fProvider->isOpen(this)) {
		// Make sure we're the one who opened our provider before we tell it to close.
		fProvider->close(this);
	}
	else {
		result = kIOReturnNotOpen;
		//IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
	}
	
    return result;
}


IOReturn MLCDriverClientClassName::sReadMSR(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments) {
    return target->readMSR((uint32_t)arguments->scalarInput[0],
                           (uint32_t)arguments->scalarInput[1],
                           (uint64_t *)arguments->structureOutput);
}
IOReturn MLCDriverClientClassName::readMSR(uint32_t inNumber1, uint32_t inNumber2, uint64_t * outStruct)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    IOReturn result;
    
    if (fProvider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    } else if (!fProvider->isOpen(this)) {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    } else {
        result = fProvider->readMSR(inNumber1, inNumber2, outStruct);
        //IOLog("%s[%p]::%s(intput = %u, output = %llu)\n", getName(), this, __FUNCTION__, (uint32_t)input[1], *output);
    }
    
    return result;
}


IOReturn MLCDriverClientClassName::sReadMSRAll(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments) {
    return target->readMSRAll((uint32_t)arguments->scalarInput[0],
                           (uint32_t)arguments->scalarInput[1],
                           (uint64_t *)arguments->structureOutput);
}
IOReturn MLCDriverClientClassName::readMSRAll(uint32_t inNumber1, uint32_t inNumber2, uint64_t * outStruct)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    IOReturn result;
    
    if (fProvider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    } else if (!fProvider->isOpen(this)) {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    } else {
        result = fProvider->readMSRAll(inNumber1, inNumber2, outStruct);
        //IOLog("%s[%p]::%s(intput = %u, output = %llu)\n", getName(), this, __FUNCTION__, (uint32_t)input[1], *output);
    }
    
    return result;
}


IOReturn MLCDriverClientClassName::sWriteMSR(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments) {
    return target->writeMSR((uint32_t)arguments->scalarInput[0],
                            (uint32_t)arguments->scalarInput[1],
                            (uint64_t *)arguments->structureInput);
}
IOReturn MLCDriverClientClassName::writeMSR(uint32_t inNumber1, uint32_t inNumber2, uint64_t * inStruct)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    IOReturn result;
    
    if (fProvider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    } else if (!fProvider->isOpen(this)) {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    } else {
        result = fProvider->writeMSR(inNumber1, inNumber2, inStruct);
        //IOLog("%s[%p]::%s(intput = %u, output = %llu)\n", getName(), this, __FUNCTION__, (uint32_t)input[1], *output);
    }
    
    return result;
}


IOReturn MLCDriverClientClassName::sWriteMSRAll(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments) {
    return target->writeMSRAll((uint32_t)arguments->scalarInput[0],
                            (uint32_t)arguments->scalarInput[1],
                            (uint64_t *)arguments->structureInput);
}
IOReturn MLCDriverClientClassName::writeMSRAll(uint32_t inNumber1, uint32_t inNumber2, uint64_t * inStruct)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    IOReturn result;
    
    if (fProvider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    } else if (!fProvider->isOpen(this)) {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    } else {
        result = fProvider->writeMSRAll(inNumber1, inNumber2, inStruct);
        //IOLog("%s[%p]::%s(intput = %u, output = %llu)\n", getName(), this, __FUNCTION__, (uint32_t)input[1], *output);
    }
    
    return result;
}

IOReturn MLCDriverClientClassName::sBindCpu(MLCDriverClientClassName* target, void* reference, IOExternalMethodArguments* arguments) {
    return target->bindCpu((uint32_t)arguments->scalarInput[0]);
}
IOReturn MLCDriverClientClassName::bindCpu(uint32_t inNumber)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    IOReturn result;
    
    if (fProvider == NULL || isInactive()) {
        result = kIOReturnNotAttached;
        IOLog("%s[%p]::%s(): returning kIOReturnNotAttached.\n", getName(), this, __FUNCTION__);
    } else if (!fProvider->isOpen(this)) {
        result = kIOReturnNotOpen;
        IOLog("%s[%p]::%s(): returning kIOReturnNotOpen.\n", getName(), this, __FUNCTION__);
    } else {
        result = fProvider->bindCpu(inNumber);
        //IOLog("%s[%p]::%s(intput = %u)\n", getName(), this, __FUNCTION__, (uint32_t)inNumber);
    }
    
    
    result = kIOReturnSuccess;
    return result;
}
