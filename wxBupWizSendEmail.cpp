/*-----------------------------------------------------------------
 * Name:        wxBupWizSendEmail.cpp
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
#include "wxBupWizTreebookh.h"
#include "wxBupWizSendEmailh.h"

#include <curl/curl.h>
#include <wx/msgdlg.h>
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to go AFTER all headers
#include <crtdbg.h>
#  ifdef _DEBUG
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  endif
#endif
#if defined( WANT_CURL )
// ------------------------------------------------------------------
wxMailer::wxMailer(const wxString& email, const wxString& password, const wxString& smtpURL)
  : m_email(email)
  , m_password(password)
  , m_smtp(smtpURL)
{
}

// ------------------------------------------------------------------

wxMailer::~wxMailer() {}

// ------------------------------------------------------------------

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp)
{
  wxEmailMessage* message = (wxEmailMessage*)userp;
  if( !message->HasMore()) 
  {
    return 0;
  }

  if((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
  {
    return 0;
  }
  size_t len = (message->Len() > (size * nmemb)) ? (size * nmemb) : message->Len();
  memcpy(ptr, message->AsChar(), len);
  message->Consume(len);
  return len;

}

// ------------------------------------------------------------------
/**
* Send the message.
* Note that, with my mods, the data from the 'incoming' message are not used
* at all, in fact the only reason for showing the 'main frame' is to use the 
* send button to initialte the 'send' function
* All the message data is supplied from the data defined above.
* Also note: no password is needed to send the message
*/
bool wxMailer::Send(const wxEmailMessage& message)
{
  CURL* curl;
  curl = curl_easy_init();

  if(curl) 
  {
    // Transport initialization
//    curl_easy_setopt(curl, CURLOPT_USERNAME, m_email.mb_str(wxConvUTF8).data());
//    curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.mb_str(wxConvUTF8).data());
    curl_easy_setopt(curl, CURLOPT_URL, m_smtp.mb_str(wxConvUTF8).data());
//    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Message details
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, message.GetFrom().mb_str(wxConvUTF8).data());
    curl_slist* recipients = NULL;
    recipients = curl_slist_append(recipients, message.GetTo().mb_str(wxConvUTF8).data());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, (void*)&message);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
//    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    CURLcode res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
      wxMessageBox(curl_easy_strerror(res), "wxEmail Error!", wxICON_ERROR | wxOK | wxCENTER);
      curl_slist_free_all(recipients);
      curl_easy_cleanup(curl);
      return false;
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
  }
  return true;
}
#endif
// ------------------------------------------------------------------
void MyBupJobDataDlg::OnNotificationSendTestMessage(wxCommandEvent& event)
{
#if defined( WANT_CURL )
  // need to use the data as shown in the dialog, in case user has modified it
  wxMailer mailer( m_textCtrlNotificationTo->GetValue(),
    wxEmptyString, 
    m_textCtrlNotificationServer->GetValue() );
  wxEmailMessage message;
 
  message.SetFrom( m_textCtrlNotificationFrom->GetValue() )
    .SetTo( m_textCtrlNotificationTo->GetValue() )
    .SetMessage(_T("Message") )
    .SetSubject( m_textCtrlNotificationSubject->GetValue() );

#if 0
  if(!GetFilePickerAttachment()->GetPath().IsEmpty()) {
    message.AddAttachment(GetFilePickerAttachment()->GetPath());
  }
  if(!GetFilePickerAttachment2()->GetPath().IsEmpty()) {
    message.AddAttachment(GetFilePickerAttachment2()->GetPath());
  }
#else
  // handle log file attachment here - none for test messages
#endif
  if ( !message.Finalize() ) // Must finalize the message before sending it
    return;   // user aborted sending the message
  wxString wsT(  _("Message Sent Successfully!") );
  if(mailer.Send(message)) 
  {
    wxLogMessage( wsT );
    ::wxMessageBox( wsT);
    wxGetApp().m_frame->GetStatusBar()->SetStatusText( wsT, 0 );
    wxGetApp().m_frame->GetStatusBar()->SetStatusText( _T(""), 1 );
  }
#endif
  event.Skip();
}

// ------------------------------- eof ------------------------------
