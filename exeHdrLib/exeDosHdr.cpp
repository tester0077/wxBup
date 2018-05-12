//-------------------------------------------------------------------
// Name:        exeDosHdr.cpp
// Purpose:     DOS Header Class
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

DosExeHdr::DosExeHdr( unsigned char *lpByte,
                             unsigned long long ullBfrSize ) 
{ 
  mp_lExeFileData = lpByte; 
  mp_ullBufferSize = ullBfrSize;
}
// ------------------------------------------------------------------
DosExeHdr::~DosExeHdr()
{
  mp_lExeFileData = NULL;
  mp_ullBufferSize = 0;
}
// ------------------------------------------------------------------
/**
 * does the file data represent a DOS EXE file - Magic = MZ
 * or Magic = ZM.
 * use the mapped file pointer directly so we can test before 
 * we need to decide on file type.
 * Also note: here we are only concerned with DOS files.
 */
bool DosExeHdr::HasDosHeader( BYTE *m_pbDisFile )
{
  return ( ( (*(USHORT *)m_pbDisFile == DOS_IMAGE_MAGIC) ||
             (*(USHORT *)m_pbDisFile == DOS_IMAGE_MAGIC2) )
    );
}
// ------------------------------------------------------------------
/**
 * Does the file data point to an old style EXE Magic = ZM
 */

bool DosExeHdr::HasOldDosHeader( BYTE *m_pbDisFile )
{
  return ( (*(USHORT *)m_pbDisFile == DOS_IMAGE_MAGIC2));
}

// ------------------------------------------------------------------
/**
 * Display EXE header data to a tree control.
 * Also check for special DOS files, such as packed with the various 
 * packer/zippers.
 * Returns true if this represents DOS only header.
 *         false otherwise
 */
