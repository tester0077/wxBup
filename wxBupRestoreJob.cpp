/*-----------------------------------------------------------------
 * Name:        wxBupRestoreJob.cpp
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
#include "wxBupFrameh.h"
#include "wxBupRestoreDialogh.h"
#include "wxBupTestRestoreThreadh.h"
#include "wx/dir.h"
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
// ------------------------------------------------------------------
/**
 * Called via the event loop when 'R' is pressed on kbd - OnRestoreJob handler.
 * 
 */
void MyFrame::OnRestoreJob(wxCommandEvent& WXUNUSED(event) )
{
  bool bRet;
  wxString wsT;
  GetGaugeProgress()->SetValue( 0 );
  // and thew taskbar icon's as well
  GetAppProgressInd()->SetValue( 0 );
  // if source root has a bag log, we have a medium i.e.
  // data has been copied to CD, DVD or floppy
  // we don't have bag directories => one level less to worry about
  bool bHaveBags = false; 
  wxString wsBagName;
  wxArrayString wasBags;
  wxString wsBagLogName;

  wxBupRestoreDlg dlg( this );
  
  // save the .INI preferences, just in case
  wxGetApp().MakeConfigCopy( g_iniPrefs, dlg.m_iniPrefs );
  dlg.Layout();
  dlg.m_bIsRestore = true;
  if ( dlg.ShowModal() != wxID_OK )
  {
    return;
  }
#if defined( WANT_LOG )
  if( g_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal )
  {
    m_textCtrlLog->Clear();
  }
#endif
  m_textCtrlSpecialFiles->Clear();
  // all info MUST be saved here or else aborting the dialog
  // will have bad side effects as some of the INI data will
  // have been changed but not everything
  wxGetApp().MakeConfigCopy( dlg.m_iniPrefs, g_iniPrefs );

  // the directory containing the data to be tested/restored
  wxFileName wfnTestRestoreSrcDir;
  wfnTestRestoreSrcDir.AssignDir( 
    g_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal );

  // check to be sure the directories exist
  if ( !wfnTestRestoreSrcDir.DirExists() )
  {
    wxLogError( _T("%s %d Cannot find source directory: %s"),__FILE__, __LINE__,
        wfnTestRestoreSrcDir.GetPathWithSep() );
    wxASSERT( true );
  }
  // the directory in which the data is to be reconstituted
  wxFileName wfnTargetDir( 
    g_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal,
    wxEmptyString );
  wxArrayString wasTargetDirs = wfnTargetDir.GetDirs(); 
  if ( !wfnTargetDir.DirExists() )
  {
    bRet = wfnTargetDir.Mkdir( wxS_IWUSR, wxPATH_MKDIR_FULL );
    wxLogError( _T("%s, %d Error creating directory: %s"), __FILE__, __LINE__,
      wfnTargetDir.GetFullPath() );
    wxASSERT( bRet );
  }
  else
  {
    if ( g_iniPrefs.data[IE_CLEAR_TARGET].dataCurrent.bVal )
    {
      wxBusyCursor wait;
      wxLogMessage( _T("Clearing target directory!") +
        wfnTargetDir.GetPathWithSep() );
      SetStatusText( _T("Clearing target directory!"), 0 );
      RemoveDirAndFiles( wfnTargetDir.GetPathWithSep() );
    }
  }
  // collect all bag directories from the test/restore 'source' dir
  wsT = wfnTestRestoreSrcDir.GetPathWithSep();
  wxDir wdSource( wfnTestRestoreSrcDir.GetPathWithSep() );
  
  if ( ! wdSource.IsOpened() )
  {
    wsT.Printf(  _T("Cannot open %s!"),
      g_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal );
    wxMessageBox( wsT, _T("Error"),  wxOK | wxICON_ERROR );
    return;
  }
#if 0//defined( WANT_LOG )
  if( g_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal )
  {
    m_textCtrlLog->Clear();
  }
#endif
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
    SetupOneBag( wfnBagLog.GetFullPath() );
    StartRestoreThread( true );
  }
  // it seems the user selected the base of a backup directory
  // all we have is 'bag' directories, so we have to loop through
  // each and test it in turn
  else
  {
    wxString wsDir;
    wxString wsPath = wfnBagLog.GetPathWithSep();
    wxString wsBagLogPathnName = wsPath;
    wxFileName wfnBagLogLocal;
    wxDir wdDirs( wfnBagLog.GetPathWithSep() );
    bool bRet = wdDirs.IsOpened();
    wxLogError( _T("%s, %d Error opening directory: %s"), __FILE__, __LINE__,
      wfnBagLog.GetPathWithSep() );
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
        // restore the current 'bag'
        SetupOneBag( wfnBagLogLocal.GetFullPath() );
        //do the same for the next one
        bRet = wdDirs.GetNext( &wsDir );
      }
      StartRestoreThread( true );
    }
  }
}

// ------------------------------------------------------------------

void MyFrame::SetupOneBag( wxString wsBagLogPath )
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

// ------------------------------------------------------------------
void MyFrame::StartRestoreThread( bool bRestore )
{
  // 'detached' threads clean up & delete themselves when they are done
  // but MUST be created on the HEAP with new()
  MyRestoreThread *thread = new MyRestoreThread(this); //<<< DETACHED is default!!

  if ( thread->Create() != wxTHREAD_NO_ERROR )
  {
    wxLogError( _T("Can't create restore thread!") );
    return;
  }
  // copy the bag logs list to the thread
  for ( std::list<MyBagLogDataEl>::iterator iter = m_FrameBagList.begin(); 
    iter != m_FrameBagList.end(); )
  {
    thread->SetThreadNextBagData(iter->m_ulTotalFilesToCheck , iter->m_wsBagLogPath );
    iter++;
  }
  m_ullTicks = 0ll;
  m_timer.Start( 1000 ); // 1 sec ticks
  // thread is not running yet, no need for crit sect
  m_bStopScan = false;
  m_bRunning = true;
  thread->m_bRestoring = bRestore;   // just to be sure
  m_JobTypeInProgress = MY_RESTORE_JOB;
  thread->Run(); //>>>>>>>>>>>>>>>>>>>> MyRestoreThread::Entry()
}

// ------------------------------- eof ------------------------------
