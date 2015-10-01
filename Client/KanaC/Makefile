#CFLAGS=-g -O0

kanac: kana.o common.o
	gcc $(CFLAGS) kana.o common.o -o kanac -Wall

kanac.o: kana.c ../SATubatu/common.h
	gcc -c $(CLAGS) kana.c -Wall

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c $(CFLAGS) ../SATubatu/common.c -Wall

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kanac