bool  DosExeHdr::DisplayDosHeader( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId )
{
  wxString  wsTemp;
  wxString  wsT;
  char      s[10];
  char      c1, c2;

  bool   bIsMSDOSHeader = true;   // false for any new style header
  // DOS Header node ===============================================
  DosExeHdr_t *pDosHdr = (DosExeHdr_t *)mp_lExeFileData;

  // signature
  wsTemp.Printf( _T("%04XH - %c%c"), 
      pDosHdr->usDosHdrId, *mp_lExeFileData, *(mp_lExeFileData+1));
  wxTreeItemId dosHdrSigId =
      pTree->AppendItem( parentId,
      _T("File Signature           00: ") + wsTemp, -1, -1 );

  // bytes on last page
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usImagRmdr, pDosHdr->usImagRmdr );
  wxTreeItemId dosHdrLPId =
      pTree->AppendItem( parentId,
      _T("Bytes on last page       02: ") + wsTemp, -1, -1 );
   
  // # of 512 byte page (including header)
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usImagPages, pDosHdr->usImagPages );
  wxTreeItemId dosHdrNPId =
      pTree->AppendItem( parentId,
      _T("File Size - 512 Byte pgs 04: ") 
      + wsTemp + _T(" (inc. header)"), -1, -1 );
  wsTemp.Printf( _T("%012XH - %d"), 
      (pDosHdr->usImagPages-1)*512 + pDosHdr->usImagRmdr, 
      (pDosHdr->usImagPages-1)*512 + pDosHdr->usImagRmdr);
  wxTreeItemId dosHdrNPId1 =
      pTree->AppendItem( dosHdrNPId,
      _T("Total File Size           ") + wsTemp 
      + _T(" Bytes"), -1, -1 );
  // # of reloc items
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usNRelocs,  pDosHdr->usNRelocs );
  wxTreeItemId dosHdrRelId =
      pTree->AppendItem( parentId,
      _T("Reloc Table Items        06: ") + wsTemp, -1, -1 );

  // header size in paragraphs
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usHdrSize, pDosHdr->usHdrSize );
  wxTreeItemId dosHdrHSId =
      pTree->AppendItem( parentId,
      _T("Header Size  (in para)   08: ") 
      + wsTemp, -1, -1 );

  // paragraphs needed above program
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usMinExtra );
  wxTreeItemId dosHdrPWId =
      pTree->AppendItem( parentId,
      _T("Extra para needed        0A: ") 
      + wsTemp, -1, -1 );

  // paragraphs wanted above program
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usMaxExtra );
  wxTreeItemId dosHdrPNId =
      pTree->AppendItem( parentId,
      _T("Extra para wanted        0C: ") 
      + wsTemp, -1, -1 );

  // stack displacement
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usSS );
  wxTreeItemId dosHdrSDId =
      pTree->AppendItem( parentId,
      _T("Initial SS               0E: ") + wsTemp, -1, -1 );

  // initial SP
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usSP );
  wxTreeItemId dosHdrISPId =
      pTree->AppendItem( parentId,
      _T("Initial SP               10: ") + wsTemp, -1, -1 );

  // checksum
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usChecksum );
  wxTreeItemId dosHdrCSId =
      pTree->AppendItem( parentId,
      _T("Checksum (DOS)           12: ") + wsTemp, -1, -1 );

  // initial IP
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usIP );
  wxTreeItemId dosHdrIPId =
      pTree->AppendItem( parentId,
      _T("Initial IP               14: ") + wsTemp, -1, -1 );

  // Code displacement
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usCS );
  wxTreeItemId dosHdrCDId =
      pTree->AppendItem( parentId,
      _T("Initial CS               16: ") + wsTemp, -1, -1 );
  // need to verify whether or not the header is
  // for a MS-DOS .EXE or a new type, either PE, NE ...
  // but ONLY if the new header offset is reasonable & lies within the file image
  // else it is likely some packed DOS executable
  wxString wsT1 = _T(" - MS-DOS Header");
  unsigned long ulpNewHdrOffset = (long ) NTSIGNATURE ( mp_lExeFileData );
  if( (ulpNewHdrOffset - (unsigned long )mp_lExeFileData) < mp_ullBufferSize )
  {
    if( (pDosHdr->usNRelocs == 0 ) && (pDosHdr->ulNewHdrOffs) != 0x40 )
    {
      bIsMSDOSHeader = false;
      wsT1 = _T(" - New Style DOS Header");
    }
  }
  // else we have to test for the various possibilities
  // in fact, for PKLite it seems the 'offset to first reloc' item 
  // is actually part of the PKLite code??   
  // for f:\dt\pklite\FT.exe => 0x50 == start of what looks like code
