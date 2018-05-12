/*-----------------------------------------------------------------
 * Name:        wxBupWorkTraverser.cpp
 * Purpose:
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
#include "wxBupWorkTraverserh.h"

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

wxDirTraverseResult MyBupWorkTraverser::OnFile( const wxString& wsSrcFileName )
{
  if ( wxGetApp().m_frame->TestCancelled() )
    return wxDIR_STOP;
  if ( m_fileExclusions.Matches( wsSrcFileName ))
  {
    return wxDIR_CONTINUE;
  }
#if defined( WANT_BACKUP_TYPE )
  if ( !m_bIsFull )   // if it is incremental check 'archive' bit
  {
#if defined( _MSC_VER ) // only good for Windows
    DWORD dwAttr = GetFileAttributes( wsSrcFileName.wc_str() );
    bool bArchive = ( dwAttr & FILE_ATTRIBUTE_ARCHIVE ) > 0;
    if ( !bArchive )
    {
      // skip file if not set, i.e. file is not modified since last full backup
      return wxDIR_CONTINUE;
    }
#endif
  }
#endif
#if defined( WANT_NAME_LEN_CHECK_IN_COUNT )
    if ( wsSrcFileName.Length() > wxGetApp().m_frame->m_ulMaxDestFileNameLen )
    {
      // play it safe
      wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
      wxGetApp().m_frame->m_ulMaxDestFileNameLen = wsSrcFileName.Length();
      wxGetApp().m_frame->m_wsLongestDestFileName = wsSrcFileName;
    }
#endif
  m_rvsFiles.push_back( wsSrcFileName );
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
    wxLogMessage( _("Worker CountFiles - pushback OnFile: ") + wsSrcFileName );
  m_ulNFiles++;
  wxString wsT;
  wsT.Printf( _("Counting Files: %d, Dirs: %d"), m_ulNFiles, m_ulNDirs );
//  wxLogMessage( wsT );
  wxGetApp().m_frame->GetStatusBar()->SetStatusText( wsT, 0 );
  return wxDIR_CONTINUE;
}

// ------------------------------------------------------------------
/**
  * will recurse through subdirs and skip any directories which match
  * any entry in the directory exclusion list
  */
wxDirTraverseResult MyBupWorkTraverser::OnDir(const wxString& wsDirName )
{
  if ( wxGetApp().m_frame->TestCancelled() )
    return wxDIR_STOP;
  if ( m_dirExclusions.Matches( wsDirName ))
  {
    return wxDIR_IGNORE;
  }
  m_ulNDirs++;
  // recurse through all subdirs, as we want to analyse the files
  return wxDIR_CONTINUE;
}

// ------------------------------- eof ------------------------------
