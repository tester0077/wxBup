/*--------------------------------------------------------------
 * Name:        wxBupTestRestoreThread.cpp
 * Purpose:     worker thread for the backup & zip utility
 *              handles the testing & restoring work as a separate thread
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 * -------------------------------------------------------------- */

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
#include "wx/dir.h"

#include "wxBuph.h"
#include "wxBupFrameh.h"
#include "wxBupConfh.h"

#include "wxBupTestRestoreThreadh.h"
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
// ------------------------------------------------------------------
// restore/test thread

// ------------------------------------------------------------------

MyRestoreThread::MyRestoreThread(MyFrame *frame)
        : wxThread()
{
  m_frame = frame;
  m_ulTotalFilesChecked = m_ulCheckedFilesGood =
      m_ulCheckedFilesFailed = m_ulTotalFiles =
    m_ulTotalFilesToCheck = 0l;
  m_bRestoring = false;
}

// ------------------------------------------------------------------

void MyRestoreThread::SetThreadNextBagData( unsigned long ulTotalFilesToCheck, wxString wsBagLogPath )
{
  MyBagLogDataEl *pMEl;
  pMEl = new MyBagLogDataEl( ulTotalFilesToCheck, wsBagLogPath );
  m_ThreadBagList.push_back( *pMEl );
  delete pMEl;  // the list makes a copy
}

// -------------------------------------------------------------
/** called when the thread exits - whether it terminates normally 
 * or is stopped with Delete() (but not when it is Kill()ed!)
 */
void MyRestoreThread::OnExit()
{
#if defined ( RESTORE_EXIT_EVENT )
  // it works equally well, AFAICT, here or at end of Entry()
  wxThreadEvent event( wxEVT_THREAD, RESTORE_WORKER_EVENT_EXIT );
  event.SetInt( -1 ); // that's all
  wxQueueEvent( m_frame, event.Clone() ); // continues @: OnRestoreEventDone
#else
  m_frame->GetGaugeProgress()->SetValue( 0 );
#endif
  if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    wxString wsT;
#if 1
    wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( _("Finish Test - ") + wsT + _T("================================") );
#else
    wxString wsDate;
    wxString wsT;
    wxDateTime dt = wxDateTime::Now();
    wsDate = dt.FormatISODate();  // note: time is part of log line already
    wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( wsDate +  _(" - Finish Test - ") + wsT + 
      _T("================================") );
#endif
  }
}

// -------------------------------------------------------------
/** This routine does all the test/restore work.
 * Input is via m_ThreadBagList
 * 
 */
void *MyRestoreThread::Entry()
{
  // if source root has a bag log, we have a medium i.e.
  // data has been copied to CD, DVD or floppy
  // we don't have bag directories => one level less to worry about
  int  bHaveBags = false; 
  wxString wsBagName;
  wxArrayString wasBags;
  wxString wsBagLogName;
  wxString wsAction = m_bRestoring ? _("Restoring") : _("Checking");
  unsigned long         ulTotalFilesChecked;
  unsigned long         ulCheckedFilesGood;
  unsigned long         ulCheckedFilesFailed;
  if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    wxString wsT;
#if 1
    wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( _("Starting Test - ") + wsT + _T("================================") );
#else
    wxString wsDate;
    
    wxDateTime dt = wxDateTime::Now();
    wsDate = dt.FormatISODate();  // note: time is part of log line already
    wsT.Printf( _T("wxBUP ver. %d.%d, build %d, verbosity: %ld "), giMajorVersion,
      giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
    wxLogMessage( wsDate +  _(" - Starting Test - ") + wsT + 
      _T("================================") );
#endif
  }
  int i = 0;
  ulTotalFilesChecked = ulCheckedFilesGood = ulCheckedFilesFailed = 0l;
  // run the thread for each list item
  for (std::list<MyBagLogDataEl>::iterator iter = m_ThreadBagList.begin(); 
    iter != m_ThreadBagList.end();  /* see note above */)
  {
    m_ulTotalFilesChecked = m_ulCheckedFilesGood = m_ulCheckedFilesFailed = 0l;
    m_ulTotalFilesToCheck = iter->m_ulTotalFilesToCheck;
    wxLogMessage( _T("%s bag %d:  %s with %ld files"), 
      wsAction, ++i,
      iter->m_wsBagLogPath, iter->m_ulTotalFilesToCheck );
    RestoreOrTestOneBag( iter->m_wsBagLogPath );
    ulTotalFilesChecked   += m_ulTotalFilesChecked;
    ulCheckedFilesGood    += m_ulCheckedFilesGood;
    ulCheckedFilesFailed  += m_ulCheckedFilesFailed;
    iter++;
  }
  wxLogMessage( _T("Total files checked: %ld, Files good: %ld, Files failed: %ld"), 
    ulTotalFilesChecked, ulCheckedFilesGood, ulCheckedFilesFailed );
  return NULL;
}

// ------------------------------- eof -------------------------