//  else
  {
    // first reloc item offset
    wsTemp.Printf( _T("%04XH"), 
        pDosHdr->usRelocOffs );
    wxTreeItemId dosHdrFROId =
        pTree->AppendItem( parentId,
        _T("First Reloc Item Offset  18: ") + wsTemp + wsT1, -1, -1 );

    // overlay number
    wsTemp.Printf( _T("%04XH"), 
        pDosHdr->usOvrlyNmbr );
    wxTreeItemId dosHdrONId =
        pTree->AppendItem( parentId,
        _T("Overlay number           1A: ") + wsTemp, -1, -1 );

    // need to verify that the header is long enough to contain
    // reasonable data
    if( (void *)((long)pDosHdr + (USHORT)(pDosHdr->usHdrSize)*0x10) 
      >= (void *)&(pDosHdr->ucRes1[7]) )
    {
      wxString wsT2 = wxEmptyString;
      if( bIsMSDOSHeader )
      {
        if( (pDosHdr->ucRes1[2]) == 0xfb )
        {
          wsT2.Printf( _T("TLink ver: %d.%d"),
            ((pDosHdr->ucRes1[3])& 0xf0) >>4,
             (pDosHdr->ucRes1[3] & 0x0f)     );
        }
      }
      // reserved space 1   
      // -- may contain non-ASCII data - i.e. non-displayable data
      wxTreeItemId dosHdrR1Id =
          pTree->AppendItem( parentId,
          _T("<reserved space 1>       1C: ") + wsT2, -1, -1 );
      s[0] = iswascii( pDosHdr->ucRes1[0] ) ? pDosHdr->ucRes1[0] : '.';
      s[1] = iswascii( pDosHdr->ucRes1[1] ) ? pDosHdr->ucRes1[1] : '.';
      s[2] = iswascii( pDosHdr->ucRes1[2] ) ? pDosHdr->ucRes1[2] : '.';
      s[3] = iswascii( pDosHdr->ucRes1[3] ) ? pDosHdr->ucRes1[3] : '.';
      s[4] = '\0'; 
      wsTemp.Printf( _T("%02X%02X%02X%02XH - %s" ), 
          pDosHdr->ucRes1[0], pDosHdr->ucRes1[1], 
          pDosHdr->ucRes1[2], pDosHdr->ucRes1[3], wxString::FromAscii( s ) );
      wxTreeItemId dosHdrR1Id1 =
          pTree->AppendItem( dosHdrR1Id,
          _T("                        :   ") + wsTemp, -1, -1 );
      s[0] = iswascii( pDosHdr->ucRes1[4] ) ? pDosHdr->ucRes1[4] : '.';
      s[1] = iswascii( pDosHdr->ucRes1[5] ) ? pDosHdr->ucRes1[5] : '.';
      s[2] = iswascii( pDosHdr->ucRes1[6] ) ? pDosHdr->ucRes1[6] : '.';
      s[3] = iswascii( pDosHdr->ucRes1[7] ) ? pDosHdr->ucRes1[7] : '.';
      s[4] = '\0'; 
      wsTemp.Printf( _T("%02X%02X%02X%02XH - %s"), 
          pDosHdr->ucRes1[4], pDosHdr->ucRes1[5], 
          pDosHdr->ucRes1[6], pDosHdr->ucRes1[7], wxString::FromAscii( s ) );
      wxTreeItemId dosHdrR1Id2 =
          pTree->AppendItem( dosHdrR1Id,
          _T("                        :   ") + wsTemp, -1, -1 );
    }
    // need to verify that the header is long enough to contain
    // reasonable data
    if( (void *)((long)pDosHdr + (USHORT)(pDosHdr->usHdrSize)*0x10) 
      >= (void *)&(pDosHdr->usOemId) )
    {
      // OEM identifier
      c1 = (pDosHdr->usOemId) & 0xFF;
      c2 = (pDosHdr->usOemId) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      s[2] = '\0';
      wsTemp.Printf( _T("%04XH - %s"), 
          pDosHdr->usOemId, wxString::FromAscii( s ) );
      wxTreeItemId dosHdrOIdId =
          pTree->AppendItem( parentId,
          _T("OEM Identifier           24: ") + wsTemp, -1, -1 );

      // OEM information
      c1 = (pDosHdr->usOemInfo) & 0xFF;
      c2 = (pDosHdr->usOemInfo) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      s[2] = '\0';
      wsTemp.Printf( _T("%04XH - %s"), 
          pDosHdr->usOemInfo, wxString::FromAscii( s ) );
      wxTreeItemId dosHdrOInId =
          pTree->AppendItem( parentId,
          _T("OEM Information          26: ") + wsTemp, -1, -1 );
    }
    // need to verify that the header is long enough to contain
    // reasonable data
    if( (void *)((long)pDosHdr + (USHORT)(pDosHdr->usHdrSize)*0x10) 
      >= (void *)(pDosHdr->usRes2) )
    {
      // reserved space 2
      wxTreeItemId dosHdrR2Id =
          pTree->AppendItem( parentId,
          _T("<reserved space 2>       28: "), -1, -1 );
      c1 = (pDosHdr->usRes2[0]) & 0xFF;
      c2 = (pDosHdr->usRes2[0]) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[1]) & 0xFF;
      c2 = (pDosHdr->usRes2[1]) >> 8;
      s[2] = iswascii( (c1 ) ) ? c1 : '.';
      s[3] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[2]) & 0xFF;
      c2 = (pDosHdr->usRes2[2]) >> 8;
      s[4] = iswascii( (c1 ) ) ? c1 : '.';
      s[5] = iswascii( (c2 ) ) ? c2 : '.';
      s[6] = '\0';
      wsT = wxString::FromAscii( s );
      wsTemp.Printf( _T("%04X%04X%04XH - %s"), 
          pDosHdr->usRes2[0], pDosHdr->usRes2[1], pDosHdr->usRes2[2], wsT );
      wxTreeItemId dosHdrR2Id1 =
          pTree->AppendItem( dosHdrR2Id,
          _T("                        :   ") + wsTemp, -1, -1 );
      c1 = (pDosHdr->usRes2[3]) & 0xFF;
      c2 = (pDosHdr->usRes2[3]) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[4]) & 0xFF;
      c2 = (pDosHdr->usRes2[4]) >> 8;
      s[2] = iswascii( (c1 ) ) ? c1 : '.';
      s[3] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[5]) & 0xFF;
      c2 = (pDosHdr->usRes2[5]) >> 8;
      s[4] = iswascii( (c1 ) ) ? c1 : '.';
      s[5] = iswascii( (c2 ) ) ? c2 : '.';
      s[6] = '\0';
      wsT = wxString::FromAscii( s );
      wsTemp.Printf( _T("%04X%04X%04XH - %s"), 
          pDosHdr->usRes2[3], pDosHdr->usRes2[4], pDosHdr->usRes2[5],
          wsT );
      wxTreeItemId dosHdrR2Id2 =
          pTree->AppendItem( dosHdrR2Id,
          _T("                        :   ") + wsTemp, -1, -1 );
      c1 = (pDosHdr->usRes2[6]) & 0xFF;
      c2 = (pDosHdr->usRes2[6]) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[7]) & 0xFF;
      c2 = (pDosHdr->usRes2[7]) >> 8;
      s[2] = iswascii( (c1 ) ) ? c1 : '.';
      s[3] = iswascii( (c2 ) ) ? c2 : '.';
      c1 = (pDosHdr->usRes2[8]) & 0xFF;
      c2 = (pDosHdr->usRes2[8]) >> 8;
      s[4] = iswascii( (c1 ) ) ? c1 : '.';
      s[5] = iswascii( (c2 ) ) ? c2 : '.';
      s[6] = '\0';
      wsT = wxString::FromAscii( s );
      wsTemp.Printf( _T("%04X%04XH%04X - %s"), 
          pDosHdr->usRes2[6], pDosHdr->usRes2[7], 
          pDosHdr->usRes2[8], wsT );
      wxTreeItemId dosHdrR2Id3 =
          pTree->AppendItem( dosHdrR2Id,
          _T("                        :   ") + wsTemp, -1, -1 );
      c1 = (pDosHdr->usRes2[9]) & 0xFF;
      c2 = (pDosHdr->usRes2[9]) >> 8;
      s[0] = iswascii( (c1 ) ) ? c1 : '.';
      s[1] = iswascii( (c2 ) ) ? c2 : '.';
      s[2] = '\0';
      wsT = wxString::FromAscii( s );
      wsTemp.Printf( _T("%04X - %s"), 
          pDosHdr->usRes2[9], wsT );
      wxTreeItemId dosHdrR2Id34 =
          pTree->AppendItem( dosHdrR2Id,
          _T("                        :   ") + wsTemp, -1, -1 );
    }
    // need to verify that the header is long enough to contain
    // reasonable data
    if( (void *)((long)pDosHdr + (USHORT)(pDosHdr->usHdrSize)*0x10) 
      >= (void *)&(pDosHdr->ulNewHdrOffs) )
    {
      // offset of new header
      unsigned long ul = pDosHdr->ulNewHdrOffs;
      wsTemp.Printf( _T("%08lXH"), ul );
        wxTreeItemId dosHdrNHId =
          pTree->AppendItem( parentId,
          _T("Offset of New Header     3C: ") + wsTemp, -1, -1 );
    }
  }
  return bIsMSDOSHeader;   
}
// ------------------------------------------------------------------
/**
 * list EXE header data, line by line to a file ptr
 */
bool  DosExeHdr::ListHeader()
{
 
  return true;
}

// ------------------------------- eof ------------------------------
