//--------------------------------------------------------------
// Name:        wxBupWorker.cpp
// Purpose:     worker thread for the backup & zip utility
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//--------------------------------------------------------------

//--------------------------------------------------------------
// Standard wxWidgets headers
//--------------------------------------------------------------
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

// For all others, include the necessary headers (this file is
// usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/msgdlg.h"
#include "wx/dir.h"
#include "wx/filename.h"

#include "wxBupWorkerh.h"
#include "wxBupExclusionsh.h"
// -------------------------------------------------------------
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
// externals
// ------------------------------------------------------------------
extern MyApp *theApp;
// ----------------------------------------------------------------------------
// worker thread
// ----------------------------------------------------------------------------

MyWorkerThread::MyWorkerThread( MyFrame *frame )
        : wxThread()
{
  m_frame = frame;
#if defined( WANT_BACKUP_TYPE )
  m_bFullBackup = true;          // true if full backup requested, false otherwise
#endif
}

// -------------------------------------------------------------
/** called when the thread exits - whether it terminates normally
 * or is stopped with Delete() (but not when it is Kill()ed!)
 */
void MyWorkerThread::OnExit()
{
#if defined ( EXIT_EVENT )
  // it works equally well, AFAICT, here or at end of Entry()
  wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_EXIT );
  event.SetInt(-1); // that's all
  wxQueueEvent( m_frame, event.Clone() );
#endif
}

// -------------------------------------------------------------
/** This routine does all the scanning work.
 *
 * The input is now via the global structure g_jobData
 */

void *MyWorkerThread::Entry()
{
  // we don't want a busy cursor, else it looks
  // like we can't stop the process
  //  wxBusyCursor wait;

  // eventually we need to test for file to be moved only
  bool bJustMoveFile = false; // part of "Zip or Copy"
  int i = 0;
  wxString wsMsg;
  wxString wsFName;
  wxFileName wfnSrc;

  // setup move only exclusions
  wxString wsMoveOnly = g_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal;
  FileExclusions fileMoveOnly( wsMoveOnly );

#if defined ( WANT_APP_DND )
  if ( theApp->m_bFromDnD )
  {
    theApp->m_bFromDnD = false;
    // fall through to backup this job
  }
#endif
  if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    wxString wsT;
    wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( _("Starting Job - ") + wsT + _T("================================") );
  }
#if defined( WANT_BACKUP_TYPE )
  DWORD dwAttr;
  m_bFullBackup =
    g_jobData.data[IEJ_BACKUP_TYPE].dataCurrent.wsVal.MakeUpper().IsSameAs( _T("FULL") );
#endif
#if defined( WANT_CLEANUP_IN_THREAD )
  // check if destination already exists.
  // give user the choice to clean it out and create a new copy,
  // or abandon the operation.
  wxString wsTest = m_frame->m_wfnDest.GetPath();
  if ( m_frame->m_wfnDest.DirExists() )
  {
    wxULongLong ullSize = wxDir::GetTotalSize( m_frame->m_wfnDest.GetFullPath() );
    if( ullSize > 0 ) // only  ask if dir is not empty
    {
      if ( ! wxGetApp().m_bQuietMode )
      {
        int answer = wxMessageBox( _("Directory ") + wsTest +
          _(" already exists!\nOverwrite all files?"),
          _("Confirm"), wxYES_NO | wxICON_QUESTION );
        if (answer != wxYES)
        {
          wxString wsT;
          wsT.Printf(
            _("Directory %s already exists.\n") +
            _("User declined overwriting it!\nCleaning up & Aborting job"),
            m_frame->m_wfnDest.GetFullPath() );
          {
            wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
            wxGetApp().m_frame->m_bCancelled = true;
            wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
            // send a log message to the main frame log window
            event.SetInt( MY_RED );   // my color enums
            event.SetExtraLong( 5 );  // verbosity
            event.SetString( wsT );
            wxQueueEvent( m_frame, event.Clone() );
          }
          return NULL;
        }
      }
    }
    //else
    // this will remove all files and sub directories, except
    // for the lowest directory corresponding to the current backup name
    // consisting of  dest path/yyyy-mm-dd-wxBU/
    // display a 'busy' cursor during this time
    wxGetApp().m_stopWatch.Start();
    {
      if ( wxGetApp().m_bQuietMode )  // if run from command line
      {
        wxBusyCursor wait;
        m_frame->m_bCleaningUp = true;
        wxWindowDisabler disableAll;
        m_frame->RemoveDirAndFiles( m_frame->m_wfnDest.GetFullPath() );
      }
      else
      {
        wxBusyInfo info( _("Cleaning up ..."), m_frame );
        wxBusyCursor wait;
        m_frame->m_bCleaningUp = true;
        m_frame->GetStatusBar()->SetStatusText( _("Cleaning up: ") + m_frame->m_wfnDest.GetFullPath() );
        wxWindowDisabler disableAll;
        m_frame->RemoveDirAndFiles( m_frame->m_wfnDest.GetFullPath() );
        m_frame->GetStatusBar()->SetStatusText( _T(""), 0 );
        wxGetApp().m_lCleanupTime = wxGetApp().m_stopWatch.Time();
      }
    }
  }
  m_frame->m_bCleaningUp = false;
  wxGetApp().m_stopWatch.Start( wxGetApp().m_lCleanupTime );
