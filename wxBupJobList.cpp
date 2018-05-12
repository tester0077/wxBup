/*-----------------------------------------------------------------
 * Name:        wxBupJobList.cpp
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
#include "wxBupFrameh.h" // needs to be first
#include "wx/dir.h"
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
/*  Since this cannot be done properly until after the .INI file has
 * been read and processed, this code was moved from the main
 * frame initialization to some time later.
 * Called from wxBuFrame.cpp
 * Note: selecting a job file also removes the unnecessary caret 
 * from this widow
 */
void MyFrame::FillJobList()
{
  m_bJobSelected = m_bHaveDirOrFile = false;
  m_lSelectedLine = -1;
  m_textCtrlJobList->Clear();
  wxString wsFileName;

  wxString wsJobFiles = _T("*.wbj");
  wxDir wdJobDir( wxGetApp().m_wsConfigDir );
  if( !wdJobDir.IsOpened() )
  {
    wxMessageBox( _("Could not open: ") + wxGetApp().m_wsConfigDir, 
    _("Error"), wxOK | wxICON_EXCLAMATION );
    return;
  }
  long lCurLineNo = 0;
  long lCurStart = 0;

  wxTextAttr wtaSelLine;
  wxTreeItemId tidCur;
  int iLineLen;
  wxString wsLastFileName;
#if defined( WANT_LAST_JOB_DATE )
  // need to strip off the 'date' string, if any
  wxString wsJobFile = g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal;
  int iDColon = wsJobFile.Find ( _T("::") );
  if ( iDColon != wxNOT_FOUND )
    wsJobFile = wsJobFile.Left( iDColon );
  wsJobFile = wsJobFile.Trim( true );
  wxFileName wfnLastFile( wsJobFile );
#else
  wxFileName wfnLastFile( g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
#endif
  if ( wfnLastFile.Exists() )
    wsLastFileName = wfnLastFile.GetFullName();
  else
    g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = wxEmptyString;
  // this only returns the file names; no path info
  bool bFound = wdJobDir.GetFirst( &wsFileName, wsJobFiles );
  while ( bFound )
  {
#if defined( WANT_LAST_JOB_DATE )
    // we want to append the last backup date to the job name
    // so now we need to extract just the one line
    // Treat the job file as a text file
    wxString wsDate;
    wxFileName wfnJobFileName;
    wfnJobFileName.SetPath( wxGetApp().m_wsConfigDir );
    wfnJobFileName.SetFullName( wsFileName );
    GetLastBackupDate( wfnJobFileName.GetFullPath(), wsDate );
#endif
    iLineLen = wsFileName.Length();
#if defined( WANT_LAST_JOB_DATE )
    m_textCtrlJobList->AppendText( wsFileName + _T("  ::  ") + wsDate + _T("\n"));
#else
    m_textCtrlJobList->AppendText( wsFileName + _T("\n"));
#endif
    if ( !wsLastFileName.IsEmpty() )
    {
      if( g_iniPrefs.data[IE_LAST_JOB_LOAD_AT_START].dataCurrent.bVal )
      {
        if ( wsFileName.Matches( wsLastFileName ) )
        {
          m_bJobSelected = true;
          m_lSelectedLine = lCurLineNo;
        }
      }
      else
      {
        m_lSelectedLine = 0;
        break;
      }
    }
    lCurLineNo++;
    lCurStart += iLineLen + 1;
    bFound = wdJobDir.GetNext( &wsFileName );
  }
  if ( wsLastFileName.IsEmpty() && !m_bJobSelected )
  {
    if ( lCurLineNo == 0 ) 
      return;
    else
    {
      m_bJobSelected = true;
      m_lSelectedLine = 0;
    }
  }
  // if we have any lines
  SelectJobLine( m_lSelectedLine );
}

// ------------------------------------------------------------------
/**
 * Select (highlight) the text of line lLine.
 * We need to handle the case of that last empty line. It seems 
 * to come back even if I try to not add it in the first place
 */
void MyFrame::SelectJobLine( long lLine )
{
  wxString  wsSel; 
  long      lCaretStart = 0, 
            lCaretEnd = 0;
  long      lNumberOfLines = m_textCtrlJobList->GetNumberOfLines() - 1;

  // make sure we stay within bounds
  if( lLine < 0 )     // -1 => no line selected
    return;
  if( lLine > lNumberOfLines )
    lLine = lNumberOfLines;
  wsSel = m_textCtrlJobList->GetLineText( lLine );
  if( lLine && wsSel.Length() == 0 )
    lLine--;
  // get length of line for highlighting
  wsSel = m_textCtrlJobList->GetLineText( lLine );
#if defined( WANT_LAST_JOB_DATE )
  // need to strip off the 'date' string
  wsSel = wsSel.BeforeFirst( ':' );
#endif
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = wsSel.Trim();
  wfnJobFile.SetFullName( wsSelection );
  g_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal = 
    wfnJobFile.GetFullPath();

  lCaretStart = m_textCtrlJobList->XYToPosition( 0, lLine );
  lCaretEnd   = m_textCtrlJobList->XYToPosition( wsSel.Length(), lLine );
  m_textCtrlJobList->SetSelection( lCaretStart, lCaretEnd );
  // enable the 'Backup' button once we have a good job file loaded;
  // done in UpdateUI handler, here we just set the controlling variable
//  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
  m_bHaveDirOrFile = true;
  m_bJobSelected = true;
  m_lSelectedLine = lLine;
  SetMainFrameTitle( wsSel );
  SetStatusText( wfnJobFile.GetFullPath(), 1 );
}

// ------------------------------------------------------------------
/** 
 * Select the job (line) the user clicked on;
 * This ought to display the details for this job on the right.
 * Added code to select the complete line the user clicked on, even if it 
 * consists of multiple words.
 * The trailing space on the last line seems to count as a separate line.
 * Code fixed to disallow user to select beyond last line.
 */
void MyFrame::OnJobsSelectLine(wxMouseEvent& event)
{
  if ( IsWorkerRunning() )  // no new selection allowed while a job is in progress
    return;
  wxPoint ptMouse = event.GetPosition();

  SelectLineFromMouse( ptMouse );
  event.Skip();
}

// ------------------------------------------------------------------
/**
 * Selecting a job in the 'job' list also loads the corresponding 
 * data into the global job data structure.
 */
void MyFrame::SelectLineFromMouse( wxPoint ptMouse )
{
  wxString          wsSel; 
  long              lx, ly;
  long              lFrom, lTo;
  // clear any current selection - should not be needed 
  // - new selection ought to cancel any existing selection
  // gets the character position in the data as a string
  // counting all line breaks as 1 extra char
  // the trailing space on the last line seems to count as a separate line
  m_lNumberOfLines = m_textCtrlJobList->GetNumberOfLines() - 1;
  if ( m_lNumberOfLines == 0 )
  {
    return;
  }
  m_textCtrlJobList->HitTest( ptMouse, &m_lJobSelLineAbsPos );
  m_textCtrlJobList->PositionToXY( m_lJobSelLineAbsPos, &lx, &ly );
  SelectJobLine( ly );
  m_textCtrlJobList->GetSelection( &lFrom, &lTo );
  if (lFrom == lTo)
  {
    m_lSelectedLine = -1;
    return; // no selection
  }
  m_lSelectedLine = ly;
  // open the job file 
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wxString wsSelection = m_textCtrlJobList->GetStringSelection().Trim();
  wfnJobFile.SetFullName( wsSelection );
  wxGetApp().RestoreJobConfig( wfnJobFile.GetFullPath() );
  SetBagSize();
  m_bHaveDirOrFile = true;
  return;
}

// ------------------------------------------------------------------
/**
 * Select a line in the job list, given the full job line text,
 * such as "job.wbj"
 */
bool MyFrame::SelectJobLineByText( wxString a_wsJobFile )
{
  wxString wsT;
  bool bFound = false;
  int iNLines = m_textCtrlJobList->GetNumberOfLines();
  for (long i = 0; i < iNLines; i++)
  {
    wsT = m_textCtrlJobList->GetLineText( i ).MakeLower();
#if defined( WANT_LAST_JOB_DATE )
    // need to strip off the 'date' string
    wsT = wsT.BeforeFirst( ':' );
    wsT.Trim();
#endif
    if (wsT.IsSameAs( a_wsJobFile.MakeLower() ))
    {
      SelectJobLine( i );
      bFound = true;
      break;  // we're done here
    }
  }
  return bFound;
}

// ------------------------------------------------------------------
/**
 * Is called when a key is pressed while the job list field has focus.
 */
void MyFrame::OnJobListChar(wxKeyEvent& event)
{
  if ( IsWorkerRunning() )  // no new selection allowed while a job is in progress
    return;
  // counting all line breaks as 1 extra char
  // the trailing space on the last line seems to count as a separate line
  m_lNumberOfLines = m_textCtrlJobList->GetNumberOfLines() - 1;
  wxChar uc = event.GetUnicodeKey();
  if ( uc != WXK_NONE )
  {
    // It's a "normal" character. Notice that this includes
    // control characters in 1..31 range, e.g. WXK_RETURN or
    // WXK_BACK, so check for them explicitly.
    if ( uc >= 32 )
    {
      ;//wxLogMessage("You pressed '%c'", uc);
    }
    else
    {
      // It's a control character 
      //...
    }
  }
  else // No Unicode equivalent.
  {
    // It's a special key, deal with all the known ones:
    switch ( event.GetKeyCode() )
    {
    case WXK_UP: 
      if ( m_lSelectedLine > 0 )
        SelectJobLine( --m_lSelectedLine );
      break;
    case WXK_DOWN:
      if( m_lSelectedLine <=  m_lNumberOfLines )
        SelectJobLine( ++m_lSelectedLine );
      break;
    case WXK_DELETE:
      // works but need to handle 'next' entry better - see FS#1885
      DeleteJobFile();
      event.Skip();
      break;
#if 0
    case WXK_F1:
      //... give help ...
      break;
#endif
    }
  }
}

// ------------------------------------------------------------------
#if defined( WANT_LAST_JOB_DATE )
bool MyFrame::GetLastBackupDate( wxString wsFileName, wxString & wsDate )
{
  wxTextFile wtfJob;
  wsDate = _T("unknown");
  size_t iLine = 0;
  wxString wsLine;
  wtfJob.Open( wsFileName );
  if ( !wtfJob.IsOpened() )
  {
    return false;
  }
  for ( iLine = 0; iLine < wtfJob.GetLineCount(); iLine++ )
  {
    wsLine = wtfJob.GetLine( iLine );
    if( wsLine.StartsWith( _T("Backup="), &wsDate ))
    {
      if( wsDate.IsEmpty() )
        wsDate = _T("unknown");
      wtfJob.Close();
      return true;
    }
  }
  wtfJob.Close();
  return false;
}
#endif
// ------------------------------- eof ------------------------------
