/*-----------------------------------------------------------------
 * Name:        wxBupCompareZippedFile.cpp
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
#include <wx/file.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>

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
bool MyRestoreThread::CompareZippedFile( wxString a_wsOrigFile, wxString a_wsTestFile )
{
  bool bRet;
  wxString wsT;
  wxFileName wfnOrigFile( a_wsOrigFile );
  wxFileName wfnTestFile( a_wsTestFile );
  wxString wsOrigName = wfnOrigFile.GetName();
  wxString wsTestName = wfnTestFile.GetName();
  // is it a 'moved only file, no unzipping needed
  if ( wsOrigName.IsSameAs( wsTestName ) )
  {
    bRet = Compare2Files( a_wsOrigFile, a_wsTestFile );
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    wsT.Printf( _("Compared: %s"), a_wsTestFile );
    event.SetInt( MY_BLACK);    // my color enums
    event.SetExtraLong( 4 );    // verbosity
    event.SetString( wsT );
    wxQueueEvent( m_frame, event.Clone() );
    return bRet;
  }
  // else we need to unzip the file first
  // create a temporary file to hold the unzipped test file
  wxFileName wfnTempName( wfnTestFile );
  wxString wsTempPath;
  wfnTempName.SetPath( 
    g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal );
  wfnTempName.SetFullName( wfnOrigFile.GetFullName() );
  wxString wsTempName = wfnTempName.GetFullPath();
  wxTempFile wtfUnzip( wfnTempName.GetFullPath() );  
  wsTempPath =  wfnTempName.GetPathWithSep();
  bRet = ExtractZipFiles( a_wsTestFile, wsTempPath );
  if ( ! bRet )
  {
    wsT.Printf( _("Zip extract failed for: %s, temp: %s "),
      a_wsTestFile, wsTempName  );
    // no file to remove
    wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
    // send a log message to the main frame log window
    event.SetInt( MY_RED );   // my color enums
    event.SetExtraLong( 5 );  // verbosity
    event.SetString( wsT );
    wxQueueEvent( m_frame, event.Clone() );
    return bRet;
  }
  bRet = Compare2Files( a_wsOrigFile, wsTempName );

  // either way delete the test file from the disk
  bool bDel = ::wxRemoveFile( wsTempName );
  wsT.Printf( _T("Removing temp file: %s"), wsTempName );
#if defined ( LOG_EVENT )
  wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
  // send a log message to the main frame log window
  event.SetInt( MY_BLACK ); // my color enums
  event.SetExtraLong( 4 );  // verbosity
  event.SetString( wsT );
  wxQueueEvent( m_frame, event.Clone() );
#else
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
    wxLogMessage( wsT );
#endif
  wxASSERT( bDel );
  return bRet;
}

// ------------------------------------------------------------------
/**
 * From: https://wiki.wxwidgets.org/WxZipInputStream
 */
#include <wx/filesys.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <memory>
 
bool MyRestoreThread::ExtractZipFiles(const wxString& a_wsZipFile, const wxString& a_wsTargetDir ) 
{
  wxString wsT;
  bool ret = true;
  std::auto_ptr<wxZipEntry> entry( new wxZipEntry() );
  do 
  {  
    wxFileInputStream in(a_wsZipFile);
    if (!in) 
    {
#if 0
      wxLogError( _T("Can not open file '") + aZipFile + _T("'.") );
#else
      wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      event.SetInt( MY_RED );   // my color enums
      event.SetExtraLong( 5 );  // verbosity
      wsT.Printf( _T("%s - %d: Can't open file '%s'."), __FILE__, __LINE__, a_wsZipFile );
      event.SetString( wsT );
      wxQueueEvent( m_frame, event.Clone() );
#endif
      ret = false;
      break;
    }
    wxZipInputStream zip(in);
 
    while (entry.reset(zip.GetNextEntry()), entry.get() != NULL) 
    {
      // access meta-data
      wxString name = entry->GetName();
      // need to strip any leading path components for my application
      wxFileName wfnZip( name );
      //name = aTargetDir + wxFileName::GetPathSeparator() + name;
      while( wfnZip.GetDirCount() )
      {
        wfnZip.RemoveDir( 0 );
      }
      wfnZip.SetPath( a_wsTargetDir + wxFileName::GetPathSeparator() );
      wxString wsOutputFile = wfnZip.GetFullPath();
      wxFileName wfnTempFile( wfnZip.GetFullPath() );
      // make sure no such directory exists in the temporary targert dir
      if ( !wfnTempFile.HasExt() )
      {
        if ( wfnTempFile.Exists() )
        {
          wfnTempFile.Rmdir( wsOutputFile, 
            wxPATH_RMDIR_RECURSIVE | wxPATH_RMDIR_FULL );
        }
      }
      // read 'zip' to access the entry's data
      if (entry->IsDir()) 
      {
        int perm = entry->GetMode();
        wxFileName::Mkdir( name, perm, wxPATH_MKDIR_FULL );
      } 
      else 
      {
        zip.OpenEntry(*entry.get());
        if (!zip.CanRead()) 
        {
          wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 );  // verbosity
          event.SetString( _T("Can't read zip entry '") + entry->GetName() + _T("'.") );
          wxQueueEvent( m_frame, event.Clone() );
          ret = false;
          break;
        }
        wxFileOutputStream file( wsOutputFile );
        if (!file) 
        {
          wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 );  // verbosity
          event.SetString( _T("Can't create file '") + wsOutputFile + _T("'.") );
          wxQueueEvent( m_frame, event.Clone() );
          ret = false;
          break;
        }
        zip.Read( file );
      }
    }
  } while(false);
  return ret;
}

// ------------------------------- eof ------------------------------
