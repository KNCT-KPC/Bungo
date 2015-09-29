#include <stdio.h>

int main(int argc, char *argv[])
{
	int len = 4, i, j;

	for (i=0; i<len; i++) {
		for(j=0; j<len; j++) {
			printf("%s%d", (i == j) ? "" : "-", i);
		}
		printf("\t");
	}

	return 0;
}
