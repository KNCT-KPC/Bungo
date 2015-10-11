#include <stdio.h>

int main(int argc, char *argv[])
{

	int i;
	for (i=0; i<32; i++) {
		printf("%d: %d\n", i, (32 - ((i + ((16) / 2)) % 32)) % 32);
	}
	return 0;



}
