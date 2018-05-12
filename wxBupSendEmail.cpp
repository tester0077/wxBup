/*-----------------------------------------------------------------
 * Name:        wxBupSendEmail.cpp
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
#include "wxBupFrameh.h" // needs to be first
#include "wxBupWizSendEmailh.h"
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

void MyFrame::SendEmail()
{
#if defined( WANT_CURL )
  // feedback for user via statusbar
  wxString wsT;
  m_bSendingMailInProgress = true;
  wsT.Printf( _("Preparing to send e-mail notification") );
  GetStatusBar()->SetStatusText( wsT, 0 );
  // reset the progress gauge for user feedback
  GetGaugeProgress()->SetValue( 0 );
  // and the taskbar icon's as well
  GetAppProgressInd()->SetValue( 0 );
  wxString wsError;
  // need to use the data as shown in the dialog, in case user has modified it
  wxMailer mailer( 
    g_jobData.data[IEJ_EMAIL_TO].dataCurrent.wsVal,    // 'To:'
    wxEmptyString,                                     // password - not needed
    g_jobData.data[IEJ_EMAIL_SMTP].dataCurrent.wsVal );// SMTP server
  wxEmailMessage message;
  if ( m_bHadError )
  {
    wsError = _(" - Errors");
  }
  wxString wsAsciiMessage;
  ComposeAsciiMessage( wsAsciiMessage );
  message.SetFrom( g_jobData.data[IEJ_EMAIL_FROM].dataCurrent.wsVal )
    .SetTo( g_jobData.data[IEJ_EMAIL_TO].dataCurrent.wsVal )
#if 1
    .SetMessage( wsAsciiMessage )
#else
    .SetMessage(_T("Message") )
#endif
    .SetSubject( g_jobData.data[IEJ_EMAIL_SUBJECT].dataCurrent.wsVal + wsError  );

  // handle log file attachment here
  if (g_jobData.data[IEJ_EMAIL_ADD_LOG].dataCurrent.bVal )
  {
    wxString wsCurLog = wxGetApp().m_wsCurLogFileName;
    message.AddAttachment( wsCurLog );
  }
  if ( !message.Finalize() ) // Must finalize the message before sending it
    return;   // user aborted sending the message
  if(mailer.Send(message))
  {
    wxString wsT( _("Message Sent Successfully!") );
    wxTextAttr taOld = m_textCtrlLog->GetDefaultStyle();
    wxTextAttr taCurrent = m_textCtrlLog->GetDefaultStyle();
    if ( m_bHadError )
    {
      taCurrent.SetTextColour( *wxRED );
      wsT += _( " - Job had errors!");
    }
    else
    {
      taCurrent.SetTextColour( *wxBLUE );
    }
    taCurrent.SetFontWeight( wxFONTWEIGHT_BOLD );
    m_textCtrlLog->SetDefaultStyle( taCurrent );
    wxLogMessage( wsT );
    m_textCtrlLog->SetDefaultStyle( taOld );
    GetStatusBar()->SetStatusText( wsT, 0 );
    wxGetApp().m_frame->GetStatusBar()->SetStatusText( _T(""), 1 );
    m_bSendingMailInProgress = false;
  }
#endif
}

// ------------------------------- eof ------------------------------
