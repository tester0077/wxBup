/*-----------------------------------------------------------------
 * Name:        wxBupWizSrcDnD.cpp
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
#include "wxBupWizSrcDnDh.h"
//#include "wxBuph.h"
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
bool MyDnDFile::OnDropFiles(wxCoord, wxCoord, const wxArrayString& wasFilenames)
{
  size_t nFiles = wasFilenames.GetCount();
  wxString wsStr;
  wsStr.Printf( _("%d files dropped"), (int)nFiles);

  if (m_pOwner != NULL)
  {
    wxLogMessage( wsStr );
    for ( size_t n = 0; n < nFiles; n++ )
    {
      if (wxDirExists(wasFilenames[n]))
      {
        wsStr = wasFilenames[n] + _T("\\");
      }
      else
      {
        wsStr = wasFilenames[n];
      }
      m_pOwner->Append( wsStr );
      wxLogMessage( wasFilenames[n] );
    }
  }
  return true;
}

// ------------------------------------------------------------------
/**
 * To work this event as expected, the user has to release the mouse
 * button after selecting the newitem BEFORE trying to move or drag it.
 * If the mopuse button is not released, the previous item is still 
 * selected and wil be used in the drag operation.
 */
void MyBupJobDataDlg::OnBeginDrag( wxTreeEvent& WXUNUSED(event ) )
{
    wxFileDataObject data;
    wxString wsPath;
    wsPath = GetGenericDirCtrlSrcPicker()->GetPath();
    data.AddFile( GetGenericDirCtrlSrcPicker()->GetPath() );

    wxDropSource dragSource(this);
    dragSource.SetData(data);
    dragSource.DoDragDrop();
}

// ------------------------------- eof ------------------------------
