/*-----------------------------------------------------------------
 * Name:        wxBupWorkTraverserh.h
 * Purpose:     header for analyzing all of the  files & dirs files using wxDirTraverser
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 * Based on code from http://wxwidgets.info/page/5/
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_WORK_TRAVERSER_H
#define _WX_BUP_WORK_TRAVERSER_H

// ------------------------------------------------------------------
 
#include <wx/wx.h>
#include <wx/dir.h>

#include <wx/filename.h>
#include "wx/treectrl.h"
#include "wx/tokenzr.h"

#include "wxBupFrameh.h"
#include "wxBupExclusionsh.h"
// ------------------------------------------------------------------
class Exclusions;
class MyBupWorkTraverser : public wxDirTraverser
{
public:
  MyBupWorkTraverser( wxVector<wxString>& rvsFiles) : m_rvsFiles( rvsFiles )
  {
    m_ulNFiles = 0l;
    m_ulNDirs  = 0l;
    m_wsJobExclusions = g_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal;
    m_fileExclusions.SetString( m_wsJobExclusions );
    m_dirExclusions  = g_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal;
    m_bIsFull = 
      g_jobData.data[IEJ_BACKUP_TYPE].dataCurrent.wsVal.MakeUpper().IsSameAs( _T("FULL") );
  }

  wxDirTraverseResult OnFile( const wxString& WXUNUSED( wsSrcFileName ) );
  wxDirTraverseResult OnDir( const wxString& WXUNUSED( wsDirName ) );
  unsigned long m_ulNFiles;
  unsigned long m_ulNDirs;
private:
  wxVector<wxString>& m_rvsFiles; 
  wxString            m_wsJobExclusions;
  wxStringTokenizer   m_wstExcludes;
  FileExclusions      m_fileExclusions;
  DirExclusions       m_dirExclusions;
  bool                m_bIsFull;
};
 
#endif  // _WX_BUP_WORK_TRAVERSER_H
// ------------------------------- eof ---------------------------
