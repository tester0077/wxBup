/*-----------------------------------------------------------------
 * Name:        wxBupSaveRenameJobs.cpp
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
#include "wxBuph.h"
#include "wxBupFrameh.h"
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
void MyFrame::OnRenameFile(wxCommandEvent& WXUNUSED( event ) )
{
  wxString wsNewJob;
  // make up the old job file name 
  wxFileName wfnOldJobFile;
  wfnOldJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  wfnOldJobFile.SetFullName( wsSelection );
  // save the file with a new name 
  if( ! SaveJobAs( wfnOldJobFile.GetFullPath(), wsNewJob ) )
  {
    return; // nothing to do
  }
  // else the job file has a new name so we can
  // remove the old file
  wxRemoveFile( wfnOldJobFile.GetFullPath() );
  wxFileName wfnNewJobFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
  
  // & rebuild the tree.
  FillJobList();
  // we ought to have at least one at this point
  int iNLines = m_textCtrlJobList->GetNumberOfLines();
  wxLogWarning( _T("%s %d 'm_textCtrlJobList->GetNumberOfLines() == 0' "), 
    __FILE__, __LINE__ );
  wxASSERT( m_textCtrlJobList->GetNumberOfLines() );
  if ( !SelectJobLineByText( wfnNewJobFile.GetFullName() ) )
  {
    // any errors were reported by the called function, 
    // nothing more to do
    SelectJobLine( 0 );
  }
  // enable the appropriate tool bar buttons once we have a good job 
  // file loaded - done in UpdateUI handler
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  m_bHaveDirOrFile = true;
}

// ------------------------------------------------------------------

void MyFrame::OnSaveAsFile(wxCommandEvent& event)
{
  wxString wsNewJob;
  // make up job file name 
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  wfnJobFile.SetFullName( wsSelection );
  SaveJobAs( wfnJobFile.GetFullPath(), wsNewJob );
}

// ------------------------------------------------------------------
/* Implementation note:
 * I had been having problem with this dialog not opening the default 
 * directory I expected. I asked on the forum and got this reply:
 *  Yes, the default directory is ignored when you pass a default path 
 *  including folder different from the default directory and the folder 
 *  from path is used instead. As I said before, that's actually a behaviour 
 *  I would expect as a user. You wanted to have the full path put into 
 *  the field for the file name?
 *  See: https://forums.wxwidgets.org/viewtopic.php?f=1&t=42293&p=171389
 *
 * Using the full path with file name seems to give me what I expect
 */
bool MyFrame::SaveJobAs( wxString a_wsFullPath, wxString& ar_wsNewFullPath )
{
  ar_wsNewFullPath = wxEmptyString;
  wxString caption = _("Save file as");
  wxString wildcard = _("WBJ files (*.wbj)|*.wbj|All files (*.*)|*.*");
  wxString defaultDir = wxGetApp().m_wsConfigDir;
  wxFileDialog dialog( this, caption, defaultDir, a_wsFullPath,
    wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
  if (dialog.ShowModal() != wxID_OK)
  {
    return false;
  }
  wxString wsDestPath = dialog.GetPath();
  wxFileName wfnNewFile( wsDestPath );

  if( ::wxFileExists( wsDestPath ) )
  {
    wxString wsT;
    wsT.Printf( _("Another file with the name %s exist in that directory\n") +
      _("Are you sure you want to overwrite it"), wfnNewFile.GetFullName() );
    if( wxMessageBox( wsT, _("Confirm"), wxYES_NO | wxICON_QUESTION ) != wxYES )
    {
      return false;
    }
  }
  else
  {
    // create new job first
    MyFileCopy( a_wsFullPath, wsDestPath, true );
    ar_wsNewFullPath = wsDestPath; 
#if 0
    // now we can delete the old one
    ::wxRemoveFile( a_wsFullPath );
    // & rebuild the tree.
    FillJobList();
    // we ought to have at least one at this point
    int iNLines = m_textCtrlJobList->GetNumberOfLines();
    wxLogWarning( _T("%s %d 'm_textCtrlJobList->GetNumberOfLines() == 0' "), 
    __FILE__, __LINE__ );
    wxASSERT( m_textCtrlJobList->GetNumberOfLines() );
    if ( !SelectJobLineByText( wfnNewFile.GetFullName() ) )
    {
      SelectJobLine( 0 );
    }
    // enable the 'Scan' button once we have a good job file loaded
    // done in UpdateUI handler
    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_bHaveDirOrFile = true;
#endif
  }
  return true;
}

// ------------------------------- eof ------------------------------