#endif
#if defined ( WANT_COUNT_IN_THREAD )
  m_frame->m_bCountingFiles = true;
  // get a count of all files
  // use the Win API functions; it seems that the wxWidgets functions
  // don't handle all of the attributes??
  {
    wxBusyInfo  *pWaitDialog;
    wxWindowDisabler disableAll;
    if ( !wxGetApp().m_bQuietMode )
    {
      pWaitDialog = new wxBusyInfo(
        _("Collecting file & directory information \n") +
        _("for backup, Please wait. ..."), m_frame );
    }
    // first, split the concatenated source paths
    // Note: we DON'T handle the case where files which fit one of the
    // exclusion masks are specifically added.
    // they will be backed up in any case.
    wxStringTokenizer tokenizer(
      g_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal, ";" );
    m_frame->m_vFiles.clear();

    while ( tokenizer.HasMoreTokens() )
    {
      wxString wsToken = tokenizer.GetNextToken();
      // process token here
      bool bIsDir = ::wxDirExists( wsToken );
      bool bIsFile = ::wxFileExists( wsToken );
      m_wsCurRootPath = wsToken;
      if( bIsFile )
        m_frame->m_vFiles.push_back( wsToken );
      else if ( bIsDir )
      {
        bool bOk = WorkerCountFiles( wsToken, _T("*.*"),
          m_frame->m_vFiles, m_frame->m_ulDirs );
      }
      else
      {
        wxString wsT;
        delete pWaitDialog;
        wsT.Printf( _("Can't find source file: '%s'!\nPlease correct!"),  wsToken );
        {
          wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
          wxMessageBox( wsT, _T("Error"),  wxOK | wxICON_ERROR );
          wxGetApp().m_frame->m_bCancelled = true;
          wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 | MY_MISSING_DIR );  // verbosity
          event.SetString( wsT );
          wxQueueEvent( m_frame, event.Clone() );
        }
        return NULL;  // no point carrying on, get user to fix the bad path
      }
    }
    if ( !wxGetApp().m_bQuietMode )
    {
      delete pWaitDialog;
    }
  }
  m_frame->m_szNTotalFiles = m_frame->m_vFiles.size();
  if( m_frame->m_szNTotalFiles == 0 )
  {
    wxString wsT1;
    wsT1.Printf( _("No 'Source' directories to backup!") );
    wsT1 += _("Nothing to do!");
    wxLogWarning( wsT1 );
    wxMessageBox( wsT1, _T("Error"),  wxOK | wxICON_ERROR );
    return NULL;
  }
  wxGetApp().m_lFileCountTime = wxGetApp().m_stopWatch.Time();
  m_frame->m_bCountingFiles = false;
