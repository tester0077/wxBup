/*-----------------------------------------------------------------
 * Name:        wxBupRestoreTest.cpp
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
#include "wx/dir.h"

#include "wxBupFrameh.h"
#include "wxBupRestoreDialogh.h"
#include "wxBupTestRestoreThreadh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
// -------------------------------------------------------------
/**
 * Called via the event loop when 'V' is pressed on kbd - OnStartTesting handler.
 * 
 */
void MyFrame::OnTestJob(wxCommandEvent& WXUNUSED(event))
{
  m_ullTotalSrcBytes = m_ullTotalZipBytes = 0ll; 
  m_szNTotalFiles = m_szNFilesProcessed = 0;
  m_ulDirs = m_ulBags = m_ulMaxDestFileNameLen = m_ulMaxDestPathLen = 0l;
  m_uiSrcDirLevelDepth = m_uiZipDirLevelDepth = 0;
  m_wsLongestDestFileName = m_wsLongestDestPath = wxEmptyString;
  GetGaugeProgress()->SetValue( 0 );
  // and thew taskbar icon's as well
  GetAppProgressInd()->SetValue( 0 );
  // if source root has a bag log, we have a medium i.e.
  // data has been copied to CD, DVD or floppy
  // we don't have bag directories => one level less to worry about
  bool bHaveBags = false; 

  wxBupRestoreDlg dlg( this );
  // save the .INI preferences, just in case
  wxGetApp().MakeConfigCopy( g_iniPrefs, dlg.m_iniPrefs );
  dlg.Layout();
  dlg.m_bIsTest = true;
  if ( dlg.ShowModal() != wxID_OK )
  {
    return;
  }
  // else save the data to the job file
  // all info MUST be saved here or else aborting the dialog
  // will have bad side effects as some of the INI data will
  // have been changed but not everything
  wxGetApp().MakeConfigCopy( dlg.m_iniPrefs, g_iniPrefs );
#if defined( WANT_LOG )
  if( g_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal )
  {
    m_textCtrlLog->Clear();
  }
#endif
  m_textCtrlSpecialFiles->Clear();
  RunTest(
    g_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal );
}

// ------------------------------------------------------------------
/**
 *  Input is via wsTestDir
 */
