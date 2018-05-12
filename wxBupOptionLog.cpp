/*-----------------------------------------------------------------
 * Name:        wxBupOptionLog.cpp
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
#include "wxBupFrameh.h"    // needs to be first
#include "wxBupOptionsh.h"

// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// only good for MSVC
// this block needs to AFTER all headers
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
// ------------------------------------------------------------------
// 'Log' panel event handlers
/**
 * Enable/disable the default path checkbox and text control and
 * the directory select button.
 */
void MyOptionsDialog::OnOptDlgLog_Log2FileClick( wxCommandEvent& WXUNUSED(event) )
{
  bool b = m_checkBoxOptLogToFile->GetValue();
  // set the state of the dependent items in the option log page
  // presumably could also be done in OnUpdateUI handler
  m_checkBoxOptLogUseDefaultPath->Enable( b );
  m_textCtrlOptLogFilesDestDir->Enable( b );
  m_buttonOptLogSelLogFilesDir->Enable( b );
  g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal = b;
  // also need to set up the log redirect
  // if we enable logging
  if ( b )
  {
#if defined( WANT_LOG_CHAIN )
    /**
     * The original log window is set up in the main frame ctor.
     * This part can only be done once the .INI file has been read in
     * Also set up the default a base paths for the log file
     */
    g_iniPrefs.data[IE_LOG_DIR_PATH].dataDefault.wsVal =
      g_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal = wxGetApp().m_wsConfigDir;
    wxGetApp().m_wsCurLogFileName = 
      g_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal +_T("\\") + WS_LOG_FILE_NAME;
    wxGetApp().CreateNewLog();
#endif
  }
  else  // the log file is open & we need to close the log file and stop redirection
  {
    if ( wxGetApp().m_pAppLog->IsLogFileOK() )
    {
      wxGetApp().m_pAppLog->CloseLog();
      delete  wxLog::SetActiveTarget( wxGetApp().m_pLogChain );
      wxGetApp().m_pLogChain = NULL;
    }
  }
}

// ------------------------------------------------------------------

void MyOptionsDialog::OnOptDlgLog_DefPathClick( wxCommandEvent& WXUNUSED(event) )
{
  bool b = m_checkBoxOptLogUseDefaultPath->GetValue();
  if ( b )  // disable the text control & button, but show the default path
  {
    m_textCtrlOptLogFilesDestDir->Enable( false );
    m_buttonOptLogSelLogFilesDir->Enable( false );
    wxString wsT = wxStandardPaths::Get().GetUserConfigDir() +
       _T("\\");// + m_wsDlgAppName;
    m_textCtrlOptLogFilesDestDir->SetValue( wsT );
  }
  else // enable the button and the path text control and clear it
  {
    m_textCtrlOptLogFilesDestDir->Enable( true );
    m_buttonOptLogSelLogFilesDir->Enable( true );
    m_textCtrlOptLogFilesDestDir->SetValue( m_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal );
  }
}

// ------------------------------------------------------------------

void MyOptionsDialog::OnOptDlgLog_SelLogFilePathClick( wxCommandEvent& WXUNUSED(event) )
{
  // pass some initial dir to wxDirDialog
  int iStyle = wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST;
  wxString dirHome = m_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal;
  wxString wsT;

  wxDirDialog dialog(this, _T("Select Log directory"), dirHome, iStyle);

  if (dialog.ShowModal() == wxID_OK)
  {
    wsT = dialog.GetPath();
    m_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal = wsT;
  }
}

// ------------------------------------------------------------------
void MyOptionsDialog::OnOptDlgLog_ClearLogAtStart(wxCommandEvent& event)
{
  // gives the checkbox state as it will be 
  bool b = event.IsChecked(); 
  m_iniPrefs.data[IE_LOG_CLEAR_AT_START].dataCurrent.bVal;
  event.Skip();
}

// ------------------------------- eof ------------------------------
