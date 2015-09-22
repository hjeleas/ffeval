
/* ffeval - file format evaluator. evaluates a file format for long-term preservation under noisy conditions.
     When files are stored and migrated, they undergo 'bit rot': the more migrations, the more artefacts in the bitstreams.
	 This file simulates these conditions and tries to recover from errors.
	 This tool will:
	  (1) visualize a binary file giving a structure
	  (2) simulate one or many store+migrate sessions 
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4996) // Disable deprecation


#define VERSION "0.1"
#define HELP "Use: ffeval [options] <filename> offset:length \"noise-model\"\n"\
			"Version " VERSION  "\n" \
            "Offset and length in bytes\nOptions:\n" \
            " -b : binary \n -a : ascii \n -h : hex (default)\n -w \"STRUCT-STRING\": audio file (.wav)\n -i \"STRUCT-STRING\" : image file (.bmp) -h \"STRUCT-STRING\"  : histogram plot (.csv)\n" \
            "\n\"STRUCT-STRING\", for example:\n* 'BYTE' : sequence of bytes \n* 'UNSIGNED,byte': sequence of unsigned integer followed by one ignored byte.\n" \
            "\n\"NOISE-MODEL\" : eg.\n* 'BSR=0.5' : 50%% bit flip"

#define NBUF 1024
unsigned char *buf[NBUF];


int main(int argc, char **argv)
{
	char tmp[1024];
	if (argc == 1) {
		fprintf(stderr, HELP);
		exit(1);
	}

	char *fn = NULL;
	unsigned offset = 0,length=1024;

	fn = argv[1];
	if (argc == 3) {
		strcpy_s(tmp, argv[2]);
		char *sp = strtok(tmp, ":");  offset = atoi(sp);
		sp = strtok(NULL, ":");  length = atoi(sp);
	}
	fprintf(stderr, "File: %s offset %d length %d\n", fn, offset, length);
	FILE *fin = fopen(fn, "rb");
	if (!fin) { fprintf(stderr, "cannot open file %s\n", fn); exit(1); }
	fseek(fin, offset, SEEK_SET);
	int l = 0;
	do {
		l = fread(buf, 1, NBUF, fin);
		length -= l;
	} while (length > 0 && l==NBUF);

	fclose(fin);



	return 0;
}
