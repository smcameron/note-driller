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

int main(int argc, char *argv[])
{
	float bpm = 40.0;
        struct timeval tv;
	useconds_t waittime_us;
	int note;

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
		printf("%c", natural_notes[note]);
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

