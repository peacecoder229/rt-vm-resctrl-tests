// (C) 2012-2014 Intel Corporation
// //
// // functions that create busy/idle threads on new sockets and bind threads
// to sockets //

// create busy threads on the cores specified in the string pcpu_str
void CreatePstateThreadsOnOtherCores(char* pcpu_str);


// create busy threads on all sockets, except socket_to_omit. socket_to_omit
// can be -1 if one wants to create busy threads on all sockets. this function
// has an inherent dependency on the CPU_Affinity global variable (core on
// which the latency thread runs: a busy thread will never be created on this
// core id
void CreatePstateThreadsOnAllSockets(int socket_to_omit);

