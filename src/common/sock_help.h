/*****************************************************
 * sock_help.h - Networking Library (Helper Library)
 * by Philip "5n4k3" Simonson
 *****************************************************
 */


#ifdef WINDOWS
#include <winsock2.h>
#include <windows.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* transfer defines */
#define MAXLINE		1024
#define TRANSFER_PORT	4545
#define TRANSFER_SEND   1
#define TRANSFER_RECV   0

/* normal defines */
#define MAX_LINE	16384
#define MAX_CLIENTS	10

static int set_nonblocking(int);	/* sets a socket nonblocking */
static int create_server(int, int, const char *);/* create a server socket */
static int create_client(int, int, const char *);/* create a client socket */
static int send_msg(int, const char *);		 /* send a full message */
static void close_conn(int);		/* close socket connection */
static int getln_remote(int, char[],
		int);			/* gets a line from remote socket */
static void strip_cmd(char[], int*);	/* strips off newline characters */
static void get_cmd(int, char[], int);	/* gets a command from remote socket */
static int pstrcmp(const char *, const char *);	/* compare strings */
static int pstricmp(const char *,
		const char *);		/* compare strings; ignoring case */
static int transfer(int, const char *, const char *, int*,
		unsigned char);		/* transfer files from/to */

#ifndef SOCK_HELP
#define SOCK_HELP

/* set_nonblocking: set a socket to non-blocking IO */
int set_nonblocking(int sock)
{
#ifdef WINDOWS
	unsigned long mode = 1;
	return ioctlsocket(sock, FIONBIO, &mode);
#else
	return fcntl(sock, F_SETFL, O_NONBLOCK);
#endif
}

/* create_server:  creates a server; return socket fd */
int create_server(int nonblocking, int port, const char *address)
{
	struct sockaddr_in server;
#ifdef WINDOWS
	SOCKET sock;
	WSADATA wsaData;
#else
	int sock;
#endif
	const int yes = 1;

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(address);

#ifdef WINDOWS
	if (WSAStartup(0x0202, &wsaData) != 0) {
		puts("Error: Could not start winsock.");
		return -1;
	}
	if ((sock = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET) {
		puts("Error: Could not create socket.");
		return -1;
	}
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes,
			sizeof(int)) < 0) {
		perror("setsockopt");
		return -1;
	}
#else
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
		perror("setsockopt");
		return -1;
	}
#endif

	if (nonblocking)
		set_nonblocking(sock);
	if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		perror("bind");
		return -1;
	}
	printf("%s bound to port %d.\nWaiting for incoming connections...\n",
			inet_ntoa(server.sin_addr), port);

	if (listen(sock, MAX_CLIENTS) < 0) {
		perror("listen");
		return -1;
	}
	return sock;
}

/* create_client:  creates a client socket to connect to a server */
int create_client(int nonblocking, int port, const char *address)
{
	struct sockaddr_in server;
#ifdef WINDOWS
	SOCKET sock;
	WSADATA wsaData;

	if (WSAStartup(0x0202, &wsaData) != 0) {
		fprintf(stderr, "Error: Cannot start winsock library.\n");
		return -1;
	}
#else
	int sock;
#endif

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(address);

#ifdef WINDOWS
	if ((sock = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, 0, 0)) == INVALID_SOCKET) {
		fprintf(stderr, "Error: Cannot create the client socket.\n");
		return -1;
	}
#else
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}
#endif
	if (nonblocking)
		set_nonblocking(sock);

	if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "Error: Cannot connect to %s.\n",
				inet_ntoa(server.sin_addr));
		return -1;
	}
	return sock;
}

/* send_msg:  send message to remote socket */
int send_msg(int sock, const char *msg)
{
	int bytes;
	if ((bytes = send(sock, msg, strlen(msg), 0)) != strlen(msg)) {
		puts("Error: Sending message failed.");
		return bytes;
	}
	return bytes;
}

