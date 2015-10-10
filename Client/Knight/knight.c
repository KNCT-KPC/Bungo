/******************************************************************************/
/*                                                                            */
/*                                   Knight                                   */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../SATubatu/common.h"
#include "../KanaC/kcommon.h"
#include <glib-2.0/glib.h>

#define	CLIENT_NAME	"Knight"
#define	SERVER_IPADDR	"127.0.0.1"
// 本番で時間計測は不要

#define	SIZE_OF_INT_1024	4096
#define	SIZE_OF_CHAR_128	128
#define	SIZE_OF_UINT8_1024	1024

clock_t global_clock;
GHashTable *global_memo;
unsigned int global_count;

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
int bestScore2(Score *best, const int *map, int *freelen);
int bestScore3(Score *best, const int *map);
static inline int isInValid(int x, int y, int x1, int y1, int x2, int y2);
int isTraveled(const int *map, int id, int x1, int y1, int x2, int y2, int len);
void putStone(int *map, const int *xy, int len, int id, int *org, int x1, int y1, int x2, int y2);
void restoreStone(int *map, const int *xy, int len, int id, const int *org, int x1, int y1, int x2, int y2);

void backTracking(Score *best, int id, Stone *stones, int n, int *map, int x1, int y1, int x2, int y2, int *operation, int *sumlen, int nowscore, int *original_stones, int first_space)
{
	int len;
	global_count++;

	if (id == n) return;
	if (bestScore2(best, map, &len)) {
		printf("[%.3fs]\tUpdate best score: (%d, %d)\t[%u]\n", (clock() - global_clock) / (double)CLOCKS_PER_SEC, best->score, best->zk, global_count);

		dumpMap2(map);
		sendMsg("S");
		sendAnswer(best->map, stones, original_stones, n);
		if (sendMsg("E") == EXIT_FAILURE) return;
	}
	if ((nowscore - sumlen[id]) > best->score) return;
	//if (isTraveled(map, id, x1, y1, x2, y2, len)) return;

	if (len == 0) return;
	if (len >= stones[id].len) {
		int x, y, i, j;
		int first = (len == first_space);

		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				int bidx = (y << 5) + x;
				if (map[bidx] == -2 || map[bidx] >= 0) continue;

				int *p = &operation[id << 8];
				len = *p;

				for (i=0; i<len; i++) {
					int xy[32];
					int *pp = &p[i << 5];
					int flg = first || (map[bidx] < -2);

					for (j=1; j<stones[id].len; j++) {
						int xx = x + pp[j << 1];
						int yy = y + pp[(j << 1) + 1];
						if (isInValid(xx, yy, x1, y1, x2, y2)) goto DAMEDESU;

						int idx = (yy << 5) + xx;
						if (map[idx] == -2 || map[idx] >= 0) goto DAMEDESU;

						xy[j << 1] = xx;
						xy[(j << 1) + 1] = yy;
						flg |= (map[idx] < -2);
					}
					if (!flg) continue;

					// Put
					xy[0] = x;
					xy[1] = y;
					int org[16];

					putStone(map, xy, stones[id].len, id, org, x1, y1, x2, y2);

					// Recursive
					backTracking(best, id+1, stones, n, map, x1, y1, x2, y2, operation, sumlen, nowscore - stones[id].len, original_stones, first_space);
					// Restore
					restoreStone(map, xy, stones[id].len, id, org, x1, y1, x2, y2);
				DAMEDESU:
					continue;	// noop
				}
			}
		}
	}

	backTracking(best, id+1, stones, n, map, x1, y1, x2, y2, operation, sumlen, nowscore, original_stones, first_space);
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	//dump(map, x1, y1, x2, y2, original_stones, n);

	// Prepare
	int i, j, k;
	Stone stones[256];
	stoneEncode(stones, original_stones, n);

	#pragma omp parallel for
	for (i=0; i<1024; i++) map[i] = (map[i] == 0) ? -1 : -2;

	// Search
	Score best;
	best.score = 1024;
	bestScore(&best, map);

	int operation[65536];

	#pragma omp parallel for
	for (i=0; i<n; i++) {
		int8_t op[256];
		BlockDefineOperation(stones[i].list, op);

		int *base = &operation[i << 8];
		*base = 0;

		for (j=0; j<8; j++) {
			int8_t *pp = &op[j << 5];
			if (pp[0] == INT8_MAX) continue;
			int *p = &base[(*base) << 5];

			for (k=1; k<stones[i].len; k++) {
				p[k << 1] = pp[k << 1];
				p[(k << 1) + 1] = pp[(k << 1) + 1];
			}

			(*base)++;
		}
	}

	int sumlen[257];
	sumlen[n] = 0;
	for (i=(n-1); i>=0; i--) sumlen[i] = sumlen[i+1] + stones[i].len;

	global_clock = clock();
	backTracking(&best, 0, stones, n, map, x1, y1, x2, y2, operation, sumlen, best.score, original_stones, best.score);

	// Best
	printf("Best Score: %d, Zk: %d\n", best.score, best.zk);
	dumpMap2(best.map);

	// Send
	sendMsg("S");
	sendAnswer(best.map, stones, original_stones, n);
	if (sendMsg("E") == EXIT_FAILURE) return EXIT_SUCCESS;
	return EXIT_FAILURE;
}


