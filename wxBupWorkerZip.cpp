/*-----------------------------------------------------------------
 * Name:        wxBupWorkerZip.cpp
 * Purpose:     use the Win API to backup & zip all of the files to be backed up.
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
#if defined( _MSC_VER ) // only good for Windows
#include <windows.h>
#endif
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#define ZIP__
#if defined( ZIP__ )
#include "wx/wfstream.h"
#include "wx/zipstrm.h"
#include "wx/datstrm.h"
#endif

#include "wxBuph.h"
#include "wxBupFrameh.h"
#include "wxBupWorkerh.h"
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
using namespace std;
/** Zip the file.
 * At this time we ought to have the source path already, no traversing of
 * directories to find the files
 * The final path: destRoot + source path CAN NOT be longer than WIN MAX_PATH
 * wxString wsSrcFile  - source file with full path
 * wxString wsDestRoot - destination root for 'bags'
 */
bool MyWorkerThread::ZipNBackupFile( wxString wsSrcFile, wxString wsDestRoot,
  size_t szI )
{
  /** compose the base destination path = destn + source
   * src = c:\bin\ls.exe
   * dest root = d:\test
   * dest path = d:\test\c\bin\ls.exe
   * dest zip  = d:\test\c\bin\ls_exe.zip  <-----------------------------------+
   * because we can have multiple files differentiated only by their extension |
   * we need to modify the destination zip file to something like -------------+
   *
   * this can be done unconditionally, or only for cases where we have files
   * with the same name, but different extension.
   * The second option would mean that we have to scan the list for duplicate
   * file names before starting to zip any file. The fact that, as it appears,
   * the files are retrieved in alphabetical order by name, will help, but
   * whether we can depend on this sort order from OS to OS or platform, is an
   * open question at this time.
   * As a start. I will simply implement the first option and modify the file
   * names unconditionally, until I run into issues and get more experience
   * with potential alternatives.
   */
#if defined _DEBUG
  wxString wsTT;
#endif
  wxFileName wfnSrcFile;
  wxFileName wfnDest;
  wfnSrcFile.Assign( wsSrcFile );
#if defined _DEBUG
  if ( wfnSrcFile.GetFullName().IsSameAs( _T("GenealogyTags.txt") ) )
    int i = 0;
#endif
  if( !wfnSrcFile.Exists() )
  {
    wxString wsT;
    wsT.Printf(  _("Skipped: File: %s does not exist any longer!\n") +
      _("Perhaps it needs to be added to the exclusion list!"),  wsSrcFile );
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    event.SetInt( MY_RED );     // my color enums
    event.SetExtraLong( 0 | MY_SKIPPED_FILE );    // verbosity
    event.SetString(wsT  );
    wxQueueEvent( m_frame, event.Clone() );

  }
  if( !wfnSrcFile.IsOk() )
  {
    wxString wsT;
    wsT.Printf(  _T("Problem: File: %s, line: %d, string: %s"), __FILE__, __LINE__, wsSrcFile );
    wxMessageBox( wsT, _T("Error"), wxOK );
    return false;
  }
  wfnDest.Assign( wsDestRoot );
  if( !wfnDest.IsOk() )
  {
    wxString wsT;
    wsT.Printf(  _T("Problem: File: %s, line: %d, string: %s"), __FILE__, __LINE__, wsDestRoot );
    wxMessageBox( wsT, _T("Error"), wxOK );
    return false;
  }
  // checking component length compliance really doesn't make sense for source.
  // The files already exist on disk, so presumably their paths - and
  // all components are OK

  /* Need to check full file path length against CDBurnerXP's
   * UDF limit of 127 Unicode characters -> 256 byte.
   * Account for the new extension: '.zip'
   *                                  V
   */
  if( wfnSrcFile.GetFullName().Len() > (127-4) )
  {
    wxString wsT;
#if defined (_DEBUG )
    int iFull = wfnSrcFile.GetFullName().Len();
#endif
    wsT.Printf(
      _("'%s\n%s'\nFile name exceeds the UDF limit of 127 Unicode characters by %d\n"),
      wfnSrcFile.GetFullName().Left( 61 ),
      wfnSrcFile.GetFullName().Mid( 61 ),
      wfnSrcFile.GetFullName().Len() - (127 - 4) );
#if 1
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window

    event.SetInt( MY_RED );     // my color enums
    event.SetExtraLong( 3 | MY_UDF_LIMIT_FILE ); // verbosity
    event.SetString( wsT );
    wxQueueEvent( m_frame, event.Clone() );
#else
    int iAnswer = wxMessageBox( wsT + _("\n\nQuit?"), _T("Confirm"), wxYES_NO );
    if ( iAnswer == wxYES )
      return false;
    // else carry on
#endif
  }
  wxString wsDestPath;
  bool bRet = MakeBackupDestnPath( wsSrcFile, wsDestRoot, wsDestPath );
  wxASSERT( bRet );
  wxFileName wfnFullDestPath;
  wxFileName wfnDestFile( wsDestPath );
  wfnFullDestPath.SetPath( wfnDestFile.GetPath() );

  // make sure the destination path exists, not worried about splitting just yet

  if( !wfnFullDestPath.DirExists() )
  {
    // create the base dir
    if( !wfnFullDestPath.Mkdir( 0777, wxPATH_MKDIR_FULL ) )
    {
      wxMessageBox(  _("ZipNBackupFile: Can't create the directory:\n") + wfnFullDestPath.GetFullPath(),
        _("Error"), wxOK | wxICON_EXCLAMATION );
      return false;
    }
  }
#if 1
  wxString wsCount;
  float fPercent = ((float)(m_frame->m_szNFilesProcessed) * 100 ) /
    (float)(m_frame->m_szNTotalFiles);
  wsCount.Printf( _("Zipping:  %Id/%Id - %3.1f%%"), m_frame->m_szNFilesProcessed + 1,
    m_frame->m_szNTotalFiles, fPercent );
  m_frame->SetStatusText( wsCount, 0 );
#endif
#if defined( ZIP__ )
#if defined( WANT_WXFILE )
  wxFile   wfFileName;
#endif
  wxString wsT1;
  wsT1 = wfnFullDestPath.GetPath();

  // build up a zip file using the wxWidgets zip streams
  wxFileName wfnZipFile;
  wfnZipFile.SetPath( wfnFullDestPath.GetPath() );
#if defined (_DEBUG )
  wxString wsPath = wfnZipFile.GetFullPath();
#endif
  wxString wsSrcFileName;
  wxString wsSrcFileExt;
  // make up the combined synthetic zip file name
  wsSrcFileName = wfnSrcFile.GetName();
  wsSrcFileExt  =  wfnSrcFile.GetExt();
  wfnZipFile.SetName( wsSrcFileName + _T("_") + wsSrcFileExt );
  wfnZipFile.SetExt( _T("zip") );

  wsT1 = wfnZipFile.GetFullName();    // full name
  wsT1 = wfnZipFile.GetFullPath();    // full path
  // we need/ought to check to ensure a file with the same
  // name & path does not exist already;
  // this is a very likely occurrence when multiple source
  // directories with related data are part of the same job file
  // such as d:\borland\bc3 & d:\borland\bc4
  if ( wfnZipFile.FileExists() )
  {
    wxString wsT;
    wxString wsT2;
#if defined( _DEBUG )
    wsT2.Printf( _T("\nFile: %s, %d"), __FILE__, __LINE__ );
#endif
    wsT.Printf(  _("File %s already exists!\nOverwrite it?%s"),
      wfnZipFile.GetFullPath(), wsT2 );
    int i = wxMessageBox( wsT, _T("Notice"), wxYES_NO | wxICON_QUESTION );
    if ( i != wxYES )
    {
      wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      event.SetInt( MY_RED );     // my color enums
      event.SetExtraLong( 5 );    // verbosity
      event.SetString(wsT  );
      wxQueueEvent( m_frame, event.Clone() );
      return false;
    }
  }
  wxFFileOutputStream out( wsT1 );
  bRet = out.IsOk();
  wxASSERT( bRet );
  wxZipOutputStream zip( out );
  bRet = zip.IsOk();
  wxASSERT( bRet );

  wxFileInputStream in( wfnSrcFile.GetFullPath());
  bRet = in.IsOk();
#if 1
  if ( !bRet )
  {
    wxString wsT;
    wsT.Printf(  _("Skipped: Cannot open: %s !\n") +
      _("Perhaps it needs to be added to the exclusion list!"),  wsSrcFile );
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    event.SetInt( MY_RED );     // my color enums
    event.SetExtraLong( 0 | MY_SKIPPED_FILE );    // verbosity
    event.SetString( wsT  );
    wxQueueEvent( m_frame, event.Clone() );
    return false;
  }
#else
  wxASSERT( bRet );
#endif
  bRet = zip.PutNextEntry( wfnSrcFile.GetFullPath() );
  wxASSERT( bRet );
  unsigned long long ullSrcSize;
  bRet = GetFileSize( wfnSrcFile.GetFullPath(), ullSrcSize );
  wxASSERT( bRet );
  m_frame->m_ullTotalSrcBytes += ullSrcSize;

  zip.Write( in ) ;
  bRet = zip.Close();
  wxASSERT( bRet );
  bRet = out.Close();
  wxASSERT( bRet );
  out.Sync();   // make sure the data gets flushed to disk
#endif
  unsigned long long ullZipSize;
  bRet = GetFileSize( wfnZipFile.GetFullPath(), ullZipSize );
  wxASSERT( bRet );
  m_frame->m_ullTotalZipBytes += ullZipSize;
  wxString wsSizeStr;
#if 0
  wsSizeStr = wfnZipFile.GetHumanReadableSize();
#else
  wsSizeStr.Printf( _T("%lld"), ullZipSize );
#endif

  m_frame->InsertFileInZipList( wfnZipFile.GetFullPath(), wfnSrcFile.GetFullPath(),
    ullSrcSize, ullZipSize );
  m_frame->m_ullTotalZipsSize += ullZipSize;

  wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
  // send a log message to the main frame log window
  event.SetInt( MY_GREEN );   // my color enums
  event.SetExtraLong( 1 );    // verbosity
  event.SetString( _("Zipped: ") + wsSrcFile + _T(" -> ") +
    wfnZipFile.GetFullPath() );
  wxQueueEvent( m_frame, event.Clone() );
#if 0 // moved to start of function
  wxString wsCount;
  float fPercent = ((float)(m_frame->m_szNFilesProcessed) * 100 ) /
    (float)(m_frame->m_szNTotalFiles);
  wsCount.Printf( _("Zipping:  %Id/%Id - %3.1f%%"), m_frame->m_szNFilesProcessed,
    m_frame->m_szNTotalFiles, fPercent );
  m_frame->SetStatusText( wsCount, 0 );
#endif
#if defined( WANT_GRID )
  // now show the data in GUI
  // for large # of files > 60,000 we run out of memory
  InsertItemInReportView( szI, wsSrcFile, wfnZipFile.GetFullPath(), wsSizeStr );
#endif
  return true;
}

