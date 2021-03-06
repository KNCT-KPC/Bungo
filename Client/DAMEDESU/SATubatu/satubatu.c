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
#include <unistd.h>
#include <sys/wait.h>
#include "common.h"

#define	CLIENT_NAME	"SATubatu"
#define	SERVER_IPADDR	"127.0.0.1"

#define	SAT_SOLVER	"glueminisat"
#define	DIMACS_INPUT_FILE	"/tmp/satubatu.dimacs"
#define	DIMACS_OUTPUT_FILE	"/tmp/satubatu.out"
#define	VARIDX(col, row, n, x, y)	((((col+2) * (row+2)) * (n) + (col+2) * (y) + (x)) + 1)


/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void dump(int *map, int x1, int y1, int x2, int y2, int *stones, int n);
void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n);
unsigned int createDIMACSfile(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n, int8_t *sat_stones);
int satSolve();
void visual(unsigned int idx, int nega, int col, int row, int *map, int x1, int y1, int x2, int y2);

int solver(FILE *fp, int8_t *sat_stones, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	x2++; y2++;
	printf("\n\n\n");
	
	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);
	
	// Z to SAT
	int col = x2 - x1;
	int row = y2 - y1;
	unsigned int vars = createDIMACSfile(fp, col, row, x1, y1, x2, y2, n, sat_stones);
	
	// Run the SAT Solver
	fflush(fp);
	int sat_flg = satSolve();
	if (sat_flg < 0) return EXIT_FAILURE;
	if (sat_flg == 0) {
		printf("UNSAT\n");
		return EXIT_FAILURE;
	}
	
	dump(map, x1, y1, x2, y2, stones, n);

	// SAT to Z
	printf("SAT\n");
	FILE *out = fopen(DIMACS_OUTPUT_FILE, "r");
	int i;
	for (i=0; i<4; i++) fgetc(out);
	int c = fgetc(out);
	if (c == 0x0d) c = fgetc(out);

	do {
		unsigned int idx = 0;
		int nega = 0;
		if (c == '-') {
			nega = 1;
			c = fgetc(out);
		}
		
		do {
			idx *= 10;
			idx += c - '0';
		} while ((c = fgetc(out)) != ' ');
		
		if (idx > vars) break;
		visual(idx, nega, col, row, map, x1, y1, x2, y2);
	} while ((c = fgetc(out)) != '0');

	fclose(out);
	

	/* Map */
	printf("\nTWO\n");
	int x, y;
	printf("Map\n");
	for (y=0; y<32; y++) {
		printf("\t");
		for (x=0; x<32; x++) printf("%d", MAP(x, y));
		printf("\n");
	}
	printf("\n");




	
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
	int8_t sat_stones[12551] = {};	// ((16 * 256) * 2) + (((1023 + (33 * 4)) * 2) + 1) + (1024 * 2)
	
	FILE *fp = fopen(DIMACS_INPUT_FILE, "w");
	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		//dump(map, x1, y1, x2, y2, stones, n);
		if (solver(fp, sat_stones, map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE)
			break;
	}
	fclose(fp);
	
	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*                                   Prepare                                  */
/*----------------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------------*/
/*                                   Clause                                   */
/*----------------------------------------------------------------------------*/
/*------------------------------------*/
/*               AtLeast              */
/*------------------------------------*/
unsigned int clauseAtLeast(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n)
{
	x1--; y1--;
	x2++; y2++;
	n++;
	
	int i, x, y;
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				fprintf(fp, "%u ", VARIDX(col, row, i, x-x1, y-y1));
			}
			fputs("0\n", fp);
		}
	}

	return (y2 - y1) * (x2 - x1);
}


/*------------------------------------*/
/*               AtMost               */
/*------------------------------------*/
unsigned int clauseAtMost(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n)
{
	x1--; y1--;
	x2++; y2++;
	n++;
	
	unsigned int clause = 0;

	int i, j, x, y;
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				for (j=i+1; j<n; j++) {
					fprintf(fp, "-%u -%u 0\n", VARIDX(col, row, i, x-x1, y-y1), VARIDX(col, row, j, x-x1, y-y1));
					clause++;
				}
			}
		}
	}

	return clause;
}


/*------------------------------------*/
/*              Obstacle              */
/*------------------------------------*/
unsigned int clauseObstacle(FILE *fp, int col, int row, int id, int8_t *obstacle, int x1, int y1, int x2, int y2)
{
	int x, y;
	unsigned int clause = 0;
	unsigned int ids[2311] = {};
	int len = 0;
	
	int idx = 0;
	while ((x = obstacle[idx++]) != -1) {
		y = obstacle[idx++];
		unsigned int cnf_id = VARIDX(col, row, id, x, y);
		fprintf(fp, "%u 0\n", cnf_id);
		clause++;
		ids[len++] = cnf_id;
	}

	x1--; y1--;
	x2++; y2++;	
	int i;
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			unsigned int cnf_id = VARIDX(col, row, id, x-x1, y-y1);
			int flg = 0;
			for (i=0; i<len; i++) {
				if (ids[i] != cnf_id) continue;
				flg = 1;
				break;
			}
			if (flg) continue;
			fprintf(fp, "-%u 0\n", cnf_id);
			clause++;
		}
	}
	
	return clause;
}


