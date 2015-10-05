CFLAGS=-Wall -O2 -march=native
#CFLAGS=-Wall -g -O0
#CFLAGS=-Wall

kanacpp: kanacpp.o common.o kcommon.o
	gcc $(CFLAGS) kanacpp.o common.o kcommon.o -o kanacpp -lglib-2.0

kanacpp.o: kanacpp.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c `pkg-config --cflags glib-2.0` $(CFLAGS) kanacpp.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kanacpp