bool MyFrame::RunTest( wxString wsTestDir )
{
  wxString wsT;
  wxString wsBagName;
  wxArrayString wasBags;
  wxString wsBagLogName;
  // the directory containing the data to be tested/restored
  wxFileName wfnTestRestoreSrcDir;
  // ensure it is treated as a path
  wfnTestRestoreSrcDir.AssignDir( wsTestDir );

  // check to be sure the directories exist
  if ( !wfnTestRestoreSrcDir.DirExists() )
  {
    wxLogError( _T("%s %d Cannot find source directory: %s"), 
      __FILE__, __LINE__, wfnTestRestoreSrcDir.GetPathWithSep() );
    wxASSERT( true );
    return false;
  }

  // collect all bag directories from the test/restore 'source' dir
  wsT = wfnTestRestoreSrcDir.GetPathWithSep();
  wxDir wdSource( wfnTestRestoreSrcDir.GetPathWithSep() );
  
  if ( ! wdSource.IsOpened() )
  {
    wsT.Printf(  _T("Cannot open %s!"),
      g_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal );
    wxMessageBox( wsT, _T("Error"),  wxOK | wxICON_ERROR );
    return false;
  }
#if defined( WANT_LOG )
  if( g_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal )
  {
    m_textCtrlLog->Clear();
  }
#endif
  m_textCtrlSpecialFiles->Clear();
  // check if we have any bag logs
  // all the bag dirs will have no '.'
  wxString wsDir = wdSource.GetNameWithSep();
  wxArrayString wasLogFileNames;
  wxArrayString wasFileNames;
  size_t zNFiles;

  // clear the bag list
  m_FrameBagList.clear();
  // since I was having problems finding files without extensions
  // I've decided to get all files in the root directory
  // and then find the one I want
  wxFileName wfnFile;
  wxArrayString wasNoExtFiles;
  zNFiles = wdSource.GetAllFiles( wsDir, &wasFileNames, _T("*"), wxDIR_FILES );
  size_t i;
  for (  i = 0; i < zNFiles; i++ )
  {
    wfnFile.Assign( wasFileNames[i] );
    if ( wfnFile.GetExt().MakeLower().IsSameAs( _T("log") ) )
      wasLogFileNames.Add( wasFileNames[i] );
    if ( wfnFile.GetExt().IsEmpty() )
      wasNoExtFiles.Add( wasFileNames[i] );
  }
  // this is needed if the user selects a 'bag' directory to test
  // now search for a file without extension, matching 
  // the log files' start string
  // its properties: xxxxxxxxnn
  // where x = arbitrary alpha char
  //       n = one of two numeric char
  int iCount = 0;
  for (size_t j = 0; j < wasNoExtFiles.GetCount(); j++)
  {
    wfnFile.Assign( wasNoExtFiles[j] );
    wxString ws3;
    wxString ws2;
    wxString ws1;
    long     lTest;
    ws2 = ws1 = wfnFile.GetName();
    if ( ws1.Length() > 8 )
      continue;
    ws1 = ws1.Left( ws1.Length() - 2 );
    ws3 = ws2.Right( ws1.Length() - 1 );
    bool bT =  ws3.ToLong( & lTest );
    if( ws1.Length() <= 8 )
    {
      wsBagName = ws1;
      wsBagLogName = ws2 + _T(".log");
      iCount++;
    }
  }
  wxFileName wfnBagLog;
  wfnBagLog.SetPath( wdSource.GetNameWithSep() );
  wfnBagLog.SetFullName( wsBagLogName );
  if (iCount == 1)
  {
    TestOneBag( wfnBagLog.GetFullPath() );
    StartRestoreThread( false );
    if ( IsWorkerCancelled() )
      return false;
  }
  // it seems the user selected the base of a backup directory
  // all we have is 'bag' directories, so we have to loop through
  // each and test it in turn
  else
  {
    if ( IsWorkerCancelled() )
      return false;
    wxString wsDir;
    wxString wsPath = wfnBagLog.GetPathWithSep();
    wxString wsBagLogPathnName = wsPath;
    wxFileName wfnBagLogLocal;
    wxDir wdDirs( wfnBagLog.GetPathWithSep() );
    bool bRet = wdDirs.IsOpened();
    if( !bRet )
    {
      wxLogError( _T("%s %d Error opening directory: %s"), 
        __FILE__, __LINE__, wfnBagLog.GetPathWithSep() );
    }
    wxASSERT( bRet );
    if (wdDirs.HasSubDirs())
    {
      bRet = wdDirs.GetFirst( &wsDir, _T(""), wxDIR_DIRS );
      while ( bRet )
      {
        wfnBagLogLocal.AssignDir( wsPath );
        wfnBagLogLocal.AppendDir( wsDir );
        wfnBagLogLocal.SetName( wsDir );
        wfnBagLogLocal.SetExt( _T("log") );
        // test the current 'bag'
        TestOneBag( wfnBagLogLocal.GetFullPath() );
        if ( IsWorkerCancelled() )
          return false;
        //do the same for the next one
        bRet = wdDirs.GetNext( &wsDir );
      }
      StartRestoreThread( false );
    }
  }
  return true;
}

// ------------------------------------------------------------------
void MyFrame::TestOneBag( wxString wsBagLogPath )
{
  wxFileName wfnBagLog( wsBagLogPath );

  // open the bag log - we have a single bag log
  // we need to open the log and get the number of files from it
  // skipping the lead-in
  wxTextFile wtfLog( wfnBagLog.GetFullPath() );
  wtfLog.Open();
  if( !wtfLog.IsOpened() )
  {
    wxMessageBox( _("Failed to open: " ) + wfnBagLog.GetFullPath(), 
      _("Error"), wxOK );
    return;
  }
  m_szNTotalFiles = wtfLog.GetLineCount();
  wxString wsCurrentLine = wtfLog.GetFirstLine();
  while ( ! wtfLog.Eof() )
  { 
    if (wsCurrentLine.Matches(_T("*======*")) )
    {
      m_szNTotalFiles--;
      break;
    }
    wsCurrentLine = wtfLog.GetNextLine();
    m_szNTotalFiles--;
  }
  wtfLog.Close();
  SetNextBagData( m_szNTotalFiles,  wfnBagLog.GetFullPath() );
}

// -------------------------------------------------------------

bool MyFrame::TestCancelled()
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectTest );
  return m_bStopScan;
}

// ------------------------------------------------------------------
#if 1
void MyFrame::SetNextBagData( unsigned long ulTotalFilesToCheck, wxString wsBagLogPath )
{
  MyBagLogDataEl *pMEl;
  pMEl = new MyBagLogDataEl( ulTotalFilesToCheck, wsBagLogPath );
  m_FrameBagList.push_back( *pMEl );
  delete pMEl;  // the list makes a copy
}
#endif
// ------------------------------- eof ------------------------------