/*------------------------------------*/
/*                Order               */
/*------------------------------------*/
int clauseOrderSubNeighbor(FILE *fp, int col, int row, int id, int x, int y, int offset_x, int offset_y, int x1, int y1, int x2, int y2)
{
	int i;
	int dx[] = {1, 0, -1, 0};
	int dy[] = {0, 1, 0, -1};

	for (i=0; i<4; i++) {
		int abs_x = offset_y + (x + dx[i]);
		int abs_y = offset_y + (y + dy[i]);
		if (abs_x < x1 || abs_y < y1 || abs_x >= x2 || abs_y >= y2) return -1;
		fprintf(fp, "-%u ", VARIDX(col, row, id, abs_x, abs_y));
	}

	return 0;
}

int clauseOrderSub(FILE *fp, int col, int row, int n1, int n2, int b_x, int b_y, int o_x, int o_y, int n, int8_t *zk, int x1, int y1, int x2, int y2)
{
	if (o_x < x1 || o_y < y1 || o_x >= x2 || o_y >= y2) return -1;

	int flg = (n1 < 0);
	fprintf(fp, "-%u ", VARIDX(col, row, flg ? 1 : n2, b_x, b_y));

	if (!flg) {
		int idx = 0;
		int offset_x = 0;
		int offset_y = 0;

		do {
			if (clauseOrderSubNeighbor(fp, col, row, n1, b_x, b_y, offset_x, offset_y, x1, y1, x2, y2) == -1) return -1;
			if (clauseOrderSubNeighbor(fp, col, row, n2, b_x, b_y, offset_x, offset_y, x1, y1, x2, y2) == -1) return -1;
			offset_x = zk[idx++];
			offset_y = zk[idx++];
		} while (offset_x != 0 || offset_y != 0);
	}

	int i;
	if (flg) {
		for (i=0; i<=n; i++) fprintf(fp, "%u ", VARIDX(col, row, i, o_x, o_y));
	} else {
		fprintf(fp, "%u ", VARIDX(col, row, n1, o_x, o_y));
		for (i=n2; i<=n; i++) fprintf(fp, "%u ", VARIDX(col, row, i, o_x, o_y));
	}

	fprintf(fp, "0\n");
	return 0;
}

unsigned int clauseOrder(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n, int8_t *sat_stones)
{
	fpos_t pos;
	unsigned int clause = 0;

	int i;
	for (i=0; i<n; i++) {
		int idx = i << 5;
		int8_t *zk = &sat_stones[i << 5];
		int x = 0, y = 0;

		do {
			int j, k;
			for (j=y1; j<y2; j++) {
				for (k=x1; k<x2; k++) {
					int base_x = (k - x1) + 1;
					int base_y = (j - y1) + 1;
					int offset_x = base_x + x;
					int offset_y = base_y + y;

					int l;
					int dx[] = {1, 0, -1, 0};
					int dy[] = {0, 1, 0, -1};

					for (l=0; l<4; l++) {
						fgetpos(fp, &pos);
						if (clauseOrderSub(fp, col, row, i-1, i, base_x, base_y, offset_x+dx[l], offset_y+dy[l], n, zk, x1, y1, x2, y2) == -1) {
							fsetpos(fp, &pos);
							continue;
						}
						clause++;
					}
				}
			}

			x = sat_stones[idx++];
			y = sat_stones[idx++];
		} while (x != 0 || y != 0);
	}
	
	for (i=0; i<300; i++) fprintf(fp, "\n");
	
	return clause;
}


/*------------------------------------*/
/*            Define of zk            */
/*------------------------------------*/
void clauseDefineOperation(const int8_t *zk, int8_t *dst)
{
	int i, j;
	
	memcpy(dst, zk, sizeof(int8_t) << 5);
	for (i=0; i<32; i+=2) {
		dst[32 + i] = -zk[i];
		dst[32 + i+1] = zk[i+1];
	}
	
	for (i=2; i<8; i++) {
		for (j=0; j<32; j+=2) {
			dst[(i << 5) + j] = -dst[(i << 5) - 63 + j];
			dst[(i << 5) + j + 1] = dst[(i << 5) - 64 + j];
		}
	}
}

