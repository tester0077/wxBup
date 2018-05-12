/*-----------------------------------------------------------------
 * Name:        wxBupRestore1ZippedFile.cpp
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
/**
 * From: https://wiki.wxwidgets.org/WxZipInputStream
 */
#include <wx/filesys.h>
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <memory>
 
//                                                     source zip,   where to send the output
bool MyRestoreThread::Restore1ZippedFile(const wxString& a_wsZipFile, const wxString& a_wsTargetDir ) 
{
  wxString wsT;
  bool ret = true;

  std::auto_ptr<wxZipEntry> entry( new wxZipEntry() );
  do 
  {  
    wxFileInputStream in(a_wsZipFile);
    if (!in) 
    {
      wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      event.SetInt( MY_RED );   // my color enums
      event.SetExtraLong( 5 );  // verbosity
      wsT.Printf( _T("%s - %d: Can't open file '%s'."), __FILE__, __LINE__, a_wsZipFile );
      event.SetString( wsT);
      wxQueueEvent( m_frame, event.Clone() );
      ret = false;
      break;
    }
    wxZipInputStream zip(in);
 
    while (entry.reset(zip.GetNextEntry()), entry.get() != NULL) 
    {
      // access meta-data
      wxString name = entry->GetName();
      // need to strip any leading path componenets for my application
      wxFileName wfnZip( name );
      //name = aTargetDir + wxFileName::GetPathSeparator() + name;
      while( wfnZip.GetDirCount() )
      {
        wfnZip.RemoveDir( 0 );
      }
      wfnZip.SetPath( a_wsTargetDir + wxFileName::GetPathSeparator() );
      wxString wsOutputFile = wfnZip.GetFullPath();
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

// ------------------------------------------------------------------
/**
 * Inputs are the full file names with path for both the source
 * and desrtination direcories
 */

bool MyRestoreThread::Restore1File(wxString a_wsOrigFile, wxString a_wsDestFile)
{
  wxFileName wfnSrc( a_wsOrigFile );
  wxFileName wfnDestFix( a_wsDestFile );
  bool bRet = m_frame->MyFileCopy( wfnSrc.GetFullPath(), wfnDestFix.GetFullPath(), 
    true );
  if( bRet == false )
  {
    wxString wsT;
    wsT.Printf( _("Restore1File: MyCopyFile returned error for %s to %s\nStop?"), 
      wfnDestFix.GetFullPath(), wfnSrc.GetFullPath() );
    int iAnswer = wxMessageBox(  wsT, _("Error"), wxYES_NO | wxICON_ERROR );
    if( iAnswer == wxYES )
    {
      wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
      wxGetApp().m_frame->m_bCancelled;
      return false;
    }
  }
  return true;
}

// ------------------------------- eof ------------------------------
