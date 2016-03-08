ifeq (DEBUG,0)
    CFLAGS=-I$(IDIR)
else
    CFLAGS=-I$(IDIR) -DNESTEST
endif

CC=gcc
IDIR=../include

ODIR=obj

_DEPS = romfile.h cpu.h instructions.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o romfile.o cpu.o instructions.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY:clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ main
