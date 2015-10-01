CFLAGS=-Wall

kanac: kana.o common.o kcommon.o
	gcc $(CFLAGS) kana.o common.o kcommon.o -o kanac

kanac.o: kana.c ../SATubatu/common.h kcommon.h
	gcc -c $(CLAGS) kana.c

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c

kcommon.o: kcommon.c kcommon.h
	gcc -c $(CFLAGS) kcommon.c

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kanac

