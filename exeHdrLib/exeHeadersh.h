//-------------------------------------------------------------------
// Name:        exeHeadersh.h
// Purpose:
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
#ifndef _EXE_HEADER_H
#define _EXE_HEADER_H

// ------------------------------------------------------------------
#define DOS_IMAGE_MAGIC   0x5A4D      // MZ
#define DOS_IMAGE_MAGIC2  0x4D5A      // ZM

/// check if an address is in a given range. 
/// i.e  between start and  start+length? 
/// Note we need (cahr *) to get correct pointer arithmetic
#define addressIsIn(address,start,length) \
    ((char *)(address) >= (char *)(start)\
  && (char *)(address) <  (char *)(start)+(length))
// ------------------------------------------------------------------
/**
 * DOS Header Block structure.
 * Decide to use my own definitions rather than depend on those 
 * from winnt.h. That way I won't need to worry about having windows.h, 
 * although it is included in many other places.
 */
typedef struct _DosExeHdr_t
{
   USHORT   usDosHdrId;         // 00 .EXE signature either MZ or ZM
   USHORT   usImagRmdr;         // 02 # if bytes in last page
   USHORT   usImagPages;        // 04 total # of 512 byte pages, incl. last page
   USHORT   usNRelocs;          // 06 # relocation entries; 0x40 or greater for new EXE
   USHORT   usHdrSize;          // 08 header size in paragraphs
   USHORT   usMinExtra;         // 0A min extra paragraphs to allocate
   USHORT   usMaxExtra;         // 0C max   "      "        "    "
   USHORT   usSS;               // 0E initial SS relative to start of executable
   USHORT   usSP;               // 10 inital SP
   USHORT   usChecksum;         // 12 checksum - one's complement of all words
                                //    in executable
   USHORT   usIP;               // 14 initail IP
   USHORT   usCS;               // 16 initial CS, relative to start of executable
   USHORT   usRelocOffs;        // 18 offset within heade of relocation table
                                //    40H or greater for new executable formats
   USHORT   usOvrlyNmbr;        // 1A overlay number;
                                //    normally 0000h = main program
   UCHAR    ucRes1[8];          // 1C start of special area used by various
                                //    compressors etc see Ralf Brown's Interrupt
                                //    list INT 214BH
                                //    Linker Info - TLink et al
   /* from: http://www.ctyme.com/intr/rb-2939.htm
   ---Borland TLINK---
   1Ch 2 BYTEs ??? (apparently always 01h 00h)
   1Eh BYTE signature FBh
   1Fh BYTE TLINK version (major in high nybble, minor in low nybble)
   20h 2 BYTEs ??? (v2.0 apparently always 72h 6Ah, v3.0+ seems always 6Ah 72h)
   ---ARJ self-extracting archive---
   1Ch 4 BYTEs signature "RJSX" (older versions, new signature is "aRJsfX" in
   the first 1000 bytes of the file) 
   ---LZEXE 0.90 compressed executable---
   1Ch 4 BYTEs signature "LZ09" 
   ---LZEXE 0.91 compressed executable---
   1Ch 4 BYTEs signature "LZ91"
   ---PKLITE compressed executable---
   1Ch BYTE minor version number

  1Dh BYTE bits 0-3:
  Major version

  bit 4:
  Extra compression

  bit 5:
  Huge (multi-segment) file
  1Eh 6 BYTEs signature "PKLITE" (followed by copyright message)
  ---LHarc 1.x self-extracting archive---
  1Ch 4 BYTEs unused???
  20h 3 BYTEs jump to start of extraction code
  23h 2 BYTEs ??? 
  25h 12 BYTEs signature "LHarc's SFX " 
  ---LHA 2.x self-extracting archive---
  1Ch 8 BYTEs ???
  24h 10 BYTEs signature "LHa's SFX " (v2.10) or "LHA's SFX " (v2.13) 
  ---TopSpeed C 3.0 CRUNCH compressed file---
  1Ch DWORD 018A0001h 
  20h WORD 1565h 
  ---PKARCK 3.5 self-extracting archive---
  1Ch DWORD 00020001h 
  20h WORD 0700h 
  ---BSA (Soviet archiver) self-extracting archive---
  1Ch WORD 000Fh 
  1Eh BYTE A7h 
  ---LARC self-extracting archive---
  1Ch 4 BYTEs ??? 
  20h 11 BYTEs "SFX by LARC " 
  ---LH self-extracting archive--- 
  1Ch 8 BYTEs ??? 
  24h 8 BYTEs "LH's SFX " 
  ---RAR self-extracting archive---
  1Ch 4 BYTEs signature "RSFX" 
  ---other linkers---
  1Ch var optional information
   */
   USHORT   usOemId;            // 24
   USHORT   usOemInfo;          // 26
   USHORT   usRes2[10];         // 28
   unsigned long ulNewHdrOffs;  // 3C
} DosExeHdr_t;
// 
typedef struct _DosReloc
{
  unsigned short usRelocOff;
  unsigned short usRelSeg;
} DosReloc_t;

