/*-----------------------------------------------------------------
 * Name:        wxBupWorkerEventLog.cpp
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
#include "wxBupResourceh.h" // for sound resources

// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to go AFTER all headers
#include <crtdbg.h>
#  ifdef _DEBUG
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  endif
#endif
#if defined( LOG_EVENT )
// ------------------------------------------------------------------
void MyFrame::OnWorkerEventLogMsg( wxThreadEvent& event)
{
  long        lLong = event.GetExtraLong();
  int         iVerbosity = lLong & 0xFF;
  long        lCause =  (lLong & ~0xFF);
  wxString    wsT = event.GetString();
  int         iColorEnum = event.GetInt();
  bool        bSpecial = false;
  bool        bTooBig = false;

  if (lCause == MY_FILE_TOO_BIG )
  {
    bSpecial = true;
    bTooBig = true;
    m_bSkippedFiles = m_bHadError = true;
    m_textCtrlErrors->SetDefaultStyle(wxTextAttr( *wxRED) );
    m_textCtrlErrors->AppendText( wsT + _T("\n") );
    m_textCtrlErrors->SetDefaultStyle(wxTextAttr( *wxBLACK) );
  }
  else if ( (lCause == MY_FAILED_FILE  ) || (lCause == MY_SKIPPED_FILE ) ||
            (lCause == MY_SPECIAL_FILE ) || (lCause == MY_UDF_LIMIT_FILE ) ||
            (lCause == MY_MISSING_DIR  ) || (lCause == MY_ERROR ) )
  {
    bSpecial = true;
    m_bHadError = true;
  }
  // these need to be independent of verbosity
  if ( bSpecial )
  {
    m_textCtrlSpecialFiles->SetDefaultStyle(wxTextAttr( *wxBLUE) );
    m_textCtrlSpecialFiles->AppendText( wsT + _T("\n") );
    m_textCtrlSpecialFiles->SetDefaultStyle(wxTextAttr( *wxBLACK) );
  }
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal >= iVerbosity )
  {
    // need to convert all single '%' to "%%" for logging
    wxString wsFix = wsT;
    size_t n = wsFix.Replace("%", "%%");
    switch( iColorEnum )
    {
    case MY_BLUE:
      m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxBLUE) );
      wxLogInfo( wsFix );
      break;
    case MY_GREEN:
      m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxGREEN) );
      wxLogInfo( wsFix );
      break;
    case MY_RED:
      m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxRED) );
      m_notebookJob->SetSelection( 2 ); // change to the 'Error' page
      wxLogError( wsFix );
      break;
    default:
      m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxBLACK) );
      wxLogMessage( wsFix );
      break;
    }
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( *wxBLACK ));
  }
#if defined( WANT_SOUND )
  if ( !m_bCancelled && g_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal )
  {
    switch( iColorEnum )
    {
      case MY_RED: 
        // use the 'file' variable, rather than the 'resource' variable
        m_wsSoundFile = g_iniPrefs.data[IE_SOUND_FILE_ERROR].dataCurrent.wsVal;
        if ( !m_wsSoundFile.IsEmpty() )
        {
          if ( !m_pSound )
          {
            m_pSound = new wxSound;
          }
          else
          {
            m_pSound->Stop(); // stop any other sounds
          }
          CreateSound(*m_pSound);
          // note: is I use wxSOUND_SYNC, the routine does not return until
          // the 'sound' is finished palying, so 'stoppping' the sound does nothing
          if (m_pSound->IsOk())
            m_pSound->Play(wxSOUND_SYNC);
        }
        break;
      case MY_BLACK:
      case MY_GREEN:
      case MY_BLUE:
      default:
        break;
    }
  }
#endif
}
#endif
// ------------------------------- eof ------------------------------
