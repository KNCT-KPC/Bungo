/******************************************************************************/
/*                                                                            */
/*                                  common.h                                  */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>

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
#ifndef	LOCAL
	#define	SERVER_PORT	25252
#else
	#define	INPUT_FILENAME	"../../Problem/27/27-local.txt"
	#define	OUTPUT_FILENAME	"-"
#endif
#define	BUF_SIZE	1027


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
#define	STONE(n, x, y)	stones[((n) << 6) + ((y) << 3) + (x)]


/*------------------------------------*/
/*               extern               */
/*------------------------------------*/
extern FILE *global_fpwrite;
extern FILE *global_fpread;
extern int sendMsg(char *msg);

extern void initClient(char *name, char *server_ipaddr, int *osfhandle, int *sd);
extern int ready(int *map, int *x1, int *y1, int *x2, int *y2, int *stones, int *n);
extern void finalClient(int osfhandle, int sd);

extern void dump(int *map, int x1, int y1, int x2, int y2, int *stones, int n);
extern void dumpMap(int *map, int x1, int y1, int x2, int y2, int n);

