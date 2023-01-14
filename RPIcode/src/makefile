OBJDIR=obj
LIBDIR=../lib
INCDIR=../include
OUTDIR=../output

CC=~/Documents/buildroot-2022.05/output/host/bin/aarch64-linux-gcc
CFLAGS=-I $(INCDIR)

LIBS=-lm -lcurl

ifdef DEF
USEDEF=-D $(DEF)
else
USEDEF=
endif

_DEPS=verbose_print.h stemq.h input.h messagestruct.h fms.h
DEPS=$(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ=main.o stemq.o input.o fms.o
OBJ=$(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(USEDEF)

rpi: $(OBJ)
	$(CC) -o $(OUTDIR)/$@ $^ $(CFLAGS) $(LIBS) $(USEDEF)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(INCDIR)/*~
