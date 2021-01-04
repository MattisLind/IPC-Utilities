CC = gcc
CFLAGS = -g

BINS = ipc_ls ipc_cat ipc_burst ipc_dump
LIBS = ipc_utils.o
build: $(BINS)

clean:
	rm -f $(BINS) $(addsuffix .o,$(BINS)) $(LIBS)

$(addsuffix .o,$(BINS)) : ipc_utils.h

ipc_ls: ipc_ls.o $(LIBS)

ipc_cat: ipc_cat.o $(LIBS)

ipc_burst: ipc_burst.o $(LIBS)

ipc_dump: ipc_dump.o $(LIBS)

