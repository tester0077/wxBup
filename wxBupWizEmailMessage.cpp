/*-----------------------------------------------------------------
 * Name:        wxBupWizEmailMessage.cpp
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
#include "wxBuph.h"
#include "wxBupWizSendEmailh.h"
#include "wxBupWizEmailMessageIdh.h"
#include <wx/datetime.h>
#include <wx/ffile.h>
#include <wx/base64.h>
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
// ------------------------------------------------------------------


#define BOUNDRY_LINE "pj+EhsWuSQJxx7pr"

wxEmailMessage::wxEmailMessage()
  : m_aschar(NULL)
  , m_pos(0)
  , m_len(0)
{
}

// ------------------------------------------------------------------

wxEmailMessage::~wxEmailMessage()
{
  if(m_aschar)
  {
    free(m_aschar);
  }
  m_aschar = NULL;
}

// ------------------------------------------------------------------

wxString wxEmailMessage::GenerateID()
{
  static time_t starupTime = time(NULL);
  time_t curtime = time(NULL);

  wxString s;
  s << starupTime << "." << curtime;
  wxEmailMessageID id(s);
  s = id.Encrypt();

  s.Prepend("<");
  s << "@wxmailer>";
  return s;
}

// ------------------------------------------------------------------

wxString wxEmailMessage::PayLoad() const
{ //, %d %b %Y %T (%Z)
  wxString wsSubject;
  wxString payload;
  /**
   * We need to conform to the date-time format specified by
   * RFC 4021 & 822
   * dd mm yy hh:mm:ss zzz
   * as used here, we get Wed, 04 Apr 2018 10:27:30 (Pacific Daylight Time)
   * However, it seems other mail clients (specifically Thunderbird) will accept 
   * and interpret other date formats, such as 04/04/18 08:59:33
   * though I am not sure what assumptions they would make about dd/mm/yyyy or mm/dd/yyyy
   */
#if defined( _DEBUG )
  wxString wsData = wxDateTime::Now().Format( _T("%a, %d %b %Y %H:%M:%S (%Z)") );
#endif
  payload << "Date: " << wxDateTime::Now().Format( _T("%a, %d %b %Y %H:%M:%S -800 (%Z)") ) << "\r\n";
  payload << "To: " << GetTo() << "\r\n";
  payload << "From: " << GetFrom() << "\r\n";
  payload << "Cc: \r\n";
  payload << "Message-ID: " << GenerateID() << "\r\n";
  // expand the subject line macros
  wsSubject << GetSubject();
  // make sure we do have one of the allowed macros,
  // otherwise
  wxString wsT;
  bool b = std::any_of(wsSubject.begin(), wsSubject.end(), ::isdigit);
  if ( b )
  {
    wsT.Printf( _("The subject line <%s> contains numeric data\n") +
        _("Make sure this is not an invalid version number\n") +
      _("Use the macros {version} & {machine} instead!\n") + 
      _("To fix, update the e-mail options in the job file!\nContinue (Yes) or fix (No)?"),  
      wsSubject );
    int iResp = wxMessageBox( wsT, _("Notice"), wxYES_NO | wxICON_ERROR | wxNO_DEFAULT );
    if ( iResp != wxYES )
      return wxEmptyString; // bail out & let user fix the string
  }
  size_t n = wsSubject.find( '{' );
  ExpandSubjectVersion( wsSubject );
  ExpandSubjectMachineName( wsSubject );
  payload << "Subject: " << wsSubject << "\r\n";

  // Sending attachment
  payload << "Content-Type: multipart/mixed; boundary=\"" << BOUNDRY_LINE << "\"\r\n";
  payload << "Mime-version: 1.0\r\n";
  payload << "\r\n";
  payload << "This is a multi-part message in MIME format. \r\n";

  // Message body
  if(!GetMessage().IsEmpty())
  {
    payload << "\r\n--" << BOUNDRY_LINE << "\r\n";
    payload << "Content-Type: text/plain; charset=\"us-ascii\"\r\n";
    payload << "Content-Transfer-Encoding: quoted-printable \r\n";
    payload << "\r\n";
    payload << GetMessage() << "\r\n";
  }

  if(!m_attachements.IsEmpty())
  {
    for(size_t i = 0; i < m_attachements.size(); ++i)
    {
      if ( CheckAttachmentSize( m_attachements.Item(i) ) == false )
        continue;
      DoAddAttachment(m_attachements.Item(i), payload);
    }
  }
  payload << "\r\n--" << BOUNDRY_LINE << "\r\n";  // terminate message
  payload << "\r\n";
  return payload;
}

