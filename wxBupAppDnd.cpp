/*-----------------------------------------------------------------
 * Name:        wxBupAppDnd.cpp
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
#include "wxBupAppDndh.h"
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
#if defined ( WANT_APP_DND )
/**
 * Seems we can drop files anywhere except on the output notebook.
 */
// Drag'n'Drop class
bool DnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& wasFilenames )
{
  int  iCount = wasFilenames.GetCount();
  wxString wsStr;
  if( iCount > 1 )
  {
   wxMessageBox( _T("For now we can only handle one file at a time") );
   return false;
  }
  wxGetApp().m_bFromDnD = true;
  // can we assume that the file exists if has just been dropped?
  // open the job file 
  wxString wsFile = wasFilenames[0];
  wxFileName wfnJobFile( wsFile );
  wxGetApp().RestoreJobConfig( wfnJobFile.GetFullPath() );
  wxGetApp().m_frame->m_bHaveDirOrFile = true;
  // create any type of command event here
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, MyFrameBase::wxID_TB_BACKUP );
  event.SetInt( 0 );
  // display a separator
  wxLogMessage( _T("---------------\n") );
  // send in a thread-safe way
  wxPostEvent( wxGetApp().m_frame, event );
  return true;
}
#endif
// ------------------------------- eof ------------------------------
