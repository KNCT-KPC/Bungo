CFLAGS=-Wall -O2 -march=native

iyarac: iyara.o common.o kcommon.o
	gcc $(CFLAGS) iyara.o common.o kcommon.o -o iyarac

iyarac.o: iyara.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) iyara.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm iyarac

