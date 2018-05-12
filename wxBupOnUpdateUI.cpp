/*-----------------------------------------------------------------
 * Name:        wxBupOnUpdateUI.cpp
 * Purpose:     Update all of the UI elements in the GUI.
 *              Note: If any of these elements have the ID wxID_ANY
 *              There seems to be a very good chance that some other
 *              elements which have a specific ID will end up in the 
 *              wrong state.
 *              This specifically happend in this app with the stop
 *              button.
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

// ------------------------------------------------------------------
void MyFrame::OnUpdateUILogWanted(wxUpdateUIEvent& event) 
{ 
  // set menu items as needed
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Check( g_iniPrefs.data[IE_SHOW_LOG].dataCurrent.bVal );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUILogClear(wxUpdateUIEvent& event) 
{ 
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( g_iniPrefs.data[IE_SHOW_LOG].dataCurrent.bVal );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiMenuHelp(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( m_bFoundHelpFile );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiMenuBackup(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable(!m_bRunning && m_bHaveDirOrFile && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUIMenuDump(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( m_bHaveDirOrFile && ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUiMenuEdit(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( m_bHaveDirOrFile && ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUINewJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
// useful to select a file not in the config dir???
void MyFrame::OnUpdateUiMenuSel(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiMenuTest(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiMenuRestore(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUiMenuDelLog(wxUpdateUIEvent& event)
{
  bool bExists = g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal &&
    wxFileName::FileExists( wxGetApp().m_wsCurLogFileName );
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress && bExists );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUiMenuOpenLog(wxUpdateUIEvent& event)
{
  bool bExists = g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal &&
    wxFileName::FileExists( wxGetApp().m_wsCurLogFileName );
  event.Enable( ! m_bRunning && bExists && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiMenuOptions(wxUpdateUIEvent& event)
{
  event.Enable( ! m_bRunning && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUiMenuCrash(wxUpdateUIEvent& event)
{
  event.Enable( ! m_bRunning );
}

// ==================================================================
// Toolbar items
void MyFrame::OnUpdateUITbNewJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && !m_bCleaningUp && !m_bCountingFiles
  && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUITbDeleteJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && !m_bCleaningUp && !m_bCountingFiles
  && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUITbOpenJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && !m_bCleaningUp && !m_bCountingFiles 
    && !m_bSendingMailInProgress );
}
// ------------------------------------------------------------------

void MyFrame::OnUpdateUITbEditJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && m_bHaveDirOrFile && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUITbBackupJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && m_bJobSelected && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUITbRestoreJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && m_bJobSelected && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------

void MyFrame::OnUpdateUITbTestJob(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( !m_bRunning && m_bJobSelected && !m_bSendingMailInProgress );
}

// ------------------------------------------------------------------
/**
 * disable the 'stop' button when no thread is running and
 * during 'cleanup' time
 */
void MyFrame::OnUpdateUiTbStop(wxUpdateUIEvent& event) 
{ 
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( m_bRunning  && !m_bCleaningUp && !m_bCountingFiles );
}

// ------------------------------------------------------------------
void MyFrame::OnUpdateUiTbHelp(wxUpdateUIEvent& event)
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  event.Enable( m_bFoundHelpFile );
}

// ------------------------------- eof ------------------------------
