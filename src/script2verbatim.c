/*--------------------------------------------------------------------
 *
 *	Copyright (c) 1991-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 *	See LICENSE.TXT file for copying and redistribution conditions.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation; version 3 or any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	Contact info: www.generic-mapping-tools.org
 *--------------------------------------------------------------------*/
/*
 * script2verbatim.c removes comments and replaces -ps from example scripts
 *
 * Author:  Florian Wobbe
 * Date:    6-JAN-2015
 * Version: 5
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_string.h"

#define MAX_LINE_LEN 256
#define FAILURE_PREFIX "script2verbatim: "

int is_comment (char *line) {
	/* return 1 if line is a comment line, 0 otherwise */
	size_t n = strspn (line, " #");  /* span ' ' and '#' */
	while (n > 0) {
		if (line[--n] == '#') /* rewind until '#' found */
			return 1;
	}
	/* not a comment line */
	return 0;
}


int main (int argc, char *argv[]) {
    	int i, nargs = 0, line_num = 0, strip_comments = 0, ps2pdf = 0;
	FILE *fp_in, *fp_out;
	char line[MAX_LINE_LEN];

	for (i = 1; i < argc; i++) {
		if (strcmp (argv[i], "--strip-comments") == 0) strip_comments = 1;
		else if (strcmp (argv[i], "--ps2pdf") == 0) ps2pdf = 1;
		else	nargs++;
	}

	if (nargs != 2) {
		fprintf (stderr, FAILURE_PREFIX "usage: script2verbatim [--strip-comments] [--ps2pdf] input output\n");
		return EXIT_FAILURE;
	}

	if ((fp_in = fopen (argv[argc-2], "r")) == NULL) {
		fprintf (stderr, FAILURE_PREFIX "error opening input file %s.\n", argv[argc-2]);
		return EXIT_FAILURE;
	}

	if ((fp_out = fopen (argv[argc-1], "w")) == NULL) {
		fprintf (stderr, FAILURE_PREFIX "error opening output file %s.\n", argv[argc-1]);
		fclose (fp_in);
		return EXIT_FAILURE;
	}

	/* Opening files succeeded */
	while (fgets (line, MAX_LINE_LEN, fp_in) != NULL) {
		size_t len = strlen (line);
		++line_num;
		if (len > 0 && line[len-1] != '\n') {
			fprintf (stderr, FAILURE_PREFIX "line %d too long: %s\n", line_num, line);
			fclose (fp_in);
			fclose (fp_out);
			return EXIT_FAILURE;
		}
		if (strip_comments && is_comment(line)) continue;
		if (ps2pdf)
			fputs (gmt_strrep(line, " -ps ", " -pdf "), fp_out);
		else
			fputs (line, fp_out);
	}

	/* Check EOF indicator */
	if (!feof (fp_in)) {
		fprintf (stderr, FAILURE_PREFIX "error: did not reach eof.\n");
		fclose (fp_in);
		fclose (fp_out);
		return EXIT_FAILURE;
	}

	fclose (fp_in);
	fclose (fp_out);
	return EXIT_SUCCESS;
}
