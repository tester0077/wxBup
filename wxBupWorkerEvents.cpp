/*-----------------------------------------------------------------
 * Name:        wxBupWorkerEvents.cpp
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
#include <wx/dir.h>
#include <algorithm>
#if defined( _MSC_VER ) // only good for Windows
#include <PowrProf.h>
#endif
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
#if defined( EXIT_EVENT )
// ------------------------------------------------------------------
/**
 * We  come here at the end of the job.
 */
void MyFrame::OnWorkerEventDone(wxThreadEvent& event)
{
  int n = event.GetInt();
  long        lLong = event.GetExtraLong();
  long        lCause =  (lLong & ~0xFF);
  wxString    wsT = event.GetString();
  wxString wsBagDir;
  if ( n == -1 )
  {
    wxString wsT;
    m_bRunning = false;
#if defined( WANT_POWER_BLOCKER )
    if( m_pShutDownBlocker )
    {
      delete m_pShutDownBlocker;
      m_pShutDownBlocker = NULL;
    }
#endif
    m_timer.Stop();
    if ( m_szNFilesProcessed < m_szNTotalFiles )
    {
      //wsT = _T("Interrupted by user!");
      wsT.Printf( _T("Interrupted by user! Processed: %d, out of %d"),
        m_szNFilesProcessed, m_szNTotalFiles );
      SetStatusText( wsT , 0 );
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
      wxLogMessage( wsT );
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxBLACK));
    }
    else
      SetStatusText( _T("Backup done!"), 0 );
    SetStatusText( wxEmptyString, 1 );
    // clear elapsed time
    SetStatusText( _T(""), 2 );
    // clear the progress gauge
    GetGaugeProgress()->SetValue( 0 );
    // and the taskbar icon's as well
    GetAppProgressInd()->SetValue( 0 );
    m_JobTypeInProgress = MY_JOB_TYPE_NONE;
    if ( m_bHadError )
    {
      if ( m_bSkippedFiles )
      {
        wsT.Printf( _T("Some files files were skipped & there were error!\nVerify?") );
        int answer = wxMessageBox( wsT, _T("COnfirm!"), wxYES_NO | wxICON_QUESTION );
        if ( answer != wxYES )
          return;
      }
      // else carry on
    }
    if ( m_bCancelled )
    {
      SetStatusText( _T("Job cancelled!"), 0 );
      return; // nothing left to do
    }
    if ( !m_bRunning && !m_bCancelled )  // thread exited cleanly
    {
#if defined( _MSC_VER ) // only good for Windows
      _flushall();  // needed to ensure we override MS lazy flushing of
      /// disk buffers
#endif
      // Moved outside the thread
      // clean & test for skipped files
      // this could get very time consuming for large # of source file
      // check if the original destination directory is empty
      // count the files and directories above the original source drive component only

#if defined( WANT_DATE_DIR )
      wsBagDir = m_wfnDest.GetFullPath();
      // we don't want to clean out what we just 'bagged' but only
      // the zipped up data
      // but first check we have a valid string for the base directory
      wxCHECK2_MSG( !g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal.IsEmpty(),
        return, _("Base directory string is empty") );
      m_wfnDest.AppendDir( g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal );
      wsBagDir = m_wfnDest.GetFullPath();
#else
      wxFileName wfnDestDir;
      // make sure the destination is treated as a directory
      wfnDestDir.AssignDir(
        g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );
      wxString wsDest = wfnDestDir.GetFullPath();
      // we don't want to clean out what we just 'bagged' but only
      // the zipped up data
      // but first check we have a valid string for the base directory
      wxCHECK2_MSG( !g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal.IsEmpty(),
        return, _("Base directory string is empty") );
      wfnDestDir.AppendDir( g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal );
      wsDest = wfnDestDir.GetFullPath();
#endif
      // don't clean up source if MoveOnly, but do clean up for Zipping
      if ( !g_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal )
      {
        wxBusyCursor wait;
        wxString wsT;
        wsT = _("Cleaning up the temporary directory \"") +
#if defined( WANT_DATE_DIR )
        wsBagDir + _T("\"");
#else
        wsDest + _T("\"");
#endif
        GetStatusBar()->SetStatusText( _T("Cleaning up! "), 0 );
        wxLogMessage( wsT );
#if defined( WANT_DATE_DIR )
        bool bRet = m_wfnDest.Rmdir( wsBagDir, wxPATH_RMDIR_RECURSIVE );
#else
        bool bRet = wfnDestDir.Rmdir( wsDest, wxPATH_RMDIR_RECURSIVE );
#endif
        if ( !bRet )
        {
          GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
          wxLogError(wsT + _(" failed!") );
          GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxBLACK));
        }
      }
      // move all of the bag logs to each bag
      // the logs, if requested, ought to exist in each base directory
      // once we copy all logs to each bag, we can no longer tell
      // the bag name directly from the one bag log.
      // Thus we create an empty file with the bag name
      // the bag log is always wanted/needed
      // no testing of backup integrity without it
      {
        size_t i, j;
        for (i = 0; i < m_wasBags.GetCount(); i++)
        {
          wxFileName wfnBagLog( m_wasBags[i] );
          wxFileName wfnBagName = wfnBagLog;
          wxFileName wfnDest;
          wxFileName wfnDestLog;
          wfnBagName.SetExt( _T("") );
          wxTextFile wtBagName( wfnBagName.GetFullPath() );
          wxString wsBagName = wfnBagLog.GetName();
          if (!wfnBagLog.Exists())
          {
            wxString wsT;
            wsT.Printf(_("Cannot find bag log %s!"),  wfnBagLog.GetFullPath() );
            GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
            wxLogWarning( wsT );
            GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxBLACK));
            continue; // try the next one
          }
          wtBagName.Create();
          wtBagName.Close();
          // now copy all logs to the other bags
          for (j = 0; j < m_wasBags.GetCount(); j++)
          {
            if ( i == j )
              continue;  // the current bag needs no copying
            // make up the destination path
            wfnDest.Assign( m_wasBags[j] );
            wfnDestLog.SetPath( wfnDest.GetPath() );
            wfnDestLog.SetFullName( wfnBagLog.GetFullName() );
            MyFileCopy( m_wasBags[i], wfnDestLog.GetFullPath(), true );
          }
        }
        for (i = 0; i < m_wasBags.GetCount(); i++)
        {
          wxFileName wfnBag( m_wasBags[i] );
          wxFileName wfnBagDir( wfnBag.GetPath() );
          // returns the size in bytes and agrees with the value returned
          // by the 'right-click 'properties' in Win 10 file explorer
          // but does not give the 'size on disk'
          wxULongLong wull = wxDir::GetTotalSize( wfnBag.GetPath() );
        }
      }
    }
    // display summary data
    wxString wsT1;
    wxFileName wfnJobFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
    // display the stopwatch times
    long lCleanupTime   = std::max( (int)(wxGetApp().m_lCleanupTime/1000), 0 );
    long lFileCountTime = std::max( (int)( wxGetApp().m_lFileCountTime/1000), 0 );
    long lZippingTime   = std::max( (int)(wxGetApp().m_lZippingTime/1000), 0 );
    long lBaggingTime   = std::max( (int)(wxGetApp().m_lBaggingTime/1000), 0 );

    wxString wsCleanupTime;
    wxString wsCountTime;
    wxString wsZipTime;
    wxString wsBagTime;

    TicksToString( lCleanupTime, wsCleanupTime );
    TicksToString( lFileCountTime - lCleanupTime, wsCountTime );
    TicksToString( lZippingTime   - lFileCountTime, wsZipTime );
    TicksToString( lBaggingTime   - lZippingTime, wsBagTime );
    TicksToString( lZippingTime, m_wsTotalTime );
    m_wsAllTimes.Printf( _("Cleanup: %s, Count: %s, Zip: %s, Bag: %s"),
      wsCleanupTime, wsCountTime, wsZipTime, wsBagTime );
    wxLogMessage(_("Cleanup: %s, Count: %s, Zip: %s, Bag: %s, Total: %s"),
      wsCleanupTime, wsCountTime, wsZipTime, wsBagTime, m_wsTotalTime );

    // total time - already displayed
    UpdateSummary( wfnJobFile.GetFullName(), wxGetApp().m_wsConfigDir, m_wsAllTimes );

    wxLogMessage( _T("Backup done!") );
    SetStatusText( _T("Backup done! ") + m_wsTotalTime, 0 );
  }

  // show the last log line
  // - GetNumberOfLines does not work
  // see wxWidgets documentation
  //m_textCtrlLog->ShowPosition( m_textCtrlLog->GetNumberOfLines() );
  while ( m_textCtrlLog->ScrollLines( 1 ) )
    ;
  m_panelSummary->Layout();
  // see if we want to verify the job
  if ( g_jobData.data[IEJ_VERIFY_JOB].dataCurrent.bVal &&
     ( m_szNFilesProcessed ) )
  {
    if ( IsWorkerCancelled() )
      return;
    // this path still contains the 'bag' directory as its last component;
    // it needs to be stripped off
    size_t n = m_wfnDest.GetDirCount();
    m_wfnDest.RemoveDir( n - 1 );
#if defined( _DEBUG )
    wxString wsTTT = m_wfnDest.GetPath();
#endif
    wxLogMessage( _T("Starting test!") );
    SetStatusText( _T("Starting test! "), 0 );
    RunTest( m_wfnDest.GetPath() );  //<<<<<<<<<<<<<<<<<<<<<
    // we MUST wait until test is done else we could shut down
    // the app or the machine before the work has been completed
    while( IsWorkerRunning() )
      wxYield();//wxSleep( 1 );
    // we have run the tests, so, if we are logging,
    // its time to close the log
    if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
    {
      if ( wxGetApp().m_pAppLog->IsLogFileOK() )
      {
        wxGetApp().m_pAppLog->CloseLog();
      }
    }
  }
  else// we did not want to test the job data
  {
    if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
    {
      if ( wxGetApp().m_pAppLog->IsLogFileOK() )
      {
        wxGetApp().m_pAppLog->CloseLog();
      }
    }
  }
