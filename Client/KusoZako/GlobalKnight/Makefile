CFLAGS=-Wall -O2 -march=native -fopenmp
#CFLAGS=-Wall -g -O0
#CFLAGS=-Wall

gknight: gknight.o common.o kcommon.o
	gcc $(CFLAGS) gknight.o common.o kcommon.o -o gknight -lglib-2.0

gknight.o: gknight.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c `pkg-config --cflags glib-2.0` $(CFLAGS) gknight.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm gknight
