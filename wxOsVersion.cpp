/*-----------------------------------------------------------------
 * Name:        wxOsVersion.cpp
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
#include "wxOsVersionh.h"
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


#ifdef __WXMSW__

wxOsVersion::wxOsVersion()
{
  m_lpVersionData = NULL;
  m_dwLangCharset = 0;
}

wxOsVersion::~wxOsVersion()
{
  Close();
}

void wxOsVersion::Close()
{
  delete[] m_lpVersionData;
  m_lpVersionData = NULL;
  m_dwLangCharset = 0;
}

bool wxOsVersion::Open(wxString strModuleName)
{
  if(strModuleName.IsEmpty())
  {
    wchar_t szExeName[MAX_PATH];
    GetModuleFileName(NULL, szExeName, sizeof (szExeName));
    strModuleName = szExeName;
  }

  wxASSERT(!strModuleName.IsEmpty());
  wxASSERT(m_lpVersionData == NULL);

  unsigned long dwHandle;
  unsigned long dwDataSize = ::GetFileVersionInfoSize(strModuleName.wc_str(wxConvUTF8), &dwHandle);
  if ( dwDataSize == 0 )
      return false;


  m_lpVersionData = new unsigned char[dwDataSize];
  if (!::GetFileVersionInfo(strModuleName.wc_str(wxConvUTF8), dwHandle, dwDataSize,
                            (void**)m_lpVersionData) )
  {
      Close();
      return false;
  }

  unsigned int nQuerySize;
  unsigned long* pTransTable;
  if (!::VerQueryValue(m_lpVersionData, wxT("\\VarFileInfo\\Translation"),
                        (void **)&pTransTable, &nQuerySize) )
  {
      Close();
      return false;
  }

  m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

  return true;
}

wxString wxOsVersion::QueryValue(wxString strValueName,
                                 unsigned long dwLangCharset /* = 0*/)
{
 
  wxASSERT(m_lpVersionData != NULL);
  if ( m_lpVersionData == NULL )
      return wxEmptyString;

  if ( dwLangCharset == 0 )
      dwLangCharset = m_dwLangCharset;

  unsigned int nQuerySize;
  void* lpData;
  wxString strValue, strBlockName;
  strBlockName = wxString::Format(wxT("\\StringFileInfo\\%08lx\\%s"),
                      dwLangCharset, strValueName);

  if ( ::VerQueryValue((void **)m_lpVersionData, strBlockName.wc_str(wxConvUTF8),
                      &lpData, &nQuerySize) )
//    strValue = wxString::FromAscii((char*)lpData);
    strValue = (wxStringCharType *)((char*)lpData );

  return strValue;
}

bool wxOsVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
{
  wxASSERT(m_lpVersionData != NULL);
  if ( m_lpVersionData == NULL )
      return false;

  unsigned int nQuerySize;
  VS_FIXEDFILEINFO* pVsffi;
  if ( ::VerQueryValue((void **)m_lpVersionData, wxT("\\"),
                        (void**)&pVsffi, &nQuerySize) )
  {
      vsffi = *pVsffi;
      return true;
  }

  return false;
}

wxString wxOsVersion::GetFixedFileVersion()
{
  wxString strVersion;
  VS_FIXEDFILEINFO vsffi;

  if ( GetFixedInfo(vsffi) )
  {
    strVersion = wxString::Format ("%u,%u,%u,%u",HIWORD(vsffi.dwFileVersionMS),
          LOWORD(vsffi.dwFileVersionMS),
          HIWORD(vsffi.dwFileVersionLS),
          LOWORD(vsffi.dwFileVersionLS));
  }
  return strVersion;
}

wxString wxOsVersion::GetFixedProductVersion()
{
  wxString strVersion;
  VS_FIXEDFILEINFO vsffi;

  if ( GetFixedInfo(vsffi) )
  {
    strVersion = wxString::Format ("%u,%u,%u,%u", HIWORD(vsffi.dwProductVersionMS),
          LOWORD(vsffi.dwProductVersionMS),
          HIWORD(vsffi.dwProductVersionLS),
          LOWORD(vsffi.dwProductVersionLS));
  }
  return strVersion;
}
#endif //__WXMSW__
// ------------------------------- eof ------------------------------
