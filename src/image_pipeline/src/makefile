CC=gcc
CFLAGS=
LIBS= -lpng -lpthread -lm

IDIR=../includes
ODIR=./obj
_HEADS = image_io.h image_processing.h convolution.h tree.h matrix.h character_detection.h dataset.h

_OBJ = test_pipeline.o image_io.o image_processing.o convolution.o tree.o matrix.o character_detection.o dataset.o
_KOBJ = test_kmeans.o image_io.o image_processing.o convolution.o tree.o matrix.o character_detection.o dataset.o clustering.o

HEADS = $(patsubst %,$(IDIR)/%,$(_HEADS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
KOBJ = $(patsubst %,$(ODIR)/%,$(_KOBJ))

# *.o depends on the .c file of the same name
# '@' is the left of the colon, '<' is the first arg right of ':'
# '^' is the right side of the ':'

$(ODIR)/%.o: %.c $(IDIR)
	$(CC) -c -o $@ $<

test: $(OBJ)
	python class_setup.py build
	cp ./build/lib.linux-x86_64-3.9/pipeline.cpython-39-x86_64-linux-gnu.so ./
	cp ./build/lib.linux-x86_64-3.9/pipeline.cpython-39-x86_64-linux-gnu.so ../../web_application/
	mv ./build/lib.linux-x86_64-3.9/pipeline.cpython-39-x86_64-linux-gnu.so ../../../tests/
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

kmeans: $(KOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -rf $(ODIR)/*.o outfile.png a.out main *.so build/ test_output_dir/* ../../../tests/pipeline.cpython-39-x86_64-linux-gnu.so ../../web_application/pipeline.cpython-39-x86_64-linux-gnu.so
