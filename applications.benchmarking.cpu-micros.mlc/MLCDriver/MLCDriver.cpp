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
#include "MLCDriver.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach-o/loader.h>

#define super IOService
#define MSR_TSC 0x10 // time-stamp counter
#define MSR_HW_PREFETCHER 0x1a4 // L2 HW PREFETCHER

#define THREAD_BIND_FUNC              ((thread_bind_t)(uintptr_t)function_ptrs[THREAD_BIND])
#define CPU_TO_PROCESSOR_FUNC         ((cpu_to_processor_t)(uintptr_t)function_ptrs[CPU_TO_PROCESSOR])
#define MACH_HEADER_ADDR 0xFFFFFF8000200000

extern "C" {
    enum {
        THREAD_BIND,
        CPU_TO_PROCESSOR,
        TOTALFUNCS
    };
    
    uintptr_t function_ptrs[TOTALFUNCS];
    int      cpu_number(void);
    processor_t	cpu_to_processor(int cpu);
    thread_t		current_thread(void);
    wait_result_t   thread_block(thread_continue_t	continuation);
    extern uint64_t thread_tid(thread_t);
    thread_t        current_thread(void);
    processor_t     cpu_to_processor(int);
    wait_result_t   thread_block(thread_continue_t  continuation);
    wait_result_t   thread_block_parameter(thread_continue_t, void*);
    boolean_t       ml_at_interrupt_context(void);
    void            mp_broadcast(void (*action_func)(void *),
                                 void *arg);
    
    
    extern void current_map();
    extern void get_map_min();
    extern void get_vmmap_size();
    extern void vm_shared_region_get();
    extern void vm_shared_region_deallocate();
    extern void task_info_func();
    extern void mach_vm_read();
    extern void mach_vm_deallocate();
    
    typedef processor_t (*thread_bind_t)(processor_t);
    typedef processor_t (*cpu_to_processor_t)(int);
    
    inline kern_return_t
    thread_bind (int cpu)
    {
        processor_t proc = NULL;
        
        if(cpu < 0 || (unsigned int)cpu >= 24) // sanity check
            return KERN_FAILURE;
        
        proc = (*CPU_TO_PROCESSOR_FUNC)(cpu);
        (*THREAD_BIND_FUNC)(proc);
        if(!ml_at_interrupt_context() && cpu_number() != cpu) {
            (void)thread_block(THREAD_CONTINUE_NULL);
        }
        return KERN_SUCCESS;
    }

    /* Borrowed from kernel source. It doesn't exist in Kernel.framework. */
    struct nlist_64 {
        union {
            uint32_t  n_strx; /* index into the string table */
        } n_un;
        uint8_t n_type;       /* type flag, see below */
        uint8_t n_sect;       /* section number or NO_SECT */
        uint16_t n_desc;      /* see <mach-o/stab.h> */
        uint64_t n_value;     /* value of this symbol (or stab offset) */
    };

    struct segment_command_64 *find_segment_64(struct mach_header_64 *mh, const char *segname);
    struct section_64 *find_section_64(struct segment_command_64 *seg, const char *name);
    struct load_command *find_load_command(struct mach_header_64 *mh, uint32_t cmd);
    uintptr_t find_symbol(struct mach_header_64 *mh, const char *name);

   
    struct segment_command_64 *
    find_segment_64(struct mach_header_64 *mh, const char *segname)
    {
        struct load_command *lc;
        struct segment_command_64 *seg, *foundseg = NULL;
        
        /* First LC begins straight after the mach header */
        lc = (struct load_command *)((uint64_t)mh + sizeof(struct mach_header_64));
        while ((uint64_t)lc < (uint64_t)mh + (uint64_t)mh->sizeofcmds) {
            if (lc->cmd == LC_SEGMENT_64) {
                /* Check load command's segment name */
                seg = (struct segment_command_64 *)lc;
                if (strcmp(seg->segname, segname) == 0) {
                    foundseg = seg;
                    break;
                }
            }
            
            /* Next LC */
            lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
        }
        
        /* Return the segment (NULL if we didn't find it) */
        return foundseg;
    }

    struct section_64 *
    find_section_64(struct segment_command_64 *seg, const char *name)
    {
        struct section_64 *sect, *foundsect = NULL;
        u_int i = 0;
        
        /* First section begins straight after the segment header */
        for (i = 0, sect = (struct section_64 *)((uint64_t)seg + (uint64_t)sizeof(struct segment_command_64));
             i < seg->nsects; i++, sect = (struct section_64 *)((uint64_t)sect + sizeof(struct section_64)))
        {
            /* Check section name */
            if (strcmp(sect->sectname, name) == 0) {
                foundsect = sect;
                break;
            }
        }
        
        /* Return the section (NULL if we didn't find it) */
        return foundsect;
    }

    struct load_command *
    find_load_command(struct mach_header_64 *mh, uint32_t cmd)
    {
        struct load_command *lc, *foundlc;
        
        /* First LC begins straight after the mach header */
        lc = (struct load_command *)((uint64_t)mh + sizeof(struct mach_header_64));
        while ((uint64_t)lc < (uint64_t)mh + (uint64_t)mh->sizeofcmds) {
            if (lc->cmd == cmd) {
                foundlc = (struct load_command *)lc;
                break;
            }
            
            /* Next LC */
            lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
        }
        
        /* Return the load command (NULL if we didn't find it) */
        return foundlc;
    }

    uintptr_t find_symbol(struct mach_header_64 *mh, const char *name)
    {
        struct symtab_command *symtab = NULL;
        struct segment_command_64 *linkedit = NULL;
        void *strtab = NULL;
        struct nlist_64 *nl = NULL;
        char *str;
        uint64_t i;
        uintptr_t addr = NULL;
        
        /*
         * Check header
         */
        if (mh->magic != MH_MAGIC_64) {
            IOLog("FAIL: magic number doesn't match - 0x%x\n", mh->magic);
            return NULL;
        }
        
        /*
         * Find the LINKEDIT and SYMTAB sections
         */
        linkedit = find_segment_64(mh, SEG_LINKEDIT);
        if (!linkedit) {
            IOLog("FAIL: couldn't find __LINKEDIT\n");
            return NULL;
        }
        symtab = (struct symtab_command *)find_load_command(mh, LC_SYMTAB);
        if (!symtab) {
            IOLog("FAIL: couldn't find SYMTAB\n");
            return NULL;
        }
        
        /*
         * Enumerate symbols until we find the one we're after
         */
        
        strtab = (void*)((uintptr_t)linkedit->vmaddr + (uintptr_t)linkedit->vmsize - symtab->strsize);
        for (i = 0, nl = ((struct nlist_64 *)((uintptr_t)strtab - symtab->nsyms*sizeof(struct nlist_64)));
             i < symtab->nsyms;
             i++, nl = (struct nlist_64 *)((uintptr_t)nl + sizeof(struct nlist_64)))
        {
            str = (char *)strtab + nl->n_un.n_strx;
            if (strcmp(str, name) == 0) {
                addr = (uintptr_t)nl->n_value;
            }
        }
        
        /* Return the address (NULL if we didn't find it) */
        return addr;
    }
}

