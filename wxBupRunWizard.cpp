/*-----------------------------------------------------------------
 * Name:        wxBupRunWizard.cpp
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
#include "wxBupFrameh.h" // needs to be first
#include "wxBupWizTreebookh.h"
#include <wx/tokenzr.h>
#include "wxBupDestPathValidatorh.h"
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
 * Some of the data is transferred to & from the dialog in 
 * bool MyBupJobDataDlg::TransferDataToWindow( void ) &
 * bool MyBupJobDataDlg::TransferDataFromWindow( void ) all in
 * wxBupWizTreebook.cpp
 */
bool MyFrame::RunWizard( bool bIsEditing )
{
  wxString wsPath;
  wxString wsT;
  wxFileName wfnJobFile;
  int w, h;
  if( !g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal.IsEmpty() )
  {
    wsPath = g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal;
    wfnJobFile.Assign( wsPath );
  }
  MyBupJobDataDlg dlg( this, wxGetApp().m_wsConfigDir, wsPath, bIsEditing );
  dlg.SetSize( g_iniPrefs.data[IE_EDIT_JOB_DLG_W].dataCurrent.lVal,
    g_iniPrefs.data[IE_EDIT_JOB_DLG_H].dataCurrent.lVal );
  dlg.GetSplitterJobEditFiles()->SetSashPosition( 
    g_iniPrefs.data[IE_EDIT_JOB_DLG_SASH].dataCurrent.lVal );
  // display the job file name in the title bar
  if ( bIsEditing )
  {
    wsT =  _T("Editing: ");
  }
  else
  {
    wsT =  _T("New Job: ");
  }
  dlg.SetLabel( wsT + 
      (g_iniPrefs.data[IE_EDIT_SHOWFULLPATH].dataCurrent.bVal ?
      wfnJobFile.GetFullPath() : wfnJobFile.GetFullName() ) );
  // clear the destn drive path explanation  for now
  dlg.GetStaticTextDestnNotes()->SetLabel( wxEmptyString );
  // parse the interfering tasks list and check the appropriate boxes
  // need to use something other than the default white space
  wxStringTokenizer wTk( g_jobData.data[IEJ_INTERFERING_TASKS].dataCurrent.wsVal, ";" );
  int i = 0;
  while ( wTk.HasMoreTokens() )
  {    
    wxString wsToken = wTk.GetNextToken();    
    // process token here
    if( wsToken.IsSameAs( _T("Thunderbird") ) )
      dlg.GetCheckListBoxTasks2Kill()->Check( 0 );
  }
#if 0
  /* Since I cannot see the character the user  might have entered, I
   * pretty well seem to have to depend on the validator code and
   * act only on the 'enter' key,verify the path as valid or complain
   * and make him/her correct the path
   * But NOTE: the enter key will open a dir select dialog, not what I want
   * so ignore the validator for now
   */
  // setup the Dest'n directory picker validator
  wxString wsStartingValue;// = wsrData;
  
  wxDirPickerCtrl* pDestDir = dlg.GetDirPickerDestinationDir();
  wxTextCtrl *pText = pDestDir->GetTextCtrl();
    // Must include the FILTER type-----------------------------------+
  pText->SetValidator( MyCellTextValidator( wxFILTER_NONE, &wsStartingValue ) );
  MyCellTextValidator *pVal = (MyCellTextValidator *)pText->GetValidator();
//  pVal->SetMaxLen( lMax );
#endif
  // setup the working copy of the app INI data
  wxGetApp().MakeConfigCopy( g_iniPrefs, dlg.m_iniPrefs );
  // setup the working copy of the job data 
  wxGetApp().MakeJobConfigCopy( g_jobData, dlg.m_jobData );
  if ( dlg.ShowModal() == wxID_OK )
  {
    // any data changed
    if ( bIsEditing )
    {
      // any changes?
      if (wxGetApp().JobCompareOldAndNew(g_jobData, dlg.m_jobData) )
      {
        // no - just return
        SelectJobLine( m_lSelectedLine );
        // save new size in any case, even if user cancelled the edit operation
        dlg.GetSize( &w, &h );
        g_iniPrefs.data[IE_EDIT_JOB_DLG_W].dataCurrent.lVal = w;
        g_iniPrefs.data[IE_EDIT_JOB_DLG_H].dataCurrent.lVal = h;
        g_iniPrefs.data[IE_EDIT_JOB_DLG_SASH].dataCurrent.lVal =
          dlg.GetSplitterJobEditFiles()->GetSashPosition();
        return true;
      }
      else
      {
        // save the new job data 
        wxGetApp().MakeJobConfigCopy( dlg.m_jobData, g_jobData );
        // need to check if the job name has changed & update it if necessary
        if( !g_jobData.data[IEJ_JOB_NAME].dataCurrent.wsVal.IsSameAs(
          wfnJobFile.GetName() ) )
        {
          wfnJobFile.SetName( g_jobData.data[IEJ_JOB_NAME].dataCurrent.wsVal );
        }
        g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal =
          wfnJobFile.GetFullPath();
        // the file name etc was set up and verified in the wizard
        if ( ::wxFileExists( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal ) )
        {
          // ask permission to overwrite - if no permission - same as 'cancel'
          int iAnswer = wxMessageBox( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal +
            _(" exists, overwrite it?"), _T("Confirm"), wxYES_NO );
          if ( iAnswer != wxYES )
            return false;
          // else we mark the last update date as 'unknown'
          g_jobData.data[IEJ_LAST_BACKUP_DATE].dataCurrent.wsVal = _T("unknown");
        }
      }
      goto WriteJobfile;
    }
    else  // copy the new data to the job file
    {
      wxGetApp().MakeJobConfigCopy( dlg.m_jobData, g_jobData );
    } 
WriteJobfile: 
    wxASSERT( !g_jobData.data[IEJ_JOB_NAME].dataCurrent.wsVal.IsEmpty() );
    wxASSERT( !g_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal.IsEmpty() );
    wxASSERT( !g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal.IsEmpty() );
    wxASSERT( !g_jobData.data[IEJ_BAG_NAME_BASE].dataCurrent.wsVal.IsEmpty() );
    wxASSERT( !g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal.IsEmpty() );
    wxASSERT( !g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal.IsSameAs(
      _T("eBagSize_Unknown") ) );
    // save new size in any case, even if user cancelled the edit operation
    dlg.GetSize( &w, &h );
    g_iniPrefs.data[IE_EDIT_JOB_DLG_W].dataCurrent.lVal = w;
    g_iniPrefs.data[IE_EDIT_JOB_DLG_H].dataCurrent.lVal = h;
    g_iniPrefs.data[IE_EDIT_JOB_DLG_SASH].dataCurrent.lVal =
      dlg.GetSplitterJobEditFiles()->GetSashPosition();
    if ( ! bIsEditing ) // is it a new job file
    {
      // so create the file & carry on 
      wxString wsT;
      wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
      wsT = wfnJobFile.GetFullPath();
      wfnJobFile.SetFullName( g_jobData.data[IEJ_JOB_NAME].dataCurrent.wsVal );
      wsT = wfnJobFile.GetFullPath();
      wfnJobFile.SetExt( _T("wbj" ) );
      wsT = wfnJobFile.GetFullPath();
      g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal =
        wfnJobFile.GetFullPath();
      g_jobData.data[IEJ_LAST_BACKUP_DATE].dataCurrent.wsVal = _T("unknown");
      // the file name etc was set up and verified in the wizard
      if ( ::wxFileExists( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal ) )
      {
        // ask permission to overwrite - if no permission - same as 'cancel'
        int iAnswer = wxMessageBox( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal +
          _(" exists, overwrite it?"), _T("Confirm"), wxYES_NO );
        if ( iAnswer != wxYES )
          return false;
      }
    }
    g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal =
      wfnJobFile.GetFullPath();
    wxString wsT;
    wsT.Printf( _T("%s %d 'm_pJobConf == NULL' failed"), __FILE__, __LINE__ ); 
    wxASSERT_MSG( m_pJobConf == NULL, wsT );

    wxGetApp().SaveCloseJobConfig( wfnJobFile.GetFullPath() );

    // add this file to the job list window
    FillJobList();
    
    // todo
    // find the line and select it - ready for further action
    wxString wsJobFile = wfnJobFile.GetFullName();
    // we ought to have at least one at this point
    int iNLines = m_textCtrlJobList->GetNumberOfLines();
    wsT.Printf( _T("%s %d 'm_textCtrlJobList->GetNumberOfLines() == 0' "), 
      __FILE__, __LINE__ );
    wxASSERT_MSG( m_textCtrlJobList->GetNumberOfLines(), wsT );
    if ( !SelectJobLineByText( wsJobFile ) )
    {
      // any errors were reported by the called function, 
      // nothing more to do
      SelectJobLine( 0 );
    }
    SetBagSize();
    // enable the 'Scan' button once we have a good job file loaded
    // done in UpdateUI handler
    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_bHaveDirOrFile = true;
    return true;
  }
  // save new size in any case, even if user cancelled the edit operation
  dlg.GetSize( &w, &h );
  g_iniPrefs.data[IE_EDIT_JOB_DLG_W].dataCurrent.lVal = w;
  g_iniPrefs.data[IE_EDIT_JOB_DLG_H].dataCurrent.lVal = h;
  g_iniPrefs.data[IE_EDIT_JOB_DLG_SASH].dataCurrent.lVal =
    dlg.GetSplitterJobEditFiles()->GetSashPosition();
  return false;
}

// ------------------------------- eof ------------------------------
