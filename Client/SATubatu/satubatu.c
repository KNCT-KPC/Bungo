/******************************************************************************/
/*                                                                            */
/*                                  SATubatu                                  */
/*                       -- Yamoto=san jissai kawaii --                       */
/*                                                                            */
/******************************************************************************/
#include "common.h"

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
/* for Sleep */
#ifdef WINDOWS
	#include <windows.h>
	#define	sleep(n)	Sleep((n) * 1000)
#else
	#include <unistd.h>
#endif

int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	/* Range */
	printf("(%d, %d) ~ (%d, %d)\n\n", x1, y1, x2, y2);

	/* Map */
	int x, y;
	printf("Map\n");
	for (y=0; y<32; y++) {
		printf("\t");
		for (x=0; x<32; x++) printf("%d", MAP(x, y));
		printf("\n");
	}
	printf("\n");

	/* Stones */
	int i;
	printf("Stones\n");
	for (i=0; i<n; i++) {
		printf("\tstone %d\n", i+1);
		for (y=0; y<8; y++) {
			printf("\t\t");
			for (x=0; x<8; x++) printf("%d", STONE(i, x, y));
			printf("\n");
		}
	}
	printf("\n");

	/* Solutions for light.txt */
	char *solutions[] = {
		"H 0 2 2\n",
		"H 0 2 2\nT 0 -6 0\n",
		"H 0 2 2\nT 0 -6 0\n\nH 0 1 -1\n",
		NULL
	};

	printf("Solutions\n");
	for (i=0; (solutions[i] != NULL); i++) {
		sleep(5);
		printf("Solution %d\n", i+1);

		// Padding
		int j, line = 0;
		for (j=0; (solutions[i][j] != '\0'); j++) {
			if (solutions[i][j] == '\n') line++;
		}

		// Like a START BIT
		sendMsg("S");

		// Main
		sendMsg(solutions[i]);
		for (j=0; j<(n - line); j++) sendMsg("");

		// Prepare for next problem
		if (sendMsg("E") == EXIT_FAILURE)
			return EXIT_SUCCESS;	// transition to `Ready state`
	}

	// Forced termination
	return EXIT_FAILURE;
}


/*------------------------------------*/
/*                Main                */
/*------------------------------------*/
int main(int argc, char *argv[])
{
	int osfhandle, sd;
	initClient(CLIENT_NAME, &osfhandle, &sd);
	
	while (1) {
		int x1, y1, x2, y2, n;
		int map[1024];
		int stones[16384];
		
		int bflg = ready(map, &x1, &y1, &x2, &y2, stones, &n);
		if (bflg || solver(map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE) break;
	}
	
	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}
