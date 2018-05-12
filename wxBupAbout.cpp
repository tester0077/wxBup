/*-----------------------------------------------------------------
 * Name:        wxBupAbout.cpp
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

#if defined( WANT_SVN_REV )
#include "wxBup-SVNh.h" // this 'feature' still needs more work
#endif
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to go AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
// ------------------------------------------------------------------
/**
 * OnAbout - display the typical 'About' screen.
 * Note: Using
 * wsBuildDate = wxString::FromAscii( __DATE__ );
 * 'freezes' the build date to the last compile time of this particular file
 * not very useful for the date of the current overall build.
 * Note: for wxWidgets before 3.1.0,_MSC_VER == 1600)
    wsCompiler = _T("2010");
  else if (_MSC_VER == 1700)
    wsCompiler = _T("2012");
  else if (_MSC_VER == 1800)
    wsCompiler = _T("2013");
  else if (_MSC_VER == 1900)
    wsCompiler = _T("2015");
  // ------------------------------------------------------------------
  wxPlatformInfo wxPI = wxPlatformInfo::Get();
  wxASSERT( wxPI.IsOk() );
  // from: https://msdn.microsoft.com/en-ca/library/windows/desktop/ms724833(v=vs.85).aspx
  wxString wsOS;
  int iMinor = wxPI.GetOSMinorVersion();
  switch ( wxPI.GetOSMajorVersion() )
  {
   case 5:
    if( iMinor == 0 )
      wsOS = _T("Win 2000");
    else if( iMinor == 1 )
      wsOS = _T("XP");
    else if( iMinor == 2 )
      if( wxPI.GetArchitecture() == wxARCH_64 )
        wsOS  = _(" Win XP 64-bit");
      else
        wsOS  = _(" Server 2003/R2");
    break;
  case 6:
    if( iMinor == 0 )
      wsOS = _T("Vista");
    else if( iMinor == 1 )
      wsOS = _T("Win 7");
    else if( iMinor == 2 )
      wsOS = _T("Win 8");
    else if (iMinor == 3)
      wsOS = _T("Win 8.1");
    break;
  case 10:
    if (iMinor == 0)
      wsOS = _T("Win 10");
    break;
  default:
    wsOS = _T("unknown" );
    break;
  }
  wxString wsDebuggerPresent =
      IsDebuggerPresent( the OS info is wrong!!
 */
void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  wxString ws1;
  wxString wsBuildDate;
  wxString wsYear;
  wxString wsMonth;
  wxString wsDay;
  wxString wsCompiler = _T("unknown");
#if defined( _MSC_VER )
  if (_MSC_VER == 1600)
    wsCompiler = _T("2010");
  else if (_MSC_VER == 1700)
    wsCompiler = _T("2012");
  else if (_MSC_VER == 1800)
    wsCompiler = _T("2013");
  else if (_MSC_VER == 1900)
    wsCompiler = _T("2015");
#endif
  // ------------------------------------------------------------------
  wxPlatformInfo wxPI = wxPlatformInfo::Get();
  wxASSERT( wxPI.IsOk() );
  // from: https://msdn.microsoft.com/en-ca/library/windows/desktop/ms724833(v=vs.85).aspx
  wxString wsOS;
  int iMinor = wxPI.GetOSMinorVersion();
  switch ( wxPI.GetOSMajorVersion() )
  {
   case 5:
    if( iMinor == 0 )
      wsOS = _T("Win 2000");
    else if( iMinor == 1 )
      wsOS = _T("XP");
    else if( iMinor == 2 )
      if( wxPI.GetArchitecture() == wxARCH_64 )
        wsOS  = _(" Win XP 64-bit");
      else
        wsOS  = _(" Server 2003/R2");
    break;
  case 6:
    if( iMinor == 0 )
      wsOS = _T("Vista");
    else if( iMinor == 1 )
      wsOS = _T("Win 7");
    else if( iMinor == 2 )
      wsOS = _T("Win 8");
    else if (iMinor == 3)
      wsOS = _T("Win 8.1");
    break;
  case 10:
    if (iMinor == 0)
      wsOS = _T("Win 10");
    break;
  default:
    wsOS = _T("unknown" );
    break;
  }
