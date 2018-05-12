/*-----------------------------------------------------------------
 * Name:        wxBupTestRestoreOneBag.cpp
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
#include <wx/textfile.h>
#include <wx/tokenzr.h>

#include "wxBupTestRestoreThreadh.h"
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
/**
 * The bag log looks something like:
      2016-05-20 10:32:38 - Bag 1 of 4
      wxBUP ver. 0.0, build 82, verbosity: 2
      Moved, Exclusions: "", MoveOnly: ""

      Original data location     Backup location
      ===================================================================
      C:\bin\ME\MEPROG.ZIP       | D:\test\tt01\bin\ME\MEPROG.ZIP
      C:\bin\ME\MEMISC.ZIP       | D:\test\tt01\bin\ME\MEMISC.ZIP
      C:\bin\ME\KEYMAP.S         | D:\test\tt01\bin\ME\KEYMAP.S
      Backup location                                  Restored Location
      ===================================================================
      D:\test\tt01\bin\ME\MEPROG.ZIP  | ?:\restoreDir\bin\ME\MEPROG.ZIP
      D:\test\tt01\bin\ME\MEMISC.ZIP  | ?:\restoreDir\bin\ME\MEMISC.ZIP
      D:\test\tt01\bin\ME\KEYMAP.S    | ?:\restoreDir\bin\ME\KEYMAP.S 
                   \--------------------------------/
      bag log
      d:\test\tt01\tt1.log
      \ ----------/  <= bag log directory       

      But note: at present the comparison is between the original and 
      the intermediate data location. for instance, if I backup from C:\bin to 
      D:\bupTest, the those are the paths recorded in the bag log, 
      but once I burn the data to a CD or DVD, I really want to 
      compare the CD or DVD data with the original location, not with 
      the intermediate staging directory. 
      In fact it might not even exist any longer if I clean up after burning 
      the data to CD or DVD.
      To do so, the bag log now records all destination paths as relative
      to the parent of the bag directory:
      C:\ansicon\ANSI64.dll | bag01\wxbup\ansicon\ANSI64_dll.zip
 */