OSDefineMetaClassAndStructors(com_intel_driver_MLCDriver, IOService)

bool MLCDriverClassName::start(IOService* provider)
{
    //IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
    
    bool success = super::start(provider);
    
    if (success) {
        // Publish ourselves so clients can find us
        registerService();
    }
    
    /*
     processor_t	cpu_to_processor(int cpu);
     thread_t		current_thread(void);
     wait_result_t   thread_block(thread_continue_t	continuation);
     extern uint64_t thread_tid(thread_t);
     int             cpu_number(void);
     wait_result_t   thread_block_parameter(thread_continue_t, void*);
     boolean_t       ml_at_interrupt_context(void);
     void            mp_rendezvous(void (*setup_func)(void *),
     void (*action_func)(void *),
     void (*teardown_func)(void *),
     void *arg);
     void            mp_broadcast(void (*action_func)(void *),
     void *arg);
     void            mp_rendezvous_no_intrs(void (*action_func)(void *),
     void *arg);
     
     IOLog ("cpu_to_processor %p, current_thread %p, thread_block %p, thread_tid %p thread_bind %p\n",
     cpu_to_processor, current_thread, thread_block, thread_tid, thread_bind);
     
     IOLog ("cpu_number %p, thread_block_parameter %p, ml_at_interrupt_context %p, mp_rendezvous %p\n",
     cpu_number, thread_block_parameter, ml_at_interrupt_context, mp_rendezvous);
     
     IOLog ("mp_broadcast %p, mp_rendezvous_no_intrs %p current_map %p get_map_min %p\n",
     mp_broadcast, mp_rendezvous_no_intrs, current_map, get_map_min);
     
     IOLog ("get_vmmap_size %p, vm_shared_region_get %p task_info_func %p mach_vm_read %p mach_vm_deallocate %p\n",
     get_vmmap_size, vm_shared_region_get, task_info_func, mach_vm_read, mach_vm_deallocate);
     */
    
    // Looking for MH Header
    uintptr_t mh = (uintptr_t) cpu_number;
    mh = (uintptr_t) cpu_number & 0xfffffffffffff000;
    
    while (((struct mach_header_64*)mh)->magic != MH_MAGIC_64 ) {
        mh -= (uintptr_t) 0x01000;
    }
    
    //IOLog("mh: %p ASLR Offset: %p\n", mh, mh - MACH_HEADER_ADDR);
    
    function_ptrs[THREAD_BIND] = find_symbol((struct mach_header_64 *)mh, "_thread_bind");
    function_ptrs[CPU_TO_PROCESSOR] = find_symbol((struct mach_header_64 *)mh, "_cpu_to_processor");

    return success;
}


