milkt: milkt.o common.o
	gcc milkt.o common.o -o milkt -Wall

milkt.o: milkt.c ../SATubatu/common.h
	gcc -c milkt.c -Wall

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c ../SATubatu/common.c -Wall


.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm milkt

