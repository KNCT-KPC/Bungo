kanac: kana.o common.o
	gcc kana.o common.o -o kanac -Wall

kanac.o: kana.c ../SATubatu/common.h
	gcc -c kana.c -Wall

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c ../SATubatu/common.c -Wall

.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm kanac