// ------------------------------------------------------------------

bool wxEmailMessage::Finalize()
{
  m_payload = PayLoad();
  if ( m_payload.IsEmpty() )
    return false;
  const char* p = m_payload.mb_str(wxConvUTF8).data();
  m_aschar = (char*)malloc(strlen(p) + 1);
  strcpy(m_aschar, p);
  m_len = strlen(m_aschar);
  m_pos = 0;
  return true;
}

// ------------------------------------------------------------------
/**
 *  Only called if the size is OK
 */
void wxEmailMessage::DoAddAttachment(const wxString& a_wsFilename, wxString& payload) const
{
  wxString base64Attachment;
  wxFileName fn(a_wsFilename);
  wxString wsT;
  size_t len;
  if(!fn.IsOk() || !fn.Exists()) return;

  FILE* fp = fopen(fn.GetFullPath().mb_str(wxConvUTF8).data(), "rb");

  if(fp)
  {
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buffer = new char[len];
    fread(buffer, 1, len, fp);
    fclose(fp);
    base64Attachment = ::wxBase64Encode(buffer, len);
    wxDELETEA(buffer);
  }

  if(!base64Attachment.IsEmpty())
  {
    payload << "\r\n--" << BOUNDRY_LINE << "\r\n";
    payload << "Content-Type: application/octet-stream; name=\"" << fn.GetFullName() << "\""
      << "\r\n";
    payload << "Content-Transfer-Encoding: base64 \r\n";
    payload << "Content-Disposition: attachement; filename=\"" << fn.GetFullName() << "\"\r\n";
    payload << "\r\n";
    // Split the content to 76 chars per line
    wxString line;
    long lLen;
    size_t nLines = 0;
    while(!base64Attachment.IsEmpty())
    {
      lLen = base64Attachment.length();
      nLines++;
      size_t bytes = (base64Attachment.length() >= 76) ? 76 : base64Attachment.length();
      line = base64Attachment.Mid(0, bytes);
      payload << line << "\r\n";
      base64Attachment = base64Attachment.Mid(bytes);

      wxString wsCount;
      float fPercent = 
        ((float)( nLines) * 100 ) / (float)(len/76);
      wsCount.Printf( _("Encoding line: %Id/%Id  - %3.1f%%"), nLines, len/76, fPercent );
      // update the progress gauge for user feedback
      wxGetApp().m_frame->GetGaugeProgress()->SetValue( 
        __min( 100, (nLines * 100)/(len/76) ) );
      // and the taskbar icon's as well
      wxGetApp().m_frame->GetAppProgressInd()->SetValue(
        __min( 100, (nLines * 100)/(len/76) ) );
      wxGetApp().m_frame->GetStatusBar()->SetStatusText( wsCount, 1 );
      ::wxYield();  // give rest of world a chance 
    }
    wxGetApp().m_frame->GetStatusBar()->SetStatusText( _("Done enoding"), 0 );
    // reset the progress gauge for user feedback
    wxGetApp().m_frame->GetGaugeProgress()->SetValue( 0 );
    // and the taskbar icon's as well
    wxGetApp().m_frame->GetAppProgressInd()->SetValue( 0 );
  }
}

