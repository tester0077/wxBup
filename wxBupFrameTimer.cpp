/*-----------------------------------------------------------------
 * Name:        wxBupFrameTimer.cpp
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
// 
void MyFrame::OnIdle( wxIdleEvent &WXUNUSED(ev) )
{
  m_iNIdleCalls++;
  if( m_splitterMain->GetClientSize().GetWidth() > 20 &&
    ! m_bSplitterSetDone )
  {
    m_bSplitterSetDone = true;
    m_splitterMain->SetSashPosition(
      g_iniPrefs.data[IE_SASH_POS].dataCurrent.lVal, true );
    m_splitterJob->SetSashPosition(
      g_iniPrefs.data[IE_JOB_SASH_POS].dataCurrent.lVal, true );
//    Disconnect( wxID_ANY, wxEVT_IDLE );
    // were asked to start with the last configuration?
    // make sure we have one
    if( !g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal.IsEmpty() && 
        g_iniPrefs.data[IE_LAST_JOB_LOAD_AT_START].dataCurrent.bVal )
    {
      // make sure it exists
      if( wxFile::Exists( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal ) )
      {
          MyOpenFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
      }
    }
  }
  if ( m_iNIdleCalls > 10)  // we won't need these Idle calls any longer
  {
    Disconnect( wxID_ANY, wxEVT_IDLE );
  }
  wxString wsTT = wxGetApp().m_wsJobFileName;
  if ( m_iNIdleCalls > 1 )
  {
    Disconnect( wxID_ANY, wxEVT_IDLE );
  }
  if ( ( m_iNIdleCalls == 1 ) && wxGetApp().m_bHaveJobFile && 
    !wxGetApp().m_wsJobFileName.IsEmpty() )
  {
    // can we assume that the file exists if it has just been dropped?
    // not if it comes from the command line //todo
    // open the job file 
    wxString wsFile = wxGetApp().m_wsJobFileName;
    wxFileName wfnJobFile( wsFile );
    wxGetApp().RestoreJobConfig( wfnJobFile.GetFullPath() );
    // create any type of command event here
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, 
      MyFrameBase::wxID_TB_BACKUP );
    event.SetInt( 0 );
    // display a separator
    wxLogMessage( _T("---------------\n") );
    // send in a thread-safe way
    wxPostEvent( wxGetApp().m_frame, event );
  }
}

// ------------------------------------------------------------------
void MyFrame::OnTimer( wxTimerEvent& event )
{
  m_ullTicks++;
  // update the timer in the status bar
  wxString wsT;

  TicksToString( m_ullTicks, wsT );
  m_statusBar->SetStatusText( wsT, 2 );  
  // 0 => no updates to summary page
  if ( g_iniPrefs.data[IE_SUMMARY_UPDATE_INT].dataCurrent.lVal > 0 )
  {
    if ( (m_ullTicks % g_iniPrefs.data[IE_SUMMARY_UPDATE_INT].dataCurrent.lVal ) == 0 )
    {
      wxFileName wfnJobFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
      UpdateSummary( wfnJobFile.GetFullName(), wxGetApp().m_wsConfigDir,
        wxEmptyString );
    }
  }
}

// ------------------------------- eof ------------------------------
