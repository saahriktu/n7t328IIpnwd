/*
 * n7t328IIpnwd v0.5 - UTF-32 to KOI8-R/ASCII lossless dump util
 * under GPLv3, 2016 (c) Artem Kurashov
 */

#include <stdio.h>
#include <string.h>
#include <uniname.h>

// KOI8-R unicode map
long long koieunimap[] = {
	0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524, 0x252C,
	0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590, 0x2591, 0x2592,
	0x2593, 0x2320, 0x25A0, 0x2219, 0x221A, 0x2248, 0x2264, 0x2265, 0x00A0,
	0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7, 0x2550, 0x2551, 0x2552, 0x0451,
	0x2553, 0x2554, 0x2555, 0x2556, 0x2557, 0x2558, 0x2559, 0x255A, 0x255B,
	0x255C, 0x255D, 0x255E, 0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563,
	0x2564, 0x2565, 0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C,
	0x00A9, 0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
	0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, 0x044C, 0x044B,
	0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A, 0x042E, 0x0410, 0x0411,
	0x0426, 0x0414, 0x0415, 0x0424, 0x0413, 0x0425, 0x0418, 0x0419, 0x041A,
	0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x042F, 0x0420, 0x0421, 0x0422,
	0x0423, 0x0416, 0x0412, 0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429,
	0x0427, 0x042A
};

void putthtwcharinfo(long long rlchrid)
{
	char wcharinfobuf[120];
	if (unicode_character_name(rlchrid, wcharinfobuf) == NULL)
		printf("[%x]", rlchrid);
	else
		printf("[%s]", wcharinfobuf);
}

int main(int argc, char **argv)
{
	char MODE_KOIE = 1,	// KOI8-R wrapper on by default
	    MODE_OCPL = 0,	// one char per line mode
	    MODE_SBNAI = 0,	// Square Brackets not as is mode
	    MODE_BE,		// BE/LE mode switcher
	    MODE_BELEG = 1;	// Guess mode switcher
	long long rcharrb;	// Real Char Reverse Bytes
	// (0x12345678 becomes 0x78563412)
	char rcptr;		// Real Char byte pointer
	short int rcbuf;	// Buffer
	int argvp;		// argv pointer
	int koiewptr;		// KOI8-R wrapper pointer
	char sndtsoflg;		// Symbol not dumped to stdout yet (flag)

	for (argvp = 1; argvp < argc; argvp++) {
		if (!strcmp(argv[argvp], "-a"))
			MODE_KOIE = 0;
		if (!strcmp(argv[argvp], "-1"))
			MODE_OCPL = 1;
		if (!strcmp(argv[argvp], "-s"))
			MODE_SBNAI = 1;
		if (!strcmp(argv[argvp], "-be"))
			MODE_BE = 1;
		MODE_BELEG = 0;
		if (!strcmp(argv[argvp], "-le"))
			MODE_BE = 0;
		MODE_BELEG = 0;
		if (!strcmp(argv[argvp], "-h")) {
			printf
			    ("Options:\n-a - disable KOI8-R wrapper (ASCII only)");
			printf("\n-1 - one char per line mode");
			printf("\n-s - square brackets not as is mode");
			printf("\n-be - UTF-32BE\n-le - UTF-32LE\n-h - help\n");
			return 0;
		}
	}

	// BE/LE mode guess
	if (MODE_BELEG) {
		rcharrb = 0;
		for (rcptr = 0; rcptr < 4; rcptr++) {
			if ((rcbuf = getchar()) == EOF) {
				printf("Error: broken stream\n");
				return 1;
			}
			rcharrb += (1 << 8 * rcptr) * rcbuf;
		}
		if (rcharrb == 4294836224) {
			MODE_BE = 1;
			printf("[UTF-32BE]");
		} else {
			MODE_BE = 0;
			printf("[UTF-32LE]");
		}
		if (MODE_OCPL)
			putchar(10);
	}
	// stream process
	rcptr = -1;
	while ((rcbuf = getchar()) != EOF) {
		rcptr++;
		if (rcptr == 0)
			rcharrb = 0;
		if (MODE_BE == 0)
			rcharrb += (1 << 8 * rcptr) * rcbuf;
		else
			rcharrb += (16777216 >> 8 * rcptr) * rcbuf;
		if (rcptr == 3) {
			rcptr = -1;	// Reset pointer
			sndtsoflg = 1;	// Reset flag
			// print char info
			if (MODE_KOIE && rcharrb > 128) {
				for (koiewptr = 0; koiewptr < 128; koiewptr++)
					if (rcharrb == koieunimap[koiewptr]) {
						putchar(128 + koiewptr);
						sndtsoflg = 0;
						break;
					}
			}
			if (sndtsoflg) {
				if (rcharrb < 128)
					if (MODE_SBNAI
					    && (rcharrb == 0x5b
						|| rcharrb == 0x5d))
						putthtwcharinfo(rcharrb);
					else
						putchar(rcharrb);	// ASCII
				else
					putthtwcharinfo(rcharrb);
			}
			if (MODE_OCPL)
				putchar(10);
		}
	}
	return 0;
}