/*----------------------------------------------------------------------------*/
/*                                    Main                                    */
/*----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	int osfhandle, sd;
	initClient(CLIENT_NAME, (argc == 2) ? argv[1] : SERVER_IPADDR, &osfhandle, &sd);

	global_memo = g_hash_table_new(g_str_hash, g_str_equal);	// 継続するならこの辺も考えないと
	global_count = 0;

	int x1, y1, x2, y2, n;
	int map[1024];
	int stones[16384];

	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		if (solver(map, x1, y1, x2+1, y2+1, stones, n) == EXIT_FAILURE) break;
	}

	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*                                     Sub                                    */
/*----------------------------------------------------------------------------*/
int bestScore2(Score *best, const int *map, int *freelen)
{
	int i, len = 0, score = 0;
	uint8_t zks[256] = {};

	//#pragma omp parallel for
	for (i=0; i<1024; i++) {
		int tmp = map[i];
		if (tmp == -1 || tmp < -2) score++;
		if (tmp < 0) continue;

		if (zks[tmp] == 1) continue;
		zks[tmp] = 1;
		len++;
	}

	*freelen = score;
	if (score < best->score || (score == best->score && len < best->zk)) {
		best->score = score;
		best->zk = len;
		memcpy(best->map, map, SIZE_OF_INT_1024);
		return 1;
	}

	return 0;
}

static inline int isInValid(int x, int y, int x1, int y1, int x2, int y2)
{
	return (x1 > x) || (x >= x2) || (y1 > y) || (y >= y2);
}

static inline int isVertex(const int *map, int x, int y, int x1, int y1, int x2, int y2)
{
	int len = 0;

	if (isInValid(x, y-1, x1, y1, x2, y2) || (map[((y-1) << 5) + x] == -1) || (map[((y-1) << 5) + x] < -2)) len++;
	if (isInValid(x+1, y, x1, y1, x2, y2) || (map[(y << 5) + (x+1)] == -1) || (map[(y << 5) + (x+1)] < -2)) len++;
	if (isInValid(x, y+1, x1, y1, x2, y2) || (map[((y+1) << 5) + x] == -1) || (map[((y+1) << 5) + x] < -2)) len++;
	if (isInValid(x-1, y, x1, y1, x2, y2) || (map[(y << 5) + (x-1)] == -1) || (map[((y+1) << 5) + x] < -2)) len++;

	return (len >= 2);
}

int isTraveled(const int *map, int id, int x1, int y1, int x2, int y2, int len)
{
	char *str = (char *)malloc(SIZE_OF_CHAR_128);
	char *p = &str[7];
	int x, y;

	sprintf(str, "%04d%03d", len, id);
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			int tmp = map[(y << 5) + x];
			if (tmp < 0 || !isVertex(map, x, y, x1, y1, x2, y2)) continue;
			*(p++) = x + 32;
			*(p++) = y + 32;
		}
	}
	*p = '\0';

	if (g_hash_table_contains(global_memo, str)) {
		free(str);
		return 1;
	}

	int *val = g_new(int8_t, 1);
	*val = 1;
	g_hash_table_insert(global_memo, str, val);
	return 0;
}

void putStone(int *map, const int *xy, int len, int id, int *org, int x1, int y1, int x2, int y2)
{
	int i, j, idx, x, y;
	int dx[] = {1, 0, -1, 0};
	int dy[] = {0, 1, 0, -1};

	// Put stone
	for (i=0; i<len; i++) {
		x = xy[i << 1];
		y = xy[(i << 1) + 1];

		idx = (y << 5) + x;
		org[i] = map[idx];
	}

	// Neighbor
	for (i=0; i<len; i++) {
		x = xy[i << 1];
		y = xy[(i << 1) + 1];

		idx = (y << 5) + x;
		map[idx] = id;

		for (j=0; j<4; j++) {
			int xx = x + dx[j];
			int yy = y + dy[j];

			if (isInValid(xx, yy, x1, y1, x2, y2)) continue;
			idx = (yy << 5) + xx;

			if (map[idx] == -2 || map[idx] >= 0) continue;
			map[idx] -= (2 + id);
		}
	}
}

void restoreStone(int *map, const int *xy, int len, int id, const int *org, int x1, int y1, int x2, int y2)
{
	int i, j;
	int dx[] = {1, 0, -1, 0};
	int dy[] = {0, 1, 0, -1};

	for (i=0; i<len; i++) {
		int x = xy[i << 1];
		int y = xy[(i << 1) + 1];

		for (j=0; j<4; j++) {
			int xx = x + dx[j];
			int yy = y + dy[j];

			if (isInValid(xx, yy, x1, y1, x2, y2)) continue;
			int idx = (yy << 5) + xx;

			if (map[idx] >= -2) continue;
			map[idx] += (2 + id);
		}
	}

	for (i=0; i<len; i++) {
		int x = xy[i << 1];
		int y = xy[(i << 1) + 1];

		map[(y << 5) + x] = org[i];
	}
}

