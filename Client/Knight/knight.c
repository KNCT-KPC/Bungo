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

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
int isAcceptSimple(const int *map, int x1, int y1, int x2, int y2);
int isTraveled(const int *map, int id, int x1, int y1, int x2, int y2);

void backTracking(Score *best, int id, Stone *stones, int n, int *map, int x1, int y1, int x2, int y2, int8_t *operation, int *sumlen, int nowscore)
{
	if (id == n) return;
	if (bestScore(best, map)) printf("[%.3fs]\tUpdate best score: (%d, %d)\n", (clock() - global_clock) / (double)CLOCKS_PER_SEC, best->score, best->zk);
	if ((nowscore - sumlen[id]) > best->score) return;
	//if (isTraveled(map, id, x1, y1, x2, y2)) return;

	int x, y, i, j;
	int *tmpmap = (int *)malloc(SIZE_OF_INT_1024);

	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			int bidx = (y << 5) + x;
			if (map[bidx] != -1) continue;

			for (i=0; i<8; i++) {
				// Check
				int8_t *p = &operation[(id << 8) + (i << 5)];
				if (p[0] == INT8_MAX) continue;

				int idxes[16];
				for (j=1; j<stones[id].len; j++) {
					int xx = x + p[j << 1];
					int yy = y + p[(j << 1) + 1];
					if (!((x1 <= xx) && (xx < x2)) || !((y1 <= yy) && (yy < y2))) goto DAMEDESU;

					int idx = (yy << 5) + xx;
					if (map[idx] != -1) goto DAMEDESU;
					idxes[j] = idx;
				}

				// Put
				memcpy(tmpmap, map, SIZE_OF_INT_1024);
				tmpmap[bidx] = id;
				for (j=1; j<stones[id].len; j++) tmpmap[idxes[j]] = id;

				if (isAcceptSimple(tmpmap, x1, y1, x2, y2)) backTracking(best, id+1, stones, n, tmpmap, x1, y1, x2, y2, operation, sumlen, nowscore - stones[id].len);

			DAMEDESU:
				continue;	// noop
			}
		}
	}

	free(tmpmap);
	backTracking(best, id+1, stones, n, map, x1, y1, x2, y2, operation, sumlen, nowscore);
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	//dump(map, x1, y1, x2, y2, original_stones, n);

	// Prepare
	int i, j;
	Stone stones[256];
	stoneEncode(stones, original_stones, n);
	for (i=0; i<1024; i++) {
		int tmp = map[i];
		map[i] = (tmp == 0) ? -1 : -2;
	}

	// Search
	Score best;
	best.score = 1024;
	bestScore(&best, map);

	int8_t operation[65536];
	for (i=0; i<n; i++) BlockDefineOperation(stones[i].list, &operation[i << 8]);

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

	global_memo = g_hash_table_new(g_str_hash, g_str_equal);


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
void dfsSimple(int *map, int x, int y, int x1, int y1, int x2, int y2)
{
	if (!(((x1 <= x) && (x < x2)) && ((y1 <= y) && (y < y2)))) return;

	int idx = (y << 5) + x;
	if (map[idx] < 0) return;

	map[idx] = -1;
	dfsSimple(map, x, y - 1, x1, y1, x2, y2);
	dfsSimple(map, x - 1, y, x1, y1, x2, y2);
	dfsSimple(map, x + 1, y, x1, y1, x2, y2);
	dfsSimple(map, x, y + 1, x1, y1, x2, y2);
}

int isAcceptSimple(const int *map, int x1, int y1, int x2, int y2)
{
	int i, x=0, y=0;
	int tmpmap[1024];

	memcpy(tmpmap, map, SIZE_OF_INT_1024);
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			if (tmpmap[(y << 5) + x] < 0) continue;

			dfsSimple(tmpmap, x, y, x1, y1, x2, y2);
			for (i=0; i<1024; i++) if (tmpmap[i] >= 0) return 0;
			return 1;
		}
	}

	return 1;
}

int isVertex(const int *map, int x, int y, int x1, int y1, int x2, int y2)
{
	int i, len = 0;
	int dx[] = {0, 1, 0, -1};
	int dy[] = {-1, 0, 1, 0};

	for (i=0; i<4; i++) {
		int xx = x + dx[i];
		int yy = y + dy[i];

		if (!((x1 <= xx) && (xx < x2)) || !((y1 <= yy) && (yy < y2)) || map[(yy << 5) + xx] == -1) len++;
	}

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

	int *val = g_new(int, 1);
	*val = 1;
	g_hash_table_insert(global_memo, str, val);
	return 0;
}

