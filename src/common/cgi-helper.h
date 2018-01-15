/* cgi-helper.h - just some common setup stuff for cgi */

#include <stdio.h>

static void header_default(const char*, const char*, const char*);
static void setup(void (*header)(void), void (*body)(void),
	void (*footer)(void));
static void setup_args1(const char*, const char*, const char*, const char*,
	void (*header)(const char*, const char*, const char*),
	void (*body)(void), void (*footer)(const char*));
static void body_default(void);
static void footer_default(const char*);

void header_default(const char* title, const char* bgcolor, const char* fgcolor)
{
	printf("Content-type: text/html\n\n");
	printf("<!DOCTYPE>\n<html>\n<head><title>%s</title></head>\n",
			title);
	printf("\n<body bgcolor=\"%s\" text=\"%s\">\n", bgcolor,
			fgcolor);
}

void setup(void (*header)(void), void (*body)(void), void (*footer)(void))
{
	if (header)
		header();
	if (body)
		body();
	if (footer)
		footer();
}

void setup_args1(const char* title, const char* backcolor, const char* fgcolor,
	const char* footer_message, void (*header)(const char*, const char*,
	const char*), void (*body)(void), void (*footer)(const char*))
	{
	if (header)
		header(title, backcolor, fgcolor);
	if (body)
		body();
	if (footer)
		footer(footer_message);
}

void body_default(void)
{
	printf("<p align=\"center\"><h3>Simple Web Page</h3></p>\n\n");
	printf("<p>This is some sample text for the body of this web page. ");
	printf("If you would like to use this just let me know. I will be ");
	printf("continuing to make this for CGI with the \"C\" language. ");
	printf("Please stay tuned for more content as I learn, so will you.");
}

void footer_default(const char* message)
{
	printf("<p align=\"center\"><h5>%s</h5></p>", message);
	printf("\n</body>\n</html>\n");
}

