//-------------------------------------------------------------------
// Name:        exePeHdr.cpp
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
#include <wx/dynarray.h>

#include "exeHeadersh.h"
#include "exeDosHdrh.h"
#include "exePeHdrh.h"

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

PeExeHdr::PeExeHdr( unsigned char *lpByte,
                             unsigned long long ullBfrSize ) 
{ 
  mp_lExeFileData = lpByte; 
  mp_ullBufferSize = ullBfrSize;
}
// ------------------------------------------------------------------
PeExeHdr::~PeExeHdr()
{
  mp_lExeFileData = NULL;
  mp_ullBufferSize = 0;
}

// ------------------------------------------------------------------
/**
 * Determine if the file as a PE style header.
 * Note: this fails if we have a PKlite compressed file
 * because these apps mess up the DOS header and may 
 * leave the value of nRelocs (at offset 0x18) with a 
 * messed up value.
 * aPack shrinks the header to 32 bytes.
 * This means the pointer we end up using may well 
 * enough point beyond the valid buffer we have allocated
 * and thus would cause a crash if we did not check for this case..
 * \param [in] - pointer to memory mapped file data
 * \param [in] - size of allocated buffer 
 */
bool PeExeHdr::HasWinPeHeader( BYTE *lpByte,    
                  unsigned long long   ulFileSize ) 
{
  DosExeHdr pm_pDOSExeHdr( lpByte, ulFileSize );
  wxASSERT( pm_pDOSExeHdr.IsDosImage( lpByte ) );
  // verify that the proposed PE header is located
  // somewhere within the file - see note above
  unsigned long ulpNewHdrOffset = (long ) NTSIGNATURE (lpByte);
  if( (ulpNewHdrOffset -(unsigned long)lpByte) > ulFileSize )
    return false;
  return  ( (*(USHORT *)lpByte == DOS_IMAGE_MAGIC) &&
      (((DosExeHdr_t *)lpByte)->usRelocOffs >= 0x40 ) &&
      // verify that the file header is at least as long as we expect
      //- see note above
      ( pm_pDOSExeHdr.DosHdrSize( lpByte ) 
            >= pm_pDOSExeHdr.DosHdrStructSize() ) &&
      (*(DWORD *)NTSIGNATURE (lpByte) == IMAGE_NT_SIGNATURE) ); 
}

// ------------------------------------------------------------------
/**
 * list EXE header data, line by line to a file ptr
 */
bool PeExeHdr::ListHeader()
{
  return true;
}

// ------------------------------------------------------------------
/**
 * Display EXE header data to a tree control
 */
bool PeExeHdr::DisplayPeHeader( wxTreeCtrl *pTree,
                                   wxTreeItemId &parentId )
{
  wxString wsTemp;                
  wxString wsT;

  DosExeHdr_t *pDosHdr = (DosExeHdr_t *)mp_lExeFileData;

  newPeExeHdr_t *pPeHdr = 
    (newPeExeHdr_t *)(mp_lExeFileData + pDosHdr->ulNewHdrOffs );
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
// ------------------------------------------------------------------
bool PeExeHdr::DisplayPeOptlHeader( wxTreeCtrl *pTree,
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
      _T("Entry Point (RVA)           : ") + wsTemp, -1, -1 );
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
        _T("Checksum (PE)               : ") + wsTemp, -1, -1 );
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
        _T("# of RVA & Sizes            : ") + wsTemp, -1, -1 );
      
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
        wxString wsT1;
        
        if( i >= NUMBER_DIRECTORY_ENTRY_NAMES )
          wsT1.Printf( _T("unused %d"), i );
        else
          wsT1 = wsImageDirectoryNames[i];
        wsTemp.Printf( _T("%-12s RVA: %08XH  Size: %08XH"), wsT1,
          pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
          pPeNtOptHdr->dataDirs[i].ulSize );
        wxTreeItemId dataDirId =
          pTree->AppendItem( neOptDataDirId, wsTemp, -1, -1 );
      }
    }
  }
  return true;
}
// ------------------------------------------------------------------

