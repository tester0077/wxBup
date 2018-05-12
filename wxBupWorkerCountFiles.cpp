/*-----------------------------------------------------------------
 * Name:        wxBupWorkerCountFiles.cpp
 * Purpose:     use the Win API to count all of the files to be backed up.
 *              Don't count files which match any of the patterns on the exclude list.
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
#include <windows.h>
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "wx/tokenzr.h"

#include "wxBupWorkerh.h"
#include "wxBupExclusionsh.h"
#include "wxBupWorkTraverserh.h"
// ------------------------------------------------------------------
#if defined(_MSC_VER ) // from Autohotkey-hummer.ahk
// only good for MSVC
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
// ------------------------------------------------------------------
extern jobPrefs_t g_jobData;
using namespace std;
/**
 * Count the files in the specified directory, excluding any that match any 
 * pattern in the exclude list.
 * As well as any which carry specific attributes - \\ todo
 * Reference: https://msdn.microsoft.com/en-us/library/aa365200%28v=vs.85%29.aspx
 * the result will be left in the vector 'files'
 * Exclusions are handled via the global g_jobData.data[IEJ_EXCLUSION_LIST]
 * Called once for each source directory component.
 * 
 * The callback functions are in wxBupWorkTraverser.cpp
 */
bool MyWorkerThread::WorkerCountFiles( 
  wxString wsPath,                // base path
  wxString mask,                  // *.* in all my calls
  wxVector<wxString>& a_rvFiles,  // output vector, files + full paths, existing content is preserved
  unsigned long& a_ulDirs )       // directory count, for feedback
{
  wxString wsT;
  unsigned long ulNFiles = 0l;
  unsigned long ulNDirs  = 0l;

  MyBupWorkTraverser MyWorkTraverser( a_rvFiles );
  wxDir curDir( wsPath );
  curDir.Traverse( MyWorkTraverser );
  ulNFiles = MyWorkTraverser.m_ulNFiles;
  ulNDirs  = MyWorkTraverser.m_ulNDirs;

  wsT.Printf( _("Counting Files: %d, Dirs: %d for %s"), ulNFiles, ulNDirs, wsPath );
  wxLogMessage( wsT );
  m_frame->GetStatusBar()->SetStatusText( wxEmptyString, 0 );
  a_ulDirs = ulNDirs;
  return true;
}

// ------------------------------- eof ------------------------------
