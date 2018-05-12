/*-----------------------------------------------------------------
 * Name:        wxBupProcessList.cpp
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
#include "wxBupWizTreebookh.h"
#include <TlHelp32.h>
//??#include <VersionHelpers.h> // only available under MSVC 2015
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
// from: https://forums.wxwidgets.org/viewtopic.php?t=20145&highlight=getprocesslist
// ------------------------------------------------------------------
// Implementation
void MyFrame::GetProcessList(std::vector<ProcessEntry> &proclist)
{
#ifdef __WXMSW__
#if 0
  if (!IsWindowsXPOrGreater())
  {
    wxMessageBox( _("You need at least Windows XP") ); //, "Version Not Supported", MB_OK);
    return;
  }
#else
  OSVERSIONINFO osver;
  // this function has been deprecated
  // Check to see if were running under Windows95 or
  // Windows NT.
  osver.dwOSVersionInfoSize = sizeof( osver ) ;
  if ( !GetVersionEx( &osver ) ) 
  {
    return;
  }
  if ( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ) 
  {
    return;
  }
#endif
  //get child processes of this node
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (!hProcessSnap) 
  {
    return;
  }
  //Fill in the size of the structure before using it.
  PROCESSENTRY32 pe;
  memset(&pe, 0, sizeof( pe ) );
  pe.dwSize = sizeof( PROCESSENTRY32 );

  // Walk the snapshot of the processes, and for each process,
  // kill it if its parent is pid.
  if (!Process32First( hProcessSnap, &pe) ) 
  {
    // Can't get first process.
    CloseHandle (hProcessSnap);
    return;
  }
  do 
  {
    ProcessEntry entry;
    entry.name = pe.szExeFile;
    entry.pid = (long)pe.th32ProcessID;
    proclist.push_back(entry);
  } while ( Process32Next( hProcessSnap, &pe) );
  CloseHandle( hProcessSnap );
#else
  //GTK and other
  wxArrayString output;
#if defined (__WXGTK__)
  ExecuteCommand(wxT("ps -A -o pid,command  --no-heading"), output);
#elif defined (__WXMAC__)
  // Mac does not like the --no-heading...
  ExecuteCommand(wxT("ps -A -o pid,command "), output);
#endif
  for (size_t i=0; i< output.GetCount(); i++) 
  {
    wxString line = output.Item(i);
    //remove whitespaces
    line = line.Trim().Trim(false);

    //get the process ID
    ProcessEntry entry;
    wxString spid = line.BeforeFirst(wxT(' '));
    spid.ToLong( &entry.pid );
    entry.name = line.AfterFirst(wxT(' '));

    if (entry.pid == 0 && i > 0) 
    {
      //probably this line belongs to the provious one
      ProcessEntry e = proclist.back();
      proclist.pop_back();
      e.name << entry.name;
      proclist.push_back( e );
    } 
    else 
    {
      proclist.push_back( entry );
    }
  }
#endif
}

// ------------------------------------------------------------------

#if 0
// Implementation
void MyFrame::ExecuteCommand(const wxString &command, wxArrayString &output, long flags)
{
#ifdef __WXMSW__
   wxExecute(command, output, flags);
#else
   FILE *fp;
   char line[512];
   memset(line, 0, sizeof(line));
   fp = popen(command.mb_str(wxConvUTF8), "r");
   while ( fgets( line, sizeof line, fp)) {
              output.Add(wxString(line, wxConvUTF8));
      memset(line, 0, sizeof(line));
   }
   pclose(fp);
#endif
}
#endif
// ------------------------------------------------------------------
void MyFrame::DisplayProcessList()
{
#if 1
  wxString wsName;
  std::vector<ProcessEntry> proclist;
  GetProcessList( proclist );
  int iSize = proclist.size();
  ProcessEntry pe;
  bool bFound = false;
  int iNClients = 0;
  wxString wsTT;
  for ( unsigned int i = 0; i < proclist.size(); i++ )
  {
    pe = proclist.at( i );
    if( pe.name.IsSameAs( _T("thunderbird.exe") ) )
    {
      bFound = true;
      iNClients++;
      wxLogWarning( _("ThunderBird is running\n") ); 
    }
    if( pe.name.IsSameAs( _T("MailClient.exe") ) )
    {
      bFound = true;
      iNClients++;
      wxLogWarning( _("eM-Client is running\n") );
    }
  }
  wxLogMessage( _("%d e-mail client(s) running"), iNClients );
#endif
}

// ------------------------------- eof ------------------------------
