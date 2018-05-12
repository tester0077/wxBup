/*-----------------------------------------------------------------
 * Name:        wxBupDebugRprt.cpp
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

#include "wxBupDebugRprth.h"
#include "wxBuph.h"
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
#if !wxUSE_DEBUGREPORT
    #error "This sample can't be built without wxUSE_DEBUGREPORT"
#endif // wxUSE_DEBUGREPORT

#if !wxUSE_ON_FATAL_EXCEPTION
    #error "This sample can't be built without wxUSE_ON_FATAL_EXCEPTION"
#endif // wxUSE_ON_FATAL_EXCEPTION

// ------------------------------------------------------------------
#if defined( WANT_DBGRPRT )
// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

// just some functions to get a slightly deeper stack trace
static void bar(const char *p)
{
  char *pc = 0;
  *pc = *p;

  printf("bar: %s\n", p);
}

void MyFrame::baz(const wxString& s)
{
    printf("baz: %s\n", (const char*)s.c_str());
}

void MyFrame::foo(int n)
{
  if ( n % 2 )
    baz("odd");
  else
    bar("even");
}

// ------------------------------------------------------------------

void MyFrame::OnCauseException( wxCommandEvent& event )
{
  // this call is going to crash
#if 0
//  foo(32);
//  foo(17);
  bar("even");
#else
  const char *p = 0;
  char *pc = 0;
  *pc = *p;
#endif
  event.Skip();
}
#endif
// ------------------------------------------------------------------

#if defined( WANT_DBGRPRT )
void MyApp::OnFatalException()
{
  // feedback for user
  wxBusyCursor bcur;
  wxWindowDisabler disableAll;
  // wxBusyInfo always stays on top, obscures the crash report when it is done
  m_pWaitDialog = new wxBusyInfo( m_wsAppName +
    _(" has encountered a problem.\nCreating a crash report.\nPlease wait ...") );
  GenerateReport(wxDebugReport::Context_Exception);
}

// ------------------------------------------------------------------

void MyApp::GenerateReport(wxDebugReport::Context ctx)
{
  wxDebugReportCompress *report = m_uploadReport ? new MyDebugReport
                                                  : new wxDebugReportCompress;

  // add all standard files: currently this means just a minidump and an
  // XML file with system info and stack trace
  report->AddAll(ctx);

  // you can also call report->AddFile(...) with your own log files, files
  // created using wxRegKey::Export() and so on, here we just add a test
  // file containing the date of the crash
  wxFileName fn(report->GetDirectory(), wxT("timestamp.my"));
  wxFFile file(fn.GetFullPath(), wxT("w"));
  if ( file.IsOpened() )
  {
    wxDateTime dt = wxDateTime::Now();
    file.Write(dt.FormatISODate() + wxT(' ') + dt.FormatISOTime());
    file.Close();
  }

  report->AddFile(fn.GetFullName(), wxT("timestamp of this report"));

  // can also add an existing file directly, it will be copied
  // automatically
#ifdef __WXMSW__
//    report->AddFile(wxT("c:\\autoexec.bat"), wxT("DOS startup file"));
#else
  report->AddFile(wxT("/etc/motd"), wxT("Message of the day"));
#endif
  delete m_pWaitDialog;
  // calling Show() is not mandatory, but is more polite
  if ( wxDebugReportPreviewStd().Show(*report) )
  {
    if ( report->Process() )
    {
      if ( m_uploadReport )
      {
        wxLogMessage(wxT("Report successfully uploaded."));
      }
      else
      {
        wxLogMessage(wxT("Report generated in \"%s\"."),
                      report->GetCompressedFileName() );
        report->Reset();
      }
    }
  }
  //else: user cancelled the report
  // removes the report file from te temporary directory
  delete report;
}
#endif
// ------------------------------- eof ------------------------------