// ------------------------------------------------------------------

#if 0
/*
#define OLDSIG          0x5a4d
#define NEWSIG          0x454e
#define SINGLEDATA      0x0001
#define MULTIPLEDATA    0x0002
#define PMODEONLY       0x0008
#define LIBRARY         0x8000
#define FASTLOAD        0x0008
*/
typedef struct
{
    BYTE    bFlags;
    WORD    wSegOffset;
} FENTRY, *PFENTRY;

typedef struct
{
    BYTE    bFlags;
    WORD    wINT3F;
    BYTE    bSegNumber;
    WORD    wSegOffset;
} MENTRY, *PMENTRY;

#define EXPORTED    0x01
#define SHAREDDATA  0x02

typedef struct
{
    WORD    wSector;
    WORD    wLength;
    WORD    wFlags;
    WORD    wMinAlloc;
} SEGENTRY, *PSEGENTRY;

#define F_DATASEG       0x0001
#define F_MOVEABLE      0x0010
#define F_SHAREABLE     0x0020
#define F_PRELOAD       0x0040
#define F_DISCARDABLE   0x1000

// The RTYPE and RINFO structures are never actually used
// they are just defined for use in the sizeof() macro when
// reading the info off the disk.  The actual data is read
// into the RESTYPE and RESINFO structures that contain these
// structures with some extra information declared at the end.

typedef struct
{
    WORD    wType;
    WORD    wCount;
    LONG    lReserved;
} RTYPE;

typedef struct
{
    WORD    wOffset;
    WORD    wLength;
    WORD    wFlags;
    WORD    wID;
    LONG    lReserved;
} RINFO;
#endif
// RESINFO2 is the same structure as RINFO with one modification.
// RESINFO2 structure uses the lower 16 bits of the lReserved from
// RINFO structure to point to a string that represents
// the resource name.  This can be done since the lReserved piece
// of this structure is used for Run-time data.  This use of the
// lReserved portion is done so that all resources of a certain
// type can be read into one allocated array, thus using 1 ALLOC
// and 1 read.  This saves memory and makes the loading faster
// so it's worth the slight confusion that might be introduced.
/*
typedef struct
{
    WORD     wOffset;
    WORD     wLength;
    WORD     wFlags;
    WORD     wID;
    PSTR     pResourceName;
    WORD     wReserved;
} RESINFO2, *PRESINFO;

extern struct tgRESTYPE;
typedef struct tgRESTYPE *PRESTYPE;

typedef struct tgRESTYPE
{
    WORD        wType;              // Resource type
    WORD        wCount;             // Specifies ResInfoArray size
    LONG        lReserved;          // Reserved for runtime use
    PSTR        pResourceType;      // Points to custom type name
    PRESINFO    pResInfoArray;      // First entry in array
    PRESTYPE    pNext;              // Next Resource type
} RESTYPE;

#define GROUP_CURSOR    12
#define GROUP_ICON      14
#define NAMETABLE       15

typedef struct tgNAME
{
    struct tgNAME  *pNext;
    WORD            wOrdinal;
    char            szName[1];      // Text goes here at allocation time
} NAME, *PNAME;
*/

// the following data is extracted from the samples on the PE format

#define SIZE_OF_NT_SIGNATURE	sizeof (DWORD)

/* global macros to define header offsets into file */
/* offset to PE file signature				       */
#define NTSIGNATURE(a) ((LPVOID)((BYTE *)a		     +	\
			((PIMAGE_DOS_HEADER)a)->e_lfanew))

/* DOS header identifies the NT PEFile signature dword
   the PEFILE header exists just after that dword	       */
#define PEFHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE))

/* PE optional header is immediately after PEFile header       */
#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE		     +	\
			 sizeof (IMAGE_FILE_HEADER)))

/* section headers are immediately after PE optional header    */
#define SECHDROFFSET(a) ((LPVOID)((BYTE *)a		     +	\
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew    +	\
			 SIZE_OF_NT_SIGNATURE		     +	\
			 sizeof (IMAGE_FILE_HEADER)	     +	\
			 sizeof (IMAGE_OPTIONAL_HEADER)))	

