//-------------------------------------------------------------------
// Name:        exeLeHdrh.h
// Purpose:     
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------
#ifndef _EXE_LE_HDR_H
#define _EXE_LE_HDR_H
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
class LeExeHdr
{
public:
  LeExeHdr( BYTE *lpByte,              ///< pointer to memory mapped file data
                  unsigned long long   ulFileSize ); ///< size of allocated buffer
  ~LeExeHdr();

  static bool HasWinLeHeader( BYTE *lpByte,      ///< pointer to memory mapped file data
              unsigned long long   ulFileSize ); ///< size of allocated buffer ); 

  /// display PE header data in a tree
  bool  DisplayLeHeader( wxTreeCtrl *pTree,   ///< ptr to tree control
    wxTreeItemId &parentId );                 ///< ptr to parent in tree

  // ------------------------------------------------------------------

  /// list header data, line by line to a file ptr
  bool  ListHeader();      
private:
  unsigned char *mp_lExeFileData;
  unsigned long long    mp_ullBufferSize;
};
 // ------------------------------------------------------------------

#endif  // _EXE_LE_HDR_H
// ------------------------------- eof ------------------------------
