#CFLAGS=-Wall -O2 -march=native
CFLAGS=-Wall

knight: knight.o common.o kcommon.o
	gcc $(CFLAGS) -lglib-2.0 knight.o common.o kcommon.o -o knight

knight.o: knight.c ../SATubatu/common.h ../KanaC/kcommon.h
	gcc -c $(CLAGS) `pkg-config --cflags glib-2.0` knight.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: ../KanaC/kcommon.c ../KanaC/kcommon.h
	gcc -c $(CFLAGS) ../KanaC/kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm knight
