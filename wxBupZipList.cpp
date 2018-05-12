/*-----------------------------------------------------------------
 * Name:        wxBupZipList.cpp
 * Purpose:     code to insert a new zip file into MyZipList, sorted in 
 *              ascebding order by file size
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
#include "wxBupZipListh.h"
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
// ------------------------------------------------------------------
/** Insert a new file into the list, according to file size
 */
bool  MyFrame::InsertFileInZipList( wxString a_wsDestFullPath,
  wxString a_wsSrcFullPath, unsigned long long a_ullSrcFileSize,
  unsigned long long a_ullZipFileSize )
{
  // check directory depth
  wxString wsFile;
  wxFileName wfnFile( a_wsDestFullPath );
  m_uiZipDirLevelDepth = __max( wfnFile.GetDirCount(), m_uiZipDirLevelDepth );
  MyZipListElement *pMEl;
  pMEl = new MyZipListElement( a_wsDestFullPath, a_wsSrcFullPath, 
      a_ullSrcFileSize, a_ullZipFileSize );
  if( m_zipList.size() == 0 )
  {
    m_zipList.push_front( *pMEl );
  }
  else  // find the proper place to insert the new item; saves sorting later
  {
    bool bFound = false;
    // iterate over the list in STL syntax
    for (std::list<MyZipListElement>::iterator iter = m_zipList.begin(); 
      iter != m_zipList.end(); ++iter)
    {  
      if( a_ullZipFileSize > iter->m_ullZipSize )
      {
        // insert new item ahead of last item tested
        m_zipList.insert( iter, *pMEl );
        bFound = true;
        break;  // .... and quit
      }
    }
    if( !bFound )
    {
      // insert new item after last item 
      m_zipList.push_back( *pMEl );
    }
  }
  // the list grabs the data; we're done with it!
  delete pMEl;
  return true;
}

// ------------------------------------------------------------------
void MyFrame::OnDumpZipList(wxCommandEvent& WXUNUSED( event ) )
{
  DumpZipList();
}

// ------------------------------------------------------------------
void MyFrame::DumpZipList()
{
  // display the   total # of files
  int i = 0;
  wxLogMessage( _T("%15d files"), m_zipList.size() );  
  // display the sorted list to the log window
  for (std::list<MyZipListElement>::iterator iter = m_zipList.begin(); 
      iter != m_zipList.end(); ++iter, i++ )
  { 
    if( i%100 )
      wxMilliSleep( 50 );//wxYield();  // try to give GUI a chance to update the clock ticks
    wxLogMessage( _T("%15llu, %s"), iter->m_ullZipSize, iter->m_wsZipFileName );  
  }
}

// ------------------------------- eof ------------------------------
