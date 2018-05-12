/*-----------------------------------------------------------------
 * Name:        wxBupContentGrid.cpp
 * Purpose:     code for the 'job contents' grid.
 *              This grid shows the files which are part of this 'job'
 *              and their cirrent status
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
//#include "wxBuph.h"
#include "wxBupFrameh.h"

// ------------------------------------------------------------------
#if defined( _MSC_VER )
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif
// ------------------------------------------------------------------
/**
 * Initialize the 'results' grid.
 */
void MyFrame::InitializeJobContentsGrid()
{
#if 0
  wxGridCellAttr *pColAttr = new wxGridCellAttr;
  pColAttr->SetAlignment(  wxALIGN_CENTRE ,  wxALIGN_CENTRE  );
  m_gridJobContents->SetColAttr( 2, pColAttr );
  // set up 'Results' data grid
  m_gridJobContents->InsertCols( 0, 5 );
  m_gridJobContents->SetRowLabelSize( 1 );
  m_gridJobContents->EnableGridLines( true );
  m_gridJobContents->SetColLabelValue( 0, _T("Source") );
  m_gridJobContents->SetColLabelValue( 1, _T("State") );
  m_gridJobContents->SetColLabelValue( 2, _T("Last Modified") );
  m_gridJobContents->SetColLabelValue( 3, _T("Last Backup") );
  m_gridJobContents->SetColLabelValue( 4, _T("Size") );
  m_gridJobContents->SetSelectionMode( wxGrid::wxGridSelectRows );
  m_gridJobContents->SetDefaultRenderer( new wxGridCellAutoWrapStringRenderer);
#endif
}
#if 0
// ------------------------------------------------------------------
/**
 * Update the 'results' grid for the currently selected location.
 */
void MyFrame::UpdateResults( MyUrlListElement *pEl )
{
  wxString wsT;
  m_gridJobContents->BeginBatch();   // wait with display until we are all done
  // clear out any old data from the display
  m_gridJobContents->ClearGrid();
  // remove all rows for now - except top labels
  // also leave one to display the URL name
  int iRows = m_gridJobContents->GetNumberRows();
  if( iRows )
    m_gridJobContents->DeleteRows( 0, iRows );
  size_t n = pEl->m_oaTargets.GetCount();
  if(  n == 0 )
  {
    m_gridJobContents->InsertRows( 0, 1 );
  }
  else
  {
    m_gridJobContents->InsertRows( 0, n );
  }
  m_gridJobContents->SetCellValue( 0, 0, pEl->m_wsUrlName );
  if( n > 0 )
  {
    for( size_t j = 0; j < n; j++ )
    {
      wsT = pEl->m_oaTargets[j].m_wsTarget;
      m_gridJobContents->SetCellValue( j, 1, pEl->m_oaTargets[j].m_wsTarget );
      if ( pEl->m_oaTargets[j].m_bFound )
      {
        m_gridJobContents->SetCellValue( j, 2, _("yes") ); 
        m_gridJobContents->SetCellTextColour( j, 2, *wxGREEN );
      }
      else
      {
        m_gridJobContents->SetCellValue( j, 2, _("no") );
        m_gridJobContents->SetCellTextColour( j, 2, *wxRED );
      }
    }
  }
  else
  {
     m_gridJobContents->SetCellValue( 0, 1, _("--none specified--") );
     m_gridJobContents->SetCellValue( 0, 2, _("n/a") );
  }
  m_gridJobContents->EndBatch();
  ResizeResultsDataGrid();
}

// ------------------------------------------------------------------
/**
 * Display the data for the given line # in 'Location' window
 */
void MyFrame::UpdateResultsForLine( long lSelectedLine )
{
  long  lLine = 0;
  MyUrlList::iterator iter; 
  for ( lLine = 0, iter = wxGetApp().m_UrlList.begin(); 
    iter != wxGetApp().m_UrlList.end(); ++iter, lLine++ ) 
  {     
    MyUrlListElement *pEl = *iter;
    if( lLine == lSelectedLine )
    {
      // update the results
      UpdateResults( pEl );
    }
  }
}
#endif
// ------------------------------------------------------------------
/**
 * This function is intended to resize the grid when the size of the
 * main frame or any of its components is changed.
 * Also after any of the editable data fields have been changed.
 * It was originally based on hgrid.h by Yuri Borsky, but I had to 
 * make significant changes for this code.
 */
void MyFrame::ResizeJobContentsGrid()
{
#if 0
  int iFullWidth = 0, h;
  int iCols = m_gridJobContents->GetNumberCols();
  iCols = iCols;  // keep compiler quiet
  wxWindow *pWinParent = m_gridJobContents->GetGridWindow();
  pWinParent->GetClientSize( &iFullWidth, &h );
  // this is needed during startup, while the grid is  being constructed
  if ( iFullWidth < 10 )
    return;
  // we need to figure out the minimum width for at least the last column,
  // to make sure it stays wide enough for it label: Found
  // first get the font
//  int x, y;
//  wxString wsT = m_gridJobContents->GetColLabelValue( 2 );
//  m_gridJobContents->GetTextExtent( wsT, &x, &y );
#if 0
  m_gridJobContents->AutoSizeColumns( false );
//#else

  // set the minimum with of the last column to 1.5 * x 
  // - why do we need the fudge??
  m_gridJobContents->SetColMinimalWidth( 0, x + x/2 );
  
  iFullWidth -= m_gridJobContents->GetRowLabelSize();
  m_gridJobContents->AutoSizeColumn( 1, false );
  iFixedWidth += m_gridJobContents->GetColSize( 0 );
  m_gridJobContents->AutoSizeColumn( 2, false );
  iFixedWidth += m_gridJobContents->GetColSize( 2 );
  m_gridJobContents->SetColSize( 1, iFullWidth - iFixedWidth );
#endif
#endif
}

// ------------------------------------------------------------------
void MyFrame::OnJobContentsGridSize(wxSizeEvent& event)
{
  ResizeJobContentsGrid();
  event.Skip(); 
}

// ------------------------------- eof ------------------------------
