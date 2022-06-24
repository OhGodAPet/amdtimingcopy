CC = gcc
CFLAGS = -I./vram -Og
RELCFLAGS = -Ofast -s
DBGCFLAGS = -ggdb3
LDFLAGS = -ljansson

all: debug

release: amdtimingcopy

debug: amdtimingcopydbg

amdtimingcopydbg: amdtimingcopy.c amdtimingutils.h navi10_json.c navi21_json.c vbios-tables.h VRAMInfo.h vram/navi.h
	$(CC) $(CFLAGS) $(DBGCFLAGS) amdtimingcopy.c navi10_json.c navi21_json.c $(LDFLAGS) -o amdtimingcopy

amdtimingcopy: amdtimingcopy.c amdtimingutils.h navi10_json.c navi21_json.c vbios-tables.h VRAMInfo.h vram/navi.h
	$(CC) $(CFLAGS) $(RELCFLAGS) amdtimingcopy.c navi10_json.c navi21_json.c $(LDFLAGS) -o amdtimingcopy
