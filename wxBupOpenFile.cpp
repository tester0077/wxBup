/*-----------------------------------------------------------------
 * Name:        wxBupOpenFile.cpp
 * Purpose:     Read a wxBup job file
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets license
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
/**
 * Open anew file using the file selector dialog.
 * Note the file name becomes the job name.
 */
// ------------------------------------------------------------------
void MyFrame::OnNewJobFile( wxCommandEvent& WXUNUSED(event) )
{
  bool bIsEditing = false;
  if ( RunWizard( bIsEditing ))
  {
    // enable the 'Scan' button once we have a good job file loaded
    // done in UpdateUI handler
    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_bHaveDirOrFile = true;

  }
}

// ------------------------------------------------------------------
/**
 * Open an existing file using the file selector dialog
 */
// ------------------------------------------------------------------
void MyFrame::OnSelJobFile( wxCommandEvent& WXUNUSED(event) )
{
  wxString wsPath;
  
  // the called routine will start from last used job directory
  if( !GetFileFromUser( wsPath ) )
    return; // abandon, if user canceled operation 
  g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wsPath;
  MyOpenFile( wsPath );
  m_statusBar->SetStatusText( wsPath, 1 );
  SetBagSize();
  // enable the 'Scan' button once we have a good job file loaded
  // done in UpdateUI handler
  m_bHaveDirOrFile = true;
  SetMainFrameTitle( wsPath );
}

// ------------------------------------------------------------------
/**
 * Get a file path & name from the user using the dialog.
 * Note: this can return file shortcuts (*.lnk  files) which cause the 
 * app to crash!!
 * use validators???!!!
 * see notes for SaveAs handler in: wxBupSaveRenameJobs.cpp
 */
bool MyFrame::GetFileFromUser( wxString &wsJobFile )
{
  wxString wsSourceDrive;
  wxString wsSourceDir;
  wxString wsDestDirRoot;
  wxString defaultDir;
  wxFileName wfnLastFile = g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal;
  wxString wsLastPath = wfnLastFile.GetPathWithSep();
 
  wxString caption = _("Choose a job file");
  wxString wildcard = _("Job files (*.wbj)|*.wbj|All files (*.*)|*.*");
  if ( wsLastPath.IsEmpty() )
    defaultDir = wxGetApp().m_wsConfigDir;
  else
    defaultDir = wsLastPath;
  wxString defaultFilename = wxEmptyString;

  wxFileDialog dialog( this, caption, defaultDir, 
    g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal,
    wildcard, wxFD_OPEN );
  if (dialog.ShowModal() != wxID_OK)
  {
    return false;
  }
  wsJobFile = dialog.GetPath();
  return true;
}

// ------------------------------------------------------------------
/**
 * Display the selected/dropped file - assumed to be a .wbj file for now.
 * The passed in filename includes the full path.
 */
void MyFrame::MyOpenFile(wxString wsFilePathAndName )
{
  wxString      wsT;
  wxBusyCursor  wait;
  wxFileName wfnJob( wsFilePathAndName );
  if( !wxFileName::FileExists( wsFilePathAndName ) )
  {
    wsT.Printf( _("The file %s\ndoes not exist!"), wsFilePathAndName );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
    wxLogError( wsT );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    wxMessageBox( wsT, _("Error"), wxOK );
    return;
  }
  // verify the file can be opened; we assume it exists
  HANDLE fh;
  fh = CreateFile( wsFilePathAndName.c_str(), GENERIC_READ, 
    0 /* no sharing! exclusive */, NULL, OPEN_EXISTING, 0, NULL);
  if ((fh == NULL) || (fh == INVALID_HANDLE_VALUE))
  {
    // Some other process must have the file open;
    // we could open it in read-only mode!!
    CloseHandle(fh);
    wsT.Printf( _("Cannot open the file %s for reading.\n") +
      _("It is likely open by another process!\n\nOpen it in read-only mode?"), 
      wsFilePathAndName );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
    wxLogError( wsT );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    wxMessageBox( wsT, _("Error"), wxOK );
    return;
  }
  CloseHandle(fh);
  m_statusBar->SetStatusText( wsFilePathAndName, 1 );
  ///    _("For now, ONLY .wbj files are handled") );
  if( ! wfnJob.GetExt().MakeUpper().Matches( _T("WBJ") ) )
  {
    wxString wsT1( _T("We can only handle .WBJ files!") );
    wxLogError( wsT1 );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wxEmptyString;
    wxMessageBox( wsT1, _("Error"), wxOK );
    return; 
  }
  wxGetApp().RestoreJobConfig( wsFilePathAndName );
  g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wsFilePathAndName;
  SetBagSize();
  // enable the 'Scan' button once we have a good job file loaded
  // done in UpdateUI handler
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  m_bHaveDirOrFile = true;
  SetMainFrameTitle( wfnJob.GetFullName() );
#if defined ( WANT_FILEHISTORY )
  // Add this file to the MRU list
  wxGetApp().m_pFileHistory->AddFileToHistory( wsFilePathAndName );
#endif
}

