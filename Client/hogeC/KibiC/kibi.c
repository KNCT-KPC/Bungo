/******************************************************************************/
/*                                                                            */
/*                                    KanaC                                   */
/*                                 -- kanaC --                                */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../SATubatu/common.h"
#include "../KanaC/kcommon.h"

#define	CLIENT_NAME	"KibiC"
#define	SERVER_IPADDR	"192.168.1.117"

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
void putStone(const Stone *stones, int first, int n, int *map, int x1, int y1, int x2, int y2)
{
	int i, x, y, j, k;
	int tmpmap[1024];
	memcpy(tmpmap, map, sizeof(int) * 1024);

	int ids[256];
	for (i=0; i<n; i++) ids[i] = (first + i) % n;

	int xlist[32], ylist[32];
	for (i=0; i<32; i++) {
		xlist[i] = (32 - ((i + ((x1 + x2) / 2)) % 32)) % 32;
		ylist[i] = (32 + ((i + ((y1 + y2) / 2)) % 32)) % 32;
	}

	int ii, jj;
	for (i=0; i<n; i++) {
		int id = ids[i];
		int8_t operation[256];
		BlockDefineOperation(stones[id].list, operation);

		for (ii=0; ii<32; ii++) {
			for (jj=0; jj<32; jj++) {
				y = ylist[ii];
				x = xlist[jj];
				if (map[(y << 5) + x] != -1) continue;

				for (j=0; j<8; j++) {
					int8_t *p = &operation[j << 5];
					if (p[0] == INT8_MAX) continue;

					for (k=0; k<stones[id].len; k++) {
						int xx = x + p[k << 1];
						int yy = y + p[(k << 1) + 1];

						if (!((x1 <= xx) && (xx < x2)) || !((y1 <= yy) && (yy < y2))) goto DAMEDESU;

						int idx = (yy << 5) + xx;
						if (tmpmap[idx] != -1) goto DAMEDESU;
						tmpmap[idx] = stones[id].id;
					}

					if (isAccept(tmpmap, x1, y1, x2, y2)) goto NEXT;

				DAMEDESU:
					memcpy(tmpmap, map, sizeof(int) * 1024);
					continue;
				}
			}
		}

	NEXT:
		memcpy(map, tmpmap, sizeof(int) * 1024);
		continue;	// noop
	}
}

void putWrapper(Score *best, Stone *stones, int n, const int *map, int x1, int y1, int x2, int y2, int *original_stones)
{
	// Score
	int i;
	Stone sorted[256];
	static int counter = 0;

	switch(counter++) {
	case 0:
		for (i=0; i<n; i++) stones[i].score = ((double)(n - i) / (double)n);
		break;
	case 1:
		for (i=0; i<n; i++) stones[i].score = (((double)(n - i) / (double)n) * (double)stones[i].len) * 0.0625;
		break;
	case 2:
		for (i=0; i<n; i++) stones[i].score = ((double)(n - i) / (double)n) * -1;
		break;
	case 3:
		for (i=0; i<n; i++) stones[i].score = (((double)(n - i) / (double)n) * (double)stones[i].len) * -0.0625;
		break;
	default:
		srand((unsigned)time(NULL));
		for (i=0; i<n; i++) stones[i].score = rand();
	}

	memcpy(sorted, stones, sizeof(Stone) * 256);
	qsort(sorted, n, sizeof(Stone), sortByScore);

	// Put
	int tmpmap[1024];
	for (i=0; i<n; i++) {
		memcpy(tmpmap, map, sizeof(int) * 1024);
		putStone(sorted, sorted[i].id, n, tmpmap, x1, y1, x2, y2);
		if (bestScore(best, tmpmap)) {
			printf("Update best score: (%d, %d)\n", best->score, best->zk);
			sendMsg("S");
			sendAnswer(best->map, stones, original_stones, n);
			if (sendMsg("E") == EXIT_FAILURE) return;
		}
	}
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	int i;
	dump(map, x1, y1, x2, y2, original_stones, n);

	// Prepare
	Stone stones[256];
	stoneEncode(stones, original_stones, n);
	for (i=0; i<1024; i++) {
		int tmp = map[i];
		map[i] = (tmp == 0) ? -1 : -2;
	}

	// Search
	Score best;
	best.score = 1024;
	for (i=0; i<10; i++) {
		putWrapper(&best, stones, n, map, x1, y1, x2, y2, original_stones);
	}

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

	int x1, y1, x2, y2, n;
	int map[1024];
	int stones[16384];

	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		if (solver(map, x1, y1, x2+1, y2+1, stones, n) == EXIT_FAILURE) break;
	}

	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}

