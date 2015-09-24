
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


#define VERSION "0.2"
#define HELP "Use: ffeval <filename> offset:length \"structure\"\n"\
			"Version " VERSION  "\n" \
            "See https://github.com/hjeleas/ffeval/tree/init" "\n"
  

#define NBUF 1024
#define NASC 100
unsigned char buf[NBUF];

const char *fmt[] = { "c","hhi","hhu","hi","hu","u","d","i","li","lu","lli","llu","f","lf","Lf",NULL };
const int   fmtsize[] = { sizeof(char),sizeof(signed char),sizeof(unsigned char),sizeof(signed short),sizeof(unsigned short),sizeof(unsigned int),sizeof(signed int),sizeof(signed int),sizeof(signed long),sizeof(unsigned long),sizeof(signed long long),sizeof(unsigned long long),sizeof(float),sizeof(double),sizeof(long double),1 };

int primative_index(char *s, int *sz)
{
	int i = 0;
	for (i = 0; fmt[i] != NULL; ++i) {
		if (!strcmp(s, fmt[i])) {
			if (sz) *sz = fmtsize[i];
			return i;
		}
	}
	return i;
}

int primitive_size(char *s,int sizefromprimitive)
{
	int len = atoi(s),sz=1,i=0;
	while (isdigit(*s)) ++s;
	
	i = primative_index(s, &sz);

	if (fmt[i] == NULL) fprintf(stderr, "\n** format '%s' unknown in structure specification, using 'byte'\n");
	return (len != 0 && sizefromprimitive==0) ? len*sz : sz;
}

void byte_seq(unsigned char *sbuf, int len, char *byteseq, int buflen)
{
	buflen -= 4;
	char s[5];
	memset(byteseq, 0x00, buflen);
	for (int i = 0; i < len; ++i) {
		sprintf(s,"%02x ", (unsigned) ((unsigned char *)sbuf)[i]);
		if (strlen(byteseq) < buflen + strlen(s)) strcat(byteseq, s); 
		else { strcat(byteseq, "..."); break; /* no more room in buffer */}
	}
}
void byte_value(unsigned char *sbuf, int len, int primlen, char *value, char *fmt,int buflen)
{
	buflen -= 4;
	char s[50];
	memset(value, 0x00, buflen);
	int pr = 0;
	
	for (int i = 0; i < len; i+=primlen) {
		switch (primative_index(fmt, NULL)) {
			/*c*/ case 0:			sprintf(s, "%c", isprint(((unsigned char *)sbuf)[i]) ? ((unsigned char *)sbuf)[i] : '.'); break;
			/*hhi*/ case 1:			sprintf(s, "%hhi ", *((signed char *)(sbuf + i))); break;
			/*hhu*/ case 2:			sprintf(s, "%hhu ", *((unsigned char *)(sbuf+i)) ); break;
			/*hi*/ case 3:			sprintf(s, "%hi ", *((signed short *)(sbuf+i))); break;
			/*hu*/ case 4:			sprintf(s, "%hu ", *((unsigned short *)(sbuf+i))); break;
			/*u*/ case 5:		    sprintf(s, "%u ", *((unsigned int *)(sbuf+i))); break;
			/*d*/ case 6:			sprintf(s, "%d ", *((signed int *)(sbuf+i))); break;
			/*i*/ case 7:			sprintf(s, "%i ", *((signed int *)(sbuf+i))); break;
			/*li*/ case 8:			sprintf(s, "%li ", *((signed long *)(sbuf+i))); break;
			/*lu*/ case 9:			sprintf(s, "%lu ", *((unsigned long  *)(sbuf+i))); break;
			/*lli*/ case 10:		sprintf(s, "%lli ", *((signed long long *)(sbuf+i))); break;
			/*llu*/ case 11:		sprintf(s, "%llu ", *((unsigned long long *)(sbuf+i))); break;
			/*f*/ case 12:			sprintf(s, "%f ", *((float *)(sbuf+i))); break;
			/*lf*/ case 13:			sprintf(s, "%lf ", *((double *)(sbuf+i))); break;
			/*Lf*/ case 14:			sprintf(s, "%Lf ", *((long double *)(sbuf+i))); break;
			default:
			  sprintf(s, "%c", isprint(((unsigned char *)sbuf)[i]) ? ((unsigned char *)sbuf)[i] : '.');
			  break;
		}
		                    
		if (strlen(value) < buflen + strlen(s)) strcat(value, s);
		else { strcat(value, "..."); break; /* no more room in buffer */ }
	}
}

int main(int argc, char **argv)
{
	char tmp[1024],byteseq[100]="", value[100]="", structstring[1024]="%16c";
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
	if (argc == 4) { /* hexdump as default */
		//strcpy( structstring, "%c%c%c%c%u%lf%lf%u%c%c%c%c%u%9c%9c%hu%8f%130c%30c%u%u%c%c%hu%f%48c%f%u%f%c%187c");  // argv[3];
		//old spc
		strcpy(structstring, "%c%c%hi%f%f%f%c%c%hi%hhi%hhi%hhi%hhi%8c%d%d%7f%130c%30c%32c");  // argv[3];
	}
	
	fprintf(stderr, "# ffeval v" VERSION" - Evaluate file format headers for long-term preservation for validation on this on this platform\n", fn, offset, length, structstring);
	fprintf(stderr, "# File: %s\nOffset %d length %d structure '%s'\n", fn, offset, length, structstring);
	FILE *fin = fopen(fn, "rb");
	if (!fin) { fprintf(stderr, "\n** cannot open file %s\n", fn); exit(1); }
	fseek(fin, offset, SEEK_SET);
	if (length>NBUF) { fprintf(stderr, "\n** length exceeds maximum (%d>%d)\n", length, NBUF); exit(1); }
	

	int origprimsize = 0,primsize =0, lenstruct = strlen(structstring),k=0,kk=0;
	char sitem = '^';
	unsigned char c = '\0';

	int i=0,l = fread(buf, 1, NBUF, fin),sz=1,primsz=1;
	if (l != length) { fprintf(stderr,"\n** Remaining file size (%u) after offset (%u) not sufficient for desired length %d\n",l,offset,length); }
	char *sp = NULL;
	unsigned  structuresize = 0;
	for (i = 0, sz=1, sp = strtok(structstring,"%"); sp!=NULL; sp = strtok(NULL, "%"),++i) {
		sz     = primitive_size(sp,0);
		primsz = primitive_size(sp,1);
		structuresize += sz;
		if (structuresize > length) { fprintf(stderr, "\n** Given structure size (%d) exceeds length (%d)\n", structuresize, length); exit(1); }

		byte_seq(buf + offset,sz, byteseq, 100);
		byte_value(buf + offset, sz, primsz, value, sp, 100);
		printf("%08u field %03d sz % 4d/%02d format %%%s bytes %s value %s\n", offset, i,  sz, primsz, sp, byteseq, value);
		offset += sz;
	}	
	fclose(fin);

	/*
	char b[100];
	memset(b, 0x00, 100);  *((unsigned*)b) = (unsigned)1602;
	memset(b, 0x00, 100);  *((double*)b) = (double)100;
	memset(b, 0x00, 100);  *((double*)b) = (double)1800;
	*/
	return 0;
}
