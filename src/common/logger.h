/***********************************************
 * logger.h - Logging Header Library
 ***********************************************
 * Use this in a program to keep a log file of
 * whatever you need to. This will automatically
 * write to a file of a desired name.
 ***********************************************
 * Create by Philip "5n4k3" Simonson
 ***********************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifndef LOGGER_H
#define LOGGER_H

#define NAMESIZ 256

static char __logName[NAMESIZ];
static FILE *__logFile = NULL;
extern const char key[];

/* -------------- function prototypes --------------- */


static int open_log(const char*, const char*);	/* initialize log buffer */
static int append_log(const char*, const char*, ...);  /* append to log file */
static int write_log(const char*, const char*, ...);   /* write to log file */
static int read_log(const char*);		/* reads the log file */
static int crypt_log(const char*);		/* encrypt/decrypt log file */
static int close_log(void);			/* close log file */


/* -------------- functions below here -------------- */


/* start_log:  initialize the buf and log file */
int open_log(const char *filename, const char *open_mode)
{
	if (filename != NULL) {
		memset(__logName, 0, sizeof __logName);
		strncpy(__logName, filename, NAMESIZ);
	}
	if ((__logFile = fopen((filename == NULL) ? __logName : filename,
			open_mode)) == NULL) {
		fprintf(stderr, "Error: Cannot open log file.\n");
		return -1;
	}
	return 0;
}

/* append_log:  append to the end of log file; string with args */
int append_log(const char *filename, const char *s, ...)
{
	va_list args;
	va_start(args, s);
	if (open_log(filename, "at") < 0) {
		va_end(args);
		return -1;
	}
	vfprintf(__logFile, s, args);
	va_end(args);
	close_log();
	return 0;
}

/* write_log:  write out to log file; given string */
int write_log(const char *filename, const char *s, ...)
{
	va_list args;
	va_start(args, s);
	if (open_log(filename, "wt") < 0) {
		va_end(args);
		return -1;
	}
	vfprintf(__logFile, s, args);
	va_end(args);
	close_log();
	return 0;
}

/* read_log:  reads an entire log file displaying contents in stdout */
int read_log(const char *filename)
{
	char buf[BUFSIZ];

	if (open_log(filename, "rt") < 0)
		return -1;
	memset(buf, 0, sizeof buf);
	while (fgets(buf, sizeof(buf)-1, __logFile) != NULL) {
		fputs(buf, stdout);
		memset(buf, 0, sizeof buf);
	}
	close_log();
	return 0;
}

/* crypt_log:  encrypts/decrypts log file */
int crypt_log(const char *filename)
{
	FILE *tmp = NULL;
	char buf[BUFSIZ];
	int bytes, bytes_wrote;
	int total;
	int c, i;

	if ((tmp = tmpfile()) == NULL)
		return -1;
	if (open_log(filename, "r+t") < 0)
		return -1;
	rewind(__logFile);
	i = 0;
	while ((c = fgetc(__logFile)) != EOF) {
		c ^= key[i%strlen(key)] & 0xf0;
		fputc(c, tmp);
		i++;
	}
	rewind(tmp);
	close_log();
	if (open_log(NULL, "wt") < 0)
		return -1;
	total = 0;
	while ((bytes = fread(buf, 1, sizeof(buf), tmp)) > 0) {
		bytes_wrote = fwrite(buf+total, 1, bytes, __logFile);
		total += bytes_wrote;
	}
	fclose(tmp);
	close_log();
	return 0;
}

/* close_log:  closes the log file stream */
int close_log(void)
{
	if (fclose(__logFile))
		return -1;
	__logFile = NULL;
	return 0;
}

#endif
