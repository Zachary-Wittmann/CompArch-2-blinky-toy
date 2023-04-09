#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))

/* Start Music Setup */

//C C# D D# E F F# G G# A A# B

/* Lower Notes */
const int noteL[] = {7645, 7216, 6811, 6428, 6068,
		     5727, 5405, 5102, 4816, 4545,
		     4290, 4050};

/* Basic Notes */
const int note[] = {3822, 3608, 3405, 3214, 3034,
		    2863, 2703, 2551, 2408, 2272,
		    2145, 2025};

/* Higher Notes */
const int noteH[] = {1911, 1804, 1703, 1607, 1517,
		     1431, 1350, 1273, 1201, 1132,
		     1067, 1004, 948};
// Rest = 0


int secret_song[] = {noteH[7], noteH[6],
		     noteH[3], note[9],
		     note[8], noteH[4],
		     noteH[8], noteH[12]};

		    
/* Take On Me */
int songar1[] = {note[9], note[9], note[5],
		 note[2], 0, note[2], 0, note[7],
		 0, note[7], 0, note[7], note[11],
		 note[11], noteH[0], noteH[2],
		 noteH[0], noteH[0], noteH[0]};


/* Duel of the Fates */
int songar2[] = {note[7], 0, note[9], 0, note[7],
		 note[6], note[4], 0, note[7], 0,
		 note[9], 0, note[7], note[6],
		 note[4], 0, 0, noteH[4], noteH[4],
		 noteH[6], noteH[6], noteH[7],
		 noteH[7], noteH[7], noteH[9],
		 noteH[9], noteH[9], note[11],
		 note[11], note[11], noteH[9],
		 noteH[9], noteH[9], noteH[7],
		 noteH[7], noteH[6], noteH[6],
		 noteH[4], noteH[4]};


int song_list[] = {songar1, songar2};

int song_speed[] = {1500, 800};

int song_pause[] = {200, 50};


/* End Music Setup */

