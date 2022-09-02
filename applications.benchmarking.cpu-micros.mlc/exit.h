// (C) 2012-2014 Intel Corporation
// // //
// // // functions that handle the exit routines for MLC. This is where we
// catch the prefetchers and restore the NUMA balancing settings. All exit
// paths from MLC, forced or regular, go through Runtime_exit
// //
//

#ifndef EXIT_INC
#define EXIT_INC

extern char numa_bal_path[64];
extern char numa_scan_path[64];

void  check_for_null_parameter(char b, char arg);
void Runtime_exit(const char*);
void exit_handler(int);

#endif
