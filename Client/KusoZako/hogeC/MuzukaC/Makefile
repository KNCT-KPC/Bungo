CFLAGS=-Wall -O2 -march=native

muzukac: muzuka.o common.o kcommon.o
	gcc $(CFLAGS) muzuka.o common.o kcommon.o -o muzukac

muzukac.o: muzuka.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) muzuka.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm muzukac

