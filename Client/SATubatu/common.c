/******************************************************************************/
/*                                                                            */
/*                                  common.c                                  */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

FILE *global_fpwrite = NULL;
FILE *global_fpread = NULL;

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
/*        Initialize & Finalize       */
/*------------------------------------*/
/* Initialize */
void initClient(char *name, char *server_ipaddr, int *osfhandle, int *sd)
{
#ifdef	LOCAL
	global_fpread = fopen(INPUT_FILENAME, "r");
	global_fpwrite = (OUTPUT_FILENAME[0] == '-') ? stdout : fopen(OUTPUT_FILENAME, "w");
#else
	// Init for winsock
	#ifdef	WINDOWS
		int sockopt = SO_SYNCHRONOUS_NONALERT;
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			perror("WSAStartup");
			return EXIT_FAILURE;
		}
		setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE, (char *)&sockopt, sizeof(sockopt));
	#endif
	
	// Connect to the server
	*sd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(server_ipaddr);
			
	if (connect(*sd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		perror(server_ipaddr);
		return EXIT_FAILURE;
	}

	// fdopen
	#ifdef	WINDOWS
		*osfhandle = _open_osfhandle(sd, _O_RDONLY);
		global_fpread = fdopen(*osfhandle, "rb");
		global_fpwrite = fdopen(*osfhandle, "wb");
	#else
		global_fpread = fdopen(*sd, "rb");
		global_fpwrite = fdopen(*sd, "wb");
	#endif
	//setvbuf(global_fpwrite, NULL, _IOFBF, 0);	/* Error in windows */
#endif

	// send Name
	sendMsg(name);
}

/* Ready */
int ready(int *map, int *x1, int *y1, int *x2, int *y2, int *stones, int *n)
{
	sendMsg("G");

	int i;
	char buf[BUF_SIZE];
	
	*n = 1;
	for (i=0; i<(*n+3); i++) {
		if (fgets(buf, BUF_SIZE, global_fpread) == NULL) {
			perror(
				#ifdef	LOCAL
					INPUT_FILENAME
				#else
					"Socket"
				#endif
			);
			return 0;
		}
		
		if (i == 0) {
			sscanf(buf, "%2d %2d %2d %2d", x1, y1, x2, y2);
			continue;
		}

		if (i == 2) {
			*n = atoi(buf);
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
	
	return 1;
}

/* Finalize */
void finalClient(int osfhandle, int sd)
{
	fclose(global_fpread);
	fclose(global_fpwrite);

#ifndef	LOCAL
	close(sd);
	#ifdef	WINDOWS
		_close(osfhandle);
		WSACleanup();
	#endif
#endif
}
