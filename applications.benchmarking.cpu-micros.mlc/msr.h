/* The following functions read_msr, write_msr are modified from Intel(R) Power Governor, as per the following license:

Copyright (c) 2012, Intel Corporation
All rights reserved.

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided \
with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANT\
ABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLA\
RY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AN\
D ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED\
 OF THE POSSIBILITY OF SUCH DAMAGE.

All rights reserved.

*/

#ifndef MSR_INC
#define MSR_INC

#if defined(LINUX) || defined(__APPLE__)
int write_msr(unsigned int address, uint64_t* value, int latency_core);
int read_msr(unsigned int address, uint64_t* value, int latency_core);
int write_msr_all(unsigned int address, uint64_t* value);
int read_msr_all(unsigned int address, uint64_t* value);
#else
int write_msr(unsigned int address, UINT64* value, int latency_core);
int read_msr(unsigned int address, UINT64* value, int latency_core);
int write_msr_all(unsigned int address, UINT64* value);
int read_msr_all(unsigned int address, UINT64* value);
BOOL init_windows_msr_access();
BOOL close_windows_msr_access();

#endif
#endif
