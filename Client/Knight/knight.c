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

#define	CLIENT_NAME	"Knight"
#define	SERVER_IPADDR	"127.0.0.1"

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void backTracking(Score *best, int id, Stone *stones, int n, int *map, int x1, int y1, int x2, int y2)
{
	if (bestScore(best, map)) printf("Update best score: (%d, %d)\n", best->score, best->zk);
	if (id == n) return;

	int x, y, i, j;
	int8_t *operation = (int8_t *)malloc(sizeof(int8_t) * 256);
	int *tmpmap = (int *)malloc(sizeof(int) * 1024);

	BlockDefineOperation(stones[id].list, operation);
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			for (i=0; i<8; i++) {
				memcpy(tmpmap, map, sizeof(int) * 1024);
				int8_t *p = &operation[i << 5];
				if (p[0] == INT8_MAX) continue;

				for (j=0; j<stones[id].len; j++) {
					int xx = x + p[j << 1];
					int yy = y + p[(j << 1) + 1];
					if (!((x1 <= xx) && (xx < x2)) || !((y1 <= yy) && (yy < y2))) goto DAMEDESU;

					int idx = (yy << 5) + xx;
					if (tmpmap[idx] != -1) goto DAMEDESU;
					tmpmap[idx] = id;
				}

				if (!isAccept(tmpmap, x1, y1, x2, y2)) goto DAMEDESU;
				backTracking(best, id+1, stones, n, tmpmap, x1, y1, x2, y2);
			DAMEDESU:
				continue;	// noop
			}
		}
	}

	backTracking(best, id+1, stones, n, map, x1, y1, x2, y2);
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	//dump(map, x1, y1, x2, y2, original_stones, n);

	// Prepare
	int i;
	Stone stones[256];
	stoneEncode(stones, original_stones, n);
	for (i=0; i<1024; i++) {
		int tmp = map[i];
		map[i] = (tmp == 0) ? -1 : -2;
	}

	// Search
	Score best;
	best.score = 1024;
	backTracking(&best, 0, stones, n, map, x1, y1, x2, y2);

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

	int x1, y1, x2, y2, n;
	int map[1024];
	int stones[16384];

	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		if (solver(map, x1, y1, x2+1, y2+1, stones, n) == EXIT_FAILURE) break;
	}

	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}

