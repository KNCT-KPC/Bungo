/******************************************************************************/
/*                                                                            */
/*                                    Cross                                   */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../SATubatu/common.h"
#include "../KanaC/kcommon.h"

#define	CLIENT_NAME	"Cross"
#define	SERVER_IPADDR	"127.0.0.1"

clock_t global_clock;
unsigned int global_count;

#define	ROW_MAX	2097152
#define	COL_MAX	1024
#define	FRAGMENT_SIZE	100
//#define	FRAGMENT	(((ROW_MAX + 1) * (COL_MAX + 1)) / FRAGMENT_SIZE)
#define	FRAGMENT	(((ROW_MAX + 1) / 100) * (COL_MAX + 1))


typedef struct __dancing_links_node {
	uint8_t id;
	unsigned int row, col, idx;
	struct __dancing_links_node *left, *right, *up, *down;
} dlx_node;

typedef struct __node_pool {
	dlx_node *of_pool[FRAGMENT_SIZE + 1];
	unsigned int count;
	unsigned int idx;
} dlx_pool;

dlx_pool pool;
dlx_node head;
dlx_node *rows[ROW_MAX + 10];
dlx_node *cols[COL_MAX + 10];

int count_one[COL_MAX + 10];
unsigned int solution[256 + 10];
uint8_t isuse[256];
int global_map[1024];
Score best;

int g_x1, g_y1, g_x2, g_y2;

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
static inline int isInValid(int x, int y, int x1, int y1, int x2, int y2)
{
	return (x1 > x) || (x >= x2) || (y1 > y) || (y >= y2);
}

dlx_node *nodeAlloc()
{
	if (pool.count++ == 0) {
		pool.of_pool[pool.idx] = malloc(sizeof(dlx_node) * FRAGMENT);
		return pool.of_pool[pool.idx];
	}

	div_t tmp = div(pool.count, FRAGMENT);
	if (tmp.quot == pool.idx) return &pool.of_pool[tmp.quot][tmp.rem];

	pool.idx++;
	pool.of_pool[pool.idx] = malloc(sizeof(dlx_node) * FRAGMENT);
	return pool.of_pool[pool.idx];
}

void nodeAllFree()
{
	int i;
	for (i=0; i<pool.idx; i++) free(pool.of_pool[i]);

	pool.count = 0;
	pool.idx = 0;
}

void dump1()
{
	dlx_node *p, *q;

	printf("-------------------------------------\n");
	for (p=head.right; p!=&head; p=p->right) {
		printf("{%d}", p->col);
	}
	printf("\n++++++++++++++\n");
	for (p=head.down; p!=&head; p=p->down) {
		printf("[%d]\t", p->row);
		for (q=p->right; q!=p; q=q->right) {
			printf("%d,", q->col);
		}
		printf("\n");
	}
	printf("-------------------------------------\n");
}

void dlx_init()
{
	memset(isuse, 0, sizeof(uint8_t) * 256);
	memset(count_one, 0, sizeof(int) * (COL_MAX + 10));
	pool.count = 0;
	pool.idx = 0;

	head.row = head.col = 0;
	head.left = head.right = head.up = head.down = &head;

	int i;
	for (i=0; i<COL_MAX; i++) {
		cols[i] = nodeAlloc();
		cols[i]->left = head.left;
		head.left = cols[i];

		//cols[i]->id = 0;
		cols[i]->col = i;
		cols[i]->right = &head;
		cols[i]->up = cols[i]->down = cols[i];
		cols[i]->left->right = cols[i];
		cols[i]->right->left = cols[i];
	}

	for (i=0; i<ROW_MAX; i++) {
		rows[i] = nodeAlloc();
		rows[i]->up = head.up;
		head.up = rows[i];

		rows[i]->id = 0;
		rows[i]->row = i;
		rows[i]->down = &head;
		rows[i]->left = rows[i]->right = rows[i];
		rows[i]->up->down = rows[i];
		rows[i]->down->up = rows[i];
	}
}