// We override stop only to log that it has been called to make it easier to follow the driver's lifecycle.
void MLCDriverClassName::stop(IOService* provider)
{
    //IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, provider);
    super::stop(provider);
}

bool MLCDriverClassName::handleOpen(IOService * forClient, IOOptionBits opts, void* args){
    return true;
}

bool MLCDriverClassName::handleIsOpen(const IOService* forClient) const{
    return true;
}

void MLCDriverClassName::handleClose(IOService* forClient, IOOptionBits opts){
}

// We override init only to log that it has been called to make it easier to follow the driver's lifecycle.
// Production drivers would only need to override init if they want to initialize data members.
bool MLCDriverClassName::init(OSDictionary* dictionary)
{
    if (!super::init(dictionary)) {
        return false;
    }
    
    // This IOLog must follow super::init because getName relies on the superclass initialization.
    //IOLog("%s[%p]::%s(%p)\n", getName(), this, __FUNCTION__, dictionary);
    
    // This is where you could set the initial value of your driver's data members.
    
    return true;
}

// We override free only to log that it has been called to make it easier to follow the driver's lifecycle.
void MLCDriverClassName::free(void)
{
    //IOLog("%s[%p]::%s()\n", getName(), this, __FUNCTION__);
    
    super::free();
}

// We override probe only to log that it has been called to make it easier to follow the driver's lifecycle.
// Production drivers can override probe if they need to make an active decision whether the driver is appropriate for
// the provider.
IOService* MLCDriverClassName::probe(IOService* provider, SInt32* score)
{
    //IOLog("%s[%p]::%s(%p, %p)\n", getName(), this, __FUNCTION__, provider, score);
    
    IOService *res = super::probe(provider, score);
    
    return res;
}

// We override willTerminate only to log that it has been called to make it easier to follow the driver's lifecycle.
//
// willTerminate is called at the beginning of the termination process. It is a notification
// that a provider has been terminated, sent before recursing up the stack, in root-to-leaf order.
//
// This is where any pending I/O should be terminated. At this point the user client has been marked
// inactive and any further requests from the user process should be returned with an error.
bool MLCDriverClassName::willTerminate(IOService* provider, IOOptionBits options)
{
    //IOLog("%s[%p]::%s(%p, %ld)\n", getName(), this, __FUNCTION__, provider, options);
    
    return super::willTerminate(provider, options);
}

// We override didTerminate only to log that it has been called to make it easier to follow the driver's lifecycle.
//
// didTerminate is called at the end of the termination process. It is a notification
// that a provider has been terminated, sent after recursing up the stack, in leaf-to-root order.
bool MLCDriverClassName::didTerminate(IOService* provider, IOOptionBits options, bool* defer)
{
    //IOLog("%s[%p]::%s(%p, %ld, %p)\n", getName(), this, __FUNCTION__, provider, options, defer);
    
    return super::didTerminate(provider, options, defer);
}


// We override terminate only to log that it has been called to make it easier to follow the driver's lifecycle.
// Production drivers will rarely need to override terminate. Termination processing should be done in
// willTerminate or didTerminate instead.
bool MLCDriverClassName::terminate(IOOptionBits options)
{
    bool	success;
    
    //IOLog("%s[%p]::%s(%ld)\n", getName(), this, __FUNCTION__, options);
    
    success = super::terminate(options);
    
    return success;
}


// We override finalize only to log that it has been called to make it easier to follow the driver's lifecycle.
// Production drivers will rarely need to override finalize.
bool MLCDriverClassName::finalize(IOOptionBits options)
{
    bool	success;
    
    //IOLog("%s[%p]::%s(%ld)\n", getName(), this, __FUNCTION__, options);
    
    success = super::finalize(options);
    
    return success;
}

static uint64_t read_msr(uint32_t addr)
{
    uint32_t msr_value_1, msr_value_0;
    asm volatile("rdmsr" : "=a" (msr_value_0), "=d" (msr_value_1) : "c" (addr) );
    return (((uint64_t)msr_value_1) << 32) | (uint64_t)msr_value_0;
}

static void write_msr(uint32_t addr, uint64_t value)
{
    uint32_t msr_value_lo = value & 0xffffffff;
    uint32_t msr_value_hi = value >> 32;
    asm volatile("wrmsr" : : "c" (addr), "a" (msr_value_lo), "d" (msr_value_hi));
}


