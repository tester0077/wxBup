/*-----------------------------------------------------------------
 * Name:        wxBupCompare2Files.cpp
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
#include <wx/ffile.h>
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
 * Compare two file for binary equality.
 * Adapted from: http://stackoverflow.com/questions/6163611/compare-two-files
 * Note: this MAY fail if we're backing up TB and it is restarted
 * before the test is completed
 */
#define BFR_SIZE 16*1024
bool MyRestoreThread::Compare2Files( wxString a_wsOrigFile, wxString a_wsTestFile )
{
  wxFFile wffOrig( a_wsOrigFile, _T("rb") );
  // really need to check if the insterfering task is running 'again'
  // we can't be sure the user will wait
  wxASSERT_MSG( wffOrig.IsOpened(), a_wsOrigFile );
  wxFFile wffTest( a_wsTestFile, _T("rb") );
  wxASSERT_MSG( wffTest.IsOpened(), a_wsTestFile );

  wxChar* pbufOrig = new wxChar[BFR_SIZE];
  wxChar* pbufTest = new wxChar[BFR_SIZE];
  do 
  {
    size_t r1 = wffOrig.Read( pbufOrig, BFR_SIZE );
    size_t r2 = wffTest.Read( pbufTest, BFR_SIZE );

    if (r1 != r2 || memcmp( pbufOrig, pbufTest, r1)) 
    {
      delete[] pbufOrig;
      delete[] pbufTest;
      wffOrig.Close();
      wffTest.Close();
      return false;  // Files are not equal
    }
  } while ( !wffOrig.Eof() && ! wffTest.Eof());
  bool bRet = wffOrig.Eof() && wffTest.Eof();
  delete[] pbufOrig;
  delete[] pbufTest;
  wffOrig.Close();
  wffTest.Close();
  return bRet;
}

// ------------------------------- eof ------------------------------
