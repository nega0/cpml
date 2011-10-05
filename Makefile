AMDLIBM_DIR=/opt/amdlibm-3-0-1-lin64/lib/dynamic

CC= gcc
LIBS= -ldl

OBJS= cpml.o

cpml: $(OBJS)
	gcc -m64 -o $@ $^ $(CFLAGS) $(LIBS)

run: cpml
	LD_LIBRARY_PATH=$(AMDLIBM_DIR) ./cpml

cmp: cpml 
	LD_LIBRARY_PATH=$(AMDLIBM_DIR) ./cpml | perl rfmt.pl

.PHONY: clean

clean:
	rm -f *.o *~ core cpml
