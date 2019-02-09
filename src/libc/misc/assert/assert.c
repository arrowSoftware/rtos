/* Copyright (C) 1996 Robert de Bath <rdebath@cix.compulink.co.uk>
 * This file is part of the Linux-8086 C library and is distributed
 * under the GNU Library General Public License.
 */

/*
 * Manuel Novoa III       Dec 2000
 *
 * Converted to use my new (un)signed long (long) to string routines, which
 * are smaller than the previous functions and don't require static buffers.
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <os/logLib.h>

extern char *__ltostr(char *buf, unsigned long uval, int base, int uppercase);

void assertLibInit(void)
{
}

void __assert(char *assertion, char *filename,
	      int linenumber, const char *function)
{
	char buf[12];
	char str[255];

	strcpy(str, filename);
	strcat(str, ":");
	strcat(str, __ltostr(buf + sizeof(buf) - 1, linenumber, 10, 0));
	strcat(str, function ? ": " : "");
	strcat(str, function ? function : "");
	strcat(str, function ? "() " : "");
	strcat(str, ": Assertion \"");
	strcat(str, assertion);
	strcat(str, "\" failed.\n");
	logMsg(str, (ARG) 1, (ARG) 2, (ARG) 3, (ARG) 4, (ARG) 5, (ARG) 6);
	exit(1); /* abort(); */
}

