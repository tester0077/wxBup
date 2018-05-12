//-------------------------------------------------------------------
// Name:        exeDosHdrh.h
// Purpose:     
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
#ifndef _EXE_DOS_HDR_H
#define _EXE_DOS_HDR_H
// ------------------------------------------------------------------
#include <wx/treectrl.h>
#include <wx/treebase.h>
#include <wx/longlong.h>

#include "exeHeadersh.h"
// ------------------------------------------------------------------
/*
 * we need the file buffer size so that we can implement a check
 * to be sure we dont try to read data from beyond the allocated 
 * buffer, which will/may crash the system.
 */
class DosExeHdr
{
public:
  DosExeHdr( BYTE *lpByte,                     ///< pointer to memory mapped file data
                   unsigned long long   ulFileSize ); ///< size of allocated buffer
  ~DosExeHdr();

  static bool HasDosHeader( BYTE *lpByte );      ///< does the file data represent a DOS MZ EXE file
  static bool HasOldDosHeader( BYTE *lpByte );   // does the file data represent a DOS ZM EXE file
//  USHORT DosHdrStructSize();
  /// display DOS header data in a tree
  bool  DisplayDosHeader( wxTreeCtrl *pTree,   ///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ptr to parent in tree

  /// list header data, line by line to a file ptr
  bool  ListHeader();  
  // ------------------------------------------------------------------
  /**
   * does the file data represent a DOS EXE file - Magic = MZ
   * or Magic = ZM.
   * use the mapped file pointer directly so we can test before 
   * we need to decide on file type.
   * Also note: here we are only concerned with DOS files.
   */
  bool IsDosImage( BYTE *lpByte ) {
    return (*(USHORT *)lpByte == DOS_IMAGE_MAGIC ) 
    || (*(USHORT *)lpByte == DOS_IMAGE_MAGIC2 );
  };
  USHORT DosHdrSize( BYTE *lpByte ) {
    return (((DosExeHdr_t *)(lpByte))->usHdrSize)*0x10;
  };
  USHORT DosHdrStructSize() {
    return sizeof( DosExeHdr_t );
  };
  USHORT DosInitIP( BYTE *lpByte ) {
    return (((DosExeHdr_t *)(lpByte))->usIP);
  };
  USHORT DosInitCS( BYTE *lpByte ) {
    return (((DosExeHdr_t *)(lpByte))->usCS);
  };
  USHORT DosInitSP( BYTE *lpByte ) {
    return (((DosExeHdr_t *)(lpByte))->usSP);
  };
  unsigned long GetNewHdrOffset( BYTE *lpByte ) {
    return (((DosExeHdr_t *)(lpByte))->ulNewHdrOffs);
  };
private:
  unsigned char         *mp_lExeFileData;
  unsigned long long    mp_ullBufferSize;

};
 // ------------------------------------------------------------------

#endif  // _EXE_DOS_HDR_H
// ------------------------------- eof ------------------------------
