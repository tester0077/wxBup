/*-----------------------------------------------------------------
 * Name:        wxBupDriveInfo.cpp
 * Purpose:     get a vector of disk drive info: 
 *                drive letter, vol name, serial no, is DVD
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
#include "wxBupFrameh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )
// only good for MSVC - see note above re __VISUALC__
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
 * Populate a vector with the relevant disk drive information
 * for all currently installed hard & USB disk, including CD/DVD drives:
 * Drive letter, Volume name, if any, serial number, is CD/DVD
 */
void MyFrame::GetDriveInfo(std::vector<DriveInfoEntry> &ar_driveInfolist)
{
  wxFileName wfnDestn;
  wxString wsVolName = wxEmptyString;
  wxString wsSerNo = wxEmptyString;
  bool bIsDVD = false;  
  bool result = false;
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
      wsDrive = szSingleDrive[0];
      bIsDVD = false;
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
        break;
      case 4:
        wsT = _("Remote");
        break;
      case 5:
        wsT = _("CD/DVD");
        bIsDVD = true;
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
        DriveInfoEntry entry;
        wsT.Printf( _T("%lu"), serialNumber );
        wsVolName = volumeName;
        entry.m_wsDriveLetter = wsDrive;
        entry.m_bIsDVD        = bIsDVD;
        entry.m_wsSerialNo    = wsT;
        entry.m_wsVolumeName  = wsVolName;
        m_DriveInfo.push_back(entry);
        if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 3 )
        {
          wxLogMessage( _(" Drive: %s:, Name: <%s>, Serial: %s %s"), 
            wsDrive, wsVolName.IsEmpty() ? _("blank") : wsVolName, wsT, bIsDVD ? _("- CD/DVD") : _T("") );
        }
      }
      // get the next drive
      szSingleDrive += wcslen(szSingleDrive) + 1;
    }
  }
}

// ------------------------------- eof ------------------------------
