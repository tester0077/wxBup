//-------------------------------------------------------------------
// Name:        exePeHdrh.h
// Purpose:     
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
#ifndef _EXE_PE_HDR_H
#define _EXE_PE_HDR_H
// ------------------------------------------------------------------
#include <wx/treectrl.h>
#include <wx/treebase.h>
#include <wx/longlong.h>
#include <wx/listctrl.h>
#include <wx/dynarray.h>

#include "exeHeadersh.h"

//--------------------------------------------------------------
// PE Header Structures
typedef  struct newPeExeHdr_  // for PE type executables
{
   ULONG    ulHdrId;          // 0x50 (P), 0x4E (E), 0x00, 0x00
   USHORT   usMachine;
   USHORT   usNSections;
   ULONG    ulTimeDate;
   ULONG    ulSymTblOffset;
   ULONG    ulNSyms;
   USHORT   usOptHdrSize;
   USHORT   usFlags;
} newPeExeHdr_t;              // size 24 bytes

typedef  struct newPeOptHdr_  // for PE type executables
{
   USHORT   usMagic;          // 0x107 - ROM image
                              // 0x10b - PE32,
                              // 0x20b PE32+ => 64 bit adress space
   UCHAR    ucLinkerMajor;
   UCHAR    ucLinkerMinor;
   ULONG    ulSizeOfCode;
   ULONG    ulSizeOfInitializedData;
   ULONG    ulSizeOfUninitializedData;
   ULONG    ulAddressEntryPoint;
   ULONG    ulBaseOfCode;
   ULONG    ulBaseOfData;     // only for PE32
} newPeOptHdr_t;              // - 28 bytes long
// ------------------------------------------------------------------

typedef struct _imageDataDir_t {
    ULONG   ulVirtualAddress;
    ULONG   ulSize;
} imageDataDir_t;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
// ------------------------------------------------------------------
// for PE type executables - second portion for PE32
typedef  struct newPeOptNtHdr_   
{
   ULONG    ulImageBase;         //
   ULONG    ulSectionAlignment;
   ULONG    ulFileAlignment;
   USHORT   usMajorOsVersion;
   USHORT   usMinorOSVersion;
   USHORT   usMajorImageVersion;
   USHORT   usMinorImageVersion;
   USHORT   usMajorSubSysVersion;
   USHORT   usMinorSubSysVersion;
   ULONG    ulReserved; // Win32 Version Value
   ULONG    ulSizeOfImage;
   ULONG    ulSizeOfHeaders;
   ULONG    ulChecksum;
   USHORT   usSubSysReq;
   USHORT   usDLLCharacteristics;
   ULONG    ulSizeOfStackReserve;
   ULONG    ulSizeOfStackCommit;
   ULONG    ulSizeOfHeapReserve;
   ULONG    ulSizeOfHeapCommit;
   ULONG    ulLoaderFlags;       // obsolete???
   ULONG    ulNumberOfRvaAndSizes;
   // array of 16 data directories follow
   imageDataDir_t dataDirs[16];
} newPeOptNtHdr_t;

//--------------------------------------------------------------
// following the 'newPeOptNtHdr' we have an array of 16
// IMAGE_DIRECTORY_ENTRY_xxx - not all of which are used
// as defined in WINNT.H??
#define  IMAGE_DIRECTORY_ENTRY_EXPORT         0
#define  IMAGE_DIRECTORY_ENTRY_IMPORT         1
#define  IMAGE_DIRECTORY_ENTRY_RESOURCE       2
#define  IMAGE_DIRECTORY_ENTRY_EXCEPTION      3
#define  IMAGE_DIRECTORY_ENTRY_SECURITY       4
#define  IMAGE_DIRECTORY_ENTRY_BASERELOC      5
#define  IMAGE_DIRECTORY_ENTRY_DEBUG          6
#define  IMAGE_DIRECTORY_ENTRY_COPYRIGHT      7  // architecture
                                                 // specific - per M$
#define  IMAGE_DIRECTORY_ENTRY_GLOBALPTR      8
#define  IMAGE_DIRECTORY_ENTRY_TLS            9  // Thread local stor.
#define  IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG   10    // ?? purpose
#define  IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT  11
#define  IMAGE_DIRECTORY_ENTRY_IAT           12
#define  IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT  13
#define  IMAGE_DIRECTORY_ENTRY_COM           14 // + runtime
// see M$ Port. Exec. & Common Object File Format spec.
// for some definition

struct imDirEntry_
{
   ULONG ulRva;   // address of table, when loaded, relative to base
                  // adress of image
   ULONG ulsize;
};
typedef struct imDirEntry_ imDirEntry_t;
// ------------------------------------------------------------------
struct importsEntry
{
  long          lHint;
  wxString      wsName;
  wxString      wsLibrary;
  unsigned long ulRVA;
};

