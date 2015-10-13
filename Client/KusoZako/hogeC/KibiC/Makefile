CFLAGS=-Wall -O2 -march=native

kibic: kibi.o common.o kcommon.o
	gcc $(CFLAGS) kibi.o common.o kcommon.o -o kibic

kibic.o: kibi.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) kibi.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kibic