#if defined( _MSC_VER )
  wxString wsDebuggerPresent =
      IsDebuggerPresent() ? _("Debugger") : _("No Debugger");
#else
  wxString wsDebuggerPresent = _("unknown");
#endif
  bool bRet;
  wxString wsIndent( _T("      ") );
  unsigned int uiYear1 = COPYRIGHT_YEAR_1;
  wxString wsCopyrightYear1;
  wsCopyrightYear1.Printf( _T("%u"), uiYear1 );
#if defined( _MSC_VER )
  bRet = wxGetApp().RetrieveLinkerTimestamp( wsBuildDate, wsYear );
  wxASSERT_MSG( bRet, _T("RetrieveLinkerTimestamp() failed") );
#endif
  if( !wsYear.IsSameAs( wsCopyrightYear1 ) )
  {
    wsYear = wsCopyrightYear1 + _T(" - ") + wsYear;
    wsIndent = wxEmptyString;
  }
  wxString wsT1 =
    _("                 %s - a Backup & Zip Utility\n" ) +
    _("            Version %d.%d - Build: %d - %s\n") +
    _("                 %sA. Wiegert \u00A9 %s\n") +
#if defined( wxUSE_UNICODE )
#if defined( _WIN64 )
    _("              Unicode version - Compiled for x64\n") +
#else if defined( _WIN32 )  // always defined
    // as per https://msdn.microsoft.com/en-CA/library/b0084kay.aspx
    _("       Unicode version - Compiled for x86 with") + wsCompiler + _T("\n") +
#endif
#else
    _("                         ANSI version\n") +
#endif
#if defined( __GNUC__ )
    _("               Built using CodeLite\n") +
#endif
#if defined ( WXUSINGDLL )
    _("           linked with wxWidgets %d.%d.%d DLLs\n") +
#else
    _("         linked with wxWidgets %d.%d.%d static libraries\n") +
#endif
#ifdef _DEBUG
#if defined( WANT_SVN_REV )
    _T("         SVN Revision: %s - %s\n") +
    _T("Repo: %s\n") +
#endif
#endif
#if defined( WANT_FILEVERSION )
//    _("                             VerLib\n" ) +
#else
#endif
#if defined( _DEBUG )
    _("                             Debug version\n") +
#else
#endif
    // Note: it seems these icons are no longer free
    // not sure where I got the idea they were free??
//    _("         Using icons from http://www.small-icons.com/ \n") +
    _("\n      Running on %s %s (%d.%d), %s") +
    _("\n     %s - Port: %s - %s") +
    _("\nConfig Dir: %s\n");
#if defined( _MSC_VER )
    ws1.Printf( wsT1,
    wxGetApp().m_wsAppName, giMajorVersion, giMinorVersion, giBuildNumber,
    wsBuildDate, wsIndent, wsYear,
    wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER,
#ifdef _DEBUG
#if defined( WANT_SVN_REV )
    wsSVNRev, wsSVNDate, wsSvnURL,
#endif
#endif
     wxPI.GetOperatingSystemFamilyName(),
      wsOS, wxPI.GetOSMajorVersion(), wxPI.GetOSMinorVersion(),
      wxPI.GetArchName(),
      wxPI.GetEndiannessName(), wxPI.GetPortIdName(), wsDebuggerPresent,
    wxGetApp().m_wsConfigDir );
#endif
  wxString ws2;
  ws2.Printf( _T("About %s"), wxGetApp().m_wsAppName );
  (void)wxMessageBox( ws1, ws2, wxICON_INFORMATION | wxOK );
}

// ------------------------------- eof ------------------------------