void PeExeHdr::DisplayPeSections( wxTreeCtrl *pTree,
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
	  for (i=0; i < nSections; i++, psh++ )
	  {
      char pszN[10];
      memset( pszN, 9, '\0' );
      strncpy( pszN, (const char *)psh->name, 8 );
      wsFlagString = getFlagString( psh->ulFlags );
      wsT.Printf( _T("%10s   : "), wxString::FromAscii(pszN) );
      pId =
        pTree->AppendItem( peSectionId,
        _T("Section: ") + wsT + wsFlagString, -1, -1 );
      wsT.Printf( _T("Flags            : 0x%08lX "), psh->ulFlags );
      pTree->AppendItem( pId,
        _T("  ") + wsT + wsFlagString, -1, -1 );
      wsT.Printf( _T("Virt Size        : 0x%08lX at 0x%08lX"),
        psh->ulVirtSize, psh->ulVirtAddr );
      pTree->AppendItem( pId, _T("  ") + wsT, -1, -1 );
      // raw
      wsT.Printf( _T("Raw Data Size    : 0x%08lX at Offset: 0x%08lX"),
        psh->ulRawSize, psh->ulRawOffset );
      pTree->AppendItem( pId, _T("  ") + wsT, -1, -1 );
      // reloc
      wsT.Printf( _T("# of Relocs      : 0x%08lX at Offset: 0x%08lX"),
        psh->ulNRelocs, psh->ulRelocOffset );
      pTree->AppendItem( pId, _T("  ") + wsT, -1, -1 );
      // line #
      wsT.Printf( _T("Number of Line #s: 0x%08lX"),
        psh->ulNLineNo );
      pTree->AppendItem( pId, _T("  ") + wsT, -1, -1 );
	  }
	}
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
wxString PeExeHdr::getFlagString( unsigned long uslFlags )
{
  wxString wsSep = _T(", ");
  wxString wsRes = _T("");
  if( uslFlags & 0x08 )
    wsRes += _T("DontPadd, ");
  if( uslFlags & IMAGE_SCN_CNT_CODE /*0x020 */ )
  {
    wsRes += _T("EXE, ");
  }
  if( uslFlags & IMAGE_SCN_CNT_INITIALIZED_DATA /*0x040 */ )
  {
    wsRes += _T("InitData, ");
  }
  if( uslFlags & IMAGE_SCN_CNT_UNINITIALIZED_DATA /* 0x080 */)
  {
    wsRes += _T("UninitData, ");
  }
  if( uslFlags & IMAGE_SCN_LNK_OTHER /*0x0100*/ ) {
    wsRes += _T("Resx100, ");
  }
  if( uslFlags & IMAGE_SCN_LNK_INFO /*0x0200*/ ) {
    wsRes += _T("LinkInfo(objs), ");
  }
  if( uslFlags & 0x0400 ) {
    wsRes += _T("Resx400, ");
  }
  if( uslFlags & IMAGE_SCN_LNK_REMOVE /*0x800*/ ) {
    wsRes += _T("LinkRem(objs), ");
  }
  if( uslFlags & IMAGE_SCN_LNK_COMDAT /*0x01000*/ ) {
    wsRes += _T("COMDAT, ");
  }
  if( uslFlags & IMAGE_SCN_GPREL /*0x08000*/ ) {
    wsRes += _T("GpRel, ");
  }
  
  if( uslFlags & IMAGE_SCN_LNK_NRELOC_OVFL /*0x01000000*/ ) {
    wsRes += _T("ExtdRelocs, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_DISCARDABLE /*0x02000000*/ ) {
    wsRes += _T("Discardable, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_NOT_CACHED /*0x04000000*/ ) {
    wsRes += _T("DontCache, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_NOT_PAGED /*0x08000000*/ ) {
    wsRes += _T("DontPage, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_SHARED /*0x010000000*/ ) {
    wsRes += _T("MemShared, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_EXECUTE /*0x020000000*/ ) {
    wsRes += _T("ExecCode, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_READ /*0x040000000*/ ) {
    wsRes += _T("MemRead, ");
  }
  if( uslFlags & IMAGE_SCN_MEM_WRITE /*0x080000000*/ ) {
    wsRes += _T("MemWrite, ");
  }

  if( uslFlags & 0x0F00000 )
  {
    wxString wsAl;
    unsigned i = (uslFlags & 0x0F00000)>>6;
    wsAl.Printf( _T(", Align: %d"), 2<<i );
    wsRes += wsAl;
  }
  return wsRes;
}

// ------------------------------------------------------------------
/// Display the directories in the PE file header.
/// \param [in] pTree - ptr to tree control
/// \param [in] parentId - ref to parent in tree
void PeExeHdr::DisplayPeDirectories( wxTreeCtrl *pTree,
    wxTreeItemId &parentId )
{
  newPeSectionHdr_t *psh;
  unsigned int iuNSections;
  unsigned int j;
  wxString wsT;
  wxString wsFlagString;
  newPeOptHdr_t *pPeOptHdr = GetPeOptlHdrPtr();
  // also need a ptr to the rest of the PE32 optional header
  // which includes the Data Directory array
  newPeOptNtHdr_t *pPeNtOptHdr =  (
    newPeOptNtHdr_t *)((unsigned long)pPeOptHdr + sizeof(newPeOptHdr_t) );

  wxTreeItemId peDirectoriesId =
      pTree->AppendItem( parentId,
        _T("PE Directories"), -1, -1 );
  
  //--------------------------------------------------------------
  // following the 'newPeOptNtHdr' we have an array of 16
  // IMAGE_DIRECTORY_ENTRY_xxx - not all of which are used
  // as defined in WINNT.H??
  for( unsigned long i = 0; i < pPeNtOptHdr->ulNumberOfRvaAndSizes; i++ )
  {
    if ( pPeNtOptHdr->dataDirs[i].ulSize == 0 )
      continue;
    // otherwise see which section this is part of
    wsT = _T("unused");
    if( i < NUMBER_DIRECTORY_ENTRY_NAMES )
      wsT = wsImageDirectoryNames[i];    
    iuNSections = GetPeHdrPtr()->usNSections;
    psh =  GetSectionDrPtr();
    if (psh != NULL)
	  {
      char pszN[10];
	    // find the section which the data directory RVA is part of 
	    for ( j = 0; j < iuNSections; j++, psh++ )
	    {
        void * pvStart;
        if( addressIsIn( 
          pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
          psh->ulVirtAddr,
          psh->ulVirtSize ))
        {
          unsigned long ulT;
          ulT = (unsigned long)(newPeExeHdr_t *)mp_lExeFileData;
          ulT += pPeNtOptHdr->dataDirs[i].ulVirtualAddress;
          ulT -= psh->ulVirtAddr;
          ulT += psh->ulRawOffset;
          pvStart = (newPeImageImportDescr_t *)ulT;
          memset( pszN, 9, '\0' );
          strncpy( pszN, (const char *)psh->name, 8 );
          wsT.Printf( _T("%-12s is part of %-8s - RVA: %08XH  Size: %08XH"), 
            wsT, wxString::FromAscii(pszN),
            pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
            pPeNtOptHdr->dataDirs[i].ulSize );
          wxTreeItemId dataDirId =
            pTree->AppendItem( peDirectoriesId, wsT, -1, -1 );
          switch ( i )
          {
            case IMAGE_DIRECTORY_ENTRY_IMPORT:        // 1
              DisplayImportDir( pTree, dataDirId,
                (newPeImageImportDescr_t *)pvStart,
                pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
                psh->ulVirtAddr );
              break;
            case IMAGE_DIRECTORY_ENTRY_IAT:           // 12
              DisplayIAT( pTree, dataDirId,
                (newPe_IMAGE_IAT_t *)pvStart,
                pPeNtOptHdr->dataDirs[i].ulVirtualAddress,
                psh->ulVirtAddr );
              break;
            case IMAGE_DIRECTORY_ENTRY_EXPORT:        // 0
            case IMAGE_DIRECTORY_ENTRY_RESOURCE:      // 2
            case IMAGE_DIRECTORY_ENTRY_EXCEPTION:     // 3
            case IMAGE_DIRECTORY_ENTRY_SECURITY:      // 4
            case IMAGE_DIRECTORY_ENTRY_BASERELOC:     // 5
            case IMAGE_DIRECTORY_ENTRY_DEBUG:         // 6
            case IMAGE_DIRECTORY_ENTRY_COPYRIGHT:     // 7
            case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:     // 8
            case IMAGE_DIRECTORY_ENTRY_TLS:           // 9
            case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:   // 10
            case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:  // 11
            
            case IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT:  // 13
            case IMAGE_DIRECTORY_ENTRY_COM:           // 14
            default:
              break;
          }
        }
      }
    }
  }
}

// ------------------------------------------------------------------
/** Display the import directory in the header panel.
 *  \todo - extra work to handle broken linkers, unbound/old-bound 
 *  and new-bound directories
 */

void PeExeHdr::DisplayImportDir( wxTreeCtrl *pTree,
                                 wxTreeItemId &parentId,
                                 newPeImageImportDescr_t *pulRawDataStart,
                                 const DWORD sectionBase, 
                                 const DWORD WXUNUSED(sectionBaseVirt ) )
{
#if 1
  #define adr(rva) ((const void*)((char*)pulRawDataStart+((DWORD)(rva))-sectionBase))
  wxString wsT;
  wxTreeItemId dataDirId;
  char *pszName;
//  IMAGE_THUNK_DATA *pImportEntry;
  newPeImageImportDescr_t * p = pulRawDataStart;
  for( p; p->Characteristics; p++ )
  {
    pszName = (char *)adr(p->Name);
    wsT.Printf( _T("++ DLL Name : %s "),  
      wxString::FromAscii( pszName ) );    
    dataDirId =
      pTree->AppendItem( parentId, wsT, -1, -1 );
    wsT.Printf( _T("Name Table (OrigFirstThunk)   : %08XH"), 
      p->OriginalFirstThunk );
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    
    //pszName = asctime(gmtime((const time_t *)&p->TimeDateStamp));
    // Matt Pietrek's code uses this form
    pszName = ctime((const time_t *)&p->TimeDateStamp);
    wsT =         _T("Date & Time                   : ");
    if( p->TimeDateStamp == 0L )
    {
      wsT += _T("00000000H - not bound");
    }
    else if( p->TimeDateStamp == ~0UL )
    {
      wsT += _T("-1 - bound, new style");
    }
    else
    {
      wsT.Printf( _T("Date & Time                   : %s - bound - old style"), 
      wxString::FromAscii( pszName ) );      
    }
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    wsT.Printf(   _T("ForwarderChain                : %08XH"), 
      p->ForwarderChain );      
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    wsT.Printf( _T("Address Table (FirstThunk RVA): %08XH"), 
      p->FirstThunk );      
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
#if 0
    causes a crash whe looking at 
    if (p->OriginalFirstThunk)
    {
      pImportEntry = (IMAGE_THUNK_DATA *)adr(p->OriginalFirstThunk);
      //mapped_entry = adr(imp->FirstThunk);
      for ( ; pImportEntry->u1.Ordinal; pImportEntry++ )
      {
        const IMAGE_IMPORT_BY_NAME *nameImport = 
            (IMAGE_IMPORT_BY_NAME *)adr(pImportEntry->u1.AddressOfData);
        pszName = (char *)nameImport->Name;
        wsT.Printf( _T("%6u %-20.50s"), 
            nameImport->Hint, wxString::FromAscii( pszName ) );
        pTree->AppendItem( dataDirId, wsT, -1, -1 );
      }
    }
#endif
  }
#else
//  #define indent "      "
  #define adr(rva) ((const IMAGE_THUNK_DATA *)\
  ((char*)sectionBase+((DWORD)(rva))\
    -sectionBaseVirt))
  // IsBadReadPtr() is a MS function, but obsolete
  // see http://msdn.microsoft.com/en-us/library/aa366713%28VS.85%29.aspx
  for (; !IsBadReadPtr(imp, sizeof(*imp)) && imp->Name; imp++)
  {
    const IMAGE_THUNK_DATA *import_entry, *mapped_entry;
    enum
    {
      bound_none, bound_old, bound_new
    }
    bound;

//    printf("\n" indent "from \"%s\":\n", (char *)adr(imp->Name));

    if (imp->TimeDateStamp == ~0UL)
    {
//      puts(indent "bound, new style");
      bound = bound_new;
    }
    else if (imp->TimeDateStamp)
    {
//      printf(indent "bound (old style) to %s", 
//        asctime(gmtime((const time_t *)&imp->TimeDateStamp)));
      bound = bound_old;
    }
    else
    {
//      puts(indent "not bound");
      bound = bound_none;
    }
//    printf(indent "name table at %#lx, address table at %#lx\n", 
//      imp->OriginalFirstThunk, imp->FirstThunk);

    if (imp->OriginalFirstThunk)
    {
      import_entry = adr(imp->OriginalFirstThunk);
      mapped_entry = adr(imp->FirstThunk);
    }
    else
    {
//      puts(indent "(hint table missing, probably Borland bug)");
      import_entry = adr(imp->FirstThunk);
      mapped_entry = 0;
      bound = bound_none;
    }

//    printf(indent "%6s %s\n", "hint", "name");
//    printf(indent "%6s %s\n", "----", "----");
    {
      int count, nextforwarder = bound==bound_old ? 
        imp->ForwarderChain : -1;
      for (count = 0; import_entry->u1.Ordinal; 
        count++, import_entry++, bound ? mapped_entry++ : 0)
      {
        if (IMAGE_SNAP_BY_ORDINAL(import_entry->u1.Ordinal))
;//          printf(indent "%6lu %-20s", 
//          IMAGE_ORDINAL(import_entry->u1.Ordinal),
//          "<ordinal>");
        else
        {
;//          const IMAGE_IMPORT_BY_NAME *name_import = 
//            adr(import_entry->u1.AddressOfData);
//          printf(indent "%6u %-20.50s", 
//            name_import->Hint, name_import->Name);
        }
        if (bound)
        //{
          if (count != nextforwarder)
;//            printf("%#12lx\n", (unsigned long)mapped_entry->u1.Function);
          else
          {
//            printf("%12s\n", "    --> forward");
            nextforwarder = (int)mapped_entry->u1.ForwarderString;
          }
          else
;//            puts("");

      }
    }
  }
//  if (IsBadReadPtr(imp, sizeof(*imp)))
//    puts(indent "!! data inaccessible!!");

//#undef indent
#endif
#undef adr
}

// ------------------------------------------------------------------
/// display the import directory in the header panel.
/// extra work to handle broken linkers, unbound/old-bound 
/// and new-bound directories

void PeExeHdr::DisplayIAT( wxTreeCtrl *pTree,
                          wxTreeItemId &parentId,
                          newPe_IMAGE_IAT_t *pulRawDataStart,
                          const DWORD sectionBase, 
                          const DWORD sectionBaseVirt )
{
  #define adr(rva) ((const void*)((char*)pulRawDataStart+((DWORD)(rva))-sectionBase))
  wxString wsT;
  wxUnusedVar( pTree );
  wxUnusedVar( parentId );
  wxUnusedVar( pulRawDataStart );
  wxUnusedVar( sectionBase );
  wxUnusedVar( sectionBaseVirt );
#if 0
  wxTreeItemId dataDirId;
  char *pszName;
  IMAGE_THUNK_DATA *pImportEntry;
  newPeImageImportDescr_t * p = pulRawDataStart;
  for( p; p->Characteristics; p++ )
  {
    pszName = (char *)adr(p->Name);
    wsT.Printf( _T("++ DLL Name : %s "),  
      wxString::FromAscii( pszName ) );    
    dataDirId =
      pTree->AppendItem( parentId, wsT, -1, -1 );
    wsT.Printf( _T("Name Table (OrigFirstThunk)   : %08XH"), 
      p->OriginalFirstThunk );
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    
    //pszName = asctime(gmtime((const time_t *)&p->TimeDateStamp));
    // Matt Pietrek's code uses this form
    pszName = ctime((const time_t *)&p->TimeDateStamp);
    wsT =         _T("Date & Time                   : ");
    if( p->TimeDateStamp == 0L )
    {
      wsT += _T("00000000H - not bound");
    }
    else if( p->TimeDateStamp == ~0UL )
    {
      wsT += _T("-1 - bound, new style");
    }
    else
    {
      wsT.Printf( _T("Date & Time                   : %s - bound - old style"), 
      wxString::FromAscii( pszName ) );      
    }
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    wsT.Printf(   _T("ForwarderChain                : %08XH"), 
      p->ForwarderChain );      
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    wsT.Printf( _T("Address Table (FirstThunk RVA): %08XH"), 
      p->FirstThunk );      
    pTree->AppendItem( dataDirId, wsT, -1, -1 );
    if (p->OriginalFirstThunk)
    {
      pImportEntry = (IMAGE_THUNK_DATA *)adr(p->OriginalFirstThunk);
      //mapped_entry = adr(imp->FirstThunk);
      for ( ; pImportEntry->u1.Ordinal; pImportEntry++ )
      {
        const IMAGE_IMPORT_BY_NAME *nameImport = 
            (IMAGE_IMPORT_BY_NAME *)adr(pImportEntry->u1.AddressOfData);
        pszName = (char *)nameImport->Name;
        wsT.Printf( _T("%6u %-20.50s"), 
            nameImport->Hint, wxString::FromAscii( pszName ) );
        pTree->AppendItem( dataDirId, wsT, -1, -1 );
      }
    }
  }
//#else
//  #define indent "      "
  #define adr(rva) ((const IMAGE_THUNK_DATA *)\
  ((char*)sectionBase+((DWORD)(rva))\
    -sectionBaseVirt))
  // IsBadReadPtr() is a MS function, but obsolete
  // see http://msdn.microsoft.com/en-us/library/aa366713%28VS.85%29.aspx
  for (; !IsBadReadPtr(imp, sizeof(*imp)) && imp->Name; imp++)
  {
    const IMAGE_THUNK_DATA *import_entry, *mapped_entry;
    enum
    {
      bound_none, bound_old, bound_new
    }
    bound;

//    printf("\n" indent "from \"%s\":\n", (char *)adr(imp->Name));

    if (imp->TimeDateStamp == ~0UL)
    {
//      puts(indent "bound, new style");
      bound = bound_new;
    }
    else if (imp->TimeDateStamp)
    {
//      printf(indent "bound (old style) to %s", 
//        asctime(gmtime((const time_t *)&imp->TimeDateStamp)));
      bound = bound_old;
    }
    else
    {
//      puts(indent "not bound");
      bound = bound_none;
    }
//    printf(indent "name table at %#lx, address table at %#lx\n", 
//      imp->OriginalFirstThunk, imp->FirstThunk);

    if (imp->OriginalFirstThunk)
    {
      import_entry = adr(imp->OriginalFirstThunk);
      mapped_entry = adr(imp->FirstThunk);
    }
    else
    {
//      puts(indent "(hint table missing, probably Borland bug)");
      import_entry = adr(imp->FirstThunk);
      mapped_entry = 0;
      bound = bound_none;
    }

//    printf(indent "%6s %s\n", "hint", "name");
//    printf(indent "%6s %s\n", "----", "----");
    {
      int count, nextforwarder = bound==bound_old ? 
        imp->ForwarderChain : -1;
      for (count = 0; import_entry->u1.Ordinal; 
        count++, import_entry++, bound ? mapped_entry++ : 0)
      {
        if (IMAGE_SNAP_BY_ORDINAL(import_entry->u1.Ordinal))
;//          printf(indent "%6lu %-20s", 
//          IMAGE_ORDINAL(import_entry->u1.Ordinal),
//          "<ordinal>");
        else
        {
;//          const IMAGE_IMPORT_BY_NAME *name_import = 
//            adr(import_entry->u1.AddressOfData);
//          printf(indent "%6u %-20.50s", 
//            name_import->Hint, name_import->Name);
        }
        if (bound)
        //{
          if (count != nextforwarder)
;//            printf("%#12lx\n", (unsigned long)mapped_entry->u1.Function);
          else
          {
//            printf("%12s\n", "    --> forward");
            nextforwarder = (int)mapped_entry->u1.ForwarderString;
          }
          else
;//            puts("");

      }
    }
  }
//  if (IsBadReadPtr(imp, sizeof(*imp)))
//    puts(indent "!! data inaccessible!!");

//#undef indent
#endif
#undef adr
}
// ------------------------------------------------------------------
/// return the import data directory in the array provided.
///
/// extra work to handle broken linkers, unbound/old-bound 
/// and new-bound directories
#if 1
// -------------------------------------------------------------
// display the imported functions in the right notebook
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ImportsArray);

void PeExeHdr::GetImports( ImportsArray &importsArray )
{
  char *pszName;
  wxString wsLibName;
  wxString wsT;
  IMAGE_THUNK_DATA *pImportEntry;
  importsEntry importEntry;

  #define adr(rva) ((const void*)((char*)\
  (newPeImageImportDescr_t *)pvStart+((DWORD)(rva))\
    -pPeNtOptHdr->dataDirs[IMAGE_DIRECTORY_ENTRY_IMPORT].ulVirtualAddress))
  unsigned int iuNSections;
  newPeSectionHdr_t *psh;

  newPeOptHdr_t *pPeOptHdr = GetPeOptlHdrPtr();
  // also need a ptr to the rest of the PE32 optional header
  // which include the Data Directory array
  newPeOptNtHdr_t *pPeNtOptHdr =  (
    newPeOptNtHdr_t *)((unsigned long)pPeOptHdr + sizeof(newPeOptHdr_t) );
  
  if ( pPeNtOptHdr->dataDirs[IMAGE_DIRECTORY_ENTRY_IMPORT].ulSize == 0 )
    return;
  // otherwise see which section this is part of
  iuNSections = GetPeHdrPtr()->usNSections;
  psh =  GetSectionDrPtr();
  if (psh != NULL)
  {
    unsigned int j;
    // find the section which the data directory RVA is part of 
    for ( j = 0; j < iuNSections; j++, psh++ )
    {
      void * pvStart;
      if( addressIsIn( 
        pPeNtOptHdr->dataDirs[IMAGE_DIRECTORY_ENTRY_IMPORT].ulVirtualAddress,
        psh->ulVirtAddr,
        psh->ulVirtSize ))
      {
        unsigned long ulT;
        ulT = (unsigned long)(newPeExeHdr_t *)mp_lExeFileData;
        ulT += pPeNtOptHdr->dataDirs[IMAGE_DIRECTORY_ENTRY_IMPORT].ulVirtualAddress;
        ulT -= psh->ulVirtAddr;
        ulT += psh->ulRawOffset;
        pvStart = (newPeImageImportDescr_t *)ulT;

        newPeImageImportDescr_t * p = (newPeImageImportDescr_t *)pvStart;
        for( p; p->Characteristics; p++ )
        {
          pszName = (char *)adr(p->Name);
          wsLibName = wxString::FromAscii( pszName );
          importEntry.wsLibrary = wsLibName; 

          //pszName = asctime(gmtime((const time_t *)&p->TimeDateStamp));
          // Matt Pietrek's code uses this form
      //    pszName = ctime((const time_t *)&p->TimeDateStamp);
          //wxString wsTemp;
      #if 0
          wsT =         _T("Date & Time                   : ");
          if( p->TimeDateStamp == 0L )
          {
            wsT += _T("00000000H - not bound");
          }
          else if( p->TimeDateStamp == ~0UL )
          {
            wsT += _T("-1 - bound, new style");
          }
          else
          {
            wsT.Printf( _T("Date & Time                   : %s - bound - old style"), 
            wxString::FromAscii( pszName ) );      
          }
 
      #endif
          if (p->OriginalFirstThunk)
          {
            long jj;
            //long *plIAT = (long *)adr(p->FirstThunk);
            pImportEntry = (IMAGE_THUNK_DATA *)adr(p->OriginalFirstThunk);
            //mapped_entry = adr(imp->FirstThunk);
            for ( jj = 0; pImportEntry->u1.Ordinal; pImportEntry++,jj++ )
            {
              const IMAGE_IMPORT_BY_NAME *nameImport = 
                  (IMAGE_IMPORT_BY_NAME *)adr(pImportEntry->u1.AddressOfData);
              pszName = (char *)nameImport->Name;
              wsT = wxString::FromAscii( pszName );
              importEntry.wsName = wsT;
              importEntry.lHint = nameImport->Hint;
              importEntry.ulRVA = pPeNtOptHdr->ulImageBase +
                  p->FirstThunk + jj * sizeof(long);
              importsArray.Add( importEntry );
            }
          }
        }
      }
    }
  }
#undef adr
}
#endif
#if 0
// ------------------------------------------------------------------
/// Find directories in the given range.
/// Look for any directories in the given section or in a part of the 
/// header (bound import!); if any are found, display the data.
///
/// \param [in] sectionStartRaw: current address of section start (raw data)
/// \param [in] sectionStartVirt: RVA of section (raw data)
/// \param [in] sectionLength: length of section in bytes

static void displayDirectories( const void *const sectionStartRaw,
                                const DWORD sectionStartVirt,
                                const size_t sectionLength,
                                const IMAGE_DATA_DIRECTORY * const directories )
{
  int directory;
  
  for (directory = 0; 
    directory < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; directory++)
    if ( directories[directory].VirtualAddress && 
          addressIsIn(directories[directory].VirtualAddress, 
          sectionStartVirt, sectionLength ))
    {
      const void *const start = 
        (char *)sectionStartRaw + (directories[directory].VirtualAddress - 
        sectionStartVirt );
      wxUnusedVar( start );
      /* (virtual address of stuff - virtual address of section) 
       * = offset of stuff in section */
      const unsigned dirLen = directories[directory].Size;
//      printf( "Directory[%d]", directory );
//      printf("\n%*sat offset %#x (%u bytes): ", indentation, "", 
//        (char *)stuff_start - (char *)section_data, stuff_length);
#if 0
      switch (directory)
      {

        case IMAGE_DIRECTORY_ENTRY_EXPORT:
          puts("Export Directory");
          dump_export_directory(section_data, section_start_virtual, 
            stuff_start, section_length);
          break;
//#endif
        case IMAGE_DIRECTORY_ENTRY_IMPORT:
          //puts("Import Directory");
          displayImportDir( sectionStartRaw, 
            sectionStartVirt, (IMAGE_IMPORT_DESCRIPTOR *)start);
          break;
//#if 0
        case IMAGE_DIRECTORY_ENTRY_RESOURCE:
          printf("Resource Directory\n%*s", 2 * indentation, "");
          dump_resource_directory(2 * indentation, 
            stuff_start, stuff_start, TRUE);
          break;
        case IMAGE_DIRECTORY_ENTRY_EXCEPTION:
          puts("Exception Directory");
          break;
        case IMAGE_DIRECTORY_ENTRY_SECURITY:
          puts("Security Directory");
          break;
        case IMAGE_DIRECTORY_ENTRY_BASERELOC:
          puts("Base Relocation Table");
          dump_reloc_directory(section_data, 
            section_start_virtual, stuff_start);
          break;
        case IMAGE_DIRECTORY_ENTRY_DEBUG:
          puts("Debug Directory");
          break;
        case IMAGE_DIRECTORY_ENTRY_COPYRIGHT:
          printf("Description String \"%.*s\"\n", stuff_length, 
            (char *)stuff_start);
          break;
        case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:
          puts("Machine Value (MIPS GP)");
          break;
        case IMAGE_DIRECTORY_ENTRY_TLS:
          puts("TLS Directory");
          break;
        case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:
          puts("Load Configuration Directory");
          break;
        case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:
          puts("Bound Import Directory");
          dump_bound_import_directory(section_data, 
            section_start_virtual, stuff_start);
          break;
        case IMAGE_DIRECTORY_ENTRY_IAT:
          puts("Import Address Table");
          break;

        default:
          puts("unknown directory");
          break;
      }
#endif
    }
}
#endif
// -------------------------------------------------------------
// display the imported functions in the right notebook
//#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(SegmentsArray);

void PeExeHdr::GetSegments( SegmentsArray &segmentsArray )
{
  wxString wsT;
  int i;
  wxString wsFlagString;
  peHdrSegmentEntry_t seg;
  newPeOptHdr_t *pPeOptHdr = GetPeOptlHdrPtr();
  // also need a ptr to the rest of the PE32 optional header
  // which include the Data Directory array
  newPeOptNtHdr_t *pPeNtOptHdr =  (
    newPeOptNtHdr_t *)((unsigned long)pPeOptHdr + sizeof(newPeOptHdr_t) );
  unsigned long ulBase = pPeNtOptHdr->ulImageBase;
  newPeSectionHdr_t *psh =  GetSectionDrPtr();
  int nSections = GetPeHdrPtr()->usNSections;
  if (psh != NULL)
	{
	  // find the section by name 
	  for (i=0; i < nSections; i++, psh++ )
	  {
      char pszN[10];
      memset( pszN, 9, '\0' );
      strncpy( pszN, (const char *)psh->name, 8 ) ;
      wsT.Printf( _T("%10s     Flags: 0x%08lX"), 
        wxString::FromAscii(pszN), psh->ulFlags );
      seg.wsName = wxString::FromAscii(pszN); 
      seg.ulFlags = psh->ulFlags;
      seg.ulStart = psh->ulVirtAddr + ulBase;
      seg.ulEnd = psh->ulVirtAddr + psh->ulVirtSize + ulBase;
      seg.ulVirtSize = psh->ulVirtSize;
      seg.ulVirtOffset = psh->ulVirtAddr;
      seg.ulRawSize = psh->ulRawSize;
      seg.ulRawOffset = psh->ulRawOffset;
      seg.ulNRelocs = psh->ulNRelocs;
      seg.ulRelocOffset = psh->ulRelocOffset;
      seg.ulNLineNbrs = psh->ulNLineNo;
      segmentsArray.Add( seg );
	  }
	}
}
// -------------------------------------------------------------

//--------------------------------------------------------------
/**
 * Return the data for a specific directory entry in the PE file's
 * header
 * \param [in]  - iDirectory - index into the 'directory' array
 * \param [out] - section name containing the directory
 * \param [out] - start address in disk image
 * \param [out] - size in disk image
 * \return true  - no problem
 *         false   for any problem        
 */
bool PeExeHdr::GetRawDirectoryData( 
        unsigned long ulDirectory, wxString &wsName,
        unsigned long &ulRawStart, unsigned long &ulRawSize )
{
  wxString wsT;
  newPeSectionHdr_t *psh;
  unsigned int  iuNSections;
  unsigned int  j;
  unsigned long ulT;

  newPeOptHdr_t *pPeOptHdr = GetPeOptlHdrPtr();
  // also need a ptr to the rest of the PE32 optional header
  // which includes the Data Directory array
  newPeOptNtHdr_t *pPeNtOptHdr =  (
    newPeOptNtHdr_t *)((unsigned long)pPeOptHdr + sizeof(newPeOptHdr_t) );

  // following the 'newPeOptNtHdr' we have an array of 16
  // IMAGE_DIRECTORY_ENTRY_xxx - not all of which are used
  // as defined in WINNT.H??

  if( ulDirectory >= pPeNtOptHdr->ulNumberOfRvaAndSizes )
    return false;
  if ( pPeNtOptHdr->dataDirs[ulDirectory].ulSize == 0 )
    return false;
  // otherwise see which section this is part of
  wsT = _T("unused");
//  if( i < NUMBER_DIRECTORY_ENTRY_NAMES )
    wsT = wsImageDirectoryNames[ulDirectory];    
  iuNSections = GetPeHdrPtr()->usNSections;
  psh =  GetSectionDrPtr();
  if (psh != NULL)
  {
    char pszN[10];
    // find the section which the data directory RVA is part of 
    for ( j = 0; j < iuNSections; j++, psh++ )
    {
      if( addressIsIn( 
        pPeNtOptHdr->dataDirs[ulDirectory].ulVirtualAddress,
        psh->ulVirtAddr,
        psh->ulVirtSize ))
      {
        ulT = (unsigned long)(newPeExeHdr_t *)mp_lExeFileData;
        ulT = pPeNtOptHdr->dataDirs[ulDirectory].ulVirtualAddress;
        ulT -= psh->ulVirtAddr;
        ulT += psh->ulRawOffset;
        ulRawStart = ulT;
        ulRawSize = pPeNtOptHdr->dataDirs[ulDirectory].ulSize;
        memset( pszN, 9, '\0' );
        strncpy( pszN, (const char *)psh->name, 8 );
        wsName = wxString::FromAscii(pszN);
        break;    // no need to look further
      }
    }
  }
  return false;
}

// ------------------------------------------------------------------
bool PeExeHdr::GetSectionHeaderByName( wxString wsName,
        newPeSectionHdr_t *pHdr, unsigned int &nSec )
{
  char pszN[10];
  wxString wsT;
  unsigned short usNSecs = GetPeHdrPtr()->usNSections;
  newPeSectionHdr_t *pH = GetSectionDrPtr();
  unsigned int i;

  for ( i =  0 ; i < usNSecs; i++, pH++ )
  {
    memset( pszN, 9, '\0' );
    strncpy( pszN, (const char *)pH->name, 8 ) ;
    wsT.Printf( _T("%s"), wxString::FromAscii(pszN) );
    if( wsT.Matches( wsName ) )
    {
      memcpy( pHdr, pH, sizeof( newPeSectionHdr_t  ) );
      nSec = i;
      return true;
    }
  }
  return false;
}
// ------------------------------- eof ------------------------------
