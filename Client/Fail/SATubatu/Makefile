satubatu: satubatu.o common.o
	gcc satubatu.o common.o -o satubatu -Wall

satubatu.o:	satubatu.c common.h
	gcc -c satubatu.c -Wall

common.o: common.c common.h
	gcc -c common.c -Wall


.PHONY: clean distclean

clean:
	-rm *.o

distclean: clean
	-rm satubatu

