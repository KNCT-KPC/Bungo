#CFLAGS=-Wall -O2 -march=native
CFLAGS=-Wall -g -O0
#CFLAGS=-Wall

cross: cross.o common.o kcommon.o
	gcc $(CFLAGS) cross.o common.o kcommon.o -o cross

cross.o: cross.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CFLAGS) cross.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm cross