void MyRestoreThread::RestoreOrTestOneBag( wxString wsBagLog )
{
  wxString wsCurrentLine;
  wxString wsExclusionPattern;
  wxString wsMoveOnlyPattern;
  bool     bFailed = false;
  size_t   szLineCount;
  bool bZipped = false;
  wxFileName wfnBagLog( wsBagLog );
  // treat the bag log as a text file
  wxTextFile wtfBagLog( wsBagLog );
  if( !wtfBagLog.Exists() )
  {
#if defined ( LOG_EVENT )
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    event.SetInt( MY_RED );   // my color enums
    event.SetExtraLong( 5 );  // verbosity
    event.SetString( wsBagLog + _(" does not exist") );
    wxQueueEvent( m_frame, event.Clone() );
#endif
    return;
  }
  wtfBagLog.Open();
  if( !wtfBagLog.IsOpened() )
  {
    wxMessageBox( _("Failed to open: " ) + wsBagLog, _("Error"),
      wxOK );
#if defined ( LOG_EVENT )
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    event.SetInt( MY_RED );   // my color enums
    event.SetExtraLong( 5 );  // verbosity
    event.SetString( _("Failed to open: " ) + wsBagLog );
    wxQueueEvent( m_frame, event.Clone() );
#endif
    return;
  }
  szLineCount = wtfBagLog.GetLineCount();
  wsCurrentLine = wtfBagLog.GetFirstLine();
  // first process the header
  while ( ! wtfBagLog.Eof() )
  { 
    szLineCount--;
    if ( wsCurrentLine.Lower().StartsWith( _T("wxbup") ) )
    {
      wsCurrentLine = wtfBagLog.GetNextLine();
      continue;
    }
    if( wsCurrentLine.IsEmpty() )
    {
      wsCurrentLine = wtfBagLog.GetNextLine();
      continue;
    }
    if ( wsCurrentLine.StartsWith( _T("====") ) )
    {
      // we're done with the setup; data follows
      break;
    }
    wxString wsToken; 
    wxStringTokenizer wsTok( wsCurrentLine, _T(",") );
    while( wsTok.HasMoreTokens() )
    {
      wsToken = wsTok.GetNextToken();
      if ( wsToken.Lower().StartsWith( _T("zipped") ) )
        bZipped = true;
      if( wsToken.Lower().StartsWith( _T("exclusions") ) )
      {
        ;
      }
      if( wsToken.Lower().StartsWith( _T("moveonly") ) )
      {
        ;
      }
    }
    wsCurrentLine = wtfBagLog.GetNextLine();
  }
  m_ulTotalFiles += szLineCount;
  bool bRet;
  wxString wsT;
  // then process the actual file list
  while ( ! wtfBagLog.Eof() && !m_frame->IsWorkerCancelled() )
  {
    wsCurrentLine = wtfBagLog.GetNextLine();
    if ( wsCurrentLine.IsEmpty() )
      continue;
#if 0//defined( _DEBUG )
//    if ( wsCurrentLine.Matches( _T("valet.zip")))
//      int iop = 0;
#endif
    wxString wsToken; 
    wxString wsOrigFile;
    wxString wsTestFile;
    wxString wsZipOrCopy;
    bool     bWasZipped;
    wxStringTokenizer wsTok( wsCurrentLine, _T("|") );
    while( wsTok.HasMoreTokens() )
    {
      wsOrigFile = wsTok.GetNextToken();
      wsTestFile = wsTok.GetNextToken();
      // clean up any leading or trailing spaces
      wsOrigFile = wsOrigFile.Trim();
      wsOrigFile = wsOrigFile.Trim( false );
      // is this a zipped or copied file?
      wsZipOrCopy = wsOrigFile.BeforeFirst( ' ' );
      // need to strip out the 'Copied/Zipped' prefix
      wsOrigFile = wsOrigFile.AfterFirst( ' ' );
      bWasZipped = wsZipOrCopy.MakeLower().IsSameAs( _T("zipped:"));
      wsTestFile = wsTestFile.Trim();
      wsTestFile = wsTestFile.Trim( false );
      {
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
        // send a log message to the main frame log window
        // to indicate which file we are working on.
        // This needed especially when the first & largest file
        // is very large, otherwise there will be no real feedback
        // until it is handled completely
        event.SetInt( MY_BLACK );     // my color enums
        event.SetExtraLong( 3 );      // verbosity
        event.SetString( _("Working on: ") + wsTestFile );
        wxQueueEvent( m_frame, event.Clone() );
        m_frame->SetStatusText( wsTestFile, 1 );
        // display feedback asap, specially important if first file
        // is very large
        if( m_ulTotalFilesToCheck == 0 )
        wxLogMessage( _T("%s %d 'm_ulTotalFilesToCheck == 0'"),
          __FILE__, __LINE__ );
        wxASSERT( m_ulTotalFilesToCheck );
        float fPercent = ((float)(m_ulTotalFilesChecked) * 100 ) /
          (float)(m_ulTotalFilesToCheck);
        wsT.Printf(  _T("Files: %ld/%d %3.1f%%, OK: %ld, Bad: %ld"), 
          m_ulTotalFilesChecked + 1, m_ulTotalFilesToCheck, 
          fPercent, m_ulCheckedFilesGood, m_ulCheckedFilesFailed );
        m_frame->GetStatusBar()->SetStatusText( wsT, 0 );
        m_frame->GetGaugeProgress()->SetValue( 
         (m_ulTotalFilesChecked * 100)/m_ulTotalFilesToCheck );
        // and the taskbar icon's as well
        m_frame->GetAppProgressInd()->SetValue(
          (m_ulTotalFilesChecked * 100)/m_ulTotalFilesToCheck );
      }
      // is the backup likely to contain zipped files
      // or are ALL just moved 'as is'
      if ( bZipped )  
      {
        m_ulTotalFilesChecked++;
        wxASSERT( m_ulTotalFilesChecked <= m_ulTotalFilesToCheck );
        if ( m_bRestoring )
        {
          // need to fix up the paths
          /*  the backed up data is relative to the 'bag' directory
                                                    V            
                 D:\test\tt01\bin\ME\KEYMAP.S    | tt01\bin\ME\KEYMAP.S 
                        |    \-- test file -/          \-target dir /    ^
                        V         +----------------------------------+
               bag log
                 d:\test\tt01\tt1.log
                 \ ----------/  <= bag log directory
            */
          wxString wsBag = wfnBagLog.GetPath();
          wxFileName wfnTargetDir( 
            g_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal,
            wxEmptyString );
          wxFileName wfnSrcFile( wsTestFile );
          wfnSrcFile.RemoveDir( 0 );
          wfnSrcFile.MakeAbsolute( wsBag );
          wxString wsSrcFile = wfnSrcFile.GetFullPath();
#if 0//defined ( _DEBUG )
          wxFileName wfnTest( wfnSrcFile.GetFullPath() );
          wxString wsTest = wfnTest.GetFullName().MakeLower();
          if (wsTest.IsSameAs(_T("meprog.zip")))
          {
            int i = 0;
            i++;
          }
#endif
          wxFileName wfnTestFile( wsTestFile );
          wfnTestFile.RemoveDir( 0 );
          wfnTestFile.MakeAbsolute( 
            g_iniPrefs.data[IE_LAST_RESTORE_TARGET].dataCurrent.wsVal );
          wsTestFile = wfnTestFile.GetFullPath();
          // need to create the path for this file in any case
          wxFileName wfnRestoredFile;
          wfnRestoredFile.SetPath ( wfnTestFile.GetPath() );
          // we only want the target dir; the file name comes from the zip file
          wxString wsRestoredFileDir = wfnRestoredFile.GetPath();
          bRet = wfnRestoredFile.Mkdir( wxS_DIR_DEFAULT , wxPATH_MKDIR_FULL );
          if ( ! bRet )
            wxLogError( _T("%s %d Cannot create %s"), 
              __FILE__, __LINE__, wsRestoredFileDir );
          wxASSERT( bRet );
          wxString wsSrc = wfnSrcFile.GetFullPath();
          wxString wsDest = wfnRestoredFile.GetFullPath() +  wfnTestFile.GetFullName();
          if( bWasZipped )
          {
            //                        source zip,   where to send the output
            bRet = Restore1ZippedFile( wfnSrcFile.GetFullPath(), wsRestoredFileDir );
            if ( ! bRet )
              wxLogWarning( _T("%s %d Restore failed for %s -> %s"), 
                __FILE__, __LINE__, wfnSrcFile.GetFullPath(), wsRestoredFileDir );
            wxASSERT( bRet );
          }
          else
          {
            // need to fix up the paths - see comment at top
            // destination path & name ---V     
            // source path & name --V
            bRet = Restore1File( wsSrc, wsDest );
          }
        }
        else // must be 'testing
        {
          // need to fix up the paths
          /*  the backed up data is relative to the 'bag' directory
                                                    V            
                 D:\test\tt01\bin\ME\KEYMAP.S    | tt01\bin\ME\KEYMAP.S 
                        |    \-- test file -/          \-target dir /    ^
                        V         +----------------------------------+
               bag log
                 d:\test\tt01\tt1.log
                 \ ----------/  <= bag log directory
            */
          wxString wsBag = wfnBagLog.GetPath();
          wxFileName wfnTestFile( wsTestFile );
          wfnTestFile.RemoveDir( 0 );
          wfnTestFile.MakeAbsolute( wsBag );
          wsBag = wfnTestFile.GetFullPath();
          bRet = CompareZippedFile( wsOrigFile, wsBag ); // sends a log message
        }
        if ( bRet )
        {
          bFailed = false;
          m_ulCheckedFilesGood++;
          if ( m_bRestoring )
            wsT.Printf( _T("Restored zipped: %s -TO- %s"),  wsOrigFile, wsTestFile );
          else
            wsT.Printf( _T("Compared zipped: %s -AND- %s"),  wsOrigFile, wsTestFile );
        }
        else
        {
          bFailed = true;
          m_ulCheckedFilesFailed++;
          if ( m_bRestoring )
            wsT.Printf( _T("Restore failed: %s -TO- %s"),  wsOrigFile, wsTestFile );
          else
            wsT.Printf( _T("Compare failed: %s -AND- %s"),  wsOrigFile, wsTestFile );
          // show the failures on the special files page - done
          // in MyFrame::OnWorkerEventLogMsg()
        }
      }
      else  // file was moved 'as is'
      {
        // need to fix up the paths
        /*  the backed up data is relative to the 'bag' directory
                                                  V            
                D:\test\tt01\bin\ME\KEYMAP.S    | tt01\bin\ME\KEYMAP.S 
                      |    \-- test file -/          \-target dir /    ^
                      V         +----------------------------------+
              bag log
                d:\test\tt01\tt1.log
                \ ----------/  <= bag log directory
          */
        wxString wsBag = wfnBagLog.GetPath();
        wxFileName wfnTestFile( wsTestFile );
        wfnTestFile.RemoveDir( 0 );
        wfnTestFile.MakeAbsolute( wsBag );
        wsBag = wfnTestFile.GetFullPath();
        if ( m_bRestoring )
        {
          // need to fix up the paths - see comment at top
          bRet = Restore1File( wsOrigFile, wsBag );
        }
        else
          bRet = Compare2Files( wsOrigFile, wsBag );
        m_ulTotalFilesChecked++;
        wxASSERT( m_ulTotalFilesChecked <= m_ulTotalFilesToCheck );
        if ( bRet )
        {
          bFailed = false;
          m_ulCheckedFilesGood++;
          if ( m_bRestoring )
            wsT.Printf( _T("Restored: %s, %s"),  wsOrigFile, wsTestFile );
          else
            wsT.Printf( _T("Compared: %s, %s"),  wsOrigFile, wsTestFile );
        }
        else
        {
          bFailed = true;
          m_ulCheckedFilesFailed++;
          if ( m_bRestoring )
            wsT.Printf( _T("Restore failed: %s, %s"),  wsOrigFile, wsTestFile );
          else
            wsT.Printf( _T("Compare failed: %s, %s"),  wsOrigFile, wsTestFile );
        }
      }
      wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      if ( bFailed )
      {
        event.SetInt( MY_RED );                   // my color enums
        event.SetExtraLong( 3 | MY_FAILED_FILE ); // verbosity
      }
      else
      {
        event.SetInt( MY_GREEN );   // my color enums
        event.SetExtraLong( 3 );      // verbosity
      }
      
      event.SetString( wsT );
      wxQueueEvent( m_frame, event.Clone() );
    }
  }
  wtfBagLog.Close();
  m_frame->SetStatusText( wxEmptyString, 1 );
}

// ------------------------------- eof ------------------------------
