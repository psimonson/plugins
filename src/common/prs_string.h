/*****************************************************
 * prs_string.h - some of my common string functions
 * that I use frequently. Contained in this header
 * library.
 *****************************************************
 * Created by Philip R. Simonson (aka 5n4k3)
 *****************************************************
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef PRS_STRING
#define PRS_STRING

/* --------------------- Function Prototypes ----------------------- */

static int readln_newline(char s[], int maxlen); /* gets string; with newline */
static int readln(char s[], int maxlen); /* gets string; no newline */
static int trim_string(char s[]);	 /* trims newlines from string */

/* --------------------- String Functions -------------------------- */

/* readln_newline:  read a line of input including newline character */
int readln_newline(char s[], int maxlen)
{
	int chars_remain;
	char ch;
	int i;
	i = 0;
	chars_remain = 1;
	while (chars_remain) {
		ch = getchar();
		if (ch == '\n' || ch == EOF) {
			s[i++] = ch;
			chars_remain = 0;
		}
		else if (ch == '\b') {
			if (i > 0)
				s[i--] = '\0';
		}
		else if (i < maxlen-2)
			s[i++] = ch;
	}
	s[i] = '\0';
	return i;
}

/* readln:  reads a line of input */
int readln(char s[], int maxlen)
{
	char ch;
	int i;
	int chars_remain;
	i = 0;
	chars_remain = 1;
	while (chars_remain) {
		ch = getchar();
		if (ch == '\n' || ch == EOF)
			chars_remain = 0;
		else if (ch == '\b') {
			if (i > 0)
				s[i--] = '\0';
		}
		else if (i < maxlen-1)
			s[i++] = ch;
	}
	s[i] = '\0';
	return i;
}

/* trim_string:  trims newline characters off string */
int trim_string(char s[])
{
	int i;
	for (i = strlen(s); i >= 0; i--)
		if (isprint(s[i]))
			break;
	s[i+1] = '\0';
	return i;
}

#endif