#endif
  bool bOK = true;

  wxString wsFile;
  size_t szNFiles = m_frame->m_szNTotalFiles;
  size_t szI;

  for ( szI = 0; bOK && !m_frame->IsWorkerCancelled() && (szI < szNFiles); szI++ )
  {
    bJustMoveFile = false;
    wsFile = m_frame->m_vFiles[szI];
    if ( g_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal )
    {
      wxFileName wfnSrcFile( wsFile );
      wxFileName wfnDestFile( m_frame->m_wfnDest.GetFullPath() );
      wxArrayString wasSrcDirs;
      wxString wsPath;
      // just add the file to the zip list, sorting by file size as we go
      // we must adjust the destination path
      // must also look after any extra source directory components
      wxArrayString wasDestDirs;
      wasDestDirs = wfnSrcFile.GetDirs();
      wxString wsSrcVol = wfnSrcFile.GetVolume();
      wfnDestFile.AppendDir( wsSrcVol );
      // add the source directory path components
      for ( size_t ii = wasSrcDirs.GetCount(); ii < wasDestDirs.GetCount(); ii++ )
      {
        wfnDestFile.AppendDir( wasDestDirs[ii] );
        wsPath = wfnDestFile.GetFullPath();
      }
      wfnDestFile.SetFullName( wfnSrcFile.GetFullName() );
      wxString wsT1 = wfnDestFile.GetFullName();

      wsT1 = wfnDestFile.GetFullPath();
      // but when we split the data into 'bags' we must not move, but
      // simply copy the source file
      wxFileName wfnSrc1( m_frame->m_vFiles[szI] );
#ifdef _DEBUG
      wxString wsTest = wfnSrc.GetFullPath();
      wxString wsTest1 = wfnSrc1.GetFullPath();
      wxString wsExt = wfnSrc1.GetExt();
      int iiii = 0;
      if ( wsExt.IsSameAs( _T("iso" ) ) )
        iiii++;
#endif
      unsigned long long ullFileSize;
      // get file size
      bool bRet = GetFileSize( wfnSrc1.GetFullPath(), ullFileSize );
      wxASSERT( bRet );
      wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
      // add it to the list       Dest full path
      //                              V                         V source full path
      m_frame->InsertFileInZipList( wfnDestFile.GetFullPath(), wsFile, ullFileSize, ullFileSize );
      m_frame->m_ullTotalZipsSize += ullFileSize;
    }
    else  // it must be zip & copy
    {
      wxFileName wfnSrc( m_frame->m_vFiles[szI] );
#ifdef _DEBUG
      wxString wsTest = wfnSrc.GetFullPath();
      wxString wsT1;
      wxString wsT2;
      wxString wsT3;
#endif
      wxString wsFileName = wfnSrc.GetFullName();
//#if !defined( WANT_EXCLUSION_CLASS )
//      wxStringTokenizer wstExcludes(
//        g_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal, _T(";") );
//#endif
#if _DEBUG
      wxString wsFileExt = wfnSrc.GetExt().MakeLower();
      // used only for testing the moveOnly code
      if( wsFileExt.IsSameAs( _T("zip") ) )
        int i = 12;
#endif
      if ( fileMoveOnly.Matches( wsFileName ))
      {
        bJustMoveFile = true;
      }
      if( bJustMoveFile ) // part of "Zip or Copy"
      {
        unsigned long long ullFileSize;
        wxString wsTT;
        wxString wsDestPath;
        MakeBackupDestnPath( m_frame->m_vFiles[szI], m_frame->m_wfnDest.GetFullPath(),
          wsDestPath );
        wxFileName wfnDestFix( wsDestPath );
#ifdef _DEBUG
        wsTT = wfnDestFix.GetFullPath();
        wxString wsTSrc = wfnSrc.GetFullPath();
#endif
        wxFileName wfnDestFixDir;
        wfnDestFixDir.AssignDir( wfnDestFix.GetPath() );
        // create the base dir
        if( !wfnDestFixDir.Mkdir( 0777, wxPATH_MKDIR_FULL ) )
        {
          wxString wsT = _("Can't create the directory:\n") + wfnDestFix.GetFullPath();
          wxMessageBox(  wsT, _("Error"), wxOK | wxICON_EXCLAMATION );
          wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 );  // verbosity
          event.SetString( wsT );
          wxQueueEvent( m_frame, event.Clone() );
          return NULL;
        }
        // must also copy the file after we created the path for it.
#ifdef _DEBUG
        wsTT = wfnDestFix.GetFullPath();
        wsTSrc = wfnSrc.GetFullPath();
        // used in zipping
        wsT1 = m_frame->m_vFiles[szI];
        wsT2 = m_frame->m_wfnDest.GetFullPath();
        wsT3 = wfnDestFix.GetFullPath();
#endif
        // we need/ought to check to ensure a file with the same
        // name & path does not exist already;
        // this is a very likely occurrence when multiple source
        // directories with related data are part of the same job file
        // such as d:\borland\bc3 & d:\borland\bc4
        if ( wfnDestFix.FileExists() )
        {
          wxString wsT;
          wxString wsT2;
#if defined( _DEBUG )
          // string is empty for release
          wsT2.Printf( _T("\nFile: %s, %d"), __FILE__, __LINE__ );
#endif
          wsT.Printf(  _("File %s already exists!\nOverwrite it?%s"),
            wfnDestFix.GetFullPath(), wsT2 );
          int i = wxMessageBox( wsT, _T("Notice"), wxYES_NO | wxICON_QUESTION );
          if ( i != wxYES )
          {
            wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
            // send a log message to the main frame log window
            event.SetInt( MY_RED );     // my color enums
            event.SetExtraLong( 5 );    // verbosity
            event.SetString(wsT  );
            wxQueueEvent( m_frame, event.Clone() );
            return false;
          }
        }
        m_frame->SetStatusText( wfnSrc.GetFullPath(), 1 );
        bool bRet;
        wxString wsTT1 = wfnDestFix.GetFullPath(); //??
        if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
        {
          wxLogMessage( _("\nSrc: %s\nDest: %s"), wfnSrc.GetFullPath(), wfnDestFix.GetFullPath());
        }
        bRet = m_frame->MyFileCopy( wfnSrc.GetFullPath(), wfnDestFix.GetFullPath(), true );
        if( bRet == false )
        {
          wxString wsT;
          wsT.Printf( _("ThreadEntry: MyCopyFile returned error for %s to %s\nStop?"),
            wfnDestFix.GetFullPath(), wfnSrc.GetFullPath() );
          int iAnswer = wxMessageBox(  wsT, _("Error"), wxYES_NO | wxICON_ERROR );
          if( iAnswer == wxYES )
          {
            wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
            wxGetApp().m_frame->m_bCancelled;
            return NULL;
          }
          continue; // ignore this file & go to next one
        }
        // get file size
        bRet = GetFileSize( wfnSrc.GetFullPath(), ullFileSize );
        wxASSERT( bRet );
        // add it to the list       Dest full path
        //                              V                         V source full path
        m_frame->InsertFileInZipList( wfnDestFix.GetFullPath(), wfnSrc.GetFullPath(),
          ullFileSize, ullFileSize );
        m_frame->m_ullTotalZipsSize += ullFileSize;
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
        // send a log message to the main frame log window
        event.SetInt( MY_BLACK );   // my color enums
        event.SetExtraLong( 2 );    // verbosity
        event.SetString( _("Copied: ") + wfnSrc.GetFullPath() +
        _T(" -> ") + wfnDestFix.GetFullPath() );
        wxQueueEvent( m_frame, event.Clone() );
      }
      else // zip the file
      {
#ifdef _DEBUG
        wsT1 = m_frame->m_vFiles[szI];
        wsT2 = m_frame->m_wfnDest.GetFullPath();
#endif
        // zip up file & add to sorted zip list
        m_frame->SetStatusText( m_frame->m_vFiles[szI], 1 );
        bOK = ZipNBackupFile( m_frame->m_vFiles[szI], m_frame->m_wfnDest.GetFullPath(), szI );
      }
    }
