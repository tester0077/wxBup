//--------------------------------------------------------------
// Name:        wxBupOptions.cpp
// Purpose:
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//--------------------------------------------------------------

//--------------------------------------------------------------
// Standard wxWidgets headers
//--------------------------------------------------------------
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

// For all others, include the necessary headers (this file is
// usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include "wxBupFrameh.h"    // needs to be first
#include "wxBupOptionsh.h"
#include "wxBupBagSizes.h"
// -------------------------------------------------------------
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
// --------------------------------------------------------------
// externals
// --------------------------------------------------------------
extern MyApp *theApp;
//---------------------------------------------------------------
// Constructor
MyOptionsDialog::MyOptionsDialog( wxWindow* parent ) : 
    MyBaseOptionsDialog( parent  )
{
  // clean up local iniPrefs_t - does NOT cause memory leaks
  m_iniPrefs.lPrefVer = 0l;
  for( int i = 0; i < IE_MAX; i++ )
  {
    m_iniPrefs.data[i].wsKeyStr = m_iniPrefs.data[i].wsPathStr = wxEmptyString;
    m_iniPrefs.data[i].eType = eLong;
  }
}

// ------------------------------------------------------------------
void MyFrame::OnOptions( wxCommandEvent& WXUNUSED(event ) )
{
  m_bEditJob = false;
  OpenOptionsDlg();
}

// ------------------------------------------------------------------
void MyFrame::OpenOptionsDlg()
{
  MyOptionsDialog optionsDlg( this ); 
  wxGetApp().MakeConfigCopy( g_iniPrefs, optionsDlg.m_iniPrefs );
  optionsDlg.m_wsDlgAppName = wxGetApp().GetAppName();
  // data setup and saving is handled by the two functions
  // MyOptionsDialog::TransferDataTo/FromWindow()
  // all we need to do is 'Show' it ...
  if ( optionsDlg.ShowModal() == wxID_OK )
  {
    // all info MUST be saved here or else aborting the dialog
    // will have bad side effects as some of the INI data will
    // have been changed but not everything
    wxGetApp().MakeConfigCopy( optionsDlg.m_iniPrefs, g_iniPrefs );
#if 0
    // handle the toolbar text

    // does refresh the toolbar layout, but the other windows do not adapt 
    // to the larger tool bar
    // the "when done" drop down is all scrunched up
    // and the tops of the main windows  - job tree & summary - are chopped off
    long style = GetToolBar()->GetWindowStyle();
    style &= ~( wxTB_HORZ_TEXT );
#if 1
    if ( g_iniPrefs.data[IE_TOOLBAR_TEXT].dataCurrent.bVal )
      style = wxTB_TEXT;
    else
      style = wxTB_HORIZONTAL;
#else
    style |= (g_iniPrefs.data[IE_TOOLBAR_TEXT].dataCurrent.bVal ? 
      wxTB_HORZ_TEXT : wxTB_TEXT);
#endif
    GetToolBar()->SetWindowStyle(style);
    GetToolBar()->SetToolBitmapSize(wxSize(16,16));
    GetToolBar()->Realize();
    Layout(); // does not help look
//    Fit(); // shrinks everything to minimum sizes; no good at all
#endif
  }
}
// ==================================================================