// ------------------------------------------------------------------
bool wxEmailMessage::CheckAttachmentSize( wxString a_wsFilename ) const
{
  wxFileName fn(a_wsFilename);
  wxString wsT;
  
  size_t len;
  if(!fn.IsOk() || !fn.Exists()) 
    return false;

  FILE* fp = fopen(fn.GetFullPath().mb_str(wxConvUTF8).data(), "rb");
  if(fp)
  {
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    // check if the attachment excceds a give size limit
    if( len > EMAIL_ATTACHMENT_LIMIT )
    {
      wsT.Printf( _("%s exceeds the size limit (%d MB) for attachments\n") +
        _("This item will be skipped!"), 
        a_wsFilename, EMAIL_ATTACHMENT_LIMIT/( 1024 *1024) );
      int iResp = wxMessageBox( wsT, _("Notice"), wxOK | wxICON_ERROR );
      fclose(fp);
      return false;
    }
    fseek(fp, 0, SEEK_SET);
  }
  return true;
}
// ------------------------------------------------------------------
void const wxEmailMessage::ExpandSubjectVersion( wxString & ar_wsSubjectLine2Expand ) const
{
  wxString wsTest = ar_wsSubjectLine2Expand;
  // expand any macros found in the subject string
  // currently we only allow {version} => major.minor.build
  int iFirstOpenBrace = wsTest.First( '{' );
  int iFirstCloseBrace = wsTest.First( '}' );
  int iDateStr;
  wxString wsVersion;
  // open < close
  if ( iFirstOpenBrace != wxNOT_FOUND )
  {
    if (iFirstCloseBrace == wxNOT_FOUND )
    {
      wxFAIL_MSG( _("Unmatched braces") );
    }
    wxASSERT( iFirstOpenBrace < iFirstCloseBrace );
    // find the expected string "{date}"
    iDateStr = wsTest.find( _T("{version}") );
    wxASSERT( iDateStr  != wxNOT_FOUND );
    if (iDateStr == wxNOT_FOUND) // something is wrong
    {
      ;
    }
    else
    { 
      wsVersion.Printf( _T("%d.%d.%d"), giMajorVersion, giMinorVersion,
        giBuildNumber );
      wsTest.replace( iFirstOpenBrace, iFirstCloseBrace - iFirstOpenBrace + 1,
        wsVersion );
    }
  }
  ar_wsSubjectLine2Expand = wsTest;
}

// ------------------------------------------------------------------

void const wxEmailMessage::ExpandSubjectMachineName( wxString & ar_wsMachine2Expand ) const
{
  wxString wsTest = ar_wsMachine2Expand;
  // expand any macros found in the subject string
  // currently we only allow {version} => major.minor.build
  int iFirstOpenBrace = wsTest.First( '{' );
  int iFirstCloseBrace = wsTest.First( '}' );
  int iMachineStr;
  //wxString wsVersion;
  // open < close
  if ( iFirstOpenBrace != wxNOT_FOUND )
  {
    if (iFirstCloseBrace == wxNOT_FOUND )
    {
      wxFAIL_MSG( _("Unmatched braces") );
    }
    wxASSERT( iFirstOpenBrace < iFirstCloseBrace );
    // find the expected string "{date}"
    iMachineStr = wsTest.find( _T("{machine}") );
    wxASSERT( iMachineStr  != wxNOT_FOUND );
    if (iMachineStr == wxNOT_FOUND) // something is wrong
    {
      ;
    }
    else
    { 
      //wsVersion.Printf( _T("%0d.%02d.%02d"), giMajorVersion, giMinorVersion,
      //  giBuildNumber );
      wsTest.replace( iFirstOpenBrace, iFirstCloseBrace - iFirstOpenBrace + 1,
        wxGetHostName() );
    }
  }
  ar_wsMachine2Expand = wsTest;
}
// ------------------------------- eof ------------------------------
