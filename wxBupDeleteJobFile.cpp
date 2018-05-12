/*-----------------------------------------------------------------
 * Name:        wxBuoDeleteJobFile.cpp
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
void MyFrame::OnDeleteFileCLick(wxCommandEvent& WXUNUSED( event ) )
{
  DeleteJobFile();
}

// ------------------------------------------------------------------

void MyFrame::DeleteJobFile()
{
  // make up job file name 
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  if ( wsSelection.IsEmpty() )
  {
    return; // nothing to do
  }
  wfnJobFile.SetFullName( wsSelection );
  if( wxMessageBox( _("Are you sure you want to delete\n") + 
    wfnJobFile.GetFullPath(),
    _("Confirm"), wxYES_NO | wxICON_QUESTION ) == wxYES )
  {
    ::wxRemoveFile( wfnJobFile.GetFullPath() );
    FillJobList();
    int iN = m_textCtrlJobList->GetNumberOfLines();
    if ( iN )
    {
      SelectJobLine( __max( 0, m_lSelectedLine - 1 ) );
    }
  }
}

// ------------------------------- eof ------------------------------