unsigned int clauseDefineSub(unsigned int vars, FILE *fp, int col, int row, int id, int b_x, int b_y, int8_t *zk)
{
	int len = 0;
	unsigned int ids[16];
	
	int idx = 0;
	int o_x = 0, o_y = 0;

	do {
		int x = b_x + o_x;
		int y = b_y + o_y;
		if (!(0 <= x && x < col) || !(0 <= y && y < row)) return 0;
		ids[len++] = VARIDX(col, row, id, x+1, y+1);
		
		o_x = zk[idx++];
		o_y = zk[idx++];
	} while (o_x != 0 || o_y != 0);

	int i;
	for (i=0; i<len; i++) fprintf(fp, "-%u %u 0\n", vars, ids[i]);
	for (i=0; i<len; i++) fprintf(fp, "-%u ", ids[i]);
	fprintf(fp, "%u 0\n", vars);
	
	return len + 1;
}

unsigned int clauseDefine(unsigned int *vars, FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n, int8_t *sat_stones)
{
	unsigned int clause = 0;
	
	int i, j, k, l;
	for (i=0; i<n; i++) {
		int8_t tmp[256];
		clauseDefineOperation(&sat_stones[i << 5], tmp);	// Vivid Read Operation

		for (j=y1; j<y2; j++) {
			for (k=x1; k<x2; k++) {
				unsigned int original_var = *vars;
				
				for (l=0; l<8; l++) {
					unsigned int success = clauseDefineSub(*vars+1, fp, col, row, i, k-x1, j-y1, &tmp[l << 5]);
					if (success == 0) continue;
					clause += success;
					(*vars)++;
				}

				/*
				unsigned int t;
				for (t=original_var; t<(*vars); t++) fprintf(fp, "-%u %u 0\n", VARIDX(col, row, i, k-x1+1, j-y1+1), t+1);
				clause++;
				for (t=original_var; t<(*vars); t++) fprintf(fp, "%u -%u 0\n", VARIDX(col, row, i, k-x1+1, j-y1+1), t+1);
				clause++;
				*/
				
				unsigned int t;
				fprintf(fp, "-%u ", VARIDX(col, row, i, k-x1+1, j-y1+1));
				for (t=original_var; t<(*vars); t++) fprintf(fp, "%u ", t+1);
				fprintf(fp, "0\n");
				clause++;
				
				/*
				fprintf(fp, "%u ", VARIDX(col, row, i, k-x1+1, j-y1+1));
				for (t=original_var; t<(*vars); t++) fprintf(fp, "-%u ", t+1);
				fprintf(fp, "0\n");
				clause++;
				*/
			}
		}
	}
	
	return clause;
}


/*------------------------------------*/
/*           Create the file          */
/*------------------------------------*/
unsigned int createDIMACSfile(FILE *fp, int col, int row, int x1, int y1, int x2, int y2, int n, int8_t *sat_stones)
{
	unsigned int clause = 0;
	unsigned int vars = (col + 2) * (row + 2) * (n + 1);
	unsigned int hidden_vars = vars;
	
	fprintf(fp, "p cnf 4294967295 4294967295\n");
	
	clause += clauseAtLeast(fp, col, row, x1, y1, x2, y2, n);
	clause += clauseAtMost(fp, col, row, x1, y1, x2, y2, n);
	clause += clauseObstacle(fp, col, row, n, &sat_stones[n << 5], x1, y1, x2, y2);
	//clause += clauseOrder(fp, col, row, x1, y1, x2, y2, n, sat_stones);
	clause += clauseDefine(&hidden_vars, fp, col, row, x1, y1, x2, y2, n, sat_stones);
	
	char str[28];
	snprintf(str, 28, "p cnf %u %u", hidden_vars, clause);
	int i, len = strlen(str);
	for (i=len; i<27; i++) str[i] = ' ';
	str[i] = '\0';
	
	fseek(fp, SEEK_SET, 0);
	fprintf(fp, "%s", str);
	
	return vars;
}


/*----------------------------------------------------------------------------*/
/*                             Run the SAT Solver                             */
/*----------------------------------------------------------------------------*/
int satSolve()
{
	pid_t pid = fork();
	if (pid < 0) return -1;
	if (pid == 0) {
		close(0);
		close(1);
		execlp(SAT_SOLVER, SAT_SOLVER, DIMACS_INPUT_FILE, DIMACS_OUTPUT_FILE, NULL);
		_exit(EXIT_FAILURE);
	}
	
	int status;
	waitpid(pid, &status, 0);
	if (!WIFEXITED(status)) return -1;
	int code = WEXITSTATUS(status);
	if (code < 10) return -1;
	
	return (code == 20) ? 0 : 1;
}


/*----------------------------------------------------------------------------*/
/*                                  Visualize                                 */
/*----------------------------------------------------------------------------*/
void visual(unsigned int idx, int nega, int col, int row, int *map, int x1, int y1, int x2, int y2)
{
	if (nega) return;

	div_t tmp1 = div(idx - 1, (col + 2) * (row + 2));
	div_t tmp2 = div(tmp1.rem, (col + 2));
	
	int n = tmp1.quot;
	int x = tmp2.rem - 1;
	int y = tmp2.quot - 1;

	if (!(x1 <= x && x < x2)) return;
	if (!(y1 <= y && y < y2)) return;
	
	map[(y << 5) + x] = n;
}

