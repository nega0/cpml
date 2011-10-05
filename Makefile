CC= gcc
LIBS= -ldl

OBJS= cpml.o

cpml: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core cpml
