/******************************************************************************/
/*                                                                            */
/*                                KanaC Common                                */
/*                           -- 現方式の共通部分 --                           */
/*                                                                            */
/******************************************************************************/
#include <stdint.h>

#ifndef INCLUDED_KCOMMON
#define	INCLUDED_KCOMMON
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
#endif

extern int sortByScore(const void *n, const void *m);
extern void stoneEncode(Stone *dst, const int *stones_base, int n);
extern void BlockDefineOperation(const int8_t *zk, int8_t *dst);

extern int isAccept(const int *map, int x1, int y1, int x2, int y2);
extern void bestScore(Score *best, const int *map);

extern void sendAnswer(const int *map, const Stone *stones, const int *original_stone, int n);

