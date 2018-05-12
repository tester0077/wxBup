//-------------------------------------------------------------------
// Name:        xxx .cpp
// Purpose:     {Enter}
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Standard wxWidgets headers
//-------------------------------------------------------------------
// this statement NEEDS to go BEFORE all headers
#define _CRTDBG_MAP_ALLOC

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

// ------------------------------------------------------------------
#if defined( __VISUALC__ )
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif
#include "exeHeadersh.h"
#include "exeHdrh.h"
// ------------------------------------------------------------------
// ------------------------------------------------------------------
// Names of the data directory elements that are defined
wxString wsImageDirectoryNames[] = 
{
  _T("EXPORT"),       _T("IMPORT"),     _T("RESOURCE"), 
  _T("EXCEPTION"),    _T("SECURITY"),   _T("BASERELOC"),
  _T("DEBUG"),        _T("COPYRIGHT"),  _T("GLOBALPTR"), 
  _T("TLS"),          _T("LOAD_CONFIG"),
  _T("BOUND_IMPORT"), _T("IAT"),    // These two entries added for NT 3.51
	_T("DELAY_IMPORT")                // This entry added in NT 5
};	
#define NUMBER_DIRECTORY_ENTRY_NAMES \
    (sizeof(wsImageDirectoryNames)/sizeof(wxString))
// ------------------------------------------------------------------

// ------------------------------------------------------------------

DisplayExeHdr::DisplayExeHdr( unsigned char *lpByte,
                             wxULongLong ullBfrSize ) 
{ 
  mp_lExeFileData = lpByte; 
  mp_ullBufferSize = ullBfrSize;
}
// ------------------------------------------------------------------
DisplayExeHdr::~DisplayExeHdr()
{
  mp_lExeFileData = NULL;
}
// ------------------------------------------------------------------
/**
 * does the file data represent a DOS EXE file - Magic = MZ
 */
bool DisplayExeHdr::HasDosHeader()
{
  return ( ( (*(USHORT *)mp_lExeFileData == DOS_IMAGE_MAGIC) ||
             (*(USHORT *)mp_lExeFileData == DOS_IMAGE_MAGIC2) )
    //&& (((DosExeHdr_t *)mp_lExeFileData)->usRelocOffs < 0x40 
    );
}
// ------------------------------------------------------------------
/**
 * Dos the file data point to an old style EXE Magic = ZM
 */

