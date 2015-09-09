/******************************************************************************/
/*                                                                            */
/*                                  SATubatu                                  */
/*                       -- Yamoto=san jissai kawaii --                       */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "common.h"

#define	CLIENT_NAME	"SATubatu"
#define	SERVER_IPADDR	"127.0.0.1"

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void dump(int *map, int x1, int y1, int x2, int y2, int *stones, int n)
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
}

void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n)
{
	int i;
	int map[1024];
	int stones[16384];
	
	memcpy(map, map_base, sizeof(int) * 1024);
	memcpy(stones, stones_base, sizeof(int) * 16384);
	
	// Stone
	for (i=0; i<n; i++) {
		int idx = (i << 5);
		int blk = 0;
		int anc_x = -1, anc_y;
				
		int x, y;
		for (y=0; y<8; y++) {
			for (x=0; x<8; x++) {
				if (STONE(i, x, y) == 0) continue;
				
				if (anc_x < 0) {
					anc_x = x;
					anc_y = y;
				} else {
					sat_stones[idx++] = x - anc_x;
					sat_stones[idx++] = y - anc_y;
				}
				
				if (++blk >= 16) goto SIKATANAINE;
			}
		}
	
	SIKATANAINE:
		sat_stones[idx++] = 0;
		sat_stones[idx] = 0;
	}
	
	// Obstacle
	int x, y;
	int idx = (n << 5);
	int anc_x = x1 - 1, anc_y = y1 - 1;
	
	for (y=anc_y; y<(y2 + 1); y++) {
		for (x=anc_x; x<(x2 + 1); x++) {
			if (y == anc_y || y == y2 || x == anc_x || x == x2 || MAP(x, y) == 1) {
				sat_stones[idx++] = x - anc_x;
				sat_stones[idx++] = y - anc_y;
				continue;
			}
		}
	}
	sat_stones[idx] = -1;
	
	// Field
	// unimplemented
}

int solver(FILE *fp, int8_t *sat_stones, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	x2++;
	y2++;
	
	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);
	
	// Create DIMACS file
	
	
	// Run SAT Solver
	fflush(fp);
	
	
	
	
	
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
	int8_t sat_stones[12550];	// ((16 * 256) * 2) + ((1023 + (33 * 4)) * 2) + (1024 * 2)
	
	FILE *fp = fopen("satubatu.dimacs", "w");
	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		dump(map, x1, y1, x2, y2, stones, n);
		if (solver(fp, sat_stones, map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE)
			break;
	}
	fclose(fp);
	
	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}
