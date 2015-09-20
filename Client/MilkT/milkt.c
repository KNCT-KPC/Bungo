/******************************************************************************/
/*                                                                            */
/*                                    MilkT                                   */
/*                           -- Yurika, Fight-oh! --                          */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../SATubatu/common.h"

#define	CLIENT_NAME	"MilkT"
#define	SERVER_IPADDR	"127.0.0.1"

#define	CSP_INPUT_FILE	"/tmp/milkt.csp"


/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n);
void createCSPfile(FILE *fp, int *map, int x1, int y1, int x2, int y2, int8_t *sat_stones, int n);
int satSolve(int *map);

int solver(FILE *fp, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	int8_t sat_stones[8192] = {};	// (16 * 2) * 256
	x2++; y2++;
	
	dump(map, x1, y1, x2, y2, stones, n);

	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);

	// Z to CSP
	createCSPfile(fp, map, x1, y1, x2, y2, sat_stones, n);

	// Run Sugar
	FILE *sat_fp;
	int code = satSolve(map);

	printf("code = %d\n", code);
	dumpMap(map, x1, y1, x2, y2);

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

	FILE *fp = fopen(CSP_INPUT_FILE, "w");
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
}


/*----------------------------------------------------------------------------*/
/*                              Create a CSP file                             */
/*----------------------------------------------------------------------------*/
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

	// dedup
	for (i=1; i<8; i++) {
		int m_idx = i << 5;

		for (j=0; j<i; j++) {
			int s_idx = j << 5;
			if (dst[s_idx] == 0xff) continue;

			for (k=0; k<30; k++) {
				if (dst[m_idx + k] != dst[s_idx + j]) goto NO_DUP;
			}
		}

		// dup
		dst[m_idx] = 0xff;

		// no
	NO_DUP:
		continue; // noop
	}
}

