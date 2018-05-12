/*-----------------------------------------------------------------
 * Name:        wxBupEmailCompose.cpp
 * Purpose:     Compose the message body for the feedback email
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
/**
 * Compose the ASCII part of the feeback message
 */
void MyFrame::ComposeAsciiMessage( wxString & ar_wsMessage )
{
  wxString wsAsciiFormat = 
    _T("Job Name: %s\nStatus: %s\nAppName: %s, Version %d.%d, build: %d");
  ar_wsMessage.Printf( wsAsciiFormat, g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal,
    m_bHadError ? _("Error") : _("OK") , 
    wxGetApp().m_wsAppName, giMajorVersion, giMinorVersion, giBuildNumber );
}

// ------------------------------- eof ------------------------------
