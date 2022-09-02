CC=gcc
CC=gcc
LINKLIBS=-lpthread
SOURCES=main.c allocation.c traffic.c initialize.c helper.c combined-loadlat.c intrd.c msr.c topology.c bandwidth.c cpuid.c files.c exit.c loadlatency.c busythread.c hittest.c membw_scan.c
OBJECTS=$(SOURCES:.c=.o)
INC=
# the following flags are mandated by the SDL process as defensive compiling
SECURITY_FLAGS=-z noexecstack -z relro -z now -fstack-protector -fPIE -fPIC -Wformat -Wformat-security -O2 -D_FORTIFY_SOURCE=2
SECURITY_FLAGS_NOO2=-z noexecstack -z relro -z now -fstack-protector -fPIE -fPIC -Wformat -Wformat-security 
ifeq ($(B),internal)
	CFLAGS=-c -DLINUX -MMD -MP -DINTERNAL -DAVX512_SUPPORT $(SECURITY_FLAGS_NOO2)
	COPTFLAGS=-c -DLINUX -MMD -MP -DINTERNAL -DAVX512_SUPPORT $(SECURITY_FLAGS)
	EXECUTABLE=mlc_internal
	STATIC=-static
	ONEOBJ=mlc_internal.o
else
	CFLAGS=-c -DLINUX -MMD -MP -DAVX512_SUPPORT $(SECURITY_FLAGS_NOO2)
	COPTFLAGS=-c -DLINUX -MMD -MP -DAVX512_SUPPORT $(SECURITY_FLAGS)
	EXECUTABLE=mlc
	ONEOBJ=mlc.o
endif

build: $(SOURCES) $(ONEOBJ) $(EXECUTABLE)

$(ONEOBJ): $(OBJECTS)
	ld -r $(OBJECTS) -g -o $@

$(EXECUTABLE) : $(ONEOBJ)
	$(CC) $(INC) $(STATIC) $(ONEOBJ) $(LINKLIBS) -g -o $@_debug
	strip $@_debug -o $@


# traffic.c has assembly routines and don't use -O3 on that. So, it is special cased
traffic.o:
	$(CC) $(CFLAGS) $(INC) traffic.c -g -o traffic.o

# hittest.c had some issues in writerthread with -O option. So avoiding -O option
hittest.o:
	$(CC) $(CFLAGS) $(INC) hittest.c -g -o hittest.o

.c.o:
	$(CC) $(COPTFLAGS) $(INC) $< -g -o $@

astyle:
	astyle --style=otbs --indent=tab --break-blocks --pad-oper --pad-header --unpad-paren --delete-empty-lines --align-pointer=type *.[ch]

clean:
	rm -rf *.o *.d mlc_debug mlc_internal_debug mlctables mlc.txt

version:
	echo \#define INTEL_MLC_VERSION \"`git describe`\" > version_new.h
	cmp -s version.h version_new.h || mv version_new.h version.h;

-include $(SOURCES:%.c=%.d)