bool DisplayExeHdr::HasOldDosHeader()
{
  return ( (*(USHORT *)mp_lExeFileData == DOS_IMAGE_MAGIC2));
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
bool DisplayExeHdr::HasWinPeHeader()
{
  wxASSERT( HasDosHeader() );
  return  ( (*(USHORT *)mp_lExeFileData == DOS_IMAGE_MAGIC) &&
      (((DosExeHdr_t *)mp_lExeFileData)->usRelocOffs >= 0x40 ) &&
      // verify that the proposed PE header is within located
      // somewhere withing the file - see note above
      ( (mp_lExeFileData) < 
            (mp_lExeFileData + mp_ullBufferSize.ToULong()) ) &&
      (*(DWORD *)NTSIGNATURE (mp_lExeFileData) == IMAGE_NT_SIGNATURE) ); 
}

// ------------------------------------------------------------------
/**
 * Display EXE header data to a tree control
 */
bool  DisplayExeHdr::DisplayDosHeader( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId )
{
  wxString wsTemp;
  // DOS Header node ===============================================
//  wxTreeItemId parentId =
//      pTree->AppendItem( parentId, _T("DOS Header") );
  DosExeHdr_t *pDosHdr = (DosExeHdr_t *)mp_lExeFileData;

  // signature
  wsTemp.Printf( _T("%04XH - %c%c"), 
      pDosHdr->usDosHdrId, *mp_lExeFileData, *(mp_lExeFileData+1));
  wxTreeItemId dosHdrSigId =
      pTree->AppendItem( parentId,
      _T("File Signature          00: ") + wsTemp, -1, -1 );

  // bytes on last page
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usImagRmdr, pDosHdr->usImagRmdr );
  wxTreeItemId dosHdrLPId =
      pTree->AppendItem( parentId,
      _T("Bytes on last page      02: ") + wsTemp, -1, -1 );
   
  // # of 512 byte page (including header)
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usImagPages, pDosHdr->usImagPages );
  wxTreeItemId dosHdrNPId =
      pTree->AppendItem( parentId,
      _T("File Size -512 Byte pgs 04: ") 
      + wsTemp + _T(" (inc. header)"), -1, -1 );

  // # of reloc items
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usNRelocs,  pDosHdr->usNRelocs );
  wxTreeItemId dosHdrRelId =
      pTree->AppendItem( parentId,
      _T("Reloc Table Items       06: ") + wsTemp, -1, -1 );

  // header size in paragraphs
  wsTemp.Printf( _T("%04XH - %d"), 
      pDosHdr->usHdrSize, pDosHdr->usHdrSize );
  wxTreeItemId dosHdrHSId =
      pTree->AppendItem( parentId,
      _T("Header Size  (in para)  08: ") 
      + wsTemp, -1, -1 );

  // paragraphs needed above program
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usMinExtra );
  wxTreeItemId dosHdrPWId =
      pTree->AppendItem( parentId,
      _T("Extra para needed       0A: ") 
      + wsTemp, -1, -1 );

  // paragraphs wanted above program
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usMaxExtra );
  wxTreeItemId dosHdrPNId =
      pTree->AppendItem( parentId,
      _T("Extra para wanted       0C: ") 
      + wsTemp, -1, -1 );

  // stack displacement
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usSS );
  wxTreeItemId dosHdrSDId =
      pTree->AppendItem( parentId,
      _T("Stack Displacement      0E: ") + wsTemp, -1, -1 );

  // initial SP
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usSP );
  wxTreeItemId dosHdrISPId =
      pTree->AppendItem( parentId,
      _T("Initial SP              10: ") + wsTemp, -1, -1 );

  // checksum
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usChecksum );
  wxTreeItemId dosHdrCSId =
      pTree->AppendItem( parentId,
      _T("Checksum                12: ") + wsTemp, -1, -1 );

  // initial IP
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usIP );
  wxTreeItemId dosHdrIPId =
      pTree->AppendItem( parentId,
      _T("Initial IP              14: ") + wsTemp, -1, -1 );

  // Code displacement
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usCS );
  wxTreeItemId dosHdrCDId =
      pTree->AppendItem( parentId,
      _T("Initial SP              16: ") + wsTemp, -1, -1 );

  // first reloc item offset
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usRelocOffs );
  wxTreeItemId dosHdrFROId =
      pTree->AppendItem( parentId,
      _T("First Reloc Item Offset 18: ") + wsTemp, -1, -1 );

  // overlay number
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usOvrlyNmbr );
  wxTreeItemId dosHdrONId =
      pTree->AppendItem( parentId,
      _T("Overlay number          1A: ") + wsTemp, -1, -1 );

  // reserved space 1
  wxTreeItemId dosHdrR1Id =
      pTree->AppendItem( parentId,
      _T("<reserved space>        1C: "), -1, -1 );
  char s[5];
  s[0] = pDosHdr->ucRes1[0];
  s[1] = pDosHdr->ucRes1[1];
  s[2] = pDosHdr->ucRes1[2];
  s[3] = pDosHdr->ucRes1[3];
  s[4] = '\0'; 
  wsTemp.Printf( _T("%02X%02X%02X%02XH - %s"), 
      pDosHdr->ucRes1[0], pDosHdr->ucRes1[1], 
      pDosHdr->ucRes1[2], pDosHdr->ucRes1[3],
      wxString::FromAscii( s ) );
  wxTreeItemId dosHdrR1Id1 =
      pTree->AppendItem( dosHdrR1Id,
      _T("                       :   ") + wsTemp, -1, -1 );
  s[0] = pDosHdr->ucRes1[4];
  s[1] = pDosHdr->ucRes1[5];
  s[2] = pDosHdr->ucRes1[6];
  s[3] = pDosHdr->ucRes1[7];
  s[4] = '\0'; 

  wsTemp.Printf( _T("%02X%02X%02X%02XH - %s"), 
      pDosHdr->ucRes1[4], pDosHdr->ucRes1[5], 
      pDosHdr->ucRes1[6], pDosHdr->ucRes1[7],
      wxString::FromAscii( s ) );
  wxTreeItemId dosHdrR1Id2 =
      pTree->AppendItem( dosHdrR1Id,
      _T("                       :   ") + wsTemp, -1, -1 );
  // OEM identifier
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usOemId );
  wxTreeItemId dosHdrOIdId =
      pTree->AppendItem( parentId,
      _T("OEM Identifier          24: ") + wsTemp, -1, -1 );

  // OEM information
  wsTemp.Printf( _T("%04XH"), 
      pDosHdr->usOemInfo );
  wxTreeItemId dosHdrOInId =
      pTree->AppendItem( parentId,
      _T("OEM Information         26: ") + wsTemp, -1, -1 );

  // reserved space 2
  wxTreeItemId dosHdrR2Id =
      pTree->AppendItem( parentId,
      _T("<reserved space>        28: "), -1, -1 );
  wsTemp.Printf( _T("%04X%04X%04XH"), 
      pDosHdr->usRes2[0], pDosHdr->usRes2[1], 
      pDosHdr->usRes2[2], pDosHdr->usRes2[3],
      pDosHdr->usRes2[4], pDosHdr->usRes2[5]);
  wxTreeItemId dosHdrR2Id1 =
      pTree->AppendItem( dosHdrR2Id,
      _T("                       :   ") + wsTemp, -1, -1 );
  wsTemp.Printf( _T("%04X%04XH"), 
      pDosHdr->usRes2[6], pDosHdr->usRes2[7], 
      pDosHdr->usRes2[8], pDosHdr->usRes2[9] );
  wxTreeItemId dosHdrR2Id2 =
      pTree->AppendItem( dosHdrR2Id,
      _T("                       :   ") + wsTemp, -1, -1 );

  // offset of new header
  unsigned long ul = pDosHdr->lNewHdrOffs;
  wsTemp.Printf( _T("%08lXH"), ul );
    wxTreeItemId dosHdrNHId =
      pTree->AppendItem( parentId,
      _T("Offset of New Header    3C: ") + wsTemp, -1, -1 );

  return true;   
}
// ------------------------------------------------------------------
/**
 * list EXE header data, line by line to a file ptr
 */
