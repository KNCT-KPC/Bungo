/******************************************************************************/
/*                                                                            */
/*                                     Max                                    */
/*                             -- Danke, Danke --                             */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include "../SATubatu/common.h"

#define	CLIENT_NAME	"Danke"
#define	SERVER_IPADDR	"127.0.0.1"

#define	DANKE_FILE	"/tmp/danke.py"

#define	MIN(a, b)	(((a) < (b)) ? (a) : (b))
#define	MAX(a, b)	(((a) > (b)) ? (a) : (b))

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n);
void createDankefile(FILE *fp, int *map, int x1, int y1, int x2, int y2, int8_t *sat_stones, int n);
int satSolve(int *map);

int solver(FILE *fp, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	int8_t sat_stones[8192] = {};	// (16 * 2) * 256
	x2++; y2++;
	
	dump(map, x1, y1, x2, y2, stones, n);

	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);
	createDankefile(fp, map, x1, y1, x2, y2, sat_stones, n);

	// Run Python module
	fflush(fp);
	/*
	int code = satSolve(map);

	printf("code = %d\n", code);
	dumpMap(map, x1, y1, x2, y2, n);
	*/

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



/*----------------------------------------------------------------------------*/
/*                                   Prepare                                  */
/*----------------------------------------------------------------------------*/
void s2s(const int *stone, int8_t *dst)
{
	int idx = 0;
	int blk = 0;
	int anc_x = -1, anc_y;
				
	int x, y;
	for (y=0; y<8; y++) {
		for (x=0; x<8; x++) {
			if (stone[(y << 3) + x] == 0) continue;
			
			if (anc_x < 0) {
				anc_x = x;
				anc_y = y;
			} else {
				dst[idx++] = x - anc_x;
				dst[idx++] = y - anc_y;
			}
			
			if (++blk >= 16) goto SIKATANAINE;
		}
	}
	
	SIKATANAINE:
		dst[idx++] = 0;
		dst[idx] = 0;
}

void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n)
{
	int i, stones[16384];
	
	memcpy(stones, stones_base, sizeof(int) * 16384);
	for (i=0; i<n; i++) s2s(&stones[i << 6], &sat_stones[i << 5]);
}

int KVStrio(int x, int y, int val)
{
	static int kvs[1156];
	
	int idx = (x + 1) * 34 + (y + 1);
	if (val >= 0) kvs[idx] = val;
	return kvs[idx];
}

/*----------------------------------------------------------------------------*/
/*                             Create a Danke file                            */
/*----------------------------------------------------------------------------*/
void BlockNormalize(int8_t *zk)
{
	int i, len = 2;

	int offset[] = {0, 0};
	for (i=0; i<32; i+=2) {
		int x = zk[i];
		int y = zk[i+1];
		if (x == 0 && y == 0) break;
		offset[0] = MIN(offset[0], x);
		offset[1] = MIN(offset[1], y);
		len += 2;
	}

	int stone[64] = {};
	for (i=0; i<len; i+=2) stone[((zk[i+1]-offset[1]) << 3) + (zk[i]-offset[0])] = 1;
	s2s(stone, zk);
}

// Vivid Red Operation
void BlockDefineOperation(const int8_t *zk, int8_t *dst)
{
	int i, j, k;
	
	// Front and Back
	memcpy(dst, zk, sizeof(int8_t) << 5);
	for (i=0; i<32; i+=2) {
		dst[32 + i] = -zk[i];
		dst[32 + i+1] = zk[i+1];
	}
	
	// Rotate
	for (i=2; i<8; i++) {
		int idx = (i << 5);
		for (j=0; j<32; j+=2) {
			dst[idx + j] = -dst[idx - 63 + j];
			dst[idx + j + 1] = dst[idx - 64 + j];
		}
	}

	// Normalize
	for (i=0; i<8; i++) BlockNormalize(&dst[i << 5]);

	// dedup
	for (i=1; i<8; i++) {
		int m_idx = i << 5;

		for (j=0; j<i; j++) {
			int s_idx = j << 5;
			if (dst[s_idx] == INT8_MAX) continue;

			int dup = 1;
			for (k=0; k<30; k++) {
				if (dst[m_idx + k] != dst[s_idx + k]) {
					dup = 0;
					break;
				}
			}

			if (!dup) continue;
			dst[m_idx] = INT8_MAX;
			break;
		}
	}
}

void OneOneOne(FILE *fp, int n, const int8_t *define, int len)
{
	int i, j, k;
	const int8_t *p;

	fprintf(fp, "Or(");
	for (i=0; i<len; i++) {
		fprintf(fp, "And(");
		for (j=0; j<len; j++) {
			p = &define[j << 5];
			fprintf(fp, "%sAnd(", (i == j) ? "" : "Not(");
			for (k=0; (k<32 && p[k] >= 0); k+=2) {
				int x = p[k] + 1;
				int y = p[k+1] + 1;
				fprintf(fp, "xs[%d] == %d,", KVStrio(x, y, -1), n);
			}
			fprintf(fp, ")%s,\n", (i == j) ? "" : ")");
		}
		fprintf(fp, "),");
	}
	fprintf(fp, ")");
}