WX_DECLARE_OBJARRAY(importsEntry, ImportsArray );
// -------------------------------------------------------------
typedef struct hdrSegmentsEntry_
{
  wxString      wsName;
  unsigned long ulStart;
  unsigned long ulEnd;
  unsigned long ulFlags;
  unsigned long ulVirtSize;
  unsigned long ulVirtOffset;
  unsigned long ulRawSize;
  unsigned long ulRawOffset;
  unsigned long ulNRelocs;
  unsigned long ulRelocOffset;
  unsigned long ulNLineNbrs;
} peHdrSegmentEntry_t;
WX_DECLARE_OBJARRAY(peHdrSegmentEntry_t, SegmentsArray );
// -------------------------------------------------------------

//--------------------------------------------------------------
typedef  struct newPeOptPPHdr_   // for PE type executables
                                 // - second portion for PE32+
{
   ULONG    ulImageBase1;         //
   ULONG    ulImageBase2;
   ULONG    ulSectionAlignment;
   ULONG    ulSizeOfInitializedData;
   ULONG    ulFileAlignment;
   USHORT   usMajorOsVersion;
   USHORT   usMinorOSVersion;
   USHORT   usMajorImageVersion;
   USHORT   usMinorImageVersion;
   USHORT   usMajorSubSysVersion;
   USHORT   usMinorSubSysVersion;
   ULONG    ulReserved;
   ULONG    ulSizeOfImage;
   ULONG    ulSizeOfHeaders;
   ULONG    ulChecksum;
   USHORT   usSubSysReq;
   USHORT   usDLLCharacteristics;
   ULONG    ulSizeOfStackReserve1;
   ULONG    ulSizeOfStackReserve2;
   ULONG    ulSizeOfStackCommit1;
   ULONG    ulSizeOfStackCommit2;
   ULONG    ulSizeOfHeapReserve1;
   ULONG    ulSizeOfHeapReserve2;
   ULONG    ulSizeOfHeapCommit1;
   ULONG    ulSizeOfHeapCommit2;
   ULONG    ulLoaderFlags;       // obsolete
   ULONG    ulNumberOfRvaAndSizes;
} newPeOptPPHdr_t;

//---------------------------------------------------------------
typedef  struct newPeSectionHdr_
{
   char     name[8];
   ULONG    ulVirtSize;    // actually a union, but not used??
   // according to Microsoft's doc on PE Rev 6, 1999
   // not a union, but used only for executable images
   // if greater than ulRawSize, zero padd the space beyond
   // raw size
   ULONG    ulVirtAddr;    // memory base address of section
   ULONG    ulRawSize;     // size of section
   ULONG    ulRawOffset;   // offset of this section in file
   ULONG    ulRelocOffset;
   ULONG    ulNRelocs;
   ULONG    ulNLineNo;
   ULONG    ulFlags;
} newPeSectionHdr_t;
//---------------------------------------------------------------
typedef struct newPe_IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 0 for terminating null import descriptor
        DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    };
    DWORD   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    DWORD   ForwarderChain;                 // -1 if no forwarders
    DWORD   Name;
    DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} newPeImageImportDescr_t;
// -------------------------------------------------------------
typedef struct newPe_IMAGE_IMPORT_BY_NAME {
    WORD    Hint;
    BYTE    Name[1];
} newPeImageImportByName_t;
// -------------------------------------------------------------
#pragma pack( push, 2 )
typedef struct newPe_IMAGE_IAT {
    WORD          JmpCode;
    unsigned long Rva;
} newPe_IMAGE_IAT_t;
#pragma pack( pop )
// --------------------------------------------------------------
/*
 * we need the file buffer size so that we can implement a check
 * to be sure we dont try to read data from beyond the allocated 
 * buffer, which will/may crash the system.
 */
class PeExeHdr
{
public:
  PeExeHdr( BYTE *lpByte,                     ///< pointer to memory mapped file data
                  unsigned long long   ulFileSize ); ///< size of allocated buffer
  ~PeExeHdr();

  static bool HasWinPeHeader( BYTE *lpByte,   ///< pointer to memory mapped 
                                              /// file data
              unsigned long long   ulFileSize );     ///< size of allocated buffer   
  
