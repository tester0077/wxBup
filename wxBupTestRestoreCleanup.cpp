  /*-----------------------------------------------------------------
 * Name:        wxBupTestRestoreCleanup.cpp
 * Purpose:     cleanout destination directory for clean restart
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
#include "wxBupTestRestoreThreadh.h"
// ------------------------------------------------------------------
#if defined(_MSC_VER ) // from Autohotkey-hummer.ahk
// only good for MSVC
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
/**
 * This will remove all files and sub directories of the specified directory
 *
 * All my attempts using the wxWidget classes wxFileName & wxDir were
 * way too complicated.
 * NOTE: this may or may NOT work on older Win Oses - i.e before Win XP.
 *
 * wxDir works well, except when we delete directories - possibly
 * also if we were to create new ones
 */
#include <string.h>
bool MyRestoreThread::RemoveTestRestoreDirAndFiles( wxString wsDir )
{
  int iRes;
  wsDir += _T("*");
  wchar_t szDir[MAX_PATH+1];  // +1 for the double null terminate
  SHFILEOPSTRUCTW fos = {0};

  wcsncpy_s(szDir, MAX_PATH, wsDir.c_str(), wsDir.Len() );
  int len = lstrlenW(szDir);
  szDir[len+1] = 0; // double null terminate for SHFileOperation

  // delete the folder and everything inside
  fos.wFunc = FO_DELETE;
  fos.pFrom = szDir;
  fos.fFlags = FOF_NO_UI;
  iRes = SHFileOperation( &fos );
  /* from the MSDN explanation:
   * Return value
   *   Type: int
   * Returns zero if successful; otherwise nonzero. Applications normally 
   * should simply check for zero or nonzero.
   *
   * It is good practice to examine the value of the fAnyOperationsAborted 
   * member of the SHFILEOPSTRUCT. SHFileOperation can return 0 for success 
   * if the user cancels the operation. If you do not check fAnyOperationsAborted 
   * as well as the return value, you cannot know that the function accomplished 
   * the full task you asked of it and you might proceed under incorrect assumptions.
   *
   * Do not use GetLastError with the return values of this function.
   *
   * To examine the nonzero values for troubleshooting purposes, they largely 
   * map to those defined in Winerror.h. However, several of its possible return 
   * values are based on pre-Win32 error codes, which in some cases overlap 
   * the later Winerror.h values without matching their meaning. Those particular 
   * values are detailed here, and for these specific values only these meanings 
   * should be accepted over the Winerror.h codes. 
   */
  return( iRes == 0 );
}
// ------------------------------- eof ------------------------------
