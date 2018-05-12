/*-----------------------------------------------------------------
 * Name:        wxBupSummary.cpp
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
#include <wx/tokenzr.h>
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
extern jobPrefs_t g_jobData;
// ------------------------------------------------------------------

void MyFrame::UpdateSummary( wxString a_wsJobFileName, wxString a_wsJobFilePath,
    wxString a_wsTimeBreakout )
{
  wxString wsT;
  wxString wsT1;
  m_richTextCtrlSummary->Clear();
  m_richTextCtrlSummary->SetDefaultStyle(wxRichTextAttr());
  m_richTextCtrlSummary->Freeze();
  m_richTextCtrlSummary->BeginSuppressUndo();

  m_richTextCtrlSummary->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->BeginFontSize(10);
  wxString lineBreak = (wxChar) 29;
  m_richTextCtrlSummary->WriteText( wxString( _("General Information")) );
  m_richTextCtrlSummary->EndFontSize();
  m_richTextCtrlSummary->EndBold();
  m_richTextCtrlSummary->Newline();
//  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
//       (m_JobTypeInProgress == MY_RESTORE_JOB)  ||
//       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  { 
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Job File Name: ") );
    m_richTextCtrlSummary->EndBold();
    if ( !a_wsJobFileName.IsEmpty() )
    {
      m_richTextCtrlSummary->WriteText( a_wsJobFileName );
    }
    m_richTextCtrlSummary->Newline();
  }
  // job file path
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Job File Path:\n") );
  m_richTextCtrlSummary->EndBold();
  if ( !a_wsJobFilePath.IsEmpty() )
  {
    m_richTextCtrlSummary->WriteText( a_wsJobFilePath );
  }
  m_richTextCtrlSummary->Newline();
  // source path
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Source Path:\n") );
  m_richTextCtrlSummary->EndBold();
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
       (m_JobTypeInProgress == MY_RESTORE_JOB)  ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  { 
    if ( !g_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal.IsEmpty() )
    {
      wxStringTokenizer tokenizer(g_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal, ";");
      wxString wsToken; // set up to not have to trim last token's newline
      while ( tokenizer.HasMoreTokens() )
      {   
        wsToken += tokenizer.GetNextToken();
        m_richTextCtrlSummary->WriteText( wsToken );
        wsToken = _T("\n");
      }
    }
  }
  else // must be 'test
  {
    if ( !g_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal.IsEmpty() )
    {
      m_richTextCtrlSummary->WriteText( 
        g_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal );
    }
  }
  
  // destination path
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
       (m_JobTypeInProgress == MY_RESTORE_JOB)  ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    m_richTextCtrlSummary->Newline();
    m_richTextCtrlSummary->BeginBold();
  
    m_richTextCtrlSummary->WriteText( _("Destination Path: ") );
    m_richTextCtrlSummary->EndBold();
    if ( !g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal.IsEmpty() )
    {
#if 1
      wxString wsTest = g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
      ExpandPath( wsTest );
      m_richTextCtrlSummary->WriteText( wsTest );
#else
      m_richTextCtrlSummary->WriteText( 
        g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal ); 
#endif
      m_richTextCtrlSummary->Newline();
    }  
  }
#if 0
  // display the selected 'bag' size
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
       (m_JobTypeInProgress == MY_RESTORE_JOB)  ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Selected Destination Size: ") );
    m_richTextCtrlSummary->EndBold();
    if ( g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal.IsEmpty() )
      m_richTextCtrlSummary->WriteText( 
        g_jobData.data[IEJ_BAG_TYPE].dataDefault.wsVal );
    else
      m_richTextCtrlSummary->WriteText( 
        g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
    m_richTextCtrlSummary->Newline();
  }
#endif
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Total Time: ") );
  m_richTextCtrlSummary->EndBold();
  if ( m_wsTotalTime.IsEmpty() )
  {
    wsT = _T("--:--:--");
  }
  else
  {
    wsT = m_wsTotalTime;
  }
  m_richTextCtrlSummary->WriteText( wsT );
  m_richTextCtrlSummary->Newline();
  if ( !a_wsTimeBreakout.IsEmpty() )
  {
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Times: ") );
    m_richTextCtrlSummary->EndBold();
    wsT = a_wsTimeBreakout;
    m_richTextCtrlSummary->WriteText( wsT );
    m_richTextCtrlSummary->Newline();
  }
  // number of files
  if ( m_szNTotalFiles )
    wsT.Printf( _T("%Iu"), m_szNTotalFiles );
  else
    wsT = _(" n/a");
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Total # of files: ") );
  m_richTextCtrlSummary->EndBold();
  m_richTextCtrlSummary->WriteText( wsT );
  m_richTextCtrlSummary->Newline();
  // number of directories
  if ( m_ulDirs )
    wsT.Printf( _T("%ld"), m_ulDirs );
  else
    wsT = _(" n/a");
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Total # of Directories: ") );
  m_richTextCtrlSummary->EndBold();
  m_richTextCtrlSummary->WriteText( wsT );
  m_richTextCtrlSummary->Newline();
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    // number of bags
    if( m_ulBags )
      wsT.Printf( _T("%ld"), m_ulBags );
    else
      wsT = _(" n/a");
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Total # of 'Bags':  ") );
    m_richTextCtrlSummary->EndBold();
    m_richTextCtrlSummary->WriteText( wsT );
    m_richTextCtrlSummary->Newline();
  }

  // bag size
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB)  ||
       (m_JobTypeInProgress == MY_RESTORE_JOB) ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    wsT.Printf( _T("%s"),
      g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );

    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("'Bag' size: ") );
    m_richTextCtrlSummary->EndBold();
    m_richTextCtrlSummary->WriteText( wsT );
    m_richTextCtrlSummary->Newline();
  }
  
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB) ||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    // max path length
    if ( m_ulMaxDestPathLen )
      wsT.Printf( _T("%lu"), m_ulMaxDestPathLen );
    else
      wsT = _(" n/a");
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Max Dest'n Path Length ") );
    m_richTextCtrlSummary->EndBold();
    m_richTextCtrlSummary->WriteText( _("(max 511 chars): ") );
    m_richTextCtrlSummary->WriteText( wsT );
    m_richTextCtrlSummary->Newline();
    // .. and the path
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Longest Dest'n Path: ") );
    m_richTextCtrlSummary->EndBold();
    if ( !m_wsLongestDestPath.IsEmpty() )
    {
      m_richTextCtrlSummary->WriteText( m_wsLongestDestPath );
    }
    else
    {
      m_richTextCtrlSummary->WriteText( _(" n/a") );
    }
    m_richTextCtrlSummary->Newline();
  
    // max file name length
    if ( m_ulMaxDestFileNameLen )
    {
      if ( m_ulMaxDestFileNameLen > 127 )
      {
        wsT.Printf( _T("%lu"), m_ulMaxDestFileNameLen, 
        m_ulMaxDestFileNameLen - 127);
        wsT1.Printf( _T(" - + %d" ), m_ulMaxDestFileNameLen - 127 );
      }
      else
        wsT.Printf( _T("%lu"), m_ulMaxDestFileNameLen );
    }
    else
      wsT = _(" n/a");
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Max Dest'n File Name Length") );
    m_richTextCtrlSummary->EndBold();
    m_richTextCtrlSummary->WriteText( _(" (max 127 chars): ") );
    m_richTextCtrlSummary->WriteText( wsT );
    if ( m_ulMaxDestFileNameLen > 127 )
    {
      m_richTextCtrlSummary->BeginBold();
      m_richTextCtrlSummary->WriteText( wsT1 );
      m_richTextCtrlSummary->EndBold();
    }
    m_richTextCtrlSummary->Newline();
    // .. and the file name
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Longest Dest'n File Name: ") );
    m_richTextCtrlSummary->EndBold();
    if ( !m_wsLongestDestFileName.IsEmpty() )
    {
      m_richTextCtrlSummary->WriteText( m_wsLongestDestFileName );
    }
    else
    {
      m_richTextCtrlSummary->WriteText( _(" n/a") );
    }
    m_richTextCtrlSummary->Newline();
  }
#if 0 
  // max directory depth in source
  wsT.Printf( _T("%u"), m_uiSrcDirLevelDepth );
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Max. Source directory depth: ") );
  m_richTextCtrlSummary->EndBold();
  m_richTextCtrlSummary->WriteText( wsT );
  m_richTextCtrlSummary->Newline();
#endif
  // max directory depth in source
  if ( (m_JobTypeInProgress == MY_BACKUP_JOB)||
       (m_JobTypeInProgress == MY_JOB_TYPE_NONE) )
  {
    if ( m_uiZipDirLevelDepth )
      wsT.Printf( _T("%u"), m_uiZipDirLevelDepth );
    else
      wsT = _(" n/a");
    m_richTextCtrlSummary->BeginBold();
    m_richTextCtrlSummary->WriteText( _("Max. Destination directory depth: ") );
    m_richTextCtrlSummary->EndBold();
    m_richTextCtrlSummary->WriteText( wsT );
    m_richTextCtrlSummary->Newline();
  }

  // total source size
  FormatSize( (double)m_ullTotalZipsSize, wsT );
//  wsT = CalculateSize( m_ullTotalSrcBytes );
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Total Source Size: ") );
  m_richTextCtrlSummary->EndBold();
  if ( !wsT.IsEmpty() )
  {
    m_richTextCtrlSummary->WriteText( wsT );
  }
  m_richTextCtrlSummary->Newline();
  // total destination size
  FormatSize( (double)m_ullTotalZipBytes, wsT );
//  wsT = CalculateSize( m_ullTotalZipBytes );
  m_richTextCtrlSummary->BeginBold();
  m_richTextCtrlSummary->WriteText( _("Total Zip Size: ") );
  m_richTextCtrlSummary->EndBold();
  if ( !wsT.IsEmpty() )
  {
    m_richTextCtrlSummary->WriteText( wsT );
  }
  m_richTextCtrlSummary->Newline();

  m_richTextCtrlSummary->EndAlignment();
  m_richTextCtrlSummary->Newline();
  m_richTextCtrlSummary->EndSuppressUndo();
  m_richTextCtrlSummary->Thaw();
}

// ------------------------------- eof ------------------------------
