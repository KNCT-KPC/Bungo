#include <stdio.h>

int main(int argc, char *argv[])
{
	int n = 5;
	int i, j;

	for (i=0; i<n; i++) {
		for (j=i+1; j<n; j++) {
			printf("%d and %d\n", i, j);
		}
	}

}
