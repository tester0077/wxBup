/*-----------------------------------------------------------------
 * Name:        wxBupEditJobFile.cpp
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *---------------------------------------------------------------- */

/*----------------------------------------------------------------
 * Standard wxWidgets headers
 *---------------------------------------------------------------- */
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
#if defined(_MSC_VER ) // from Autohotkey-hummer.ahk
// only good for MSVC
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
 * Open the options dialog & present the Jobs page.
 */
void MyFrame::OnEditJobFile(wxCommandEvent& WXUNUSED(event) )
{
  m_bEditJob = true;
  if ( RunWizard( m_bEditJob ) )
  {
    // enable the 'Scan' button once we have a good job file loaded
    // done in UpdateUI handler
    wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
    m_bHaveDirOrFile = true;
  }
  SelectJobLine( m_lSelectedLine );
}

// ------------------------------- eof ------------------------------
