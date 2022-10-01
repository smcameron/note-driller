/*
	Copyright (C) 2022 Stephen M. Cameron
	Author: Stephen M. Cameron

	This file is part of Notedriller.

	Notedriller is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Notedriller is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Notedriller; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

const char natural_notes[] = "ABCDEFG";

int select_note(int last_note)
{
	int new_note;

	do {
		new_note = rand() % (sizeof(natural_notes) - 1);
	} while (new_note == last_note);
	return new_note;
}

int select_sharp_flat(int note)
{
	char sfc[] = " #b";
	int sf = rand() % 3;
	if (sf == 1 && (note == 1 || note == 4)) /* can't sharp B or E */
		sf = 0;
	if (sf == 2 && (note == 2 || note == 5)) /* can't flat C or F */
		sf = 0;
	return sfc[sf];
}

int main(int argc, char *argv[])
{
	float bpm = 40.0;
        struct timeval tv;
	useconds_t waittime_us;
	int note;
	char sharpflat;

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);

	if (argc >= 2) {
		float newbpm;
		int rc;

		rc = sscanf(argv[1], "%f", &newbpm);
		if (rc == 1)
			bpm = newbpm;
	}
	printf("bpm = %f\n", bpm);
	waittime_us = (useconds_t) ((60.0 / bpm) * 1000000.0);

	note = select_note(-1);
	do {
		note = select_note(note);
		sharpflat = select_sharp_flat(note);
		printf("%c%c", natural_notes[note], sharpflat);
		fflush(stdout);
		for (int i = 0; i < 12; i++) {
			usleep(waittime_us);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
		usleep(waittime_us);
	} while (1);

	return 0;
}

