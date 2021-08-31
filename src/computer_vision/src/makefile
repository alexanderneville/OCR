CC=gcc
CFLAGS=
LIBS= -lpng

IDIR=../includes
ODIR=./obj
_HEADS = image_io.h image_processing.h convolution.h tree.h matrix.h character_detection.h dataset.h
_OBJ = main.o image_io.o image_processing.o convolution.o tree.o matrix.o character_detection.o dataset.o

HEADS = $(patsubst %,$(IDIR)/%,$(_HEADS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# *.o depends on the .c file of the same name
# '@' is the left of the colon, '<' is the first arg right of ':'
# '^' is the right side of the ':'

$(ODIR)/%.o: %.c $(IDIR)
	$(CC) -c -o $@ $<

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(ODIR)/*.o outfile.png a.out main open_close.cpython-39-x86_64-linux-gnu.so build/
