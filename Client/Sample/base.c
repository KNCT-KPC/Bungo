/* Windows or Linux */
#if defined(_WIN32) || defined(_WIN64)
	#define	WINDOWS
#endif


/* Include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef	WINDOWS
	#include <winsock.h>
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


/* Macros */
#define	MAP(x, y)		map[(y) * 32 + (x)]
#define	STONE(n, x, y)	stones[(8*8) * (i) + ((y) * 8) + (x)]


/* Constant */
#define	BUF_SIZE	(32 * 32 + 1 + 1)
#ifndef	CLIENT_NAME
	#define	CLIENT_NAME	"NoName"
#endif
#ifndef	SERVER_IPADDR
	#define	SERVER_IPADDR	"127.0.0.1"
#endif
#ifndef	SERVER_PORT
	#define	SERVER_PORT	25252
#endif


/* Solver */
int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n, FILE *fp);


/* Gobal */
FILE *global_fpwrite = NULL;
FILE *global_fpread = NULL;


/* Base */
int sendMsg(char *msg, FILE *fp)
{
	fputs(msg, fp);
	int a = fflush(fp);
	printf("SEND: %d:%s", a, msg);

	if (msg[0] != 'E') return EXIT_SUCCESS;
	char tmp[3];
	printf("KITAYO\n");
	if (fgets(tmp, 3, fp) == NULL) return EXIT_FAILURE;
	printf("tmp = %s\n", tmp);
	return (tmp[0] == 'X') ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{

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
	server.sin_addr.s_addr = inet_addr((argc == 2) ? argv[1] : SERVER_IPADDR);
			
	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		perror((argc == 2) ? argv[1] : SERVER_IPADDR);
		return EXIT_FAILURE;
	}

	
	// fdopen
	FILE *fp;
#ifdef	WINDOWS
	osfhandle = _open_osfhandle(sd, _O_RDONLY);
	printf("r+b\n");
	fp = fdopen(osfhandle, "r+b");
#else
	fp = fdopen(sd, "r+");
#endif
	//setvbuf(fp, NULL, _IOFBF, 0);	/* error in windows */

	fputs(CLIENT_NAME, fp);
	fputc('\n', fp);
	fflush(fp);

	while (1) {
		sendMsg("G\n", fp);

		int i;
		char buf[BUF_SIZE];
		
		int x1, y1, x2, y2, n = 1;
		int map[32 * 32];
		int stones[256 * 8 * 8];
		for (i=0; i<(n+3); i++) {
			fgets(buf, BUF_SIZE, fp);

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
				for (j=0; j<(32 * 32); j++)
					map[j] = buf[j] - '0';
			} else {
				for (j=0; j<(8 * 8); j++)
					stones[(8 * 8 * (i - 3)) + j] = buf[j] - '0';
			}
		}

		if (solver(map, x1, y1, x2, y2, stones, n, fp) == EXIT_FAILURE) break;
	}

	fclose(fp);
	close(sd);
#ifdef	WINDOWS
	_close(osfhandle);
	WSACleanup();
#endif

	return EXIT_SUCCESS;
}

