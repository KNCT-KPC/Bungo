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

//#define	SIZE_OF_INT_1024	(sizeof(int) << 10)
#define	SIZE_OF_INT_1024	4096

clock_t global_clock;
GHashTable *global_memo;
unsigned int global_count;

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
int bestScore2(Score *best, const int *map, int *freelen);
static inline int isInValid(int x, int y, int x1, int y1, int x2, int y2);
int isTraveled(const int *map, int id, int x1, int y1, int x2, int y2);
int neighborIdx(const int *map, int x1, int y1, int x2, int y2, int *neighbor);

void backTracking(Score *best, int id, Stone *stones, int n, int *map, int x1, int y1, int x2, int y2, int *operation, int *sumlen, int nowscore)
{
	int len;
	global_count++;
	
	if (id == n) return;
	if (bestScore2(best, map, &len)) printf("[%.3fs]\tUpdate best score: (%d, %d)\t[%u]\n", (clock() - global_clock) / (double)CLOCKS_PER_SEC, best->score, best->zk, global_count);
	if ((nowscore - sumlen[id]) > best->score) return;
	if (isTraveled(map, id, x1, y1, x2, y2)) return;
	
	if (len == 0) return;
	if (len >= stones[id].len) {
		int x, y, i, j;
		int *tmpmap = (int *)malloc(SIZE_OF_INT_1024);
		memcpy(tmpmap, map, SIZE_OF_INT_1024);
		
		int *neighbor = (int *)malloc(SIZE_OF_INT_1024);
		int first = neighborIdx(map, x1, y1, x2, y2, neighbor);
		
		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
				int bidx = (y << 5) + x;
				if (map[bidx] != -1) continue;
				
				int *p = &operation[id << 7];
				len = *p;
				for (i=0; i<len; i++) {
					int idxes[16];
					int flg = first || neighbor[bidx];
					int *pp = &p[i << 4];
					
					for (j=1; j<stones[id].len; j++) {
						int idx = bidx + pp[j];
						if (map[idx] != -1) goto DAMEDESU;
						idxes[j] = idx;
						if (neighbor[idx]) flg = 1;
					}
					if (!flg) continue;

					// Put
					memcpy(tmpmap, map, SIZE_OF_INT_1024);
					tmpmap[bidx] = id;
					for (j=1; j<stones[id].len; j++) tmpmap[idxes[j]] = id;
					backTracking(best, id+1, stones, n, tmpmap, x1, y1, x2, y2, operation, sumlen, nowscore - stones[id].len);

				DAMEDESU:
					continue;	// noop
				}
			}
		}

		free(tmpmap);
		free(neighbor);
	}
	
	backTracking(best, id+1, stones, n, map, x1, y1, x2, y2, operation, sumlen, nowscore);
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	//dump(map, x1, y1, x2, y2, original_stones, n);

	// Prepare
	int i, j, k;
	Stone stones[256];
	stoneEncode(stones, original_stones, n);
	for (i=0; i<1024; i++) map[i] = (map[i] == 0) ? -1 : -2;

	// Search
	Score best;
	best.score = 1024;
	bestScore(&best, map);

	int operation[33024];
	for (i=0; i<n; i++) {
		int8_t op[256];
		BlockDefineOperation(stones[i].list, op);
		int *base = &operation[i << 7];

		*base = 0;
		for (j=0; j<8; j++) {
			int8_t *pp = &op[j << 5];
			if (pp[0] == INT8_MAX) continue;
			int *p = &base[(*base) << 4];
			for (k=1; k<stones[i].len; k++) p[k] = (pp[(k << 1) + 1] << 5) + pp[k << 1];
			(*base)++;
		}
	}
		
	int sumlen[257];
	sumlen[n] = 0;
	for (i=(n-1); i>=0; i--) sumlen[i] = sumlen[i+1] + stones[i].len;

	global_clock = clock();
	backTracking(&best, 0, stones, n, map, x1, y1, x2, y2, operation, sumlen, best.score);

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
	initClient(CLIENT_NAME, SERVER_IPADDR, &osfhandle, &sd);

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
	int i, j, len = 0, score = 0;
	int zks[256] = {};

	for (i=0; i<1024; i++) {
		int tmp = map[i];
		if (tmp == -1) score++;
		if (tmp < 0) continue;

		for (j=0; j<len; j++) {
			if (tmp == zks[j]) goto NEXTNEXT;
		}
		zks[len++] = tmp;

	NEXTNEXT:
		continue;
	}
	
	*freelen = score;
	if (score < best->score || (score == best->score && len < best->zk)) {
		best->score = score;
		best->zk = len;
		memcpy(best->map, map, sizeof(int) << 10);
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

	if (isInValid(x, y-1, x1, y1, x2, y2) || map[((y-1) << 5) + x] == -1) len++;
	if (isInValid(x+1, y, x1, y1, x2, y2) || map[(y << 5) + (x+1)] == -1) len++;
	if (isInValid(x, y+1, x1, y1, x2, y2) || map[((y+1) << 5) + x] == -1) len++;
	if (isInValid(x-1, y, x1, y1, x2, y2) || map[(y << 5) + (x-1)] == -1) len++;

	return (len >= 2);
}

int isTraveled(const int *map, int id, int x1, int y1, int x2, int y2)
{
	char *str = (char *)malloc(sizeof(char) * 128);
	char *p = &str[7], buf[8];
	int x, y, len = 0;

	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			int tmp = map[(y << 5) + x];
			if (tmp == -1) len++;
			if (tmp < 0 || !isVertex(map, x, y, x1, y1, x2, y2)) continue;
			*(p++) = x + 32;
			*(p++) = y + 32;
		}
	}

	*p = '\0';
	sprintf(buf, "%04d%03d", len, id);
	memcpy(str, buf, sizeof(char) * 7);

	if (g_hash_table_contains(global_memo, str)) {
		free(str);
		return 1;
	}

	int *val = g_new(int8_t, 1);
	*val = 1;
	g_hash_table_insert(global_memo, str, val);
	return 0;
}

int neighborIdx(const int *map, int x1, int y1, int x2, int y2, int *neighbor)
{
	int x, y, i;
	int dx[] = {0, 1, 0, -1};
	int dy[] = {-1, 0, 1, 0};
	int first = 1;
	
	memset(neighbor, 0, SIZE_OF_INT_1024);
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			if (map[(y << 5) + x] < 0) continue;

			for (i=0; i<4; i++) {
				int xx = x + dx[i];
				int yy = y + dy[i];
				if (isInValid(xx, yy, x1, y1, x2, y2) || map[(yy << 5) + xx] != -1) continue;
				neighbor[(yy << 5) + xx] = 1;
			}
			
			first = 0;
		}
	}
	
	return first;
}
