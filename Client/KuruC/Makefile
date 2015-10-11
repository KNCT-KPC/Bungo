CFLAGS=-Wall -O2 -march=native

kuruc: kuru.o common.o kcommon.o
	gcc $(CFLAGS) kuru.o common.o kcommon.o -o kuruc

kuruc.o: kuru.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) kuru.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kuruc

