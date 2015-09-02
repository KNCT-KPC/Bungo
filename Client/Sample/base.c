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


/* Macros */
#define	MAP(x, y)	map[((y) << 5) + (x)]
#define	STONE(n, x, y)	stones[((i) << 6) + ((y) << 4) + (x)]


/* Constant */
#define	BUF_SIZE	1026
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
int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n);


/* Gobal */
FILE *global_fpwrite = NULL;
FILE *global_fpread = NULL;


/* Base */
int sendMsg(char *msg)
{
	if (msg[0] != '\0')
		fputs(msg, global_fpwrite);
	fputc('\n', global_fpwrite);
	fflush(global_fpwrite);

	if (msg[0] != 'E')
		return EXIT_SUCCESS;
	
	char tmp[3];
	char *r = fgets(tmp, 3, global_fpread);
	return (r == NULL || tmp[0] == 'X') ? EXIT_FAILURE : EXIT_SUCCESS;
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
#ifdef	WINDOWS
	osfhandle = _open_osfhandle(sd, _O_RDONLY);
	global_fpread = fdopen(osfhandle, "rb");
	global_fpwrite = fdopen(osfhandle, "wb");
#else
	global_fpread = fdopen(sd, "rb");
	global_fpwrite = fdopen(sd, "wb");
#endif
	//setvbuf(global_fpwrite, NULL, _IOFBF, 0);	/* Error in windows */

	sendMsg(CLIENT_NAME);
	while (1) {
		sendMsg("G");

		int i;
		char buf[BUF_SIZE];
		
		int x1, y1, x2, y2, n = 1;
		int map[1024];		// 32 * 32
		int stones[16384];	// (8 * 8) * 256
		for (i=0; i<(n+3); i++) {
			fgets(buf, BUF_SIZE, global_fpread);

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
					stones[((i - 3) << 5) + j] = buf[j] - '0';
			}
		}

		if (solver(map, x1, y1, x2, y2, stones, n) == EXIT_FAILURE) break;
	}

	fclose(global_fpread);
	fclose(global_fpwrite);
	close(sd);

#ifdef	WINDOWS
	_close(osfhandle);
	WSACleanup();
#endif

	return EXIT_SUCCESS;
}