  /// display PE header data in a tree
  bool  DisplayPeHeader( wxTreeCtrl *pTree,   ///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ref to parent in tree
  bool DisplayPeOptlHeader( wxTreeCtrl *pTree,///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ref to parent in tree
  void DisplayPeSections( wxTreeCtrl *pTree,  ///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ref to parent in tree
  void DisplayPeDirectories( wxTreeCtrl *pTree,///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ref to parent in tree
  wxString GetMachine( unsigned short type );
  void DisplayImportDir( wxTreeCtrl *pTree,   ///< ptr to tree control
    wxTreeItemId &parentId,                   ///< ref to parent in tree
    newPeImageImportDescr_t *ulRawDataStart, 
    const DWORD sectionBase, 
    const DWORD sectionBaseVirt );
//  void GetImportDir( newPeImageImportDescr_t *pulRawDataStart,
//                     const DWORD sectionBase, 
//                     const DWORD sectionBaseVirt );
  void DisplayIAT( wxTreeCtrl *pTree, 
    wxTreeItemId &parentId,                   ///< ref to parent in tree
    newPe_IMAGE_IAT_t *ulRawDataStart, 
    const DWORD sectionBase, 
    const DWORD sectionBaseVirt );

  void DisplayImports( wxListCtrl *pListCtrl,
                       //wxTreeItemId &parentId,
                       newPeImageImportDescr_t *pulRawDataStart,
                       const DWORD sectionBase, 
                       const DWORD sectionBaseVirt );
  void GetImports( ImportsArray &importsArray );
  void GetSegments( SegmentsArray &segmentsArray );
  bool GetRawDirectoryData( unsigned long ulDir, wxString &wsName,
    unsigned long &ulRawStart, unsigned long &ulRawSize );
  bool GetSectionHeaderByName( wxString wsName, newPeSectionHdr_t *pHdr,
    unsigned int &nSec );
  wxString getFlagString( unsigned long uslFlags );
  DosExeHdr_t *GetDosHdrPtr() {
    return (DosExeHdr_t *)mp_lExeFileData;
  };
  newPeExeHdr_t *GetPeHdrPtr() {
    return (newPeExeHdr_t *)(mp_lExeFileData 
      + GetDosHdrPtr()->ulNewHdrOffs  );
  };
  newPeOptHdr_t *GetPeOptlHdrPtr() {
    return (newPeOptHdr_t *)((unsigned long)GetPeHdrPtr()
      + sizeof(newPeExeHdr_t));
  };
  newPeSectionHdr_t *GetSectionDrPtr() {
    return (newPeSectionHdr_t *)((unsigned long )GetPeOptlHdrPtr() +
      SIZE_OF_NT_SIGNATURE + sizeof( newPeExeHdr_t) + sizeof( newPeOptNtHdr_t ) );
  }

  bool IsDosImage( BYTE *lpByte ) {
    return (*(USHORT *)lpByte == DOS_IMAGE_MAGIC ) 
    || (*(USHORT *)lpByte == DOS_IMAGE_MAGIC2 );
  }
  USHORT DosHdrSize( BYTE *lpByte ) {
    return 0x10*((DosExeHdr_t *)(lpByte))->usHdrSize;
  };
  USHORT GetNmbrPeSection() {
    return GetPeHdrPtr()->usNSections;
  };
  bool IsPeImage( BYTE *lpByte ) {
    return (IsDosImage( lpByte ) ) &&
      // make sure the DOS header is large enough to have a possible entry
      // pointing to a PE header - some packed files don't - such as apack
      ( DosHdrSize( lpByte ) >= sizeof( DosExeHdr_t ) ) &&
      (*(long *)((newPeExeHdr_t *)(lpByte + GetDosHdrPtr()->ulNewHdrOffs ) )
     == 0x4550 ) ;
  }
  // ------------------------------------------------------------------

  unsigned long GetPeEntryPoint() {
    return GetPeOptlHdrPtr()->ulAddressEntryPoint;
  }
  unsigned long GetPeCodeSize() {
    return GetPeOptlHdrPtr()->ulSizeOfCode;
  }
  unsigned long GetPeCodeBase() {
    return GetPeOptlHdrPtr()->ulBaseOfCode;
  }
  unsigned long GetPeDataBase() {
    return GetPeOptlHdrPtr()->ulBaseOfData;
  }
  unsigned long GetPeImageBase() {
    return GetNtHdrPtr()->ulImageBase;
  }
  unsigned long GetPeTotalHeaderSize() {
    return GetNtHdrPtr()->ulSizeOfHeaders;
  }
  newPeOptNtHdr_t *GetNtHdrPtr() {
    return (newPeOptNtHdr_t *)((unsigned long )GetPeOptlHdrPtr() +
      SIZE_OF_NT_SIGNATURE + sizeof( newPeExeHdr_t) );
  }

// ------------------------------------------------------------------
/// list header data, line by line to a file ptr
  bool  ListHeader();      
private:
  unsigned char *mp_lExeFileData;
  unsigned long long    mp_ullBufferSize;
};


#endif  // _EXE_PE_HDR_H
// ------------------------------- eof ------------------------------
