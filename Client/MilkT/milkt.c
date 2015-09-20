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
#define	CSP_OUTPUT_FILE	"/tmp/milkt.out"


/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void dump(int *map, int x1, int y1, int x2, int y2, int *stones, int n);
void stone2satstone(int8_t *sat_stones, const int *map_base, int x1, int y1, int x2, int y2, const int *stones_base, int n);



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
	int k;
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

	return len;
}




void createCSPfile(FILE *fp, int *map, int x1, int y1, int x2, int y2, int8_t *sat_stones, int n)
{
	int x,y, i, j, k, l;

	// Mass Define
	fprintf(fp, "(domain zk 0 %d)\n", n - 1);
	fprintf(fp, "(domain obstacle %d)\n", n);
	
	for(y=(y1-1); y<=y2; y++) {
		for(x=(x1-1); x<=x2; x++) {
			fprintf(fp, "(int x_%d_%d %s)\n", x, y, (!(y1 <= y < y2) || !(x1 <= x < x2) || MAP(x, y) == 1) ? "obstacle" : "zk");
		}
	}


	// Block Define
	fprintf(fp, "(domain intbool 0 1)\n");
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				fprintf(fp, "(int y_%d_%d_%d)\n", x, y, i);
			}
		}
	}

	fprintf(fp, "(predicate (onlyonesub a0 a1 a2 a3 a4 a5 a6 a7) (and a0 (not a1) (not a2) (not a3) (not a4) (not a5) (not a6 )(not a7)))\n");
	fprintf(fp, "(predicate (onlyone b0 b1 b2 b3 b4 b5 b6 b7) (or (onlyonesub b0 b1 b2 b3 b4 b5 b6 b7) (onlyonesub b1 b2 b3 b4 b5 b6 b7 b0) (onlyonesub b2 b3 b4 b5 b6 b7 b0 b1) (onlyonesub b3 b4 b5 b6 b7 b0 b1 b2) (onlyonesub b4 b5 b6 b7 b0 b1 b2 b3) (onlyonesub b5 b6 b7 b0 b1 b2 b3 b4) (onlyonesub b6 b7 b0 b1 b2 b3 b4 b5) (onlyonesub b7 b0 b1 b2 b3 b4 b5 b6)))\n");

	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<n; i++) {
				int8_t tmp[256];
				BlockDefineOpration(&sat_stones[i << 5], tmp);	// Vivid Red Operation

				int len = 8;
				fprintf(fp, "(iff (= y_%d_%d_%d 1) (onlyone", x, y, i);

				for (j=0; j<8; j++) {
					int idx = j << 5;
					if (tmp[idx] == 0xff) continue;

					fpos_t pos = fgetpos(fp, &pos);
					fprintf(fp, " (and");

					int o_x = 0, o_y = 0;
					do {
						int a_x = x + o_x;
						int a_y = y + o_y;
						if (!(y1 <= y < y2) || !(x1 <= x < x2)) goto DAMEDESU;
						fprintf(fp, " (= x_%d_%d %d)", a_x, a_y, i);
					} while (o_x != 0 || o_y != 0);

					fprintf(fp, ")");
					len--;
					continue;

				DAMEDESU:
					fsetpos(fp, &pos);	// これ以降を0埋めするとかはないので、ある程度長い文を書いていたら困る
				}

				for (j=0; j<len; j++) fprintf(" false");
				fprintf(fp, "))\n");
			}
		}
	}


	// Only one railgun
	for (i=0; i<n; i++) {
		fprintf(fp, "(<= (+");
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				printf(fp, " y_%d_%d_%d", x, y, i);
			}
		}
		fprintf(fp, ") 1)\n");
	}


	// Order
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
				fprintf(fp, "))");

				for (k=i+1; k<n; k++) {
					for (l=0; l<4; l++) {
						fprintf(fp, "(=> (and (= x_%d_%d %d) (= x_%d_%d %d))", x, y, i, x+dx[l], y+dx[l], k);
						int xd, yd;
						for (yd=y1; yd<y2; yd++){
							for (xd=x1; xd<x2; xd++) {
								fprintf(fp, " (or (>= %d x_%d_%d) (>= x_%d_%d %d))", i, xd, yd, xd, yd, k);
							}
						}
					}
				}

			}
		}
	}


}










int solver(FILE *fp, int8_t *sat_stones, int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	x2++; y2++;
	
	// Prepare
	stone2satstone(sat_stones, map, x1, y1, x2, y2, stones, n);

	// Z to CSP


	/*
	int col = x2 - x1;
	int row = y2 - y1;
	createCSPfile(fp
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
	int8_t sat_stones[12551] = {};	// ((16 * 256) * 2) + (((1023 + (33 * 4)) * 2) + 1) + (1024 * 2)
	
	FILE *fp = fopen(CSP_INPUT_FILE, "w");
	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
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


