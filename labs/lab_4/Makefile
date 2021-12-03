CC := gcc
CFLAGS := -g -std=c18 -O2 -Wall -Wextra -Werror -Wno-error=unused-parameter

all: workload_wraddr workload_readonly workload_rdfile workload_wrfile

workload_readonly: workload_readonly.o userswap.o userswap.h

workload_wraddr: workload_wraddr.o userswap.o userswap.h

workload_rdfile: workload_rdfile.o userswap.o userswap.h

workload_wrfile: workload_wrfile.o userswap.o userswap.h

clean:
	rm -f *.o workload_wraddr workload_readonly workload_rdfile workload_wrfile

.PHONY: all clean
