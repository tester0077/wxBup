  /*-----------------------------------------------------------------
 * Name:        wxBupRestoreEvent.cpp
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
#if defined( RESTORE_EXIT_EVENT )
// ------------------------------------------------------------------
/**
 * We  come here at the end of the job.
 */
void MyFrame::OnRestoreEventDone(wxThreadEvent& event)
{
  int n = event.GetInt();
  wxString wsT;
  if ( n == -1 )
  {
    TicksToString( m_ullTicks, wsT );
    m_bRunning = false;
    m_timer.Stop(); 
    SetStatusText( _T("Done!"), 0 );
    // clear elapsed time
    SetStatusText( _T(""), 2 );
    // clear the progress gauge
    GetGaugeProgress()->SetValue( 0 );
    // and thew taskbar icon's as well
    GetAppProgressInd()->SetValue( 0 );
  }
  // show the last log line 
#if 0
  // - GetNumberOfLines does not work 
  // see wxWidgets documentation
  m_textCtrlLog->ShowPosition( m_textCtrlLog->GetNumberOfLines() );
#else
  while ( m_textCtrlLog->ScrollLines( 1 ) )
    ;
#endif
  // be sure to display the last job file name in the summary page
  wxFileName wfnJobFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
  UpdateSummary( wfnJobFile.GetFullName(), wxGetApp().m_wsConfigDir,
      wxEmptyString );
}
#endif

// ------------------------------- eof ------------------------------
