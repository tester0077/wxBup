/*-----------------------------------------------------------------
 * Name:        wxBupFileLog.cpp
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
#include "wxBupFileLogh.h"

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
MyFileLog::MyFileLog(const wxString& fileName)         
{  
  wxLogNull ln;
  if ( m_logFile.Open(fileName, "a+") )       
    m_logFile.Write(wxString::Format("(%s) === LOGGING STARTED ===\n",
      wxDateTime::Now().FormatISOCombined(' ')));
  /*
   * works - any message below warning is dropped
   * but, note, so is the feedback to the log window ;-)
   * The DEBUG & TRACE messages are ONLY show in the debug version
   * they are suppessed as it is in release versions.
   */
//  wxLog::SetLogLevel( wxLOG_Warning ); 
}

// ------------------------------------------------------------------

MyFileLog::~MyFileLog()
{
  CloseLog();
}

// ------------------------------------------------------------------

void MyFileLog::Flush()
{
  if ( m_logFile.IsOpened() )
    m_logFile.Flush();
}

// ------------------------------------------------------------------
void MyFileLog::OpenLog( const wxString& fileName )
{
  wxLogNull ln;
  if ( m_logFile.Open(fileName, "a+") )       
    m_logFile.Write(wxString::Format("(%s) === LOGGING STARTED ===\n",
      wxDateTime::Now().FormatISOCombined(' ')));
}

// ------------------------------------------------------------------

void MyFileLog::CloseLog()
{
  if ( IsLogFileOK() )
  {
    m_logFile.Write(wxString::Format("(%s) === LOGGING FINISHED ===\n\n",
      wxDateTime::Now().FormatISOCombined(' ')));
    m_logFile.Flush();
    m_logFile.Close();
  }
}

// ------------------------------------------------------------------
void MyFileLog::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
{
  if ( !IsLogFileOK() )
    return;
  wxString prefix;
  prefix.Printf("(%s) ", wxDateTime(info.timestamp).FormatISOCombined(' '));
  switch ( level )
  {
  case wxLOG_Error:
    prefix += "ERROR: ";
    break;
  case wxLOG_Warning:
    prefix += "Warning: ";
    break;
  case wxLOG_Message:
    prefix += "Message: ";
    break;
  case wxLOG_Status:
    prefix += "Status: ";
    break;
  case wxLOG_Info:
    prefix += "Info: ";
    break;
  case wxLOG_Debug:               
    prefix += "Debug: ";
    break;
  case wxLOG_Trace:
    prefix += "Trace: ";
    break;
  default:
    prefix += "Other: ";
  }       
  m_logFile.Write(wxString::Format("%s%s [%s in %s(%d)]\n", 
    prefix, msg, info.func, info.filename, info.line));       
}

// ------------------------------- eof ------------------------------
