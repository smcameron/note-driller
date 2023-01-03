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
#include <getopt.h>

const char natural_notes[] = "ABCDEFG";

static void print_string(int fret)
{
	char fretchar;
	int fret_spacing = 7;
	printf(" ");
	for (int i = 0; i < 24; i++) {
		printf("|");
		if ((i % 12) == fret - 1)
			fretchar = '#';
		else
			fretchar = '-';
		for (int j = 0; j < fret_spacing; j++)
			printf("%c", fretchar);
		if ((i % 6) == 0)
			fret_spacing--;
	}
	printf("|\n");

}

static void print_fret_numbers(void)
{
	int fret_spacing = 6;
	for (int i = 0; i < 24; i++) {
		printf("%2d", i);
		for (int j = 0; j < fret_spacing; j++)
			printf(" ");
		if ((i % 6) == 0)
			fret_spacing--;
	}
	printf("24\n");
}

static int fretnumber(char note, char sharpflat, char string)
{
	int fret;
	const char fretnote[] = "A A#B C C#D D#E F F#G G#";

	if (sharpflat == 'b') {
		sharpflat = '#';
		if (note == 'A')
			note = 'G';
		else
			note--;
	}


	for (int i = 0; i < 24; i+=2)
	{
		if (fretnote[i] == string && fretnote[i+1] == ' ') {
			fret = 0;
			for (int j = i; ; j += 2) {
				if (j >= 24)
					j = 0;
				if (fretnote[j] == note && fretnote[j+1] == sharpflat) {
					if (fret == 0)
						fret = 12;
					return fret;
				}
				fret++;
			}
		}
	}
	return 0;
}

static void print_fretboard(int note, char sharpflat)
{
	printf("\n\n\n");
	print_string(fretnumber(natural_notes[note], sharpflat, 'E'));
	print_string(fretnumber(natural_notes[note], sharpflat, 'B'));
	print_string(fretnumber(natural_notes[note], sharpflat, 'G'));
	print_string(fretnumber(natural_notes[note], sharpflat, 'D'));
	print_string(fretnumber(natural_notes[note], sharpflat, 'A'));
	print_string(fretnumber(natural_notes[note], sharpflat, 'E'));
	print_fret_numbers();
	printf("\n\n\n");
}

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

static void usage(void)
{
	fprintf(stderr, "usage:\n	notedriller [--bpm 120.0]\n");
	exit(1);
}

static struct program_options {
	float bpm;
} program_options = {
	.bpm = 40.0,
};

static struct option options[] = {
	{ "bpm", required_argument, 0, 'b' },
	{ NULL, 0, 0, 0 },
};

static void parse_float_arg(char *arg, float *value)
{
	float v;
	int rc;

	rc = sscanf(arg, "%f", &v);
	if (rc != 1)
		usage();
	*value = v;
}

static void process_options(int argc, char *argv[], struct program_options *opt)
{
	int option_index, c;
	while (1) {
		c = getopt_long(argc, argv, "b:", options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'b':
			parse_float_arg(optarg, &opt->bpm);
			break;
		case '?':
			usage();
			break;
		default:
			break;
		}
	}
}

static void note_driller(void)
{
	int note;
	char sharpflat;
	unsigned int waittime_us;

	printf("bpm = %f\n", program_options.bpm);
	waittime_us = (unsigned int) ((60.0 / program_options.bpm) * 1000000.0);

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
		print_fretboard(note, sharpflat);
		usleep(waittime_us);
	} while (1);
}

int main(int argc, char *argv[])
{
        struct timeval tv;

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);

	process_options(argc, argv, &program_options);
	note_driller();
	return 0;
}

