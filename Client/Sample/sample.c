#define	CLIENT_NAME		"Sample"
#define	SERVER_IPADDR	"127.0.0.1"

#include "base.c"


/* Sleep用。使わないなら消すとすっきりする */
#ifdef WINDOWS
	#include <windows.h>
	#define	sleep(n)	Sleep((n) * 1000)
#else
	#include <unistd.h>
#endif


int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n, FILE *fp)
{
	// とりあえず表示しておく
	printf("(%d, %d) ~ (%d, %d)\n\n", x1, y1, x2, y2);

	int x, y;
	printf("Map\n");
	for (y=0; y<32; y++) {
		printf("\t");
		for (x=0; x<32; x++) printf("%d", MAP(x, y));
		printf("\n");
	}
	printf("\n");

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


	// 野生の勘で導き出した答え
	char *solutions[] = {
		"H 0 2 2\n",
		"H 0 2 2\nT 0 -6 0\n",
		"H 0 2 2\nT 0 -6 0\n\nH 0 1 -1\n",
		NULL
	};

	printf("Solutions\n");
	for (i=0; (solutions[i] != NULL); i++) {
		sleep(5);
		
		sendMsg("S\n", fp);

		int j, line = 0;
		for (j=0; (solutions[i][j] != '\0'); j++) {
			if (solutions[i][j] == '\n') line++;
		}

		sendMsg(solutions[i], fp);
		for (j=0; j<(n - line); j++) sendMsg("\n", fp);

		if (sendMsg("E\n", fp) == EXIT_FAILURE) return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}