#ifdef DOSHEADERS   // to make sure this code is declared only once
TAGGEDLISTSTRUCT tlDosHeader =
{ 21,
   {{ "DOS Header",0 },
    { "",0},
    { "FileSignature: %#x", sizeof(WORD) },         
    { "Bytes on last page: %#x", sizeof(WORD) },          
    { "Size of file in 512 byte pages (inc header): %#x", sizeof(WORD) },
    { "Relocation Table Items: %#x", sizeof(WORD) },  
    { "Header Size in paragraphs: %#x", sizeof(WORD) },
    { "Paragraphs needed above program: %#x", sizeof(WORD) },              
    { "Paragraphs wanted above program: %#x", sizeof(WORD) },          
    { "Stack displacement: %#x", sizeof(WORD) }, 
    { "Initial SP value: %#x", sizeof(WORD) },                    
    { "CheckSum: %#x", sizeof(WORD) },
    { "Initial IP value: %#x", sizeof(WORD) },
    { "Code displacement: %#x", sizeof(WORD) },      
    { "First relocation item @ offset %#x",sizeof(WORD) },   
    { "Overlay number: %#x", sizeof(WORD) },
	  { "<reserved space>",4*sizeof(WORD) },
  	{ "OEM identifier: %#x",sizeof(WORD) },
  	{ "OEM information: %#x",sizeof(WORD) },
    { "<reserved space>",10*sizeof(WORD) },
    { "offset of new exe header: %#8lx", sizeof(int)}
   }
};          
#endif

#ifdef NEHEADERS
TAGGEDLISTSTRUCT tlNEHeader =
{ 33,
   {{ "New executable header",0 },
    { "",0 },
    { "NewSignature: %#x", sizeof(WORD) },         
    { "Linker version: %#x",sizeof(BYTE) },
    { "Linker Revision: %#x",sizeof(BYTE) },
    { "Entry table offset: %#x",sizeof(WORD) },
    { "Entry table size: %#x",sizeof(WORD) },
    { "check sum: %#8lx",sizeof(int) },
    { "flags: %#x",sizeof(WORD) },
    { "Automatic data in segment %#x",sizeof(WORD) },
    { "Initial heap size: %#x",sizeof(WORD) },
    { "Initial stack size: %#x",sizeof(WORD) },
    { "Initial IP value: %#x",sizeof(WORD) },
    { "Initial CS value: %#x",sizeof(WORD) },
    { "Initial SP value: %#x", sizeof(WORD) },
    { "Initial SS value: %#x", sizeof(WORD) },
    { "Entries in segment table: %#x",sizeof(WORD) },
    { "Module reference table entries: %#x",sizeof(WORD) },
    { "Non-resident name table size: %#x",sizeof(WORD) },
    { "Segment table offset: %#x",sizeof(WORD) },
    { "Resource table offset: %#x",sizeof(WORD) },
    { "Resident name table offset: %#x",sizeof(WORD) },
    { "Module reference table offset: %#x",sizeof(WORD) },
    { "imort name table offset: %#x",sizeof(WORD) },
    { "Non-resident name table offset: %#8lx",sizeof(int) },
    { "Moveable entries in entry table: %#x",sizeof(WORD) },
    { "Segment alignment shift count: %#x",sizeof(WORD) },
    { "Resource segments: %#x",sizeof(WORD) },
    { "loader flags: %#x",sizeof(WORD) },
    { "Offset of fast load area: %#x",sizeof(WORD) },
    { "Size of fast load area: %#x", sizeof(WORD) },
    { "<reserved>",sizeof(WORD) },
    { "Expected Windows version number: %#x",sizeof(WORD) }
  } 
};
#endif

#if  PEHEADERS
ANYELEMENT alPEChars[] =
       {{"reloctables", "no reloctables",0x0001},  // Relocation info stripped from file.
        {"unresolved externals", "executable file",0x0002},  // File is executable  (i.e. no unresolved externel references).
        {"line numbers","no line numbers",0x0004},  // Line nunbers stripped from file.
        {"local symbols","no local symbols",0x0008},  // Local symbols stripped from file.
        {"low bytes in regular order","low bytes are reversed",0x0080},  // Bytes of machine word are reversed.
        {"no 32-bit words","machine has 32-bit words",0x0100},  // 32 bit word machine.
        {"debug info present","no debug info",0x0200},  
        {"user-mode file","system file",0x1000},  // System File.
        {"EXE (not a DLL)","DLL",0x2000},  // File is a DLL.
        {"high bytes in regular order","high bytes are reversed",0x8000},  // Bytes of machine word are reversed.
       };
      
CHAR *DirectoryString[16] =
       {"Export Directory","Import Directory","Resource Directory","Exception Directory",
        "Security Directory","Base relocation Table","Debug Directory","Copyright String",  
		"Global pointer","TLS directory","Load Configuration Directory","not in use",
		"not in use","not in use","not in use","not in use"};

