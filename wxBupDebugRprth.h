/*-----------------------------------------------------------------
 * Name:        wxBupDebugRprth.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#include "wxBupPreProcDefsh.h" // needs to be first
//-----------------------------------------------------------------------------
#ifndef _WX_GED_DEBUG_RPRT_H
#define _WX_GED_DEBUG_RPRT_H

#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/icon.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/button.h"
#include "wx/dcclient.h"

#include "wx/datetime.h"
#include "wx/ffile.h"
#include "wx/filename.h"
#if defined( WANT_DBGRPRT )
#include "wx/debugrpt.h"
#include "wx/stackwalk.h"
#include "wx/xml/xml.h"
#endif
#if defined( WANT_DBGRPRT )
#if !wxUSE_DEBUGREPORT
    #error "This sample can't be built without wxUSE_DEBUGREPORT"
#endif // wxUSE_DEBUGREPORT

#if !wxUSE_ON_FATAL_EXCEPTION
    #error "This sample can't be built without wxUSE_ON_FATAL_EXCEPTION"
#endif // wxUSE_ON_FATAL_EXCEPTION
// ------------------------------------------------------------------
#if defined( WANT_DBGRPRT )
// ----------------------------------------------------------------------------
// custom debug reporting class
#if 1
// ----------------------------------------------------------------------------
// http://leno-10/flyspray/index.php?do=details&task_id=1887
// this is your custom debug reporter: it will use curl program (which should
// be available) to upload the crash report to the given URL (which should be
// set up by you)
class MyDebugReport : public wxDebugReportUpload
{
public:
  MyDebugReport() : wxDebugReportUpload
                      (
                      //wxT("http://leno-10/"),
                      wxT("http://192.168.1.70/DebugReports/"),
                      wxT("report:file"),
                      wxT("action")
                      )
  {
  }

protected:
  // this is called with the contents of the server response: you will
  // probably want to parse the XML document in OnServerReply() instead of
  // just dumping it as I do
  virtual bool OnServerReply(const wxArrayString& reply)
  {
    if ( reply.IsEmpty() )
    {
      wxLogError(wxT("Didn't receive the expected server reply."));
      return false;
    }

    wxString s(wxT("Server replied:\n"));

    const size_t count = reply.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
      s << wxT('\t') << reply[n] << wxT('\n');
    }
    wxLogMessage(wxT("%s"), s );
    wxLogDebug(wxT("%s"), s );
    wxSleep( 2 );
    return true;
  }
};
#else
// another possibility would be to build email library from contrib and use
// this class, after uncommenting it:
#if 1

#include "wx/net/email.h"

class MyDebugReport : public wxDebugReportCompress
{
public:
    virtual bool DoProcess()
    {
        if ( !wxDebugReportCompress::DoProcess() )
            return false;
        wxMailMessage msg(GetReportName() + wxT(" crash report"),
                          wxT("10bxjfhf@Telus.net"),
                          wxEmptyString, // mail body
                          wxEmptyString, // from address
                          GetCompressedFileName(),
                          wxT("crashreport.zip"));

        return wxEmail::Send(msg);
    }
};

#endif // 0
#endif
WX_CHECK_BUILD_OPTIONS("wxQA")

// ----------------------------------------------------------------------------
// XmlStackWalker: stack walker specialization which dumps stack in XML
// ----------------------------------------------------------------------------

#if wxUSE_STACKWALKER

class XmlStackWalker : public wxStackWalker
{
public:
  XmlStackWalker(wxXmlNode *nodeStack)
  {
    m_isOk = false;
    m_nodeStack = nodeStack;
  }

  bool IsOk() const { return m_isOk; }

protected:
  virtual void OnStackFrame(const wxStackFrame& frame);

  wxXmlNode *m_nodeStack;
  bool m_isOk;
};
#endif
#endif  // #if defined( WANT_DBGRPRT )
#endif  // _WX_GED_DEBUG_RPRT_H
#endif
// ------------------------------- eof ---------------------------