#if defined( WANT_BACKUP_TYPE )
    if ( m_bFullBackup )   // if it is full backup, reset 'archive' bit
    {
      dwAttr = GetFileAttributes( m_frame->m_vFiles[szI].wc_str() );
      dwAttr = dwAttr & (~FILE_ATTRIBUTE_ARCHIVE);
      bool bRet = SetFileAttributes( m_frame->m_vFiles[szI].wc_str(), dwAttr ) ? true : false;
#if _DEBUG
      dwAttr = GetFileAttributes( m_frame->m_vFiles[szI].wc_str() );
#endif
    }
#endif
#if 1 // don't show in GUI
    wxString wsT1;
    m_frame->m_szNFilesProcessed++;
    m_frame->GetGaugeProgress()->SetValue(
      (m_frame->m_szNFilesProcessed * 100)/m_frame->m_szNTotalFiles );
    // and the taskbar icon's as well
    m_frame->GetAppProgressInd()->SetValue(
      (m_frame->m_szNFilesProcessed * 100)/m_frame->m_szNTotalFiles );
    wxString wsCount;
    float fPercent = ((float)(m_frame->m_szNFilesProcessed) * 100 ) /
      (float)(m_frame->m_szNTotalFiles);
    if ( bJustMoveFile || g_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal )
      wsT1 = _("Copying: ");
    else
      wsT1 = _("Zipping:  ");
    wsCount.Printf( _("%s  %Id/%Id - %3.1f%%"), wsT1, m_frame->m_szNFilesProcessed,
      m_frame->m_szNTotalFiles, fPercent );
    m_frame->SetStatusText( wsCount, 0 );
