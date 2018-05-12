/*-----------------------------------------------------------------
 * Name:        wxBupCopyFile.cpp
 * Purpose:     copy a file from source to dest'n using the Win
 *              API functions
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
/**
 * We use the native CopyFile() function which can handle UNC file names.
 * The destination path MUST be fully specified, it MUST not exist.
 */
bool MyFrame::MyFileCopy( const wxString &a_rwsSrc, wxString &a_rwsDest,
  bool bFailIfExists )
{
  if( CopyFile( a_rwsSrc.wc_str(), a_rwsDest.wc_str(), bFailIfExists ) == 0 )
  {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
    wxLogMessage( _("%s  Error Code: %ld\n  Src: %s\n  Dest: %s"), 
      (wxChar *)lpMsgBuf, (long)dw, a_rwsSrc, a_rwsDest );
    LocalFree(lpMsgBuf);
    m_textCtrlLog->SetDefaultStyle( wxTextAttr( wxTextAttr(*wxBLACK) ) );
    return false;
  }
  return true;
}

// ------------------------------- eof ------------------------------
