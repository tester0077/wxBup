/*-----------------------------------------------------------------
 * Name:        wxBupResultsGrid.cpp
 * Purpose:     code for the 'results' grid.
 *
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
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
#if defined( WANT_GRID )
// ------------------------------------------------------------------
/**
 * Initialize the 'results' grid.
 * Note: labels are saved in and set from .INI file
 */
void MyFrame::InitializeResultsGrid()
{
  size_t i = 0;
  m_grid->InsertCols( 0, m_wasColLabels.GetCount() );
  m_grid->SetRowLabelSize( 1 );
  m_grid->EnableGridLines( true );
  m_grid->SetSelectionMode( wxGrid::wxGridSelectRows );
  for ( i = 0; i <  m_wasColLabels.GetCount(); i++ )
  {
    m_grid->SetColLabelValue( i, m_wasColLabels[i] );
    m_grid->SetColSize( i, m_walColWidths[i] );
  }
  ResizeResultsDataGrid();
}

// ------------------------------------------------------------------
/**
 * This function is intended to resize the grid when the size of the
 * main frame or any of its components is changed.
 * Also after any of the editable data fields have been changed.
 * It was originally based on hgrid.h by Yuri Borsky, but I had to 
 * make significant changes for this code.
 */
void MyFrame::ResizeResultsDataGrid()
{
#if 0
  int iFixedWidth = 0;
  int iFullWidth = 0, h;
  int iCols = m_grid->GetNumberCols();
  iCols = iCols;  // keep compiler quiet
  wxWindow *pWinParent = m_grid->GetGridWindow();
  pWinParent->GetClientSize( &iFullWidth, &h );
  // this is needed during startup, while the grid is being constructed
  if ( iFullWidth < 10 )
    return;
  m_grid->BeginBatch();
  size_t i = 0;
  // get total assigned width
  long lTotalWidth = 0;

  for ( i = 0; i <  m_wasColLabels.GetCount(); i++ )
  {
    if( i <  m_wasColLabels.GetCount() - 1 )
    {
      lTotalWidth += m_walColWidths[i];
    }
    else
      m_grid->SetColSize( i, __max( 0, iFullWidth - (int)lTotalWidth ) );
  }
  m_grid->EndBatch();
#endif
}

// ------------------------------------------------------------------
void MyFrame::OnGridSize(wxSizeEvent& event)
{
  ResizeResultsDataGrid();
  event.Skip(); 
}

// ------------------------------------------------------------------

void MyFrame::OnGridColSize( wxGridSizeEvent& ev )
{
  const int col = ev.GetRowOrCol();
  m_walColWidths[col] = m_grid->GetColSize(col);
  ResizeResultsDataGrid();
  ev.Skip();
}
#endif
// ------------------------------- eof ------------------------------