bool  DisplayExeHdr::ListHeader()
{
 
  return true;
}

// ------------------------------------------------------------------
/**
 * Display EXE header data to a tree control
 */
bool  DisplayExeHdr::DisplayPeHeader( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId )
{
  wxString wsTemp;                
  wxString wsT;

  DosExeHdr_t *pDosHdr = (DosExeHdr_t *)mp_lExeFileData;

  newPeExeHdr_t *pPeHdr = 
    (newPeExeHdr_t *)(mp_lExeFileData + pDosHdr->lNewHdrOffs );
  wsT = GetMachine( pPeHdr->usMachine );
  // machine
  wsTemp.Printf( _T("%04XH - %s"), pPeHdr->usMachine, wsT );
  wxTreeItemId peHdrmachId =
    pTree->AppendItem( parentId,
    _T("Machine                   : ") + wsTemp, -1, -1 );

  // number of sections
  wsTemp.Printf( _T("%d"), pPeHdr->usNSections );
  wxTreeItemId peHdrNSnId =
    pTree->AppendItem( parentId,
    _T("Number of Sections        : ") + wsTemp, -1, -1 );

  // Time & Date stamp
  char *pszDateTime;
  pszDateTime = ctime( (const time_t *)&(pPeHdr->ulTimeDate) );
  wsTemp.Printf( _T("%08lXH - %s"), pPeHdr->ulTimeDate,
    wxString::FromAscii(pszDateTime) );
  wxTreeItemId peHdrDTId =
    pTree->AppendItem( parentId,
    _T("Time & Date Stamp         : ") + wsTemp, -1, -1 );

  // symbol table offset
  wsTemp.Printf( _T("%8lXH"), pPeHdr->ulSymTblOffset );
  wxTreeItemId peHdrSTOId =
    pTree->AppendItem( parentId,
    _T("Symbol Table Offset       : ") + wsTemp, -1, -1 );
  // symbol count
  wsTemp.Printf( _T("%8lXH"), pPeHdr->ulNSyms );
  wxTreeItemId peHdrNSId =
    pTree->AppendItem( parentId,
    _T("Symbol Count              : ") + wsTemp, -1, -1 );
  // optional header size
  wsTemp.Printf( _T("%4XH"), pPeHdr->usOptHdrSize );
  wxTreeItemId peHdrOHSId =
    pTree->AppendItem( parentId,
    _T("Optional Header Size      : ") + wsTemp, -1, -1 );

  // Flags
  wsTemp.Printf( _T("%04XH"), pPeHdr->usFlags );
  wxTreeItemId peHdrFlId =
    pTree->AppendItem( parentId,
    _T("Flags                     : ") + wsTemp, -1, -1 );
  int il;
  unsigned short usVal = pPeHdr->usFlags;
  for ( il = 0; il < 16 && usVal; il++, usVal=(usVal>>1) )
  {
    if( usVal&0x01 )
    {
      switch( il )
      { 
        case 0:
          wsT = _T("Relocs Stripped");
          break;
        case 1:
          wsT = _T("Executable Image");
          break;
        case 2:
          wsT = _T("Line#s Stripped");
          break;
        case 3:
          wsT = _T("Local Syms Stripped");
          break;
        case 4:
          wsT = _T("Aggressive WS Trim");
          break;
        case 5:
          wsT = _T("Large Address Aware (>2GB)");
          break;
        case 6:
          wsT = _T("16-Bit Machine - reserved");
          break;
        case 7:
          wsT = _T("Little Endian");
          break;
        case 8:
          wsT = _T("32-Bit Machine");
          break;
        case 9:
          wsT = _T("Debug Stripped");
          break;
        case 10:
          wsT = _T("Run from Swap");
          break;
        case 11:
          wsT = _T("Image is File System");
          break;
        case 12:
          wsT = _T("DLL");
          break;
        case 13:
          wsT = _T("For UP Machine only");
          break;
        case 14:
          wsT = _T("Big Endian");
          break;
        case 15:
          wsT = _T("bit 15 -- ??");
          break;
      }
      wxTreeItemId peHdrFlags =
        pTree->AppendItem( peHdrFlId, wsT, -1, -1 );
    }
  }
  return true;   
}
#if 1
// ------------------------------------------------------------------

