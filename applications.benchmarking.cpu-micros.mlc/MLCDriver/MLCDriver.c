//
//  MLCDriver.c
//  MLCDriver
//
//  Created by Pallavi Mehrotra on 4/1/15.
//  Copyright (c) 2015 Pallavi Mehrotra. All rights reserved.
//

#include <mach/mach_types.h>

kern_return_t MLCDriver_start(kmod_info_t * ki, void *d);
kern_return_t MLCDriver_stop(kmod_info_t *ki, void *d);

kern_return_t MLCDriver_start(kmod_info_t * ki, void *d)
{
    return KERN_SUCCESS;
}

kern_return_t MLCDriver_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
