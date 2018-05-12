/*-----------------------------------------------------------------
 * Name:        wxBupExclusions.cpp
 * Purpose:     custom class to handle file exclusions for wxBup
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *---------------------------------------------------------------- */

/*----------------------------------------------------------------
 * Standard wxWidgets headers
 *---------------------------------------------------------------- */
// Note __VISUALC__ is defined by wxWidgets, not by MSVC IDE
// and thus won't be defined until some wxWidgets headers are included
#if defined( _MSC_VER ) 
#  if defined ( _DEBUG )
 // this statement NEEDS to go BEFORE all headers
#    define _CRTDBG_MAP_ALLOC
#  endif
#endif
#include "wxBupPreProcDefsh.h"   // needs to be first

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

/* For all others, include the necessary headers
 * (this file is usually all you need because it
 * includes almost all "standard" wxWidgets headers) */
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
// ------------------------------------------------------------------
#include "wxBupExclusionsh.h"

// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to go AFTER all headers
#include <crtdbg.h>
#  ifdef _DEBUG
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  endif
#endif

// ------------------------------------------------------------------
FileExclusions::FileExclusions( wxString wsList )
{
  m_wsList = wsList;
}

// ------------------------------------------------------------------
void FileExclusions::SetString( wxString wsList )
{
  m_wsList = wsList;
}

// ------------------------------------------------------------------
/**
 * Incoming is the full path, but we want to compare only the file name pattern
 */
bool FileExclusions::Matches( wxString wsFilename )
{
  wxFileName wfnTestFile( wsFilename );
  wxString wsFile = wfnTestFile.GetFullName();
  wxStringTokenizer wstExcludes( m_wsList, _T(";") );

  while ( wstExcludes.HasMoreTokens() )
  {
      wxString wsTok = wstExcludes.GetNextToken().MakeLower();
      if( wsFile.MakeLower().Matches( wsTok ) )
      {
        return true; // found match, done here
      }
  }
  return false;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
DirExclusions::DirExclusions( wxString wsList )
{
  m_wsList = wsList;
}

// ------------------------------------------------------------------
void DirExclusions::SetString( wxString wsList )
{
  m_wsList = wsList;
}

// ------------------------------------------------------------------
/**
 * Incoming is the full path, but we want to compare only the dir pattern
 */
bool DirExclusions::Matches( wxString wsFilename )
{
  wxStringTokenizer wstExcludes( m_wsList, _T(";") );

  while ( wstExcludes.HasMoreTokens() )
  {
      wxString wsTok = wstExcludes.GetNextToken().MakeLower();
      if( wsFilename.MakeLower().Matches( wsTok ) )
      {
        return true; // found match, done here
      }
  }
  return false;
}

// ------------------------------- eof ------------------------------