wxString DisplayExeHdr::GetMachine( unsigned short type )
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
// ------------------------------------------------------------------
bool DisplayExeHdr::DisplayPeOptlHeader( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId )
{
  wxString wsTemp;
  // handle the optional header
  newPeExeHdr_t *pPeHdr = GetPeHdrPtr();
  newPeOptHdr_t *pPeOptHdr = GetPeOptlHdrPtr();
  if ( pPeHdr->usOptHdrSize )
  {
    wxString wsT;
    // optional PE Header node ================
    wxTreeItemId neOptHeaderId =
      pTree->AppendItem( parentId,
        _T("Optional PE Header"), -1, -1 );
    // Flags
    wsTemp.Printf( _T("0x%04X"), pPeOptHdr->usMagic );
    switch( pPeOptHdr->usMagic ) 
    {
    case 0x107:
      wsT = _T("ROM Image" );
      break;
    case 0x10b:
      wsT = _T("PE32");
      break;
    case 0x20b:
      wsT = _T("PE32+");
      break;
    default:
      wsT = _T("");
    }
    wxTreeItemId peOptHeaderMNId =
      pTree->AppendItem( neOptHeaderId,
      _T("Magic Number                : ") + wsTemp + _T(" - ") + wsT,
      -1, -1 );
    // Linker version and revision
    wsTemp.Printf( _T("%d.%d"), pPeOptHdr->ucLinkerMajor,
      pPeOptHdr->ucLinkerMinor);
    wxTreeItemId peOptHeaderLkrId =
      pTree->AppendItem( neOptHeaderId,
      _T("Linker                      : ") + wsTemp, -1, -1 );
    // size of code
    wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulSizeOfCode );
    wxTreeItemId peOptHeaderCSId =
      pTree->AppendItem( neOptHeaderId,
      _T("Size of Code                : ") + wsTemp, -1, -1 );
    // size of initialized data
    wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulSizeOfInitializedData );
    wxTreeItemId peOptHeaderIDId =
      pTree->AppendItem( neOptHeaderId,
      _T("Size of Initialized Data    : ") + wsTemp, -1, -1 );
    // size of uninitialized data
    wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulSizeOfUninitializedData );
    wxTreeItemId peOptHeaderUIDId =
      pTree->AppendItem( neOptHeaderId,
      _T("Size of Uninitialized data  : ") + wsTemp, -1, -1 );
    // entry point address
    wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulAddressEntryPoint );
    wxTreeItemId peOptHeaderIPId =
      pTree->AppendItem( neOptHeaderId,
      _T("Entry  Point (RVA)          : ") + wsTemp, -1, -1 );
    // base of code
    wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulBaseOfCode );
    wxTreeItemId peOptHeaderCBId =
      pTree->AppendItem( neOptHeaderId,
      _T("Code Base Address           : ") + wsTemp, -1, -1 );
  
    // if PE32 - base address of data & more
    if( pPeOptHdr->usMagic == 0x10b )
    {
      // data base address
      wsTemp.Printf( _T("%8lXH"), pPeOptHdr->ulBaseOfData );
      wxTreeItemId peOptHeaderDBId =
        pTree->AppendItem( neOptHeaderId,
        _T("Data Base Address           : ") + wsTemp, -1, -1 );
      //=====================================================
      // also handle the rest of the PE32 optional header
      newPeOptNtHdr_t *pPeNtOptHdr =  (
        newPeOptNtHdr_t *)((unsigned long)pPeOptHdr + sizeof(newPeOptHdr_t) );
      // image base address
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulImageBase );
      wxTreeItemId peOptNtHeaderIBId =
        pTree->AppendItem( neOptHeaderId,
        _T("Image Base Address          : ") + wsTemp, -1, -1 );      
      // section alignment
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSectionAlignment );
      wxTreeItemId peOptNtHeaderAlId =
        pTree->AppendItem( neOptHeaderId,
        _T("Section Alignment           : ") + wsTemp, -1, -1 ); 
      // file alignment
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulFileAlignment );
      wxTreeItemId peOptNtHeaderFAId =
        pTree->AppendItem( neOptHeaderId,
      _T("File Alignment              : ") + wsTemp, -1, -1 );
      // OS version
      wsTemp.Printf( _T("%d.%d"), pPeNtOptHdr->usMajorOsVersion,
        pPeNtOptHdr->usMinorOSVersion );
      wxTreeItemId peOptNtHeaderOSId =
        pTree->AppendItem( neOptHeaderId,
        _T("OS Version                  : ") + wsTemp, -1, -1 );
      // Image version
      wsTemp.Printf( _T("%d.%d"), pPeNtOptHdr->usMajorImageVersion,
        pPeNtOptHdr->usMinorImageVersion );
      wxTreeItemId peOptNtHeaderImId =
        pTree->AppendItem( neOptHeaderId,
        _T("Image version               : ") + wsTemp, -1, -1 );
      // Subsys version
      wsTemp.Printf( _T("%d.%d"), pPeNtOptHdr->usMajorSubSysVersion,
        pPeNtOptHdr->usMinorSubSysVersion );
      wxTreeItemId peOptNtHeaderSSId =
        pTree->AppendItem( neOptHeaderId,
        _T("Subsys version              : ") + wsTemp, -1, -1 );
      // reserved
      wsT = (pPeNtOptHdr->ulReserved) ? 
        _T(" - !!??!! not zero") : _T("");
      wsTemp.Printf( _T("0x%08lX"), pPeNtOptHdr->ulReserved );
      wxTreeItemId peOptNtHeaderRsdId =
        pTree->AppendItem( neOptHeaderId,
        _T("<reserved. (must be zero)   : ") + wsTemp + wsT, -1, -1 );
      // size of image
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfImage );
      wxTreeItemId peOptNtHeaderISId =
        pTree->AppendItem( neOptHeaderId,
      _T("Size of Image               : ") + wsTemp, -1, -1 );
      // size of header
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfHeaders );
      wxTreeItemId peOptNtHeaderHSId =
        pTree->AppendItem( neOptHeaderId,
        _T("Size of Headers             : ") + wsTemp, -1, -1 );
      // checksum
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulChecksum );
      wxTreeItemId peOptNtHeaderCSId =
        pTree->AppendItem( neOptHeaderId,
        _T("Checksum                    : ") + wsTemp, -1, -1 );
      // subsystem
      switch( pPeNtOptHdr->usSubSysReq )
      {
      case 0:
        wsT = _T("unknown");
        break;
      case 1:
        wsT = _T("native");
        break;
      case 2:
        wsT = _T("Windows GUI");
        break;
      case 3:
        wsT = _T("Windows CU");
        break;
      case 7:
        wsT = _T("Posix CU");
        break;
      case 9:
        wsT = _T("Windows CE");
        break;
      case 10:
        wsT = _T("Extensible Firmware (EFI)");
        break;
      case 11:
        wsT = _T("Extensible Firmware Driver with Boot services");
        break;
      case 13:
        wsT = _T("Extensible Firmware ROM Immage");
        break;
      case 14:
        wsT = _T("XBox");
        break;
      default:
        wsT = _T("");
        break;
      }
      wsTemp.Printf( _T("%4XH"), pPeNtOptHdr->usSubSysReq );
      wxTreeItemId peOptNtHeaderCSRId =
        pTree->AppendItem( neOptHeaderId,
        _T("Subsystem Required          : ") + wsTemp + _T(" - ") + wsT, -1, -1 );

      // DLL characteristics
      switch( pPeNtOptHdr->usDLLCharacteristics )
      {
      case 0x0:
      case 0x2:
      case 0x4:
      case 0x8:
      case 0x1000:
        wsT = _T("reserved");
      case 0x40:
        wsT = _T("DLL relocatable at load time");
        break;
      case 0x80:
        wsT = _T("Code Integrity Checks are enforced");
        break;
      case 0x100:
        wsT = _T("Image is NX compoatible");
        break;
      case 0x200:
        wsT = _T("Isolation aware - do not isolate");
        break;
      case 0x400:
        wsT = _T("No Structured Exception Handler used");
        break;
      case 0x800:
        wsT = _T("Do not bind the image");
        break;
      case 0x2000:
        wsT = _T("WDM Driver");
        break;
      case 0x8000:
        wsT = _T("Terminal Server Aware");
      default:
        wsT = _T("");
        break;
      }
      wsTemp.Printf( _T("%4XH"), pPeNtOptHdr->usDLLCharacteristics );
      wxTreeItemId peOptNtHeaderDLLId =
        pTree->AppendItem( neOptHeaderId,
        _T("DLL Characteristics         : ") + wsTemp + _T(" - ") + wsT, -1, -1 );
      // reserved stack space
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfStackReserve );
      wxTreeItemId peOptNtHeaderRSMId =
        pTree->AppendItem( neOptHeaderId,
        _T("Reserved Stack Memory       : ") + wsTemp, -1, -1 );
      // committed stack space
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfStackCommit );
      wxTreeItemId peOptNtHeaderCSMId =
        pTree->AppendItem( neOptHeaderId,
        _T("Committed Stack Memory      : ") + wsTemp, -1, -1 );

      // reserved heap space
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfHeapReserve );
      wxTreeItemId peOptNtHeaderRHMId =
        pTree->AppendItem( neOptHeaderId,
        _T("Reserved Heap Memory        : ") + wsTemp, -1, -1 );
      // committed heap space
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulSizeOfHeapCommit );
      wxTreeItemId peOptNtHeaderCHMId =
        pTree->AppendItem( neOptHeaderId,
        _T("Committed Heap Memory       : ") + wsTemp, -1, -1 );

      // loader flags
      wsT = (pPeNtOptHdr->ulLoaderFlags) ? 
        _T(" - !!??!! not zero") : _T("");
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulLoaderFlags );
      wxTreeItemId peOptNtHeaderLFId =
        pTree->AppendItem( neOptHeaderId,
        _T("Loader Flags                : ") + wsTemp + wsT, -1, -1 );
      // # of RVA & sizes
      wsTemp.Printf( _T("%8lXH"), pPeNtOptHdr->ulNumberOfRvaAndSizes );
      wxTreeItemId peOptNtHeaderRVAId =
        pTree->AppendItem( neOptHeaderId,
        _T("# of RVA & Sizes: ") + wsTemp, -1, -1 );
      
      //--------------------------------------------------------------
      // following the 'newPeOptNtHdr' we have an array of 16
      // IMAGE_DIRECTORY_ENTRY_xxx - not all of which are used
      // as defined in WINNT.H??
      wxTreeItemId neOptDataDirId =
        pTree->AppendItem( parentId,
        _T("Data Directory"), -1, -1 );
      for( unsigned long i = 0; i < pPeNtOptHdr->ulNumberOfRvaAndSizes; i++ )
      {
        // be sure not to access any elements beyond the defined ones!!!!!!!
        wxString wsT;
        
        if( i >= NUMBER_DIRECTORY_ENTRY_NAMES )
          wsT = _T("unused");
        else
          wsT = wsImageDirectoryNames[i];
        wsTemp.Printf( _T("%-12s  RVA: %08XH  Size: %08XH"), wsT,
          pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
          pPeNtOptHdr->dataDirs[i].ulSize );
        wxTreeItemId dataDirId =
          pTree->AppendItem( neOptDataDirId, wsTemp, -1, -1 );
      }
      // ------------------------------------------------------------------
