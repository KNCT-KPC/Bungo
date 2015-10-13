CFLAGS=-Wall -O2 -march=native

okac: oka.o common.o kcommon.o
	gcc $(CFLAGS) oka.o common.o kcommon.o -o okac

okac.o: oka.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) oka.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm okac

