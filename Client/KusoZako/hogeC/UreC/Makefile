CFLAGS=-Wall -O2 -march=native

urec: ure.o common.o kcommon.o
	gcc $(CFLAGS) ure.o common.o kcommon.o -o urec

urec.o: ure.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) ure.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm urec

