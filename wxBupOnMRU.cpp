/*-----------------------------------------------------------------
 * Name:        wxBupOnMRU.cpp
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
/**
 * Retrieve a file name from the MRU list and make sure it exists
 */
#if defined( WANT_FILEHISTORY )
void MyFrame::OnMruFile( wxCommandEvent & event )
{
  size_t i;
  wxFileName wfName;
  i = event.GetId() - wxID_FILE1;
  wxString wsFile( wxGetApp().m_pFileHistory->GetHistoryFile( i ) );
  
  if( ! wsFile.IsEmpty() )
  {
    wfName = wsFile;
    // we've found a file in the history list;
    // see if we can find it
    if( !wfName.FileExists() )
    {
      wxString wsMsg;
      wsMsg.Printf( _("File %s does not exist!\n") +
        _("Remove it from the MRU list?"), wsFile );
      int answer = wxMessageBox( wsMsg, _("Error"), wxYES_NO | wxICON_ERROR );
      if ( answer == wxYES )
      {
        // remove it from the list so it will move to the top
        // when LoadFile() adds it
        wxGetApp().m_pFileHistory->RemoveFileFromHistory( i );
        return;
      }
      else
        return;
    }
    MyOpenFile( wsFile );
  }
}
#endif

// ------------------------------- eof ------------------------------
