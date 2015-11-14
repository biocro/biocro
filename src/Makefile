#$(CC) $(CFLAGS) -o $@ $(OBJ1) `xml2-config --libs`
CC=gcc
CFLAGS=-Wall -Wstrict-prototypes -Wmissing-prototypes -g -O3
#CFLAGS+=-I "/c/Program Files/R/R-3.1.2/include"
CFLAGS+=-I"C:\\Program Files\\R\\R-3.1.2\\include"
CFLAGS+=-std=c99 # MinGW seems to need this for certain constructs we use.
_OBJ = BioCro.o AuxBioCro.o c4photo.o c3photo.o 
OBJ = $(patsubst %,$(objdir)/%,$(_OBJ))

objdir = obj

$(objdir)/%.o: %.c
	$(CC) $(CFLAGS) -c $<  -o $@

biocro: $(OBJ)
	cd .

.PHONY: clean

clean:
	rm -f $(objdir)/*.o