// ------------------------------------------------------------------
bool MyWorkerThread::GetFileSize( wxString wsFileFullPath, unsigned long long& ullFileSize )
{
#if defined( WANT_WXFILE )
  wxFile   wfFileName;
  bool bRet = wfFileName.Open( wsFileFullPath );
  wxASSERT( bRet );
  ullFileSize = wfFileName.Length();
  wfFileName.Close();
  return bRet;
#else
  wxFileName wfnFile( wsFileFullPath );
  ullFileSize = wfnFile.GetSize().ToULong();
#endif
}

// ------------------------------------------------------------------
  // src root dirs in    m_frame->m_wasSourceRootDirs
  // we have src root    x:\x1\x2\x3\x4\src\ ....
  //                                   \file1.ext ...
  //         dest root   y:\y1\y2\y3\
  // current file path   x:\x1\x2\x3\x4\src\src1\src3\src.ext
  // strip               \------------/
  // prepend                        /----\
  // prepend             /---------\
  // change vol     -----v          ====== add for now
  // we want dest path   y:\y1\y2\y3\wxbup\src\src1\src3\src.ext
  //                                      \file1.ext ...
  //                                \bag01\ ..
  //                                ...
  //                                \bag0n\ ..
  //
bool MyWorkerThread::MakeBackupDestnPath( wxString wsSourceFile, wxString a_wsDestRoot,
  wxString &ar_wsDestPath )
{
  size_t i;
  wxString wsTT;
  wxString wsTT2;
  wxArrayString wasSourceFileDirs;
  wxFileName wfnSrcRoot;
  wxFileName wfnSourceFile( wsSourceFile );
  wxFileName wfnDest;

  wfnDest.AssignDir( a_wsDestRoot );
  // append the 'separator' dir
  // but first check we have a valid string for the base directory
  if ( g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal.IsEmpty() )
  {
    wxMessageBox( _("Base directory string is empty\nCannot continue ..."),
      _("Error"), wxOK  );
    return false;
  }
  wfnDest.AppendDir( g_iniPrefs.data[IE_BASE_DIR].dataCurrent.wsVal );
#if defined _DEBUG
  wsTT = wfnDest.GetFullPath();
#endif
  // process token here
  wasSourceFileDirs = wfnSourceFile.GetDirs();
#if defined _DEBUG
  int i2 = wasSourceFileDirs.GetCount();
#endif

#if defined _DEBUG
  wsTT2 = wfnSourceFile.GetFullPath();
#endif
  wxArrayString wasLeftoverSrcDirs = wfnSourceFile.GetDirs();
  if ( wasLeftoverSrcDirs.GetCount() )
  {
    // strip all of the xn directories up to ...\src
    for ( i = 0; i < wasLeftoverSrcDirs.GetCount(); i++ )
    {
      wfnDest.AppendDir( wasLeftoverSrcDirs[i] );

#if defined _DEBUG
      wsTT = wfnDest.GetFullPath();
#endif
    }
#if defined _DEBUG
    wsTT = wfnDest.GetFullPath();
#endif
  }
  wfnDest.SetFullName( wfnSourceFile.GetFullName() );
  ar_wsDestPath = wfnDest.GetFullPath();
  return true;
}

// ------------------------------- eof ------------------------------
