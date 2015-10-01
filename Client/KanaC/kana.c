/******************************************************************************/
/*                                                                            */
/*                                    Fuck                                    */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include "../SATubatu/common.h"
#include <time.h>

#define	CLIENT_NAME	"KanaC"
#define	SERVER_IPADDR	"127.0.0.1"

typedef struct {
	int id;
	int8_t list[32];
	int len;
	double score;
} Stone;

typedef struct {
	int score;
	int zk;
	int map[1024];
} Score;

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
int sortByScore(const void *n, const void *m);
void stoneEncode(Stone *dst, const int *stones_base, int n);
void BlockDefineOperation(const int8_t *zk, int8_t *dst);

int isAccept(const int *map, int x1, int y1, int x2, int y2);
void bestScore(Score *best, const int *map);
void s2s4z(const int *map, int8_t *dst);
void mawareSetsugetsuka(int front, int angle, const int *stone, int *x, int *y);

void putStone(const Stone *stones, int first, int n, int *map, int x1, int y1, int x2, int y2)
{
	int i, x, y, j, k;
	int tmpmap[1024];
	memcpy(tmpmap, map, sizeof(int) * 1024);

	int ids[256];
	for (i=0; i<n; i++) ids[i] = (first + i) % n;

	int id = stones[0].id;
	for (i=0; i<n; i++) {
		int id = ids[i];
		int8_t operation[256];
		BlockDefineOperation(stones[id].list, operation);

		for (y=y1; y<y2; y++) {
			for (x=x1; x<x2; x++) {
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

void sendAnswer(const int *map, const Stone *stones, const int *original_stone, int n)
{
	int i, j, k;

	for (i=0; i<n; i++) {
		int len = 0;
		int zkmap[1024] = {};

		for (j=0; j<1024; j++) {
			if (map[j] != i) continue;
			zkmap[j] = 1;
			len++;
		}

		if (len == 0) {
			sendMsg("");
			continue;
		}

		int8_t tmp[32];
		int8_t operation[256];
		BlockDefineOperation(stones[i].list, operation);
		s2s4z(zkmap, tmp);

		int idx = 0;
		for (j=0; j<8; j++) {
			int8_t *p = &operation[j << 5];
			if (p[0] == INT8_MAX) continue;
			for (k=0; k<32; k++) if (p[k] != tmp[k]) goto NONO;
			idx = j;
			break;
		NONO:
			continue;
		}

		int x, y, first_x, first_y;
		char front = (idx % 2 == 0) ? 'F' : 'B';
		int angle = idx / 2;

		mawareSetsugetsuka((front == 'F') ? 1 : 0, angle, &original_stone[i << 6], &first_x, &first_y);
		for (j=0; j<32; j++) {
			for (k=0; k<32; k++) {
				if (map[(j << 5) + k] != i) continue;
				x = k;
				y = j;
				goto MATAGOTOKAYO;
			}
		}

	MATAGOTOKAYO:
		sendPutMsg(front, angle * 90, x - first_x, y - first_y);
	}
}

void putWrapper(Score *best, Stone *stones, int n, const int *map, int x1, int y1, int x2, int y2)
{
	// Score
	int i;
	Stone sorted[256];
	static int counter = 0;

	if (counter == 0) {
		for (i=0; i<n; i++) stones[i].score = (((double)(n - i) / (double)n) * (double)stones[i].len) * 0.0625;
	} else if (counter == 2) {
		for (i=0; i<n; i++) stones[i].score = ((double)(n - i) / (double)n);
	} else if (counter == 1 || counter == 3) {
		for (i=0; i<n; i++) stones[i].score *= -1;
	} else {
		srand((unsigned)time(NULL));
		for (i=0; i<n; i++) stones[i].score = rand();
	}

	counter++;
	memcpy(sorted, stones, sizeof(Stone) * 256);
	qsort(sorted, n, sizeof(Stone), sortByScore);

	// Put
	int tmpmap[1024];
	for (i=0; i<n; i++) {
		memcpy(tmpmap, map, sizeof(int) * 1024);
		putStone(sorted, sorted[i].id, n, tmpmap, x1, y1, x2, y2);
		bestScore(best, tmpmap);
	}
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	int i, j;
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
		putWrapper(&best, stones, n, map, x1, y1, x2, y2);
	}

	// Best
	printf("Score: %d, Zk: %d\n", best.score, best.zk);
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
		if (solver(map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE) break;
	}

	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*                                   Prepare                                  */
/*----------------------------------------------------------------------------*/
int sortByScore(const void *n, const void *m)
{
	double tmp = (*(Stone *)m).score - (*(Stone *)n).score;
	return (tmp < 0) ? -1 : (tmp > 0) ? 1 : 0;
}

int s2s(const int *stone, int8_t *dst)
{
	int idx = 0, len = 0;
	int anc_x = -1, anc_y;

	int x, y;
	for (y=0; y<8; y++) {
		for (x=0; x<8; x++) {
			if (stone[(y << 3) + x] == 0) continue;
			if (anc_x < 0) {
				anc_x = x;
				anc_y = y;
			}

			dst[idx++] = x - anc_x;
			dst[idx++] = y - anc_y;
			if (++len >= 16) return len;
		}
	}

	for (x=idx; x<32; x++) dst[x] = INT8_MAX;
	return len;
}

// s2s for Map
void s2s4z(const int *map, int8_t *dst)
{
	int idx = 0, len = 0;
	int anc_x = -1, anc_y;

	int x, y;
	for (y=0; y<32; y++) {
		for (x=0; x<32; x++) {
			if (map[(y << 5) + x] == 0) continue;
			if (anc_x < 0) {
				anc_x = x;
				anc_y = y;
			}

			dst[idx++] = x - anc_x;
			dst[idx++] = y - anc_y;
			if (++len >= 16) return;
		}
	}

	for (x=idx; x<32; x++) dst[x] = INT8_MAX;
}

void stoneEncode(Stone *dst, const int *stones_base, int n)
{
	int i;
	for (i=0; i<n; i++) {
		dst[i].id = i;
		dst[i].len = s2s(&stones_base[i << 6], dst[i].list);
	}
}


/*----------------------------------------------------------------------------*/
/*                                     Put                                    */
/*----------------------------------------------------------------------------*/
void BlockNormalize(int8_t *zk)
{
	int i, len = 0;

	int offset[] = {0, 0};
	for (i=0; i<32; i+=2) {
		int x = zk[i];
		if (x == INT8_MAX || x == -INT8_MAX) break;

		int y = zk[i+1];
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

			for (k=0; k<32; k++) {
				if (dst[m_idx + k] != dst[s_idx + k]) goto NODUP;
			}

			dst[m_idx] = INT8_MAX;
			break;
		NODUP:
			continue;
		}
	}
}

void dfs(int *map, int x, int y, int x1, int y1, int x2, int y2, int id)
{
	if (!(((x1 <= x) && (x < x2)) && ((y1 <= y) && (y < y2)))) return;

	int idx = (y << 5) + x;
	if (map[idx] < 0 || map[idx] < id) return;

	id = map[idx];
	map[idx] = -1;
	dfs(map, x, y - 1, x1, y1, x2, y2, id);
	dfs(map, x - 1, y, x1, y1, x2, y2, id);
	dfs(map, x + 1, y, x1, y1, x2, y2, id);
	dfs(map, x, y + 1, x1, y1, x2, y2, id);
}

int isAccept(const int *map, int x1, int y1, int x2, int y2)
{
	int x, y, i, first = 256;
	int tmpmap[1024];

	for (i=0; i<1024; i++) {
		if (map[i] < 0) continue;
		first = MIN(first, map[i]);
	}

	memcpy(tmpmap, map, sizeof(int) * 1024);
	for (y=y1; y<y2; y++) {
		for (x=x1; x<x2; x++) {
			if (tmpmap[(y << 5) + x] != first) continue;
			dfs(tmpmap, x, y, x1, y1, x2, y2, first);

			for (i=0; i<1024; i++) if (tmpmap[i] >= 0) return 0;
			return 1;
		}
	}
}

void bestScore(Score *best, const int *map)
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

	if (score < best->score || (score == best->score && len < best->zk)) {
		best->score = score;
		best->zk = len;
		memcpy(best->map, map, sizeof(int) * 1024);
		return;
	}
}

void mawareSetsugetsuka(int front, int angle, const int *stone, int *x, int *y)
{
	int i, j, k, sidx, didx;
	int dst[64], dst2[64];

	// Front or Back
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			sidx = (i << 3) + j;
			didx = (i << 3) + (front ? j : (7 - j));
			dst[didx] = stone[sidx];
		}
	}

	// Rotate
	for (i=0; i<angle; i++) {
		int tmp[64];
		memcpy(tmp, dst, sizeof(int) * 64);

		for (j=0; j<8; j++) {
			for (k=0; k<8; k++) {
				sidx = (j << 3) + k;
				didx = (k << 3) + (7 - j);
				tmp[didx] = dst[sidx];
			}
		}

		memcpy(dst, tmp, sizeof(int) * 64);
	}

	// Search
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if (dst[(i << 3) + j] != 1) continue;
			*x = j;
			*y = i;
			return;
		}
	}
}
