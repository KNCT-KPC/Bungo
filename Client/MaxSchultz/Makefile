danke: danke.o common.o
	gcc danke.o common.o -o danke -Wall

danke.o: danke.c ../SATubatu/common.h
	gcc -c danke.c -Wall

common.o: ../SATubatu/common.c ../SATubatu/common.h
	gcc -c ../SATubatu/common.c -Wall


.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm danke

