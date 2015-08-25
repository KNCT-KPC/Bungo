#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define	CLIENT_NAME	"Sample"
#define	SERVER_IPADDR	"127.0.0.1"
#define	SERVER_PORT	25252
#define	BUF_SIZE	(32 * 32 + 1 + 1)

int solver(int *map, int x1, int y1, int x2, int y2, int *stones, int n, FILE *fp)
{
	int x, y;
	
	printf("HANI (%d, %d) ~ (%d, %d)\n", x1, y1, x2, y2);

	printf("MAP\n");
	for (y=0; y<32; y++) {
		for (x=0; x<32; x++) {
			printf("%d", map[y * 32 + x]);
		}
		printf("\n");
	}

	int i;
	printf("STONES\n");
	for (i=0; i<n; i++) {
		printf("stone %d\n", i);
		for (y=0; y<8; y++) {
			for (x=0; x<8; x++) {
				printf("%d", stones[(8*8) * i + (y * 8) + x]);
			}
			printf("\n");
		}
	}

	return EXIT_FAILURE;
}





int main(int argc, char *argv[])
{
	int sd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(SERVER_IPADDR);

	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) != 0) {
		perror(SERVER_IPADDR);
		return EXIT_SUCCESS;
	}

	FILE *fp = fdopen(sd, "r+");
	setvbuf(fp, NULL, _IOLBF, 0);


	fputs(CLIENT_NAME, fp);
	fputc('\n', fp);

	while (1) {
		fputs("G\n", fp);

		int i;
		char buf[BUF_SIZE];
		
		int x1, y1, x2, y2, n = 1;
		int map[32 * 32];
		int stones[256 * 8 * 8];
		for (i=0; i<(n+3); i++) {
			fgets(buf, BUF_SIZE, fp);
			printf("RECV: %s", buf);

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

	return EXIT_SUCCESS;
}