void createDankefile(FILE *fp, int *map, int x1, int y1, int x2, int y2, int8_t *sat_stones, int n)
{
	int x,y, i, j, k, xx, yy;

	// Mass Define
	i = 0;
	fprintf(fp, "from z3 import *\n\ns = Solver()\n\n# Mass Def\nxs = []\n");
	for(y=(y1-1); y<=y2; y++) {
		for(x=(x1-1); x<=x2; x++) {
			fprintf(fp, "xs.append(Int('x_%d_%d'))\n", x, y);
			if (!((y1 <= y) && (y < y2)) || !((x1 <= x) && (x < x2)) || MAP(x, y) == 1) {
				fprintf(fp, "s.add(xs[%d] == %d)\n", i, n);
			} else {
				fprintf(fp, "s.add(0 <= xs[%d], xs[%d] < %d)\n", i, i, n);
			}
			KVStrio(x, y, i++);
		}
	}
	
	// Block Define
	int col = y2 - y1;
	fprintf(fp, "\n# Block Def\nys = []\n");
	for (i=0; i<n; i++) {
		int len = 0;
		int8_t tmp[256];
		int8_t define[262144] = {};
		BlockDefineOperation(&sat_stones[i << 5], tmp);
		
		fprintf(fp, "ys.append(Bool('y_%d'))\n", i);
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				for (j=0; j<8; j++) {
					int idx = j << 5;
					if (tmp[idx] == INT8_MAX) continue;

					k = len << 5;
					int l = 0, o_x = 0, o_y = 0;
					do {
						int a_x = x + o_x;
						int a_y = y + o_y;
						if (!((y1 <= a_y) && (a_y < y2)) || !((x1 <= a_x) && (a_x < x2))) goto DAMEDESU;
						define[k++] = a_x;
						define[k++] = a_y;
						o_x = tmp[idx++];
						o_y = tmp[idx++];
						l++;
					} while (o_x != 0 || o_y != 0);
					
					len++;
					if (l < 16) define[k] = -1;

				DAMEDESU:
					continue;	// NOOP
				}
			}
		}

		fprintf(fp, "s.add(ys[%d] == (", i);
		OneOneOne(fp, i, define, len);
		fprintf(fp, "))\n");
	}

	// Anchor
	for (i=0; i<n; i++) {
		fprintf(fp, "s.add(Implies(ys[%d] == False, And(", i);

		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				fprintf(fp, "xs[%d] != %d,", KVStrio(x, y, -1), i);
			}
		}

		fprintf(fp, ")))\n");
	}

	/*
	// Only one railgun
	for (i=0; i<n; i++) {
		fprintf(fp, "(<= (+");
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				fprintf(fp, " y_%d_%d_%d", x, y, i);
			}
		}
		fprintf(fp, ") 1)\n");
	}

	// TEST
	fprintf(fp, "(not (= x_3_0 5))\n");
	fprintf(fp, "(not (= x_3_3 6))\n");
	//fprintf(fp, "(not (= x_3_3 6))\n");
	//fprintf(fp, "(not (= x_3_3 5))\n");

	// Order
	for (i=0; i<n; i++) {
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				for (j=i+1; j<n; j++) {
					fprintf(fp, "(=> (and");
					isUse(fp, x1, y1, x2, y2, i, j);
					fprintf(fp, " (= x_%d_%d %d)) (or", x, y, j);
					
					int dx[] = {1, 0, -1, 0};
					int dy[] = {0, 1, 0, -1};
					for (k=0; k<4; k++) fprintf(fp, " (= x_%d_%d %d)", x+dx[k], y+dy[k], i);
					
					fprintf(fp, "))\n");
				}
			}
		}
	}
	*/

	// Dump
	fprintf(fp, "\nr = s.check()\nif r != sat: exit(10)\n\nm = s.model()\nfor d in m: print(d, '=', m[d])\n");
}


/*----------------------------------------------------------------------------*/
/*                                  Run Sugar?                                 */
/*----------------------------------------------------------------------------*/
int satSolve(int *map)
{
	FILE *fp;
	char buf[128];

	snprintf(buf, 128, "python %s", DANKE_FILE);
	if ((fp = popen(buf, "r")) == NULL) return -1;

	int i;
	for (i=0; i<1024; i++) map[i] = -1;

	while (fgets(buf, 128, fp) != NULL) {
		printf("DEBUG: %s", buf);
		if (buf[0] != 'x') continue;

		char *p = strchr(buf, '\n');
		if (p != NULL) *p = '\0';
		
		int x, y, n;
		sscanf(buf, "x_%d_%d = %d", &x, &y, &n);
		
		if ((x < 0) || (x >= 32) || (y < 0) || (y >= 32)) continue;
		map[((y) << 5) + (x)] = n;
	}

	int code = pclose(fp);
	return code == 0 ? 1 : code == 10 ? 0 : -1;
}