#else
    // now show the data in GUI
    // for large # of files > 60,000 we run out of memory
    //::  InsertItemInReportView( szI, wsSrcFile, wfnZipFile.GetFullPath(), wsSizeStr );
#endif
  }
  if ( m_frame->IsWorkerCancelled() )
  {
    m_frame->SetStatusText( wxEmptyString, 0 );
	  return NULL;
  }
  wxGetApp().m_lZippingTime = wxGetApp().m_stopWatch.Time();
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
  {
    // causes infinite loop
//    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_frame->SetStatusText( _("Dumping ZipList"), 0 );
    m_frame->DumpZipList();
  }
  wxString wsT;
#if defined ( LOG_EVENT )
  // define the new event data ahead of the switch
  // to avoid compiler complaints
  wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
#endif
  // now split the files into as many bags as necessary
  // if 'splitting' is requested
  m_frame->SetStatusText( wxEmptyString, 0 );
  bool bRet = false;
  int iBagType = EBupBagSizeTypes::AsEnum(
      g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
  switch ( iBagType )
  {
    case EBupBagSizeTypes::eBagSize_Custom:
      wsT = _T("Custom 'bag' size not implemented yet!!");
      wxMessageBox( wsT, _T("Error"), wxOK | wxICON_ERROR);
      // send a log message to
      event.SetInt( MY_RED ); // my color enums
      event.SetString( wsT );
      wxQueueEvent( m_frame, event.Clone() );
      break;
    case EBupBagSizeTypes::eBagSize_144MB:
    case EBupBagSizeTypes::eBagSize_5MB:
    case EBupBagSizeTypes::eBagSize_10MB:
    case EBupBagSizeTypes::eBagSize_650MB:
    case EBupBagSizeTypes::eBagSize_700MB:
    case EBupBagSizeTypes::eBagSize_3_7GB:
    case EBupBagSizeTypes::eBagSize_4_7GB:
      bRet = SplitFilesIntoBags();
      if ( bRet == false )
        return NULL;
      // check if we were asked to exit
      if ( TestDestroy() || wxGetApp().m_frame->m_bCancelled )
          break;
      wxGetApp().m_lBaggingTime = wxGetApp().m_stopWatch.Time();
      break;
    case EBupBagSizeTypes::eBagSize_8_5GB:
    case EBupBagSizeTypes::eBagSize_25GB:
    default:
      wsT.Printf( _T("Split - 'bag' size %s is not supported"),
        g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
      wxMessageBox( wsT, _T("Error"), wxOK | wxICON_ERROR );
      if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 1 )
      {
        // send a log message to
        event.SetInt( MY_RED ); // my color enums
        event.SetString( wsT );
        wxQueueEvent( m_frame, event.Clone() );
      }
      break;
  }
  if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    wxString wsT;
     wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( _("Finish Job - ") + wsT + _T("================================") );
  }
  return NULL;
}
// -------------------------------------------------------------
/**
 * Called via the event loop when 'Backup' is clicked - OnStartWorker handler.
 *
 * Data input is via the global structure g_jobData which is filled in when a
 * job is selected.
 * Eventually calls void *MyWorkerThread::Entry() above
 */