void createCSPfile(FILE *fp, int *map, int x1, int y1, int x2, int y2, int8_t *sat_stones, int n)
{
	int x,y, i, j, k, l, m;

	// Mass Define
	fprintf(fp, "(domain zk 0 %d)\n", n - 1);
	fprintf(fp, "(domain obstacle %d)\n", n);
	
	for(y=(y1-1); y<=y2; y++) {
		for(x=(x1-1); x<=x2; x++) {
			fprintf(fp, "(int x_%d_%d %s)\n", x, y, (!((y1 <= y) && (y < y2)) || !((x1 <= x) && (x < x2)) || MAP(x, y) == 1) ? "obstacle" : "zk");
		}
	}


	// Block Define
	fprintf(fp, "(domain intbool 0 1)\n");
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				fprintf(fp, "(int y_%d_%d_%d intbool)\n", x, y, i);
			}
		}
	}

	fprintf(fp, "(predicate (onlyonesub a0 a1 a2 a3 a4 a5 a6 a7) (and a0 (not a1) (not a2) (not a3) (not a4) (not a5) (not a6 )(not a7)))\n");
	fprintf(fp, "(predicate (onlyone b0 b1 b2 b3 b4 b5 b6 b7) (or (onlyonesub b0 b1 b2 b3 b4 b5 b6 b7) (onlyonesub b1 b2 b3 b4 b5 b6 b7 b0) (onlyonesub b2 b3 b4 b5 b6 b7 b0 b1) (onlyonesub b3 b4 b5 b6 b7 b0 b1 b2) (onlyonesub b4 b5 b6 b7 b0 b1 b2 b3) (onlyonesub b5 b6 b7 b0 b1 b2 b3 b4) (onlyonesub b6 b7 b0 b1 b2 b3 b4 b5) (onlyonesub b7 b0 b1 b2 b3 b4 b5 b6)))\n");

	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				int8_t tmp[256];
				BlockDefineOperation(&sat_stones[i << 5], tmp);

				int len = 8;
				fprintf(fp, "(iff (= y_%d_%d_%d 1) (onlyone", x, y, i);

				for (j=0; j<8; j++) {
					int idx = j << 5;
					if (tmp[idx] == 0xff) continue;

					fpos_t pos;
					fgetpos(fp, &pos);
					fprintf(fp, " (and");

					int o_x = 0, o_y = 0;
					do {
						int a_x = x + o_x;
						int a_y = y + o_y;
						if (!((y1 <= y) && (y < y2)) || !((x1 <= x) && (x < x2))) goto DAMEDESU;
						fprintf(fp, " (= x_%d_%d %d)", a_x, a_y, i);
					} while (o_x != 0 || o_y != 0);

					fprintf(fp, ")");
					len--;
					continue;

				DAMEDESU:
					fsetpos(fp, &pos);	// これ以降を0埋めするとか切り詰めるとかはしないので、すごい長い文を書いていたら困る
				}

				for (j=0; j<len; j++) fprintf(fp, " false");
				fprintf(fp, "))\n");
			}
		}
	}


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


	// Order
	/*
	fprintf(fp, "(int first_zk 0 %d)\n", n - 1);
	for (i=0; i<n; i++) {
		fprintf(fp, "(iff (= first_zk %d) (and", i);
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				fprintf(fp, " (>= x_%d_%d %n)", x, y, i);

				int dx[] = {1, 0, -1, 0};
				int dy[] = {0, 1, 0, -1};
				fprintf(fp, "(=> (= x_%d_%d %d) (or", x, y, i);
				for (k=0; k<4; k++) fprintf(fp, " (>= x_%d_%d %d)", x+dx[k], y+dy[k], n);
				fprintf(fp, "))\n");

				for (k=i+1; k<n; k++) {
					for (l=0; l<4; l++) {
						int xb = x + dx[l];
						int yb = y + dy[l];
						fprintf(fp, "(=> (and (= x_%d_%d %d) (= x_%d_%d %d)) (or", x, y, i, xb, yb, k);
						for (m=0; m<4; m++) {
							int xd = xb + dx[m];
							int yd = xb + dy[m];
							if (!(((y1 - 1) <= yd) && (yd <= y2))
								|| !(((x1 - 1) <= xd) && (xd <= x2))
								|| ((xd == x) && (yd == y))) goto YANMAR;
							fprintf(fp, " (>= x_%d_%d %d)", xd, yd, k);
							continue;
						YANMAR:
							fprintf(fp, " false");
						}
						fprintf(fp, "))\n");
					}
				}
			}
		}
		fprintf(fp, "))\n");
	}
	*/
}


/*----------------------------------------------------------------------------*/
/*                                  Run Sugar                                 */
/*----------------------------------------------------------------------------*/
int startsWith(const char *s1, const char *s2, int s1len, int s2len)
{
	if (s1len > s2len) return 0;
	
	int i;
	for (i=0; i<s1len; i++) if (s1[i] != s2[i]) return 0;
	return 1;
}

int satSolve(int *map)
{
	FILE *fp;
	char buf[128];
	int code = 1;

	snprintf(buf, 128, "sugar %s", CSP_INPUT_FILE);
	if ((fp = popen(buf, "r")) == NULL) return -1;

	int i;
	for (i=0; i<1024; i++) map[i] = -1;

	printf("COMMAND: %s\n", buf);

	while (fgets(buf, 128, fp) != NULL) {
		printf("DOUYO: %s", buf);

		char *p = strchr(buf, '\n');
		if (p != NULL) *p = '\0';

		if (buf[0] == 's') {
			int len = strlen(&buf[1]);
			if (startsWith(" SATISFIABLE", &buf[1], 12, len)) continue;
			code = startsWith(" UNSATISFIABLE", &buf[1], 14, len) ? 0 : -1;
			goto END;
		}

		if (buf[0] == 'a') {
			if (buf[1] == '\n') goto END;
			if (buf[2] != 'x') continue;

			int x, y, n;
			sscanf(&buf[2], "x_%d_%d\t%d", &x, &y, &n);

			if ((x < 0) || (x >= 32) || (y < 0) || (y >= 32)) continue;
			map[((y) << 5) + (x)] = n;
			continue;
		}

		//code = -1;
		//goto END;
	}

END:
	pclose(fp);
	return code;
}

