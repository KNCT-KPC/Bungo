/******************************************************************************/
/*                                                                            */
/*                                    Fuck                                    */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include "../SATubatu/common.h"

#define	CLIENT_NAME	"KanaC"
#define	SERVER_IPADDR	"127.0.0.1"

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
int cmp(const void *n, const void *m)
{
	return *(double *)n - *(double *)m;
}

void search(double best, double *scores, int n)
{

}


int solver(FILE *fp, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	int i, j;
	dump(map, x1, y1, x2, y2, stones, n);

	// Score
	double scores[256] = {};
	double sorted[256] = {};
	for (i=0 i<n; i++) scores[i] = 0;
	memcpy(sorted, scores, sizeof(double));
	qsort(sorted, n, sizeof(double), cmp);

	for (i=0; i<n; i++) {
		search(sorted[i], scores);
	}


	/*
	sendMsg("S");
	sendMsg("");
	if (sendMsg("E") == EXIT_FAILURE) return EXIT_SUCCESS;
	*/
	return EXIT_FAILURE;
}


/*----------------------------------------------------------------------------*/
/*                                    Main                                    */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	int osfhandle, sd;
	initClient(CLIENT_NAME, SERVER_IPADDR, &osfhandle, &sd);
	
	int x1, y1, x2, y2, n;
	int map[1024];
	int stones[16384];

	FILE *fp = fopen(DANKE_FILE, "w");
	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		if (solver(fp, map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE) break;
	}
	fclose(fp);
	
	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}

