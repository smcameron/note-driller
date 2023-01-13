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

const char *chord_shape[] = { "C", "A", "G", "E", "D" };

const char caged_fret[12][6] = {
		/* frets for each string, low to high */
	/* C  */ { 'x', '3', '2', '0', '1', '0', },
	/* A  */ { 'x', '0', '2', '2', '2', '0', },
	/* G  */ { '3', '2', '0', '0', '0', '3', },
	/* E  */ { '0', '2', '2', '1', '0', '0', },
	/* D  */ { 'x', 'x', '0', '2', '3', '2', },
	/* Cm */ { 'x', '3', '1', '0', '1', 'x', },
	/* Am */ { 'x', '0', '2', '2', '1', '0', },
	/* Gm */ { '3', '1', '0', '0', '3', '3', },
	/* Em */ { '0', '2', '2', '0', '0', '0', },
	/* Dm */ { 'x', 'x', '0', '2', '3', '1', },
};

const char *strings = "EADGBE";

const char natural_notes[] = "ABCDEFG";

static void print_string(int fret)
{
	char fretchar;
	int fret_spacing = 7;
	printf(" ");
	for (int i = 0; i < 24; i++) {
		printf("|");
		if ((i % 12) + 1 == fret)
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

static void print_chord_on_fretboard(int shape, int chord, char sharpflat)
{
	/* We have to figure how much to shift the given chord shape to make the chosen chord */
	const char *chromatic[] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
	int c, offset;
	char chordname[10];

	c = -1;
	for (int i = 0; i < 12; i++) {
		if (strcmp(chord_shape[shape % 5], chromatic[i]) == 0) {
			c = i;
			break;
		}
	}
	if (c == -1) {
		fprintf(stderr, "Hmm, didn't find the shape?\n");
		return;
	}
	/* At this point, c contains index into chromatic pointing to correct shape (one of CAGED) */
	sprintf(chordname, "%c", natural_notes[chord]);
	if (sharpflat != ' ') {
		if (sharpflat == 'b') { /* convert flats to sharps because chromatic[] only has sharps. */
			if (chordname[0] == 'A') { /* Ab becomes G# */
				strcpy(chordname, "G#");
			} else {
				chordname[0]--;
				strcat(chordname, "#");
			}
		} else {
			strcat(chordname, "#");
		}
	}
	/* At this point chordname contains the name of the chord we want (converted to
	 * sharp form).  Now we scan through chromatic[] starting at c, until we get to
	 * our chord, counting up the offset. */
	offset = 0;
	for (int i = 0; i < 12; i++) {
		if (strcmp(chordname, chromatic[(i + c) % 12]) == 0)
			break;
		offset++;
	}
	/* At this point offset contains how many frets to shift the chord shape to get the required chord */

	printf("\n\n\n");
	for (int i = 5; i >= 0; i--) { /* for each string from high to low */
		int fret = caged_fret[shape][i];
		if (fret == 'x') {
			print_string(-1);
			continue;
		}
		fret = (fret - '0' + offset) % 12;
		if (fret == 0)
			fret = 12;
		print_string(fret);
	}
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

int select_string(int last_string)
{
	int new_string;
	do {
		new_string = rand() % 5;
	} while (new_string == last_string);
	return new_string;
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

static int select_major_minor(void)
{
	return rand() % 2;
}

static int select_chord_shape(int major_minor)
{
	return (rand() % 5) + 5 * (!major_minor);
}

static void usage(void)
{
	fprintf(stderr, "usage:\n	notedriller [--bpm 120.0] [--chord | --single-string]\n");
	exit(1);
}

static struct program_options {
	float bpm;
	int mode;
#define NOTE_MODE 0
#define CHORD_MODE 1
#define SINGLE_STRING_MODE 2
} program_options = {
	.bpm = 40.0,
	.mode = NOTE_MODE,
};

static struct option options[] = {
	{ "bpm", required_argument, 0, 'b' },
	{ "chord", no_argument, 0, 'c' },
	{ "single-string", no_argument, 0, 's' },
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
		c = getopt_long(argc, argv, "b:cs", options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'b':
			parse_float_arg(optarg, &opt->bpm);
			break;
		case 'c':
			opt->mode = CHORD_MODE;
			break;
		case 's':
			opt->mode = SINGLE_STRING_MODE;
			break;
		case '?':
			usage();
			break;
		default:
			break;
		}
	}
}

static void note_driller(int single_string_mode)
{
	int note;
	char sharpflat;
	unsigned int waittime_us;
	int string;

	printf("Note drilling mode.\n");
	printf("bpm = %f\n", program_options.bpm);
	waittime_us = (unsigned int) ((60.0 / program_options.bpm) * 1000000.0);

	note = select_note(-1);
	do {
		string = select_string(string);
		note = select_note(note);
		sharpflat = select_sharp_flat(note);
		if (single_string_mode)
			printf("%c%c on the %c string", natural_notes[note], sharpflat, strings[string]);
		else
			printf("%c%c", natural_notes[note], sharpflat);
		fflush(stdout);
		for (int i = 0; i < 12; i++) {
			usleep(waittime_us);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
		if (single_string_mode) {
			print_string(fretnumber(natural_notes[note], sharpflat, strings[string]));
			print_fret_numbers();
		} else {
			print_fretboard(note, sharpflat);
		}
		usleep(waittime_us);
	} while (1);
}

static void chord_driller(void)
{
	int shape;
	int note;
	int major_minor;
	char sharpflat;
	unsigned int waittime_us;

	printf("Chord drilling mode.\n");
	printf("bpm = %f\n", program_options.bpm);
	waittime_us = (unsigned int) ((60.0 / program_options.bpm) * 1000000.0);

	note = select_note(-1);
	do {
		note = select_note(note);
		sharpflat = select_sharp_flat(note);
		major_minor = select_major_minor();
		shape = select_chord_shape(major_minor);
		printf("%s%s-shaped %c%c %s", chord_shape[shape % 5],
				major_minor ? "" : "-minor",
				natural_notes[note],
				sharpflat, major_minor ? "major" : "minor");
		fflush(stdout);
		for (int i = 0; i < 12; i++) {
			usleep(waittime_us);
			printf(".");
			fflush(stdout);
		}
		printf("\n");
		print_chord_on_fretboard(shape, note, sharpflat);
		usleep(waittime_us);
	} while (1);
}

int main(int argc, char *argv[])
{
        struct timeval tv;

        gettimeofday(&tv, NULL);
        srand(tv.tv_usec);

	process_options(argc, argv, &program_options);
	switch (program_options.mode) {
	case NOTE_MODE:
	case SINGLE_STRING_MODE:
		note_driller(program_options.mode == SINGLE_STRING_MODE);
		break;
	case CHORD_MODE:
		chord_driller();
		break;
	}
	return 0;
}

