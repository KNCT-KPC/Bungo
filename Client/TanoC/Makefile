CFLAGS=-Wall -O2 -march=native

tanoc: tano.o common.o kcommon.o
	gcc $(CFLAGS) tano.o common.o kcommon.o -o tanoc

tanoc.o: tano.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) tano.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm tanoc