// Called automagically by InitDialog() to set up the data
// prior to the dialog being shown
// Note: since this overrides a predefined function, it MUST have
// the name and return type as given here
bool MyOptionsDialog::TransferDataToWindow( void )
{
  // General Page -----------------------------------------------------
  m_spinCtrlSummaryUpdate->SetValue( 
    m_iniPrefs.data[IE_SUMMARY_UPDATE_INT].dataCurrent.lVal );
  m_checkBoxLoadLastJobAtStart->SetValue( 
    m_iniPrefs.data[IE_LAST_JOB_LOAD_AT_START].dataCurrent.bVal );
  m_checkBoxToolbarText->SetValue( 
    m_iniPrefs.data[IE_TOOLBAR_TEXT].dataCurrent.bVal );
  m_checkBoxCheck4NewVersionAtStart->SetValue(
    m_iniPrefs.data[IE_CHECK4NEW_VERSION].dataCurrent.bVal );
  m_checkBoxDisplayDestnWarning->SetValue(
    m_iniPrefs.data[IE_DELETE_DEST_WARN].dataCurrent.bVal );

  // temporary dir
  m_checkBoxOptLogUseTempDefaultPath->SetValue(
    m_iniPrefs.data[IE_USE_GLOBAL_DEFAULT].dataCurrent.bVal );
  m_dirPickerOptTempFilesDir->SetPath( 
    m_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal );
  if (m_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    m_dirPickerOptTempFilesDir->Enable();
  }
  else
  {
    m_dirPickerOptTempFilesDir->Disable();
  }
  // Bag size
  m_choiceDlgOutputBagSize->Clear();
  // fill the choice box
  for ( int i = EBupBagSizeTypes::eBagSize_NoSplit; i <= EBupBagSizeTypes::eBagSize_25GB; i++ )
  {
    m_choiceDlgOutputBagSize->Append( EBupBagSizeTypes::AsWxName(i) );
  }
  // set selection
  m_choiceDlgOutputBagSize->SetSelection(
    m_iniPrefs.data[IE_DEFAULT_BAG_SIZE].dataCurrent.lVal );

  // Log Page ---------------------------------------------------------
  m_checkBoxClearLogAtStart->SetValue( 
    m_iniPrefs.data[IE_LOG_CLEAR_AT_START].dataCurrent.bVal );
  m_checkBoxClearLogAtTest->SetValue( 
    m_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal );
  m_checkBoxOptLogToFile->SetValue( 
    m_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal );
  m_checkBoxOptLogUseDefaultPath->SetValue( 
    m_iniPrefs.data[IE_USE_LOG_DEF_DIR].dataCurrent.bVal );
  m_sliderOptLogVerbosity->SetValue( 
    m_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
  m_textCtrlOptLogFilesDestDir->SetValue( 
    m_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal );
  
  bool b =  m_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal;
  m_checkBoxOptLogToFile->SetValue( b );
  if ( b )
  {
    b = m_iniPrefs.data[IE_USE_LOG_DEF_DIR].dataCurrent.bVal;
    m_checkBoxOptLogUseDefaultPath->SetValue( b );
    if ( b )
    {
       m_buttonOptLogSelLogFilesDir->Enable( false );
       m_textCtrlOptLogFilesDestDir->Disable();
    }
    else
    {
       m_buttonOptLogSelLogFilesDir->Enable();
       m_textCtrlOptLogFilesDestDir->Enable();
    }
  }
  else
  {
    m_checkBoxOptLogUseDefaultPath->Disable();
    m_buttonOptLogSelLogFilesDir->Enable( false );
    m_textCtrlOptLogFilesDestDir->Disable();
  }

  m_sliderOptLogVerbosity->SetValue(m_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
 
  // log frequency
  m_radioBoxNewLogWhen->SetSelection( m_iniPrefs.data[IE_LOG_NEW_LOG].dataCurrent.lVal );
  // E-mail client page

  // sound page
  m_checkBoxSound->SetValue(
    m_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal );

  m_filePickerSoundError->Enable(
    m_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal );
  m_filePickerSoundWarning->Enable(
    m_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal );
  m_filePickerSoundCompln->Enable(
    m_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal);

  m_filePickerSoundError->SetPath( 
    m_iniPrefs.data[IE_SOUND_FILE_ERROR].dataCurrent.wsVal );
  m_filePickerSoundWarning->SetPath( 
    m_iniPrefs.data[IE_SOUND_FILE_WARN].dataCurrent.wsVal );
  m_filePickerSoundCompln->SetPath( 
    m_iniPrefs.data[IE_SOUND_FILE_DONE].dataCurrent.wsVal );
  // set the initial directory
  m_filePickerSoundError->SetInitialDirectory( wxGetApp().m_wsConfigDir );
  m_filePickerSoundWarning->SetInitialDirectory( wxGetApp().m_wsConfigDir );
  m_filePickerSoundCompln->SetInitialDirectory( wxGetApp().m_wsConfigDir );
  
  // default e-mail data
  m_checkBoxDefaultEmail->SetValue(
    m_iniPrefs.data[IE_EMAIL_DFLT_USE].dataCurrent.bVal );
  m_textCtrlDefaultServer->SetLabelText(
    m_iniPrefs.data[IE_EMAIL_DFLT_SERVER].dataCurrent.wsVal );
  m_textCtrlDefaultFrom->SetLabelText(
    m_iniPrefs.data[IE_EMAIL_DFLT_FROM].dataCurrent.wsVal );
  m_textCtrlDefaultTo->SetLabelText(
    m_iniPrefs.data[IE_EMAIL_DFLT_TO].dataCurrent.wsVal );
  m_textCtrlDefaultSubject->SetLabelText(
    m_iniPrefs.data[IE_EMAIL_DFLT_SUBJECT].dataCurrent.wsVal );

  // restore the last page shown
  int szPageCount = m_nbOptions->GetPageCount();
  m_nbOptions->SetSelection( __min( szPageCount,
    m_iniPrefs.data[IE_LAST_OPTION_TAB].dataCurrent.lVal ) );
  m_nbOptions->SetSelection( m_iniPrefs.data[IE_LAST_OPTION_TAB].dataCurrent.lVal );
  return true;
}

// ==================================================================

// Called automagically when dialog is dismissed via OK button
// Note: since this overrides a predefined function, it MUST have
// the name and return type as given here

bool MyOptionsDialog::TransferDataFromWindow( void )
{
  // General Page -----------------------------------------------------
  m_iniPrefs.data[IE_SUMMARY_UPDATE_INT].dataCurrent.lVal =
    m_spinCtrlSummaryUpdate->GetValue();
  m_iniPrefs.data[IE_LAST_JOB_LOAD_AT_START].dataCurrent.bVal = 
    m_checkBoxLoadLastJobAtStart->GetValue();
  m_iniPrefs.data[IE_TOOLBAR_TEXT].dataCurrent.bVal =
    m_checkBoxToolbarText->GetValue();
  m_iniPrefs.data[IE_CHECK4NEW_VERSION].dataCurrent.bVal =
    m_checkBoxCheck4NewVersionAtStart->GetValue();
  m_iniPrefs.data[IE_DELETE_DEST_WARN].dataCurrent.bVal =
    m_checkBoxDisplayDestnWarning->GetValue();
  m_iniPrefs.data[IE_EDIT_SHOWFULLPATH].dataCurrent.bVal =
    m_checkBoxShowFullJobPath->GetValue();
  
  // temporary dir
  m_iniPrefs.data[IE_USE_GLOBAL_DEFAULT].dataCurrent.bVal =
    m_checkBoxOptLogUseTempDefaultPath->GetValue();
  m_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal =
    m_dirPickerOptTempFilesDir->GetPath();
#if defined( WANT_TEMP_CHECK )
  if ( !wxDir::Exists( m_dirPickerOptTempFilesDir->GetPath() ) )
  {
    wxString wsT;
    wsT.Printf( _("The path to the temporary directory: \n%s\n"),
      m_dirPickerOptTempFilesDir->GetPath() );
    wxMessageBox( wsT + _(" does not exist!  Please correct before proceeding.") ,
      _("Error"), wxOK | wxICON_ERROR );
    return false;
  }
#endif
  // save the default bag size
  m_iniPrefs.data[IE_DEFAULT_BAG_SIZE].dataCurrent.lVal =
    m_choiceDlgOutputBagSize->GetSelection();
  // default bag name - is part of job config

  // -------------------------------------------------------
  // 'Log' page
  m_iniPrefs.data[IE_LOG_CLEAR_AT_START].dataCurrent.bVal =
    m_checkBoxClearLogAtStart->GetValue();
  m_iniPrefs.data[IE_LOG_CLEAR_AT_TEST].dataCurrent.bVal =
    m_checkBoxClearLogAtTest->GetValue();
  m_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal =
    m_checkBoxOptLogToFile->GetValue();
  m_iniPrefs.data[IE_USE_LOG_DEF_DIR].dataCurrent.bVal =
   m_checkBoxOptLogUseDefaultPath->GetValue();
  m_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal
    = m_textCtrlOptLogFilesDestDir->GetValue();
  m_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal =
    m_sliderOptLogVerbosity->GetValue();
  // log frequency
  m_iniPrefs.data[IE_LOG_NEW_LOG].dataCurrent.lVal = 
    m_radioBoxNewLogWhen->GetSelection();

  // E-mail client page
  
  // sound page
  m_iniPrefs.data[IE_SOUND_FEEDBACK].dataCurrent.bVal =
    m_checkBoxSound->GetValue();
  m_iniPrefs.data[IE_SOUND_FILE_ERROR].dataCurrent.wsVal =
    m_filePickerSoundError->GetPath();
  m_iniPrefs.data[IE_SOUND_FILE_WARN].dataCurrent.wsVal =
    m_filePickerSoundWarning->GetPath();
  m_iniPrefs.data[IE_SOUND_FILE_DONE].dataCurrent.wsVal =
    m_filePickerSoundCompln->GetPath();

  // default e-mail data
  m_iniPrefs.data[IE_EMAIL_DFLT_USE].dataCurrent.bVal = 
    m_checkBoxDefaultEmail->GetValue();
  m_iniPrefs.data[IE_EMAIL_DFLT_SERVER].dataCurrent.wsVal =
    m_textCtrlDefaultServer->GetLabelText();
  m_iniPrefs.data[IE_EMAIL_DFLT_FROM].dataCurrent.wsVal =
    m_textCtrlDefaultFrom->GetLabelText();
  m_iniPrefs.data[IE_EMAIL_DFLT_TO].dataCurrent.wsVal =
    m_textCtrlDefaultTo->GetLabelText();
  m_iniPrefs.data[IE_EMAIL_DFLT_SUBJECT].dataCurrent.wsVal =
    m_textCtrlDefaultSubject->GetLabelText();

  // save the last page shown
  m_iniPrefs.data[IE_LAST_OPTION_TAB].dataCurrent.lVal =
    m_nbOptions->GetSelection();
  return true;
}

// ------------------------------------------------------------------
void MyOptionsDialog::OnOptDlgGen_UseDefaultTempDirClick( 
  wxCommandEvent& WXUNUSED( event ) )
{
  bool b = m_checkBoxOptLogUseTempDefaultPath->GetValue();
  m_dirPickerOptTempFilesDir->Enable( !b );
  if ( b )
  {
    m_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal =
      wxStandardPaths::Get().GetTempDir();
    m_dirPickerOptTempFilesDir->SetPath( 
      m_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal );
  }
}

// ------------------------------------------------------------------
void MyOptionsDialog::OnOptDlgSnd_WantSound(wxCommandEvent& event)
{
  // gives the checkbox state as it will be 
  bool b = event.IsChecked(); 
  m_filePickerSoundError->Enable( b );
  m_filePickerSoundWarning->Enable( b );
  m_filePickerSoundCompln->Enable( b );
  event.Skip();
}

// ------------------------------- eof -------------------------
