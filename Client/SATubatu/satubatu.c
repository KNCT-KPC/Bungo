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

#define	VARIDX(col, row, n, x, y)	(((col+2) * (row+2)) * (n) + (col+2) * (y) + (x))


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


unsigned long long clauseAtLeast(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n)
{
	n++;
	x1--; x2++;
	y1--; y2++;

	int i, x, y;
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				fprintf(fp, "%d ", VARIDX(col, row, i, x-x1, y-y1));
			}
			fputs("\n", fp);
		}
	}

	return (y2 - y1) * (x2 - x1);
}


unsigned long long clauseAtMost(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n)
{
	n++;
	x1--; x2++;
	y1--; y2++;

	unsigned long long clause = 0;

	int i, j, x, y;
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				for (j=i+1; j<n; j++) {
					fprintf(fp, "-%d -%d\n", VARIDX(col, row, i, x-x1, y-y1), VARIDX(col, row, j, x-x1, y-y1));
					clause++;
				}
			}
		}
	}

	return clause;
}


unsigned long long clauseObstacle(FILE *fp, int col, int row, int id, int8_t *obstacle)
{
	int x, y;
	unsigned long long clause = 0;
	
	int idx = 0;
	while ((x = obstacle[idx++]) != -1) {
		y = obstacle[idx++];
		fprintf(fp, "%d\n", VARIDX(col, row, id, x, y));
		clause++;
	}

	return clause;
}


int clauseOrderSubNeighbor(FILE *fp, int col, int row, int n, int x, int y, int offset_x, int offset_y)
{
	int i;
	int dx[] = {1, 0, -1, 0};
	int dy[] = {0, 1, 0, -1};

	for (i=0; i<4; i++) {
		int abs_x = offset_y + (x + dx[i]);
		int abs_y = offset_y + (y + dy[i]);
		if (abs_x < 0 || abs_y < 0) return -1;
		fprintf(fp, "-%d ", VARIDX(col, row, n, abs_x, abs_y));
	}

	return 0;
}

int clauseOrderSub(FILE *fp, int col, int row, int n1, int n2, int x1, int y1, int x2, int y2, int n, int8_t *n1_stones)
{
	fprintf(fp, "-%d ", VARIDX(col, row, n1, x1, y1));

	int idx = 0;
	if (n2 > 0) {
		int offset_x = 0;
		int offset_y = 0;
		do {
			if (clauseOrderSubNeighbor(fp, col, row, n1, x1, y1, offset_x, offset_y) == -1)
				return -1;
			if (clauseOrderSubNeighbor(fp, col, row, n2, x1, y1, offset_x, offset_y) == -1)
				return -1;
			offset_x = n1_stones[idx++];
			offset_y = n1_stones[idx++];
		} while (offset_x != 0 || offset_y != 0);
	}

	if (x2 < 0 || y2 < 0) return -1;
	fprintf(fp, "%d ", VARIDX(col, row, n1, x2, y2));
	
	int i;
	for (i=n2; i<=n; i++) {
		fprintf(fp, "%d ", VARIDX(col, row, i, x2, y2));
	}

	fprintf(fp, "\n");
	return 0;
}

unsigned long long clauseOrder(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n, int8_t *sat_stones)
{
	fpos_t pos;
	unsigned long long clause = 0;

	int i;
	for (i=0; i<n; i++) {
		int idx = i << 5;
		int x = 0, y = 0;
		int n1, n2;
		
		if (i == 0) {
			n1 = 0;
			n2 = -1;
		} else {
			n1 = i - 1;
			n2 = i;
		}

		do {
			int j, k;
			for (j=y1; j<y2; j++) {
				for (k=x1; k<x2; k++) {
					fgetpos(fp, &pos);
					if (clauseOrderSub(fp, col, row, n1, n2, k-x1+1, j-y1+1, k-x+1, j-y+1, n, &sat_stones[idx]) == -1) {
						fsetpos(fp, &pos);
					} else {
						clause++;
					}
				}
			}

			x = sat_stones[idx++];
			y = sat_stones[idx++];
		} while (x != 0 || y != 0);
	}

	return clause;
}


int solver(FILE *fp, int8_t *sat_stones, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	x2++; y2++;
	
	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);
	
	// Create DIMACS file
	int col = x2 - x1;
	int row = y2 - y1;

	unsigned long long clause = 0;
	unsigned long long vars = (col + 2) * (row + 2) * (n + 1);

	fprintf(fp, "c at-leaset\n");
	clause += clauseAtLeast(fp, col, row, x1, y1, x2, y2, n);
	
	fprintf(fp, "c at-most\n");
	clause += clauseAtMost(fp, col, row, x1, y1, x2, y2, n);

	fprintf(fp, "c obstacle\n");
	clause += clauseObstacle(fp, col, row, n, &sat_stones[n << 5]);

	fprintf(fp, "c order\n");
	clause += clauseOrder(fp, col, row, x1, y1, x2, y2, n, sat_stones);

	fprintf(fp, "c define\n");

	printf("clause = %llu, vars = %llu\n", clause, vars);

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
	int8_t sat_stones[12551];	// ((16 * 256) * 2) + (((1023 + (33 * 4)) * 2) + 1) + (1024 * 2)
	
	FILE *fp = fopen("/tmp/satubatu.dimacs", "w");
	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		dump(map, x1, y1, x2, y2, stones, n);
		if (solver(fp, sat_stones, map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE)
			break;
	}
	fclose(fp);
	
	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}