void dlx_link(const Stone *stones, int n, const int *map, int x1, int y1, int x2, int y2)
{
	int i, j, k, x, y;
	dlx_node *p;

	// Obstacle
	for (i=0; i<1024; i++) {
		if (map[i] != -2) continue;
		cols[i]->left->right = cols[i]->right;
		cols[i]->right->left = cols[i]->left;
	}

	// Stone
	for (i=0; i<256; i++) {
		int idxes[16];
		int8_t op[256];
		int out_flg = (i >= n);
		if (!out_flg) BlockDefineOperation(stones[i].list, op);

		int r1 = (i << 13);
		for (j=0; j<8; j++) {
			int len = stones[i].len;
			int8_t *pp = &op[j << 5];
			int fail_flg = out_flg || (pp[0] == INT8_MAX);
			if (!fail_flg) for (k=0; k<len; k++) idxes[k] = (pp[(k << 1) + 1] << 5) + pp[k << 1];

			for (y=0; y<32; y++) {
				for (x=0; x<32; x++) {
					int bidx = ((y << 5) + x);
					int row = r1 + (bidx << 3) + j;
					int flg = fail_flg || isInValid(x, y, x1, y1, x2, y2);

					for (k=0; k<len; k++) {
						if (flg) break;
						int idx = idxes[k] + bidx;
						if (map[idx] != -1) flg = 1;
					}

					if (flg) {
						rows[row]->up->down = rows[row]->down;
						rows[row]->down->up = rows[row]->up;
						continue;
					}

					for (k=0; k<len; k++) {
						int col = idxes[k] + bidx;

						p = nodeAlloc();
						p->row = row;
						p->col = col;
						p->id = i;
						p->idx = idxes[k] + bidx;

						p->up = cols[col];
						p->down = cols[col]->down;
						cols[col]->down->up = p;
						cols[col]->down = p;

						p->left = rows[row];
						p->right = rows[row]->right;
						rows[row]->right->left = p;
						rows[row]->right = p;
						rows[row]->id = 1;

						count_one[col]++;
					}
				}
			}
		}
	}
}

void printColumn()
{
	dlx_node *p;

	for (p=head.right->right; p!=&head; p=p->right) {
		printf("%d, ", p->col);
	}
	printf("\n");
}

void printRow(int col)
{
	dlx_node *p;

	printf("COL: %d[count: %d?]\n", col, count_one[col]);
	for (p=cols[col]->down; p!=cols[col]; p=p->down) {
		printf("\t%d\n", p->row);
	}
	printf("\n");
}


void output(int depth)
{
	dlx_node *p;
	int i, map[1024];

	memcpy(map, global_map, sizeof(int) * 1024);

	//printf("OUTPUT\n");
	for (i=0; i<depth+1; i++) {
		int r = solution[i];

		for (p=rows[r]->right; p!=rows[r]; p=p->right) {
			//printf("\t[%d]: %d, %d\n", r, p->id, p->idx);
			map[p->idx] = p->id;
		}
	}

	//if (isAccept(map, g_x1, g_y1, g_x2, g_y2) && bestScore(&best, map)) {
	//if (bestScore(&best, map)) {
		//printf("Update best score: (%d, %d)\n", best.score, best.zk);
		dumpMap2(map);
	//}
}