void MyFrame::OnStartWorker(wxCommandEvent& WXUNUSED(event))
{
  int         n = 0;
  wxString    wsMsg;

  m_ullTotalSrcBytes = m_ullTotalZipBytes = 0ll;
  m_szNTotalFiles = m_szNFilesProcessed = 0;
  m_ulDirs = m_ulBags = m_ulMaxDestFileNameLen = m_ulMaxDestPathLen = 0l;
  m_uiSrcDirLevelDepth = m_uiZipDirLevelDepth = 0;
  m_wsLongestDestFileName = m_wsLongestDestPath = wxEmptyString;
  m_bHadError = false;
  GetGaugeProgress()->SetValue( 0 );
  // and the taskbar icon's as well
  GetAppProgressInd()->SetValue( 0 );
  m_ullTicks = 0ll;
  m_timer.Start( 1000 ); // 1 sec ticks
#if defined( WANT_LOG )
  if( g_iniPrefs.data[IE_LOG_CLEAR_AT_START].dataCurrent.bVal )
  {
    m_textCtrlLog->Clear();
    GetStatusBar()->SetStatusText( _T(""), 1 );
  }
#endif
  m_textCtrlSpecialFiles->Clear();
  m_textCtrlErrors->Clear();
  UpdateSummary( wxEmptyString, wxEmptyString, wxGetApp().m_wsConfigDir );
  // check if m_ullSelectedBagSize = 0
  if ( m_ullSelectedBagSize == 0ll )
  {
    wxString wsT;
    wsT.Printf( _T("m_ullSelectedBagSize == 0") );
    wxMessageBox( wsT );
    GetStatusBar()->SetStatusText( _T("") );
    wxLogMessage( wsT );
    return;
  }
  // ensure the dest path is seen as a directory
  wxString wsDest = g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
  if( wsDest.IsEmpty() )
  {
    wxString wsT = _T("Destination path is empty!\nPlease correct.");
    wxMessageBox( wsT, _T("Error"), wxOK | wxICON_EXCLAMATION );
    if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 0 )
    {
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
      wxLogError( wsT );
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxBLACK));
    }
    return;
  }
  wxString wsTest = g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
  // expand any of the macros/templates used for the destination direcory
  ExpandPath( wsTest );
  m_wfnDest.AssignDir( wsTest );

  // make sure that the destination directory exists
  if (!m_wfnDest.DirExists())
  {
    wxString wsT;
    if ( ! wxGetApp().m_bQuietMode )
    {
      wsT = _T("Destination ") + m_wfnDest.GetFullPath() +
        _(" does not exist! Create?.");
      int answer = wxMessageBox( wsT, _("Confirm"), wxYES_NO | wxICON_QUESTION );
      if (answer != wxYES)
        return;
    }
    // else create the directory
    bool bRet = m_wfnDest.Mkdir( wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL );
    if ( !bRet )
    {
      wsT.Printf( _("Failed to create dir: %s"), m_wfnDest.GetFullPath() );
      wxMessageBox( wsT, _("Notice"), wxOK |wxICON_EXCLAMATION );
      return;
    }
  }
  // check the destination path length
  if ( m_wfnDest.IsOk() )
  {
    DWORD dwDestFlags;
    DWORD dwLen;
    wxString wsDestVol = m_wfnDest.GetVolume();
    wsDestVol += _T(":\\" );
    if( !wsDestVol.IsEmpty() && GetVolumeInformation( wsDestVol.wc_str(wxConvUTF8),
      NULL, 0, NULL, &dwLen, &dwDestFlags, NULL, 0 ) )
    {
      m_ulMaxDestPathCompLen = dwLen;
    }
    else
      m_ulMaxDestPathCompLen = 0;
  }
  wxLogMessage( wsMsg );
  UpdateSummary( wxEmptyString, wxGetApp().m_wsConfigDir, wxEmptyString );
  if ( g_iniPrefs.data[IE_LAST_DIR].dataCurrent.wsVal.IsEmpty() )
  {
    wxString wsMsg = _T("Base directory for scanning is empty!");
    (void)wxMessageBox( wsMsg, _T("Error"), wxICON_INFORMATION | wxOK );
    wxLogError( wsMsg );
    m_bStopScan = false;
    return;
  }
