//-------------------------------------------------------------------
// Name:        exeLeHdr.cpp
// Purpose:     
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Standard wxWidgets headers
//-------------------------------------------------------------------
#if defined( _MSC_VER )
// this statement NEEDS to go BEFORE all headers
#define _CRTDBG_MAP_ALLOC
#endif
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// For all others, include the necessary headers (this file is
// usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "exeHeadersh.h"
#include "exeDosHdrh.h"
#include "exeLeHdrh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )
// this block needs to go AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

// ------------------------------------------------------------------

// ------------------------------------------------------------------

LeExeHdr::LeExeHdr( unsigned char *lpByte,
                             unsigned long long ullBfrSize ) 
{ 
  mp_lExeFileData = lpByte; 
  mp_ullBufferSize = ullBfrSize;
}
// ------------------------------------------------------------------
LeExeHdr::~LeExeHdr()
{
  mp_lExeFileData = NULL;
}

// ------------------------------------------------------------------
/**
 * Determine if the file as a PE style header.
 * Note: this fails if we have a PKlite compressed file
 * beause these apps mess up the DOS header and may 
 * leave the value of nRelocs (at offset 0x18) with a 
 * messed up value
 * This means the pointer we end up using may well 
 * enough point beyond the valid buffer we have allocated
 * and thus would cause a crash if we did not check for this case..
 */
bool LeExeHdr::HasWinLeHeader( BYTE *lpByte,    ///< pointer to memory mapped file data
                  unsigned long long   ulFileSize ) ///< size of allocated buffer );    // does the file data represent a DOS PE EXE file)
{
  wxASSERT( DosExeHdr::HasDosHeader( lpByte ) );
  // verify that the proposed PE header is located
  // somewhere within the file - see note above
  unsigned long ulpNewHdrOffset = (long ) NTSIGNATURE (lpByte);
  if( (ulpNewHdrOffset -(unsigned long)lpByte) > ulFileSize )
    return false;
  return  ( (*(USHORT *)lpByte == DOS_IMAGE_MAGIC) &&
      (((DosExeHdr_t *)lpByte)->usRelocOffs >= 0x40 ) &&
      (LOWORD (*(DWORD *)NTSIGNATURE (lpByte)) == IMAGE_OS2_SIGNATURE_LE) ); 
}

// ------------------------------------------------------------------
/**
 * list EXE header data, line by line to a file ptr
 */
bool LeExeHdr::ListHeader()
{
  return true;
}

// ------------------------------------------------------------------
/**
 * Display EXE header data to a tree control
 */
