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
#include <sys/time.h>

#define	CLIENT_NAME	"UreC"
#define	SERVER_IPADDR	"192.168.1.117"

struct timeval tv;




/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        mt[mti] &= 0xffffffffUL;
    }
}
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}


void putStone(const Stone *stones, int first, int n, int *map, int x1, int y1, int x2, int y2)
{
	int i, x, y, j, k;
	int tmpmap[1024];
	memcpy(tmpmap, map, sizeof(int) * 1024);

	int ids[256];
	for (i=0; i<n; i++) ids[i] = (first + i) % n;

	int xlist[32], ylist[32];
	for (i=0; i<32; i++) {
		xlist[i] = i;
		ylist[i] = i;
	}

	for (i=0; i<32; i++) {
		int r = genrand_int32() % 32;
		if (r < 0 || r == i) continue;
		int t = xlist[i];
		xlist[i] = xlist[r];
		xlist[r] = t;
	}

	for (i=0; i<32; i++) {
		int r = genrand_int32() % 32;
		if (r < 0 || r == i) continue;
		int t = ylist[i];
		ylist[i] = ylist[r];
		ylist[r] = t;
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

	gettimeofday(&tv, NULL);
	init_genrand(tv.tv_usec);
	printf("INITED: %u\n", tv.tv_usec);

	while (ready(map, &x1, &y1, &x2, &y2, stones, &n)) {
		if (solver(map, x1, y1, x2+1, y2+1, stones, n) == EXIT_FAILURE) break;
	}

	finalClient(osfhandle, sd);
	return EXIT_SUCCESS;
}