#if defined( WANT_SOUND )
  if ( g_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal )
  {
    // use the 'file' variable, rather than the 'resource' variable
    if ( m_bHadError )
    {
      m_wsSoundFile = g_iniPrefs.data[IE_SOUND_FILE_ERROR].dataCurrent.wsVal;
    }
    else
    {
      m_wsSoundFile = g_iniPrefs.data[IE_SOUND_FILE_DONE].dataCurrent.wsVal;
    }
    if ( !m_wsSoundFile.IsEmpty() )
    {
#if defined( WANT_MEMORY_SOUND )
      m_bUseMemory = false;
#endif
      if ( !m_pSound )
      {
        m_pSound = new wxSound;
      }
      else
        m_pSound->Stop(); // stop any other sounds
      CreateSound(*m_pSound);
      if (m_pSound->IsOk())
        m_pSound->Play(wxSOUND_SYNC);
      wxDELETE(m_pSound);
      m_pSound = NULL;
    }
  }
#endif
#if defined( WANT_LAST_JOB_DATE )
  // now we can update the last job date without the  modifications
  // to the job file causing the integrity test to fail.
  wxDateTime dt = wxDateTime::Now();
  wxString wsDate = dt.FormatISODate();
  wxString wsTime = dt.FormatISOTime();
  g_jobData.data[IEJ_LAST_BACKUP_DATE].dataCurrent.wsVal =
    wsDate + _T("-") + wsTime;
  wxGetApp().SaveCloseJobConfig( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
  // update the job list
  FillJobList();
#endif
  // check if the user wants email feedback
  if ( g_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal )
  {
    if ( (m_bHadError && g_jobData.data[IEJ_EMAIL_ERROR_ONLY].dataCurrent.bVal)
      || ! g_jobData.data[IEJ_EMAIL_ERROR_ONLY].dataCurrent.bVal )
    {
      SendEmail();
    }
  }
  // Autoshutdown at end of command line job
  if( wxGetApp().m_bConsoleMode )
  {
    wxGetApp().ExitMainLoop();
  }
  else // see what the user wants us to do at the end of a job
  {
    if ( m_checkBoxWhenDone->IsChecked() )
    {
      wxWindow *pTopLevel;
      int i = m_choiceWhenDone->GetSelection() + 1; // make up for 'unknown == 0
      wxASSERT( (i >= (EWhenDoneTypes::eWhenDone_Unknown )) &&
        (i <= EWhenDoneTypes::eWhenDone_Shutdown) );
      switch ( i )
      {
      case EWhenDoneTypes::eWhenDone_Exit:
        pTopLevel = wxGetApp().GetTopWindow();
        pTopLevel->Destroy();
        break;
      case EWhenDoneTypes::eWhenDone_Hibernate:
        if( IsPwrHibernateAllowed() )
        {
          if ( SetSuspendState( true, true, false ) )
            break;
        }
        // do nothing if it fails
        break;
      case EWhenDoneTypes::eWhenDone_LogOff:
        ExitWindows( 0, 0 );
        break;
      case EWhenDoneTypes::eWhenDone_Shutdown:
        ::wxShutdown( wxSHUTDOWN_POWEROFF );
        break;
      // Sleep is not implemented because I don't know how
      // to do this - yet
      //case EWhenDoneTypes::eWhenDone_Sleep:
      case EWhenDoneTypes::eWhenDone_Unknown:
      case EWhenDoneTypes::eWhenDone_DoNothing:
      default:
        break;
      }

    }
  }
}
#endif  // EXIT_EVENT

// ------------------------------------------------------------------
void MyFrame::TicksToString( long lTicks, wxString& wsT )
{
  long lSecs = (lTicks)%60;
  long lMins = (lTicks/(60))%60;
  long lHours = ((lTicks)/(60*60))%24;
  wsT.Printf( _T("%2ld:%02ld:%02ld" ), lHours, lMins, lSecs );
}

// ------------------------------- eof ------------------------------
