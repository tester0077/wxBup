/*-----------------------------------------------------------------
 * Name:        wxBupRestoreDialog.cpp
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
#include <wx/dir.h>
#include "wxBupFrameh.h"
#include "wxBupRestoreDialogh.h"
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

wxBupRestoreDlg::wxBupRestoreDlg( MyFrame* parent )
    : wxBupRestoreDlgBase(parent )
{
  m_pParent     = parent;
  m_bIsRestore  = m_bIsTest = false;
  m_iniPrefs.lPrefVer = 0l;
  memset(m_iniPrefs.data, 0, sizeof(m_iniPrefs.data) - 1);
}

// ------------------------------------------------------------------

wxBupRestoreDlg::~wxBupRestoreDlg()
{
}

// ------------------------------------------------------------------

// Called automagically by InitDialog() to set up the data
// prior to the dialog being shown
// Note: since this overrides a predefined function, it MUST have
// the name and return type as given here
bool wxBupRestoreDlg::TransferDataToWindow( void )
{
  if ( m_bIsTest )
  {
    SetLabel( _("Test a backup") );
    m_dirPickerRestoreSrc->SetToolTip( 
      _("Base directory of data to be tested.") );
    m_panelTarget->Hide();
  }
  if( m_bIsRestore )
  {
    m_dirPickerRestoreSrc->SetToolTip( 
      _("Base directory of data to be restored.") );
    m_dirPickerRestoreTarget->SetPath( 
      m_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal );
    m_checkBoxClearTarget->SetValue(
      m_iniPrefs.data[IE_CLEAR_TARGET].dataCurrent.bVal );
  }
  m_dirPickerRestoreSrc->SetPath( 
    m_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal );
#if 0 // only used for testing
  m_dirPickerRestoreSrc->SetPath( _T("d:\\test") );
  m_dirPickerRestoreTarget->SetPath( _T("k:\\restoreTest") );
#endif
  Fit();
  return true;
}

//------------------------------------------------------------------
// Called automagically when dialog is dismissed via OK button
// Note: since this overrides a predefined function, it MUST have
// the name and return type as given here

bool wxBupRestoreDlg::TransferDataFromWindow( void )
{
  wxString wsT;
  wsT = m_dirPickerRestoreSrc->GetPath();
  m_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal = wsT;
  wsT = m_dirPickerRestoreTarget->GetPath();
  m_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal = wsT;
  if( m_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal.IsSameAs( 
    m_iniPrefs.data[IE_LAST_RESTORE_SOURCE].dataCurrent.wsVal ) )
  {
    wxMessageBox( _T("Destination and source are the same; please correct! "), _T("Error"), 
      wxOK | wxICON_ERROR );
    return false;
  }
  m_iniPrefs.data[IE_CLEAR_TARGET].dataCurrent.bVal =
    m_checkBoxClearTarget->GetValue();
  return true;
}

// ------------------------------- eof ------------------------------
