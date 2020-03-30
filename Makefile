CC = gcc
CFLAGS = -I./vram -Og
RELCFLAGS = -Ofast -s
DBGCFLAGS = -ggdb3 -fstack-protector-all

all: debug

release: amdtimingcopy

debug: amdtimingcopydbg

amdtimingcopydbg: amdtimingcopy.c vbios-tables.h
	$(CC) $(CFLAGS) $(DBGCFLAGS) amdtimingcopy.c -o amdtimingcopy

amdtimingcopy: amdtimingcopy.c vbios-tables.h
	$(CC) $(CFLAGS) $(RELCFLAGS) amdtimingcopy.c -o amdtimingcopy