bool LeExeHdr::DisplayLeHeader( wxTreeCtrl *WXUNUSED(pTree),
                                wxTreeItemId &WXUNUSED(parentId ) )
{
  wxString wsTemp;                
  wxString wsT;
#if 0
  DosExeHdr_t *pDosHdr = (DosExeHdr_t *)mp_lExeFileData;
  newNeExeHdr_t *pNeHdr = 
    (newNeExeHdr_t *)(mp_lExeFileData + pDosHdr->lNewHdrOffs );
  // Linker
  wsTemp.Printf( _T("%d.%d"), pNeHdr->ucNeLinkrVer, 
    pNeHdr->ucLinkrRev );
  wxTreeItemId peHdrmachId =
    pTree->AppendItem( parentId,
    _T("Linker                    : ") + wsTemp, -1, -1 );

  // Entry Table Offset
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeEntrTblOffs );
  wxTreeItemId peHdrETO =
    pTree->AppendItem( parentId,
    _T("Entry Table Offset        : ") + wsTemp, -1, -1 );

  // Entry Table Length
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeEntrTblLen );
  wxTreeItemId peHdrETL =
    pTree->AppendItem( parentId,
    _T("Entry Table Length        : ") + wsTemp, -1, -1 );
  // File CRC
  wsTemp.Printf( _T("%08lXH"), pNeHdr->ulNeFileCRC );
  wxTreeItemId peHdrCRC =
    pTree->AppendItem( parentId,
    _T("File CRC                  : ") + wsTemp, -1, -1 );
  // Flags
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeFlags );
  wxTreeItemId peHdrFlags =
    pTree->AppendItem( parentId,
    _T("Flags                     : ") + wsTemp, -1, -1 );
  // Auto Data Segment
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeNAutoSegs );
  wxTreeItemId peHdrAutoData =
    pTree->AppendItem( parentId,
    _T("Automatic Data in Seg     : ") + wsTemp, -1, -1 );
  // Initial Heap Size
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitHeapSize );
  wxTreeItemId peHdrInitlHeap =
    pTree->AppendItem( parentId,
    _T("Initial Heap Size         : ") + wsTemp, -1, -1 );
  // Initial Stack Size
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitStackSize );
  wxTreeItemId peHdrInitlStack =
    pTree->AppendItem( parentId,
    _T("Initial Stack Size        : ") + wsTemp, -1, -1 );
  // Initial CS
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitCS );
  wxTreeItemId peHdrInitlCS =
    pTree->AppendItem( parentId,
    _T("Initial CS                : ") + wsTemp, -1, -1 );
  // Initial IP
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitIP );
  wxTreeItemId peHdrInitlIP =
    pTree->AppendItem( parentId,
    _T("Initial IP                : ") + wsTemp, -1, -1 );
  // Initial SS
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitSS );
  wxTreeItemId peHdrInitlSS =
    pTree->AppendItem( parentId,
    _T("Initial SS                : ") + wsTemp, -1, -1 );
  // Initial SP
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNeInitSP );
  wxTreeItemId peHdrInitlSP =
    pTree->AppendItem( parentId,
    _T("Initial SP                : ") + wsTemp, -1, -1 );
  // # of Entries in Segment Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNEntrSegTbl );
  wxTreeItemId peHdrET =
    pTree->AppendItem( parentId,
    _T("Entries in Segment Table  : ") + wsTemp, -1, -1 );
  // # of Entries in Module Reference Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNEntrModRef );
  wxTreeItemId peHdrRT =
    pTree->AppendItem( parentId,
    _T("Entries in Mod Ref Table  : ") + wsTemp, -1, -1 );
  // # of Entries in Non-resident Name Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNByteNonResTbl );
  wxTreeItemId peHdrNRT =
    pTree->AppendItem( parentId,
    _T("Bytes in Non-Res Table    : ") + wsTemp, -1, -1 );
  // Offset (in bytes) to Segment Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usSegTblOffs );
  wxTreeItemId peHdrSO =
    pTree->AppendItem( parentId,
    _T("Segment Table Offset      : ") + wsTemp, -1, -1 );
  // Offset (in bytes) to Resource Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usResrcTblOffs );
  wxTreeItemId peHdrRsrcT =
    pTree->AppendItem( parentId,
    _T("Resource Table Offset     : ") + wsTemp, -1, -1 );
  // Offset (in bytes) to Resident Name Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usResNamTblOffs );
  wxTreeItemId peHdrRNT =
    pTree->AppendItem( parentId,
    _T("Resident Name Table Offset: ") + wsTemp, -1, -1 );
  // Offset (in bytes) to module-reference Table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usModRefTblOffs );
  wxTreeItemId peHdrMRT =
    pTree->AppendItem( parentId,
    _T("Module Ref Table Offset   : ") + wsTemp, -1, -1 );
  // Offset (in bytes) to imported-names table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usImpNamTblOffs );
  wxTreeItemId peHdrINT =
    pTree->AppendItem( parentId,
    _T("Imported Name Table Offset: ") + wsTemp, -1, -1 );
  // Offset offset to nonresident-name table
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNonResNamOffset );
  wxTreeItemId peHdrNResT =
    pTree->AppendItem( parentId,
    _T("Non-Res Name Table Offset : ") + wsTemp, -1, -1 );
  // # of Moveable Entries
  wsTemp.Printf( _T("%04XH"), pNeHdr->usVMovableEntries );
  wxTreeItemId peHdrME =
    pTree->AppendItem( parentId,
    _T("Moveable Entries          : ") + wsTemp, -1, -1 );
  // Shift Count
  wsTemp.Printf( _T("%04XH"), pNeHdr->usShiftCount );
  wxTreeItemId peHdrSC =
    pTree->AppendItem( parentId,
    _T("Shift Count               : ") + wsTemp, -1, -1 );
  // # of # of resource segs
  wsTemp.Printf( _T("%04XH"), pNeHdr->usNResSegs );
  wxTreeItemId peHdrRS =
    pTree->AppendItem( parentId,
    _T("Resource Segments         : ") + wsTemp, -1, -1 );

  // Flags
  wsTemp.Printf( _T("%04XH"), pNeHdr->usOsFlags );
  wxTreeItemId peHdrOsFlgs =
    pTree->AppendItem( parentId,
    _T("Flags                     : ") + wsTemp, -1, -1 );
  int il;
  unsigned short usVal = pNeHdr->usOsFlags;
  for ( il = 0; il < 16 && usVal; il++, usVal=(usVal>>1) )
  {
    if( usVal&0x01 )
    {
      switch( il )
      { 
        case 0:
          wsT = _T("unknown");
          break;
        case 1:
          wsT = _T("MS OS/2");
          break;
        case 2:
          wsT = _T("MS Windows");
          break;
        case 3:
          wsT = _T("reserved");
          break;
        case 4:
          wsT = _T("reserved");
          break;
        default:
           wsT = _T("unknown");
          break;
      }
      wxTreeItemId peHdrFlags =
        pTree->AppendItem( peHdrOsFlgs, wsT, -1, -1 );
    }
  }
  // Offset of Fast Load area
  wsTemp.Printf( _T("%04XH"), pNeHdr->usGangStart );
  wxTreeItemId peHdrFLA =
    pTree->AppendItem( parentId,
    _T("Fast Load Are Offset     : ") + wsTemp, -1, -1 );
  // Size of Fast Load area
  wsTemp.Printf( _T("%04XH"), pNeHdr->usGangLength );
  wxTreeItemId peHdrFLASize =
    pTree->AppendItem( parentId,
    _T("Fast Load Are Size       : ") + wsTemp, -1, -1 );
  // Min code swap
  wsTemp.Printf( _T("%04XH"), pNeHdr->usMinCodeSwap );
  wxTreeItemId peHdrMinCodeSwp =
    pTree->AppendItem( parentId,
    _T("Min Code swap - reserved : ") + wsTemp, -1, -1 );
  // expected windows version
  wsTemp.Printf( _T("%04XH"), pNeHdr->usExpWinVer );
  wxTreeItemId peHdrWinVer =
    pTree->AppendItem( parentId,
    _T("Expected Windows Version : ") + wsTemp, -1, -1 );
