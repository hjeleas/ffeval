# ffeval - a file format evaluator for long-term preservation 

Goal
====

Decode and validate a file-format structure on your platform.

Compile the source code on the  target platform and run it 
against a given structure (see example below). 
The tool will read out the structure and fill in the variables  using the compiler settings
(see https://en.wikipedia.org/wiki/C_data_types).

This tool can also be used for file validation.

Usage 

$ ffeval <filename> <offset:length> <structure-specification>


EXAMPLE 1. : Hexdump of a .SPC file
=====================================================

* Hexdump of a .spc file :

$ c++ -o ffeval main.cpp 
$ ffeval \prj\DOERNER_40842.spc 0:208 

NOTE: Absence of the structure specification defaults to "%16c" 

# OUTPUT: 

File: \prj\DOERNER_40842.spc
Offset 0 length 1024 structure '%16c'

00000000: 00 4d 0f 00 00 40 c8 44 00 00 c8 42 00 00 e1 44   .M...@.D...B...D
00000016: 0d 00 71 00 05 18 0a 20 00 00 00 00 00 00 00 00   ..q.... ........
00000032: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000048: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000064: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000080: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000096: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000112: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000128: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000144: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000160: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000176: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
00000192: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................

* Decode a .spc header on your platform
  
  NOTE: see typedef spc.h below
        the size of a primative datatype (below) is estimated by the sizeof() operator, except when printing string
  
  %Nc   : printable N character string, N bytes  (example: "%16c" results in hexdump above)
  %c    : printable char (1 byte)
  %hhi  : numerical signed byte
  %hhu  : numerical unsigned byte
  %hi   : short int,  at least [-32767,+32767] 
  %hu   : unsigned short 
  %hi   : short int  
  %hu   : unsigned short 
  %u    : unsigned int  
  %d    : signed int,  at least [-32767,+32767] 
  %i    : signed int,  at least [-32767,+32767] 
  %li   : signed long,  at least [-2147483647,+2147483647]
  %lu   : unsigned long
  %lli  : long long int
  %llu  : unsigned long long
  %f    : float
  %lf   : double
  %Lf   : long double
  
The .spc (old style OSPC header) below  translates to:
 "%c%c%hi%f%f%f%c%c%hi%hhi%hhi%hhi%hhi%8c%d%d%7f%130c%30c%32c""

OUTPUT
======
# ffeval v0.2 - Evaluate file format headers for long-term preservation for validation on this on this platform
# File: \prj\DOERNER_40842_Naphtholgelb-S_785nm_1200T_500H_100S_100x_10perc_12sec_4Acc_02_Bearb_03.spc
Offset 0 length 1024 structure '%c%c%hi%f%f%f%c%c%hi%hhi%hhi%hhi%hhi%8c%d%d%7f%130c%30c%32c'
00000000 field 000 sz    1/01 format %c bytes 00  value .
00000001 field 001 sz    1/01 format %c bytes 4d  value M
00000002 field 002 sz    2/02 format %hi bytes 0f 00  value 15
00000004 field 003 sz    4/04 format %f bytes 00 40 c8 44  value 1602.000000
00000008 field 004 sz    4/04 format %f bytes 00 00 c8 42  value 100.000000
00000012 field 005 sz    4/04 format %f bytes 00 00 e1 44  value 1800.000000
00000016 field 006 sz    1/01 format %c bytes 0d  value .
00000017 field 007 sz    1/01 format %c bytes 00  value .
00000018 field 008 sz    2/02 format %hi bytes 71 00  value 113
00000020 field 009 sz    1/01 format %hhi bytes 05  value 5
00000021 field 010 sz    1/01 format %hhi bytes 18  value 24
00000022 field 011 sz    1/01 format %hhi bytes 0a  value 10
00000023 field 012 sz    1/01 format %hhi bytes 20  value 32
...
  
typedef struct
   {
   BYTE  oftflgs;
   BYTE  oversn;	/* 0x4D rather than 0x4C or 0x4B */
   short oexp;		/* Word rather than byte */
   float onpts; 	/* Floating number of points */
   float ofirst;	/* Floating X coordinate of first pnt (SP rather than DP) */
   float olast; 	/* Floating X coordinate of last point (SP rather than DP) */
   BYTE  oxtype;	/* Type of X units */
   BYTE  oytype;	/* Type of Y units */
   WORD  oyear; 	/* Year collected (0=no date/time) - MSB 4 bits are Z type */
   BYTE  omonth;	/* Month collected (1=Jan) */
   BYTE  oday;		/* Day of month (1=1st) */
   BYTE  ohour; 	/* Hour of day (13=1PM) */
   BYTE  ominute;	/* Minute of hour */
   char  ores[8];	/* Resolution text (null terminated unless 8 bytes used) */
   WORD  opeakpt;
   WORD  onscans;
   float ospare[7];
   char  ocmnt[130];
   char  ocatxt[30];
   char  osubh1[32];	/* Header for first (or main) subfile included in main header */
   } OSPCHDR