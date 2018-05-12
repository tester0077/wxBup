/*-----------------------------------------------------------------
 * Name:        wxBupMoveFile.cpp
 * Purpose:     move a file from source to dest'n using the Win
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
#include "wxBupWorkerh.h"
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
 * We use the native MoveFile() function which can handle UNC file names.
 * Both paths include the path & full file name, incl. extension.
 */
bool MyWorkerThread::MyFileMove( const wxString &a_rwsSrc, wxString &a_rwsDest )
{
  wxString wsT;

  if( MoveFile( a_rwsSrc.wc_str(), a_rwsDest.wc_str() ) == 0 )
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
    wsT.Printf( _("%sError Code: %ld\nSrc: %s\nDest: %s"), 
        (wxChar *)lpMsgBuf, (long)dw, a_rwsSrc, a_rwsDest );
	  {
	    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
	    // send a log message to the main frame log window
	    event.SetInt( MY_RED );   // my color enums
	    event.SetExtraLong( 5 | MY_ERROR );  // verbosity
	    event.SetString( wsT );
	    wxQueueEvent( m_frame, event.Clone() );
    }
    LocalFree(lpMsgBuf);
    return false;
  }
  else
  { 
    if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 3 )
    {
      {
        wsT.Printf( _("Source file: %s"), a_rwsSrc );
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
        // send a log message to the main frame log window
        event.SetInt( MY_BLUE );  // my color enums
        event.SetExtraLong( 4 );  // verbosity
        event.SetString( wsT );
        wxQueueEvent( m_frame, event.Clone() );
      }
      {
        wsT.Printf( _("  Destin file: %s"), a_rwsDest );
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
        // send a log message to the main frame log window
        event.SetInt( MY_BLUE );  // my color enums
        event.SetExtraLong( 4 );  // verbosity
        event.SetString( wsT );
        wxQueueEvent( m_frame, event.Clone() );
      }
    }
  }
  return true;
}

// ------------------------------- eof ------------------------------