void readMsrHelper(void* data)
{
    cpu_msr_t* cpu = (cpu_msr_t*) data;
    int currentCpu = cpu_number();
    
    if (cpu->cpu == currentCpu)
    {
        uint64_t values = read_msr(cpu->msr);
        cpu->val0 = (uint32_t)values;
        cpu->val1 = (uint32_t)(values >> 32);
    }
}

void readAllMsrHelper(void* data)
{
    cpu_msr_t* cpus = (cpu_msr_t*) data;
    int cpu = cpu_number();
    
    uint64_t values = read_msr(cpus->msr);
    cpus[cpu].val0 = (uint32_t)values;
    cpus[cpu].val1 = (uint32_t)(values >> 32);
}

IOReturn MLCDriverClassName::readMSR(uint32_t cpu, uint32_t msr, uint64_t* output)
{
    
    //cpu_msr_t* value = (cpu_msr_t*) IOMallocAligned(sizeof(cpu_msr_t), 16);
    cpu_msr_t value;
    
    value.cpu = cpu;
    value.msr = msr;
    
    mp_rendezvous(NULL, (void (*)(void *))readMsrHelper, NULL, (void*) &value);
    
    *output = (uint64_t) value.val0;
    
    return kIOReturnSuccess;
}

IOReturn MLCDriverClassName::readMSRAll(uint32_t cpu, uint32_t msr, uint64_t* output)
{
    cpu_msr_t* values = (cpu_msr_t*) IOMallocAligned(sizeof(cpu_msr_t) * cpu, 16);
    
    int i = 0;
    for (i = 0; i < cpu; i++)
    {
        values[i].cpu = i;
        values[i].msr = msr;
    }
    
    mp_rendezvous(NULL, (void (*)(void *))readAllMsrHelper, NULL, (void*) values);
    
    for (i = 0; i < cpu; i++)
    {
        output[i] = (uint64_t) values[i].val0;
    }
    
    IOFreeAligned((void*)values, sizeof(cpu_msr_t) * cpu);
    
    return kIOReturnSuccess;
}

void writeMsrHelper(void* data)
{
    cpu_msr_t* cpu = (cpu_msr_t*) data;
    int currentCpu = cpu_number();
    
    if (cpu->cpu == currentCpu)
    {
        write_msr(cpu->msr, (uint64_t)(cpu->val0));
    }
}

void writeAllMsrHelper(void* data)
{
    cpu_msr_t* cpus = (cpu_msr_t*) data;
    int cpu = cpu_number();
    
    write_msr(cpus[cpu].msr, (uint64_t)(cpus[cpu].val0));
}


IOReturn MLCDriverClassName::writeMSR(uint32_t cpu, uint32_t msr, uint64_t* input)
{
    //cpu_msr_t* value = (cpu_msr_t*) IOMallocAligned(sizeof(cpu_msr_t), 16);
    cpu_msr_t value;
    
    value.cpu = cpu;
    value.msr = msr;
    value.val0 = (uint32_t)*input;
    value.val1 = (uint32_t)(*input >> 32);
    
    mp_rendezvous(NULL, (void (*)(void *))writeMsrHelper, NULL, (void*) &value);
    return kIOReturnSuccess;
}

IOReturn MLCDriverClassName::writeMSRAll(uint32_t cpu, uint32_t msr, uint64_t* input)
{
    cpu_msr_t* values = (cpu_msr_t*) IOMallocAligned(sizeof(cpu_msr_t) * cpu, 16);
    
    int i = 0;
    for (i = 0; i < cpu; i++)
    {
        values[i].cpu = i;
        values[i].msr = msr;
        values[i].val0 = (uint32_t)input[i];
        values[i].val1 = (uint32_t)(input[i] >> 32);
    }

    mp_rendezvous(NULL, (void (*)(void *))writeAllMsrHelper, NULL, (void*) values);
    
    IOFreeAligned((void*)values, sizeof(cpu_msr_t) * cpu);
    
    return kIOReturnSuccess;
}

IOReturn MLCDriverClassName::bindCpu(uint32_t cpu)
{
    //IOLog("Input CPU to be bound to = %d\n", cpu);
    
    //IOLog("Before bind cpu_number = %d, Thread_TID: %x\n", (int) cpu_number(), (unsigned int) thread_tid(current_thread()));
    
    // Bind the the thread to specified CPU
    thread_bind(cpu);
    
    //IOLog("After bind cpu_number = %d, Thread_TID: %x\n", (int) cpu_number(), (unsigned int) thread_tid(current_thread()));

    return kIOReturnSuccess;
}

