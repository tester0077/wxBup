/*-----------------------------------------------------------------
 * Name:        wxBupExclusionsh.h
 * Purpose:     custom class to handle file exclusions for wxBup
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_EXCLUSIONS_H
#define _WXBUP_EXCLUSIONS_H

// ------------------------------------------------------------------
#include "wx/string.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
class FileExclusions
{
public:
  FileExclusions() {};
  FileExclusions( wxString wsList );
  ~FileExclusions() {};

  bool Matches( wxString wsFilename );
  void SetString( wxString wsList );
  wxString m_wsList;
};

// ------------------------------------------------------------------
class DirExclusions
{
public:
  DirExclusions() {};
  DirExclusions( wxString wsList );
  ~DirExclusions() {};

  bool Matches( wxString wsFilename );
  void SetString( wxString wsList );
  wxString m_wsList;
};
#endif  // _WXBUP_EXCLUSIONS_H
// ------------------------------- eof ---------------------------

