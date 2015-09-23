
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
#define HELP "Use: ffeval <filename> offset:length \"structure\"\n"\
			"Version " VERSION  "\n" \
            "Structure: eg. 'float' : array of floats"
            

#define NBUF 1024
#define NASC 100
unsigned char buf[NBUF];
unsigned char primative[NBUF];



int main(int argc, char **argv)
{
	char tmp[1024],*structstring="c";
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
	if (argc == 4) {
		structstring =  argv[3];
	}
	fprintf(stderr, "File: %s\nOffset %d length %d structure '%s'\n", fn, offset, length, structstring);
	FILE *fin = fopen(fn, "rb");
	if (!fin) { fprintf(stderr, "cannot open file %s\n", fn); exit(1); }
	fseek(fin, offset, SEEK_SET);
	
	char asc[NASC] = "";	
	unsigned char tmps[NASC]; memset(tmps, 0x00, NASC);
	int origprimsize = 0,primsize =0, lenstruct = strlen(structstring),k=0,kk=0;
	char sitem = '^';
	int  wiggle = 0;
	unsigned char c = '\0';
	for (int l = NBUF, i = 0; length > 0 && l == (NBUF-wiggle); i += l) {
		printf("\n*** file boundary %d length %d\n", i, NBUF - wiggle)
		l = fread(buf+wiggle, 1, NBUF-wiggle, fin);
		length -= l;


		for (int j = 0 ; j < l; ++j) {
			c = ((unsigned char *)buf)[j];

			if (primsize == 0) {

				if (sitem != '^') { /* ignore the first pass*/

					switch (sitem) {
					case 'c': { asc[0] = (isprint(((unsigned char*)tmps)[0])) ? (((unsigned char*)tmps)[0]) : '.'; asc[1] = '\0';   break; }   /* byte = single character */
					case 'C': { for (kk = 0; kk < 16; ++kk) asc[15-kk] = (isprint(((unsigned char*)tmps)[kk])) ? (((unsigned char*)tmps)[kk]) : '.'; asc[16] = '\0';  break; }  /* per 16 bytes character */
					}
					for (kk = origprimsize-1; kk >=0 ; --kk) printf("%02x ", tmps[kk]);
				}
				printf("  %s\n%08d: ", asc, j);  memset(asc, 0x00, NASC);


				sitem = structstring[k%lenstruct]; k++;
				switch (sitem) {
				 case 'c': { origprimsize=primsize = 1;  break; }   /* byte = single character */
				 case 'C': { origprimsize=primsize = 16;  break; }  /* per 16 bytes character */
				 default: { fprintf(stderr, "-- Unknown structure specification '%c'\n", sitem); exit(1);  }
				}
				
			}

			tmps[--primsize] = c;

	    }   /* through readed buffer*/
	}   /* entire file per buffer of size nbuf (largest size smaller than NBUF and multiple of sum of primitive of lenstruct*/

	fclose(fin);



	return 0;
}
