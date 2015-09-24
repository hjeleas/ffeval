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
  
On the Platform: 
 "Windows 7, x64 64 bit Core-i3 Intel processor + VS.NET 2015 compile target to "86"
 
The .spc (SPCHDR header) below  translates to:
 %c%c%c%c%u%lf%lf%u%c%c%c%c%u%9c%9c%hu%8f%130c%30c%u%u%c%c%hu%f%48c%f%u%f%c%187c    

  
typedef struct
   {
   BYTE   ftflgs;	/* Flag bits defined below */
   BYTE   fversn;	/* 0x4B=> new LSB 1st, 0x4C=> new MSB 1st, 0x4D=> old format */
   BYTE   fexper;	/* Instrument technique code (see below) */
   char   fexp; 	/* Fraction scaling exponent integer (80h=>float) */
   DWORD  fnpts;	/* Integer number of points (or TXYXYS directory position) */
   double ffirst;	/* Floating X coordinate of first point */
   double flast;	/* Floating X coordinate of last point */
   DWORD  fnsub;	/* Integer number of subfiles (1 if not TMULTI) */
   BYTE   fxtype;	/* Type of X axis units (see definitions below) */
   BYTE   fytype;	/* Type of Y axis units (see definitions below) */
   BYTE   fztype;	/* Type of Z axis units (see definitions below) */
   BYTE   fpost;	/* Posting disposition (see GRAMSDDE.H) */
   DWORD  fdate;	/* Date/Time LSB: min=6b,hour=5b,day=5b,month=4b,year=12b */
   char   fres[9];	/* Resolution description text (null terminated) */
   char   fsource[9];	/* Source instrument description text (null terminated) */
   WORD   fpeakpt;	/* Peak point number for interferograms (0=not known) */
   float  fspare[8];	/* Used for Array Basic storage */
   char   fcmnt[130];	/* Null terminated comment ASCII text string */
   char   fcatxt[30];	/* X,Y,Z axis label strings if ftflgs=TALABS */
   DWORD  flogoff;	/* File offset to log block or 0 (see above) */
   DWORD  fmods;	/* File Modification Flags (see below: 1=A,2=B,4=C,8=D..) */
   BYTE   fprocs;	/* Processing code (see GRAMSDDE.H) */
   BYTE   flevel;	/* Calibration level plus one (1 = not calibration data) */
   WORD   fsampin;	/* Sub-method sample injection number (1 = first or only ) */
   float  ffactor;	/* Floating data multiplier concentration factor (IEEE-32) */
   char   fmethod[48];	/* Method/program/data filename w/extensions comma list */
   float  fzinc;	/* Z subfile increment (0 = use 1st subnext-subfirst) */
   DWORD  fwplanes;	/* Number of planes for 4D with W dimension (0=normal) */
   float  fwinc;	/* W plane increment (only if fwplanes is not 0) */
   BYTE   fwtype;	/* Type of W axis units (see definitions below) */
   char   freserv[187]; /* Reserved (must be set to zero) */
   } SPCHDR;

  