/*TAGGEDELEMENT csSectionHeaders[9] =
      {{IMAGE_DIRECTORY_ENTRY_EXPORT},            
       {IMAGE_DIRECTORY_ENTRY_IMPORT},         
       {IMAGE_DIRECTORY_ENTRY_RESOURCE},       
       {IMAGE_DIRECTORY_ENTRY_EXCEPTION},      
       {IMAGE_DIRECTORY_ENTRY_SECURITY},       
       {IMAGE_DIRECTORY_ENTRY_BASERELOC},      
       {IMAGE_DIRECTORY_ENTRY_DEBUG},         
       {IMAGE_DIRECTORY_ENTRY_COPYRIGHT},      
       {IMAGE_DIRECTORY_ENTRY_GLOBALPTR},      
       {IMAGE_DIRECTORY_ENTRY_TLS },           
       {IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG}
      };   
*/
CHAR *csSectionHeaders[9] =
       {".text",".bss",".data",".rdata",".rsrc",".edata",".idata",".debug",".pdata"};
#endif

/*
 * dexehdr.h header for MS-DOS .exe file headers
 *
 * used by  dfl.c       - defined
 *          dheader.c   - extern
 *          filstats.c  -   "
 *          lreleocs.cpp    "
 */
// ---------------------------------------------------------------
#if !defined( _DEXEHDR_H )
#define  _DEXEHDR_H

typedef  struct newNeExeHdr_ // for NE type executables
{
  USHORT usNeHdrId;         //  0 lo byte = 0x4E = N, hi byte = 0x45 = E
  UCHAR  ucNeLinkrVer;
  UCHAR  ucLinkrRev;
  USHORT usNeEntrTblOffs;   //  4 Entry table offset, relative to new header
  USHORT usNeEntrTblLen;    //  6  Entry Table length in bytes
  ULONG  ulNeFileCRC;       //  8 CRC/checksum of whole file
  USHORT usNeFlags;         // 0c
  USHORT usNeNAutoSegs;     // 0e
  USHORT usNeInitHeapSize;  // 10 size (in bytes) of local heap; 0 if no
                            //    local heap
  USHORT usNeInitStackSize; // 12 size (in bytes) of stack; 0 if SS != DS
                            //    (as in libs)
  USHORT usNeInitCS;        // 14 initial value of CS:IP
  USHORT usNeInitIP;
  USHORT usNeInitSS;      /* 18 note value specified by SS is an index
                           *    into the module's segment table
                           *    (starting at ordinal 1);
                           *    If SS addresses the automatic data
                           *    segment and SP is 0, SP is set to the
                           *    address obtained by adding the size of
                           *    the automatic data segment to the size
                           *    of the stack
                           */
  USHORT usNeInitSP;
  USHORT usNEntrSegTbl;   // 1c # of entries in segment table
  USHORT usNEntrModRef;   // 1e # of entries in module reference table
  USHORT usNByteNonResTbl; // 20 # of bytes in nonresident-name table
  USHORT usSegTblOffs;    // 22 offset (in bytes) to segment table
  USHORT usResrcTblOffs;  // 24 offset (in bytes) resource table
  USHORT usResNamTblOffs; // 26 offset (in bytes) to resident-name table
  USHORT usModRefTblOffs; // 28 offset (in bytes0 to module-reference
                          //    table
  USHORT usImpNamTblOffs; // 2a offset (in bytes) to imported-names table
  USHORT usNonResNamOffset;//2c offset to nonresident-name table
  USHORT usSpacr;
  USHORT usVMovableEntries;//30
  USHORT usShiftCount;    // 32
  USHORT usNResSegs;      // 34 # of resource segs;
  USHORT usOsFlags;       /* 36 0 = unknown;
                           *    1 = MS OS/2
                           *    2 = MS Windows
                           *    3   reserved
                           *    4   reserved
                           */
  //UCHAR  ucMoreFlags;
  USHORT usGangStart;
  USHORT usGangLength;
  USHORT usMinCodeSwap;
  USHORT usExpWinVer;     // expected windows version
} newNeExeHdr_t;

typedef  struct newSegTbl_
{
   USHORT   segOffs;    // logical sector offset (in bytes)relative to start of file
                        // 0 => no segment data
   USHORT   segLen;     // 0 => segment = 64K, unless offset == 0
   USHORT   dataSeg : 1;   // 1 => data segment, 0 => code segment
   USHORT   ldrAlloc: 1;
   USHORT   loaded  : 1;
   USHORT   resvd1  : 1;
   USHORT   moveable: 1;
   USHORT   pure    : 1;
   USHORT   preload : 1;
   USHORT   segType : 1;
   USHORT   relData : 1;
   USHORT   conform : 1;
   USHORT   resrvd2 : 2;
   USHORT   discard : 1;
   USHORT   minAlloc;      // min allocation size in bytes
}  newSegTbl_t;
// -------------------------------------------------------------

#endif   //_DEXEHDR_H
#endif    // _EXE_HEADER_H
// ------------------------------- eof ------------------------------