#if 1   // warn user if an interfering task is running
  // parse the interfering tasks list and check the appropriate boxes
  std::vector<ProcessEntry> proclist;
  ProcessEntry pe;
  GetProcessList( proclist );
  int iSize = proclist.size();
  wxStringTokenizer wTk( g_jobData.data[IEJ_INTERFERING_TASKS].dataCurrent.wsVal, ";" );
  int i = 0;
  while ( wTk.HasMoreTokens() )
  {
    wxString wsToken = wTk.GetNextToken();
    // process token here
    if (wsToken.IsSameAs(_T("Thunderbird")))
    {
      // scan the process list for this task
      for ( unsigned int i = 0; i < proclist.size(); i++ )
      {
        pe = proclist.at( i );
        if( pe.name.IsSameAs( _T("thunderbird.exe") ) )
        {
          wxLogWarning( _("ThunderBird is running") );
          /* see msgdlg.h for some comments about message box icons etc.
           */
          int iResponse = wxMessageBox(
            _("ThunderBird is running\nPlease save all work in progress\n") +
            _("Click 'OK' to stop & exit Thunderbird and continue the backup job\n") +
            _("Click 'Cancel' to stop the backup job and keep Thunderbird running."),
            _("Confirm"),  wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING );
          if( iResponse != wxOK )
          {
            wxLogMessage( _("Backup job cancelled") );
            return;
          }
          // else kill Thunderbird and carry on
          wxKillError keError;
          int iErr = wxKill( pe.pid, wxSIGTERM, &keError, wxKILL_CHILDREN );
          if ( iErr == 0 )
          {
            wxLogMessage( _("Thunderbird killed") );
            wxLogMessage( _("Backup job continues") );
          }
          else
          {
            wxString wsT1;
            switch ( keError )
            {
            case wxKILL_BAD_SIGNAL:
              wsT1 = _("No such signal" );
              break;
            case wxKILL_ACCESS_DENIED:
              wsT1 = _("Permission denied" );
              break;
            case wxKILL_NO_PROCESS:
              wsT1 = _("No such process" );
              break;
            case wxKILL_ERROR:
            default:
              wsT1 = _("Unspecified error" );
              break;
            }
            m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxRED) );
            wxLogError( _("Stopping Thunderbird failed: ") + wsT1 );
            m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxBLACK ));
          }
        }
#if 0
        if( pe.name.IsSameAs( _T("MailClient.exe") ) )
        {
          wxLogMessage( _("eM-Client is running\n") );
        }
#endif
      }
    }
  }
#endif
  // 'detached' threads clean up & delete themselves when they are done
  // but MUST be created on the HEAP with new()
  MyWorkerThread *thread = new MyWorkerThread(this); //<<< DETACHED is default!!

  if ( thread->Create() != wxTHREAD_NO_ERROR )
  {
    wxLogError( _T("Can't create thread!") );
    return;
  }
  // returns 50 - the default
//  unsigned int i = thread->GetPriority();
#if defined( WANT_GRID )
  m_grid->ClearGrid();   // clear any old data
  n =  m_grid->GetNumberRows();
  if( n )
    m_grid->DeleteRows( 0, n );
#endif
  // thread is not running yet, no need for crit sect
  m_bStopScan = false;
  m_bCancelled = false;
  m_bRunning = true;
  SetStatusText( wxEmptyString, 1 );
  m_JobTypeInProgress = MY_BACKUP_JOB;
#if defined( WANT_POWER_BLOCKER )
  m_pShutDownBlocker = new
    wxPowerResourceBlocker( wxPOWER_RESOURCE_SYSTEM, "Backup job in progress");
  if ( !m_pShutDownBlocker->IsInEffect() )
  {
    // If the resource could not be acquired, tell the user that he has
    // to keep the system alive
    wxLogMessage("Warning: system may suspend while backing up.");
  }
#endif
  thread->Run();
}

// -------------------------------------------------------------
/**
 * Check if the worker thread is still running.
 */
bool MyFrame::IsWorkerCancelled()
{
  wxCriticalSectionLocker lock( m_critsectWork );
  return m_bStopScan;
}

// -------------------------------------------------------------
/**
 * Check if the worker thread is still running.
 * Note: in most places m_bRunning is already protected by
 * the critical section
 */
bool MyFrame::IsWorkerRunning()
{
  wxCriticalSectionLocker lock( m_critsectWork );
  return m_bRunning;
}

// ------------------------------- eof -------------------------