#endif
  return true;   
}
#if 0
// ------------------------------------------------------------------

wxString PeExeHdr::GetMachine( unsigned short type )
{
  wxString wsTemp;
  switch( type )
  {
  case 0x14c:
    wsTemp = _T("i386 and compat.");
    break;
  case 0x184:
    wsTemp = _T("Alpha IXP");
    break;
  case 0x1c:
    wsTemp = _T("ARM" );
    break;
  case 0x284:
    wsTemp = _T("Alpha AXP 64-bit" );
    break;
  case 0x200:
    wsTemp = _T("Intel IA64" );
    break;
  case 0x268:
    wsTemp = _T("Motorola 68000 Series" );
    break;
  case 0x266:
    wsTemp = _T("MIPS" );
    break;
  case 0x366:
    wsTemp = _T("MIPS with FPU" );
    break;
  case 0x466:
    wsTemp = _T("MIPS16 with FPU" );
    break;
  case 0x1F0:
    wsTemp = _T("PowerPC, little endian" );
    break;
  case 0x162:
    wsTemp = _T("R3000" );
    break;
  case 0x166:
    wsTemp = _T("MIPS little endian - R4000" );
    break;
  case 0x168:
    wsTemp = _T("R1000" );
    break;
  case 0x1A2:
    wsTemp = _T("Hitachi SH3" );
    break;
  case 0x1A6:
    wsTemp = _T("Hitachi SH4" );
    break;
  case 0x1C2:
    wsTemp = _T("Thumb" );
    break;
  default:
    wsTemp = _T("Unknown" );
    break;
  }
  return wsTemp;
}
#endif

// ------------------------------- eof ------------------------------
