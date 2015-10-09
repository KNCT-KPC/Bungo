/******************************************************************************/
/*                                                                            */
/*                          Sample Client for Hikari                          */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/*------------------------------------*/
/*                FLAG                */
/*------------------------------------*/
// LOCAL mode
#ifndef LOCAL
	#define LOCAL
#endif
//#undef LOCAL

// Windows or Linux
#if defined(_WIN32) || defined(_WIN64)
	#define	WINDOWS
#endif

/*------------------------------------*/
/*              Constant              */
/*------------------------------------*/
#define	CLIENT_NAME	"Sample"
#ifndef	LOCAL
	#define	SERVER_IPADDR	"127.0.0.1"
	#define	SERVER_PORT	25252
#else
//	#define	INPUT_FILENAME	"../../../Problem/light/light-local.txt"
//	#define	INPUT_FILENAME	"../../../Problem/27/27-local.txt"
//	#define	INPUT_FILENAME	"../../../Problem/pentomino/8x8-local.txt"
	#define	INPUT_FILENAME	"../../../Problem/41/41-local.txt"
//	#define INPUT_FILENAME  "../../../Problem/official/quest1-local.txt"
	#define	OUTPUT_FILENAME	"-"
	/* KORE => if (OUTPUT_FILENAME[0] == '-') { global_fpwrite = stdout; } <== */
#endif
#define	BUF_SIZE	2048

/*------------------------------------*/
/*             LOCAL mode             */
/*------------------------------------*/
#ifndef LOCAL
	#ifdef	WINDOWS
		#include <winsock.h>	// http://mattn.kaoriya.net/software/windows/20071001121756.htm
		#include <io.h>
		#include <fcntl.h>
		#pragma comment (lib, "ws2_32.lib")
	#else
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <netdb.h>
		#include <unistd.h>
	#endif
#endif

/*------------------------------------*/
/*          MACROS and Global         */
/*------------------------------------*/
#define	MAP(x, y)	map[((y) << 5) + (x)]
#define	STONE(n, x, y)	stones[((i) << 6) + ((y) << 3) + (x)]

FILE *global_fpwrite = NULL;
FILE *global_fpread = NULL;
int sendMsg(char *msg);
int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n);

//morishita added
void FullSearch(const int* map, const int x1, const int y1, const int x2, const int y2, const int* stones, int stonesNum, char* solutions);


/*------------------------------------*/
/*          Local/NW Wrapper          */
/*------------------------------------*/
int sendMsg(char *msg)
{
	if (msg[0] != '\0') fputs(msg, global_fpwrite);
	fputc('\n', global_fpwrite);
	fflush(global_fpwrite);
	if (msg[0] != 'E') return EXIT_SUCCESS;
	
#ifdef	LOCAL
	return EXIT_SUCCESS;
#else
	char tmp[3];
	char *r = fgets(tmp, 3, global_fpread);
	return (r == NULL || tmp[0] == 'X') ? EXIT_FAILURE : EXIT_SUCCESS;
#endif
}

/*------------------------------------*/
/*               Solver               */
/*------------------------------------*/
/* for Sleep */
#ifdef WINDOWS
	#include <windows.h>
	#define	sleep(n)	Sleep((n) * 1000)
#else
	#include <unistd.h>
#endif

int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n)
{
	/*
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
	*/

	char solution[25600] = {0};
	FullSearch(map, x1, y1, x2, y2, stones, n, solution);

	return EXIT_FAILURE;
	//send solution
	/*
	int j, line = 0;
	for (j=0; (solution[j] != '\0'); j++) {
		if (solution[j] == '\n') line++;
	}

	sendMsg("S");
	sendMsg(solution);
	for (j=0; j<(n - line); j++) sendMsg("");

	if(sendMsg("E") == EXIT_FAILURE){
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;

	char *solutions[] = {
		"H 0 2 2\n",
		"H 0 2 2\nT 0 -6 0\n",
		"H 0 2 2\nT 0 -6 0\n\nH 0 1 -1\n",
		NULL
	};
	*/

	/*
	printf("Solutions\n");
	for (i=0; (solutions[i] != NULL); i++) {
		sleep(5);
		printf("Solution %d\n", i+1);

		// Padding
		int j, line = 0;
		for (j=0; (solutions[i][j] != '\0'); j++) {
			if (solutions[i][j] == '\n') line++;
		}

		// Like a START BIT
		sendMsg("S");

		// Main
		sendMsg(solutions[i]);
		for (j=0; j<(n - line); j++) sendMsg("");

		// Prepare for next problem
		if (sendMsg("E") == EXIT_FAILURE) {
			delete[] solutions;
			return EXIT_SUCCESS;	// transition to `Ready state`
		}
	}

	// Forced termination
	delete[] solutions;
	return EXIT_FAILURE;
	*/
}


/*------------------------------------*/
/*                Main                */
/*------------------------------------*/
int main(int argc, char *argv[])
{
#ifdef	LOCAL
	global_fpread = fopen(INPUT_FILENAME, "r");
	global_fpwrite = (OUTPUT_FILENAME[0] == '-') ? stderr : fopen(OUTPUT_FILENAME, "w");
#else
	// Init for winsock
	#ifdef	WINDOWS
		int osfhandle;
		int sockopt = SO_SYNCHRONOUS_NONALERT;
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			perror("WSAStartup");
			return EXIT_FAILURE;
		}
		setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE, (char *)&sockopt, sizeof(sockopt));
	#endif
	
	// Connect to the server
	int sd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_IPADDR);
			
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		perror(SERVER_IPADDR);
		return EXIT_FAILURE;
	}

	// fdopen
	#ifdef	WINDOWS
		osfhandle = _open_osfhandle(sd, _O_RDONLY);
		global_fpread = fdopen(osfhandle, "rb");
		global_fpwrite = fdopen(osfhandle, "wb");
	#else
		global_fpread = fdopen(sd, "rb");
		global_fpwrite = fdopen(sd, "wb");
	#endif
	//setvbuf(global_fpwrite, NULL, _IOFBF, 0);	/* Error in windows */
#endif

	// Handshake
	sendMsg(CLIENT_NAME);
	while (1) {
		sendMsg("G");

		int i;
		char buf[BUF_SIZE];
		
		int x1, y1, x2, y2, n = 1;
		int map[1024];		// 32 * 32
		int stones[16384];	// (8 * 8) * 256
		int bflg = 0;
		for (i=0; i<(n+3); i++) {
			if (fgets(buf, BUF_SIZE, global_fpread) == NULL) {
				bflg = 1;
				perror(
					#ifdef	LOCAL
						INPUT_FILENAME
					#else
						"Socket"
					#endif
				);
				break;
			}
			
			if (i == 0) {
				sscanf(buf, "%2d %2d %2d %2d", &x1, &y1, &x2, &y2);
				continue;
			}

			if (i == 2) {
				n = atoi(buf);
				continue;
			}

			int j;
			if (i == 1) {
				for (j=0; j<1024; j++)
					map[j] = buf[j] - '0';
			} else {
				for (j=0; j<64; j++)
					stones[((i - 3) << 6) + j] = buf[j] - '0';
			}
		}

		if (bflg || solver(map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE) break;
	}

	fclose(global_fpread);
	fclose(global_fpwrite);
#ifndef	LOCAL
	close(sd);
	#ifdef	WINDOWS
		_close(osfhandle);
		WSACleanup();
	#endif
#endif

	return EXIT_SUCCESS;
}
