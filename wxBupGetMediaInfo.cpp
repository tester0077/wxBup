/*-----------------------------------------------------------------
 * Name:        wxBupGetMediaInfo.cpp

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
#    include <stdlib.h>
#    include <crtdbg.h>
#  endif
#endif
#include "wxBupPreProcDefsh.h"   // MUST be first
// ------------------------------------------------------------------
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
//#include <wx/config.h>
//#include <wx/fileconf.h>
//#include "wxBuph.h"
//#include "wxBupConfh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )
// only good for MSVC
// this block needs to AFTER all headers
# include <stdlib.h>
# include <crtdbg.h>
# ifdef _DEBUG
#   ifndef DBG_NEW
#     define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#     define new DBG_NEW
#   endif
# endif
#endif
// ------------------------------------------------------------------
/**
 * Test the given path to see if it is a fixed or removable medium.
 * return false if there is a problem,
 *        true otherwise
 * a_wsPathName MUST be a path only
 * return values:
 *  bIsFixed = true if fixed, false other wise
 *  wsVolName = volumen name, if any of destination medium
 *  &wsSerNo  = drive seria number as string
 */
bool MyBupJobDataDlg::GetMediaInfo( wxString a_wsPathName,
  bool &ar_bIsFixed, bool &ar_bIsDVD,
  wxString &ar_wsVolName,
  wxString &ar_wsSerNo )
{
  wxFileName wfnDestn;
  ar_wsVolName = ar_wsSerNo = wxEmptyString;
  ar_bIsFixed = ar_bIsDVD = false;  // set default
  wfnDestn.SetPath( a_wsPathName );
  // directory name may contain 'macros' - such as {date}
  // - but first we need to check for any directory macros, such as {date}
  // throw it back to caller if we have any

  int iBrace= a_wsPathName.find( '{' );
  if( iBrace != wxNOT_FOUND )
  {
    wxLogMessage( a_wsPathName + _(" contains {macro}") );
    return false;
  }
  if ( !wfnDestn.IsDir() )
  {
    wxLogMessage( a_wsPathName + _(" MUST be a path name") );
    return false;
  }
  bool result = false;
  wxString wsVolName = wfnDestn.GetVolume();  // gets drive letter
#if defined( _MSC_VER )
  wsVolName = wfnDestn.GetVolumeString( wsVolName[0]).MakeUpper();
  TCHAR volumeName[MAX_PATH + 1] = { 0 };
  TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
  DWORD serialNumber = 0;
  DWORD maxComponentLen = 0;
  DWORD fileSystemFlags = 0;
  wxString wsT;
  UINT nDriveType;
  DWORD dwSize = MAX_PATH;
  wchar_t szLogicalDrives[MAX_PATH] = {0};
  DWORD dwResult = GetLogicalDriveStrings(dwSize,szLogicalDrives);
  wxString wsDrive;
  if (dwResult > 0 && dwResult <= MAX_PATH)
  {
    wchar_t* szSingleDrive = szLogicalDrives;

    while(*szSingleDrive)
    {
      //printf("Drive: %s\n", szSingleDrive);
      wsDrive = szSingleDrive;
      if ( wsVolName.IsSameAs( wsDrive ) )
      {
        nDriveType = GetDriveType( szSingleDrive );
        wsT.Printf( _T("%d"), nDriveType );
        switch( nDriveType )
        {
        case 1:
          wsT = _("No root directory");
          break;
        case 2:
          wsT = _("Removable");
          break;
        case 3:
          wsT = _("Fixed");
          ar_bIsFixed = true;
          break;
        case 4:
          wsT = _("Remote");
          break;
        case 5:
          wsT = _("CD/DVD");
          ar_bIsDVD = true;
          break;
        case 6:
          wsT = _("RAM disk");
          break;
        case 0:
        default:
          wsT = _("Unknown");
          break;
        }
        // ------------------------------------------------------------------
        if (GetVolumeInformation(
          szSingleDrive,
          volumeName,
          ARRAYSIZE(volumeName),
          &serialNumber,
          &maxComponentLen,
          &fileSystemFlags,
          fileSystemName,
          ARRAYSIZE(fileSystemName)))
        {
          ar_wsVolName = volumeName;
          wsT.Printf( _T("%lu"), serialNumber );
          ar_wsSerNo = wsT;
        }
        break;  // we just want one drive
      }
      // get the next drive
      szSingleDrive += wcslen(szSingleDrive) + 1;
    }
  }
#endif
  return result;
}

// ------------------------------- eof ------------------------------