#if 0
      // display the sections
      displayPeSections( parentId, 
        (unsigned long)(m_pbDisFile + ulOffset +
        sizeof(newPeExeHdr_t) + sizeof(newPeOptHdr_t) ), 
        pPeHdr->usNSections );
#endif
    }
  }
  return true;
}
// ------------------------------------------------------------------

void DisplayExeHdr::displayPeSections( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId ) 
{
  wxString wsT;
  wxString wsFlagString;
  wxTreeItemId peSectionId =
      pTree->AppendItem( parentId,
        _T("PE Sections"), -1, -1 );
  newPeSectionHdr_t *psh =  GetSectionDrPtr();
  int nSections = GetPeHdrPtr()->usNSections;
  int i;
  wxTreeItemId pSectionId;
  wxTreeItemId pId;
  if (psh != NULL)
	{
	  // find the section by name 
	  for (i=0; i < nSections; i++)
	  {
      char pszN[10];
      memset( pszN, 9, '\0' );
      strncpy( pszN, (const char *)psh->name, 8 ) ;
      wsT.Printf( _T("%10s     Flags: 0x%08lX"), wxString::FromAscii(pszN) );
      wsFlagString = getFlagString( psh->ulFlags );
      pId =
        pTree->AppendItem( peSectionId,
        _T("Section: ") + wsT, -1, -1 );
      pTree->AppendItem( pId,
        _T("  ") + wsFlagString, -1, -1 );
      wsT.Printf( _T("Virt Size        : 0x%08lX at 0x%08lX"),
        psh->ulVirtSize, psh->ulVirtAddr );
      pTree->AppendItem( pId,
        _T("  ") + wsT, -1, -1 );
      // raw
      wsT.Printf( _T("Raw Data Size    : 0x%08lX at Offset: 0x%08lX"),
        psh->ulRawSize, psh->ulRawOffset );
      pTree->AppendItem( pId,
        _T("  ") + wsT, -1, -1 );
      // reloc
      wsT.Printf( _T("# of Relocs      : 0x%08lX at Offset: 0x%08lX"),
        psh->ulNRelocs, psh->ulRelocOffset );
      pTree->AppendItem( pId,
        _T("  ") + wsT, -1, -1 );
      // line #
      wsT.Printf( _T("Number of Line #s: 0x%08lX"),
        psh->ulNLineNo );
      pTree->AppendItem( pId,
        _T("  ") + wsT, -1, -1 );
		  psh++;
	  }
	}
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
wxString DisplayExeHdr::getFlagString( unsigned long uslFlags )
{
  wxString wsSep = _T(", ");
  wxString wsRes = _T("");
  if( uslFlags & 0x08 )
    wsRes += _T("DontPadd, ");
  if( uslFlags & 0x020 )
  {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("EXE, ");
  }
  if( uslFlags & 0x040 )
  {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("InitData ,");
  }
  if( uslFlags & 0x040 )
  {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("UninitData, ");
  }
  if( uslFlags & 0x0200 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("Comments, ");
  }
  if( uslFlags & 0x01000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("COMDAT, ");
  }
  if( uslFlags & 0x01000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("ExtdRelocs, ");
  }
  if( uslFlags & 0x02000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("Discardable, ");
  }
  if( uslFlags & 0x04000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("DontCache, ");
  }
  if( uslFlags & 0x08000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("DontPage, ");
  }
  if( uslFlags & 0x010000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("MemShare, ");
  }
  if( uslFlags & 0x020000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("ExecCode, ");
  }
  if( uslFlags & 0x040000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("MemRead, ");
  }
  if( uslFlags & 0x080000000 ) {
    //if( wsRes.IsEmpty() ) wsRes += wsSep;
    wsRes += _T("MemWrite, ");
  }

  if( uslFlags & 0x0F00000 )
  {
    wxString wsAl;
    unsigned i = (uslFlags & 0x0F00000)>>6;
    wsAl.Printf( _T(", Align: %d"), 2<<i );
    wsRes += wsAl;
  }
  return _T("  ") + wsRes;
}

// ------------------------------- eof ------------------------------
