CFLAGS=-Wall -O2 -march=native

sabic: sabi.o common.o kcommon.o
	gcc $(CFLAGS) sabi.o common.o kcommon.o -o sabic

sabic.o: sabi.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) sabi.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm sabic

