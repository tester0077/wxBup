/*-----------------------------------------------------------------
 * Name:        wxBubJobListContextMenu.cpp
 * Purpose:     Job List context menu handlers
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *---------------------------------------------------------------- */

/*----------------------------------------------------------------
 * Standard wxWidgets headers
 *---------------------------------------------------------------- */
#// Note __VISUALC__ is defined by wxWidgets, not by MSVC IDE
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
#include "wx/filefn.h"
#include "wxBuph.h"
#include "wxBupFrameh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )
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
// verify that the item is ok and insult the user if it is not
#define CHECK_ITEM( item ) if ( !item.IsOk() ) {                                 \
                             wxMessageBox(wxT("Please select some item first!"), \
                                          wxT("wxBU Job tree error"),            \
                                          wxOK | wxICON_EXCLAMATION,             \
                                          this);                                 \
                             return;                                             \
                           }
// ------------------------------------------------------------------
/**
 * Get the job tree item info. Note we cannot get the label directly.
 * Likely have to add it as a data item.
 */
void MyFrame::OnJobListRightClick( wxContextMenuEvent& event)
{
  if ( IsWorkerRunning() )  // no new selection allowed while a job is in progress
    return;
  wxPoint ptMouse = event.GetPosition();
  wxPoint wptJobList = m_textCtrlJobList->ScreenToClient( ptMouse );
  SelectLineFromMouse( wptJobList );

  ShowContextMenu( ptMouse );
}

// ------------------------------------------------------------------

void MyFrame::ShowContextMenu(const wxPoint& pos)
{
  wxMenu menu;

  menu.Append(wxID_CONTEXT_EDIT_JOB,   _T("Edit job"));
  menu.Append(wxID_CONTEXT_DELETE_JOB, _T("Delete job"));
  menu.Append(wxID_CONTEXT_RENAME_JOB, _T("Rename job ..."));
  menu.Append(wxID_CONTEXT_SAVE_AS,    _T("Save job as ..."));
  menu.Append(wxID_CONTEXT_RUN_JOB,    _T("Run job"));
  int x = pos.x;
  int y = pos.y;
  ScreenToClient( &x, &y );

  PopupMenu(&menu, x, y);
}

// ------------------------------------------------------------------
/**
 * Handle the right-click pop-up menu item to edit an existing job.
 * All job files are expected to reside in the default job file directory.
 */
void MyFrame::OnContextEditJob( wxCommandEvent& WXUNUSED( event ) )
{
  m_bEditJob = true;
  if ( RunWizard( m_bEditJob ) )
  {
    // enable the 'Scan' button once we have a good job file loaded
    // done in UpdateUI handler
    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_bHaveDirOrFile = true;
  }
}

// ------------------------------------------------------------------
/**
 * Handle the right-click pop-up menu item to run an existing job.
 * All job files are expected to reside in the default job file directory.
 */
void MyFrame::OnContextRunJob( wxCommandEvent& event )
{
  OnStartWorker(event);
}
// ------------------------------------------------------------------
/**
 * Handle the right-click pop-up menu item to delete an existing job.
 * All job files are expected to reside in the default job file directory.
 */
void MyFrame::OnContextDeleteJob( wxCommandEvent& WXUNUSED( event ) )
{
  DeleteJobFile();
}

// ------------------------------------------------------------------

/**
 * Handle the right-click pop-up menu item to save an existing job under
 * a new name - mostly for backup or to modify an existing job temporarily.
 * All job files are expected to reside in the default job file directory.
 */
void MyFrame::OnContextJobSaveAs( wxCommandEvent& WXUNUSED( event ) )
{
  wxString wsNewJob;
  // make up job file name 
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  wfnJobFile.SetFullName( wsSelection );
  if( !SaveJobAs( wfnJobFile.GetFullPath(), wsNewJob ) )
  {
    // any errors were reported by the called function, 
    // nothing more to do
    return;
  }
  else
  {
    // rebuild the tree.
    FillJobList();
    // & select the old job file again
    if ( !SelectJobLineByText( wsSelection ) )
    {
      SelectJobLine( 0 );
    }
  }
}

// ------------------------------------------------------------------
/**
 * Handle the right-click pop-up menu item to edit an existing job.
 * All job files are expected to reside in the default job file directory.
 */
void MyFrame::OnContextRenameJob( wxCommandEvent& WXUNUSED( event ) )
{
  wxString wsNewJob;
  // make up job file name 
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  wfnJobFile.SetFullName( wsSelection );
  // save the file with a new name 
  if( !SaveJobAs( wfnJobFile.GetFullPath(), wsNewJob ) )
  {
    return; // nothing more to do
  }
  else
  {
    // and then remove the old file if user didn't cancel
    wxRemoveFile( wfnJobFile.GetFullPath() );
    // & rebuild the tree.
    FillJobList();
    wfnJobFile.Assign( wsNewJob );
    wxString wsJobFile = wfnJobFile.GetFullName();
    if ( !SelectJobLineByText( wsJobFile ) )
    {
      // any errors were reported by the called function, 
      // nothing more to do
      SelectJobLine( 0 );
    }
  }
}

// ------------------------------- eof ------------------------------