int crossChannel(int depth, int n)
{
	dlx_node *p;
	int selected_col, min;

	if (depth == n) return 1;
	if (head.right == &head) return 1;

	// Col
	min = ROW_MAX;
	for (p=head.right; p!=&head; p=p->right) {
		if (min <= count_one[p->col]) continue;
		min = count_one[p->col];
		selected_col = p->col;
		if (min <= 1) break;
	}
	if (min == 0) return 0;	// 0は除いてみようか

	// Row
	dlx_node *q, *r, *s;
	for(p=cols[selected_col]->down; p!=cols[selected_col]; p=p->down) {
		//if (isuse[p->id]) continue;
		//isuse[p->id] = 1;

		solution[depth] = p->row;
		output(depth);

		// Delete for Z-Problem
		/*
		int i, offset = (p->id << 13);
		for (i=0; i<8192; i++) {
			int r = i + offset;
			if (rows[r]->id != 1) continue;
			if (r == p->row) continue;

			for (q=rows[r]->right; q!=rows[r]; q=q->right) {
				q->up->down = q->down;
				q->down->up = q->up;
				count_one[q->col]--;
			}

			rows[r]->id = 2;
		}
		*/

		printf("IKUZO\n");
		//dump1();

		// Delete for Algorithm-X
		int f1 = 1;
		for(q=p; (f1 || q!=p); q=q->right) {
			if (q == rows[p->row]) continue;
			printf("%p, %d, %d, %d, %d\n", q, q == &head, q == rows[p->row], q->idx, q->id);

			// こことかの「cols」は、ちゃんと消されたcolsかなぁ
			for (r=cols[q->col]; r!=cols[q->col]; r=r->down) {
				int f2 = 1;
				for(s=r->right; (f2 || s!=r); s=s->right) {
					s->up->down = s->down;
					s->down->up = s->up;
					count_one[q->col]--;
					f2 = 0;
				}
			}

			cols[q->col]->left->right = cols[q->col]->right;
			cols[q->col]->right->left = cols[q->col]->left;
			f1 = 0;
		}

		/*
		for (q=rows[p->row]->right; q!=rows[p->row]; q=q->right) {
			for (r=cols[q->col]->down; r!=cols[q->col]; r=r->down) {
				if (r->row == p->row) {
					r->up->down = r->down;
					r->down->up = r->up;
					count_one[r->col]--;

					//r->left->right = r->right;
					//r->right->left = r->left;
				} else {
					for (s=rows[r->row]->right; s!=rows[r->row]; s=s->right) {
						s->up->down = s->down;
						s->down->up = s->up;
						count_one[s->col]--;

						//s->left->right = s->right;
						//s->right->left = s->left;
					}
				}

				//rows[r->row]->id = 0;
			}

			cols[q->col]->left->right = cols[q->col]->right;
			cols[q->col]->right->left = cols[q->col]->left;
		}
		*/

		//dump1();




		// Recursive
		//if (crossChannel(depth + 1, n)) return 1;
		if (crossChannel(depth + 1, n)) return 1;

		printf("TORITORI\n");
		exit(1);



		// Restore for Z-Problem
		/*
		for (i=0; i<8192; i++) {
			int r = i + offset;
			if (rows[r]->id != 2) continue;
			if (r == p->row) continue;

			for (q=rows[r]->right; q!=rows[r]; q=q->right) {
				q->up->down = q;
				q->down->up = q;
				count_one[q->col]++;
			}

			rows[r]->id = 1;
		}
		*/

		// Restore for Algorithm-X
		for (q=rows[p->row]->right; q!=rows[p->row]; q=q->right) {
			cols[q->col]->left->right = cols[q->col];
			cols[q->col]->right->left = cols[q->col];

			for (r=cols[q->col]->down; r!=cols[q->col]; r=r->down) {
				if (r->row == p->row) {
					r->up->down = r;
					r->down->up = r;
					count_one[r->col]++;

					r->left->right = r;
					r->right->left = r;
				} else {
					for (s=rows[r->row]->right; s!=rows[r->row]; s=s->right) {
						s->up->down = s;
						s->down->up = s;
						count_one[s->col]++;

						s->left->right = s;
						s->right->left = s;
					}
				}

				//rows[r->row]->id = 1;
			}
		}
	}

	return 0;
}

int solver(int *map, int x1, int y1, int x2, int y2, int *original_stones, int n)
{
	dump(map, x1, y1, x2, y2, original_stones, n);


	g_x1 = x1;
	g_y1 = y1;
	g_x2 = x2;
	g_y2 = y2;




	// Prepare
	int i, j;
	Stone stones[256];
	stoneEncode(stones, original_stones, n);
	for (i=0; i<1024; i++) map[i] = (map[i] == 0) ? -1 : -2;

	best.score = 1024;

	// Node Link
	dlx_init();
	dlx_link(stones, n, map, x1, y1, x2, y2);





	memcpy(global_map, map, sizeof(int) * 1024);
	int ret = crossChannel(0, n);
	printf("Ret = %s\n", (ret == 1) ? "OK" : "NG");

	/*
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

