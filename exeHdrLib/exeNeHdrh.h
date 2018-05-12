//-------------------------------------------------------------------
// Name:        exeNeHdrh.h
// Purpose:     
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
#ifndef _EXE_NE_HDR_H
#define _EXE_NE_HDR_H
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
class NeExeHdr
{
public:
  NeExeHdr( BYTE *lpByte,                   ///< pointer to memory mapped file data
                  unsigned long long   ulFileSize ); ///< size of allocated buffer
  ~NeExeHdr();

  static bool HasWinNeHeader( BYTE *lpByte, ///< pointer to memory mapped file data
              unsigned long long   ulFileSize );   ///< size of allocated buffer );  

  /// display PE header data in a tree
  bool  DisplayNeHeader( wxTreeCtrl *pTree,   ///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ptr to parent in tree

  DosExeHdr_t *GetDosHdrPtr() {
    return (DosExeHdr_t *)mp_lExeFileData;
  };

  newNeExeHdr_t *GetNeHdrPtr() {
    return (newNeExeHdr_t *)(mp_lExeFileData 
      + GetDosHdrPtr()->ulNewHdrOffs  );
  };

  /// list header data, line by line to a file ptr
  bool  ListHeader();      
private:
  unsigned char         *mp_lExeFileData;
  unsigned long long    mp_ullBufferSize;
};
 // ------------------------------------------------------------------

#endif  // _EXE_NE_HDR_H
// ------------------------------- eof ------------------------------