/* close_conn:  closes remote connection */
void close_conn(int sock)
{
#ifdef WINDOWS
	closesocket(sock);
	WSACleanup();
#else
	close(sock);
#endif
}

/* getln_remote:  gets a remote string */
int getln_remote(int sock, char s[], int size)
{
	char ch;
	int chars_remain;
	int i;
	i = 0;
	chars_remain = 1;
	while (chars_remain) {
		recv(sock, &ch, 1, 0);
		if (ch == '\n' || ch == EOF)
			chars_remain = 0;
		else if (ch == '\b')
			s[i--] = '\0';
		else if (i < size-1)
			s[i++] = ch;
	}
	s[i] = '\0';
	return i;
}


/* strip_cmd:  strips off newlines */
void strip_cmd(char buf[], int *len)
{
	int i = *len;

	while (i > 0) {
		if (isprint(buf[i]))
			break;
		--i;
	}
	buf[i+1] = '\0';
	*len = i+1;
}

/* get_cmd:  gets a remote string; removing newlines */
void get_cmd(int sock, char buf[], int size)
{
	int i;

	memset(buf, 0, size);
	i = getln_remote(sock, buf, size);
	strip_cmd(buf, &i);
}

/* pstrcmp:  compare string s1 with s2 */
int pstrcmp(const char *p1, const char *p2)
{
	register const unsigned char *s1 = (const unsigned char *)p1;
	register const unsigned char *s2 = (const unsigned char *)p2;
	unsigned char c1, c2;

	do {
		c1 = (unsigned char)*s1++;
		c2 = (unsigned char)*s2++;
		if (c1 == '\0')
			return c1-c2;
	} while (c1 == c2);

	return c1-c2;
}

/* pstricmp:  *nix doesn't have stricmp like windows */
int pstricmp(const char *p1, const char *p2)
{
	register const unsigned char *s1 = (const unsigned char *)p1;
	register const unsigned char *s2 = (const unsigned char *)p2;
	unsigned char c1, c2;

	do {
		c1 = (unsigned char)*s1++;
		c2 = (unsigned char)*s2++;
		if (c1 == '\0')
			return tolower(c1)-tolower(c2);
	} while (tolower(c1) == tolower(c2));

	return tolower(c1)-tolower(c2);
}

/* transfer:  upload/download files from remote machine */
int transfer(int sock, const char *address, const char *fname,
		int *bytes, unsigned char sending)
{
	FILE *fp = NULL;
	char buf[MAXLINE];
	int bytesRead, bytesWritten;
	int total_read, total;

	total = total_read = 0;
	if (sending) {
		if ((fp = fopen(fname, "rb")) == NULL) {
			fprintf(stderr, "Cannot open %s for reading.\n", fname);
			return -1;
		}
		while ((bytesRead = fread(buf, 1, sizeof buf, fp)) > 0) {
			bytesWritten = send(sock, buf, bytesRead, 0);
			total += bytesWritten;
			total_read += bytesRead;
		}
		if (bytesRead < 0) {
			fprintf(stderr, "Error: reading file %s.\n", fname);
			fclose(fp);
			return -1;
		}
	} else {
		if ((fp = fopen(fname, "wb")) == NULL) {
			fprintf(stderr, "Cannot open %s for writing.\n", fname);
			return -1;
		}
		while ((bytesRead = recv(sock, buf, sizeof buf, 0)) > 0) {
			bytesWritten = fwrite(buf, 1, bytesRead, fp);
			total += bytesWritten;
			total_read += bytesRead;
		}
		if (bytesRead < 0) {
			fprintf(stderr, "Error: writing file %s.\n", fname);
			fclose(fp);
			return -1;
		}
	}
	fclose(fp);
	if (total == total_read) {
		puts("File transfer complete.");
	} else {
		puts("File not fully transfered.");
	}
	*bytes = total;
	return 0;
}
#endif