// ------------------------------------------------------------------
bool MyFrame::OpenExistingFile(wxString wsFilePathAndName )
{
  wxString      wsT;
  wxBusyCursor  wait;

  wxFileName wfnJob( wsFilePathAndName );
  if( wxFileName::FileExists( wsFilePathAndName ) )
  {
    wsT.Printf( 
      _("The file %s already exist exist!\nOverwrite it and loose all existing data?"), 
      wsFilePathAndName );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
    wxLogWarning( wsT );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    int iRet = wxMessageBox( wsT, _("Warning"), wxICON_QUESTION | wxYES_NO );
    if ( iRet != wxYES )
    {
      return false;
    }
    // delete the file - will be done by creating the new file below
  }
  // verify the file can be opened for writing
  HANDLE fh;
  fh = CreateFile( wsFilePathAndName.c_str(), GENERIC_WRITE, 
    0 /* no sharing! exclusive */, NULL, CREATE_ALWAYS, 0, NULL);
  if ((fh == NULL) || (fh == INVALID_HANDLE_VALUE))
  {
    // Some other process must have the file open;
    // we could open it in read-only mode!!
    CloseHandle(fh);
    wsT.Printf( _("Cannot open the file %s for writing.\n"), 
      wsFilePathAndName );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
    wxLogError( wsT );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    wxMessageBox( wsT, _("Error"), wxOK );
    return false;
  }
  CloseHandle(fh);
  m_statusBar->SetStatusText( wsFilePathAndName, 1 );

  ///    _("For now, ONLY .WBJ files are handled") );
  if( ! wfnJob.GetExt().MakeUpper().Matches( _T("WBJ") ) )
  {
    wxString wsT2( _T("We can only handle .WBJ files!") );
    wxLogError( wsT2 );
    m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wxEmptyString;
    wxMessageBox( wsT2, _("Error"), wxOK );
    return false; 
  }
  g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wsFilePathAndName;
  SetBagSize();
  // enable the 'Scan' button once we have a good job file loaded
  // done in UpdateUI handler
  m_bHaveDirOrFile = true;
  SetMainFrameTitle( wfnJob.GetFullName() );
#if defined ( WANT_FILEHISTORY )
  // Add this file to the MRU list
  wxGetApp().m_pFileHistory->AddFileToHistory( wsFilePathAndName );
#endif
  return true;
}

// ------------------------------------------------------------------
void MyFrame::SetBagSize()
{
  wxString wsT;

  //save the selected 'bag' size
  int i = /*m_eSelectedBagSize = */ EBupBagSizeTypes::AsEnum(
    g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
  switch( i )
  {
  case EBupBagSizeTypes::eBagSize_Custom:
    wsT = _T("Custom 'bag' size not implemented yet");
    wxMessageBox( wsT, _T("Error"), wxOK );
    if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 1 )
    {
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
      wxLogError( wsT );
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    }
    break;
  case EBupBagSizeTypes::eBagSize_144MB:
    // Note: I really need to take into account the space requirements 
    // of directory entries, as well as any extra space required 
    // because od differnet allocation ubit or sector sizes
    // for now fudge it
    m_ullSelectedBagSize = 1400 * 1024 - 
      g_iniPrefs.data[IE_BAG_FLOPPY_RESERVE].dataCurrent.lVal;
    m_ulMediumAllocUnit = 512;
    break;
  case EBupBagSizeTypes::eBagSize_5MB:
    m_ullSelectedBagSize = 5 * 1024 * 1024;
    m_ulMediumAllocUnit = 0;
    break;
  case EBupBagSizeTypes::eBagSize_10MB:
    m_ullSelectedBagSize = 10 * 1024 * 1024;
    m_ulMediumAllocUnit = 0;
    break;
  case EBupBagSizeTypes::eBagSize_650MB:
    m_ullSelectedBagSize = 681984000;
    m_ulMediumAllocUnit = 2048;
    break;
  case EBupBagSizeTypes::eBagSize_700MB:
    m_ullSelectedBagSize = 737280000;
    m_ulMediumAllocUnit = 2048;
    break;
  case EBupBagSizeTypes::eBagSize_3_7GB:
    m_ullSelectedBagSize = 3723500000;
    m_ulMediumAllocUnit = 2048;
    break;
  case EBupBagSizeTypes::eBagSize_4_7GB:
    // seems I need to allow some leeway for DVDs
    // to be burned with CDBurnerXP, at least
    m_ullSelectedBagSize = 4700000000 - 
      g_iniPrefs.data[IE_BAG_4_7_RESERVE].dataCurrent.lVal;
    m_ulMediumAllocUnit = 2048;
    break;
  case EBupBagSizeTypes::eBagSize_8_5GB:
    // this needs MS Large integer support, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/hh802931%28v=vs.85%29.aspx
    // uses Multiply128 function
#if 1
    wsT = _T("DVD-DL not implemented yet");
    m_ullSelectedBagSize = 0;
    m_ulMediumAllocUnit = 0;
    goto bagError;
#else
    m_ullSelectedBagSize = 2 * 3723500000;
#endif
    break;
  case EBupBagSizeTypes::eBagSize_25GB:
    // this needs MS Large integer support, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/hh802931%28v=vs.85%29.aspx
    // uses Multiply128 function
#if 1
    wsT = _T("Blueray DVDs not implemented yet");
    m_ullSelectedBagSize = 0;
    m_ulMediumAllocUnit = 0;
    goto bagError;
#else
    m_ullSelectedBagSize = 25 * 1024 * 1024 * 1024;
#endif
    break;
  default:
    wsT = _T("Default 'bag' size - why??");
  bagError:
    wxMessageBox( wsT, _T("Error"), wxOK );
    if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 1 )
    {
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr(*wxRED));
      wxLogError( wsT );
      GetTextCtrlLog()->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
    }
    break;
  }
}

// ------------------------------- eof ------------------------------
