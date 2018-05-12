
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

/* Code to obtain the application version information from the 
 * executable at run-time to display as part of 'About' dialog
 * or wherever needed.
 */
#include <wx/strconv.h>

#include "fileVerh.h"

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
// from: http://wxforum.shadonet.com/viewtopic.php?t=8193&highlight=fileversion
#ifdef __WXMSW__
// needed since the conversion to Unicode
#define cs2ws(s) (wxString(s,wxConvUTF8))

// ------------------------------------------------------------------

wxFileVersion::wxFileVersion()
{
  m_lpVersionData = NULL;
  m_dwLangCharset = 0;
  m_ulMajorVersion = m_ulMinorVersion =
    m_ulBuildNumber = m_ulSpecialBuild = 0;
}
// ------------------------------------------------------------------

wxFileVersion::~wxFileVersion()
{
  Close();
}
// ------------------------------------------------------------------

void wxFileVersion::Close()
{
  delete[] m_lpVersionData;
  m_lpVersionData = NULL;
  m_dwLangCharset = 0;
}
// ------------------------------------------------------------------

bool wxFileVersion::Open( wxString strModuleName )
{
  DWORD dwRet;
  if(strModuleName.IsEmpty())
  {
    wxChar szExeName[MAX_PATH];
    dwRet = ::GetModuleFileName(NULL, szExeName, sizeof (szExeName));
    strModuleName = szExeName;
    if( dwRet == 0 )
    {
      LPVOID lpMsgBuf;
      DWORD dw = GetLastError(); 
      FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
      wxString wsBfr = (LPTSTR)lpMsgBuf;
      wxString wsT1;
      wsT1.Printf( _T("%s, %d\n%s\nError Code: %d"), __FILE__, __LINE__, wsBfr, dw );
      wxMessageBox( wsT1, _T("Error") );
      LocalFree(lpMsgBuf);
      return false;
    }
  }

  wxASSERT(!strModuleName.IsEmpty());
  wxASSERT(m_lpVersionData == NULL);

  unsigned long dwHandle;
  unsigned long dwDataSize =
    ::GetFileVersionInfoSize(
#if  wxUSE_UNICODE	
    // true if at least 2.9.3
#if wxCHECK_VERSION( 2,9,3 )
     strModuleName.c_str(),
#else
		(WCHAR *)strModuleName.c_str(),
#endif   
#else
		(char *)strModuleName.c_str(),
#endif
		&dwHandle);
  if ( dwDataSize == 0 )
  {
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );
    wxString wsT1;
    wxString wsBfr = (LPTSTR)lpMsgBuf;
    wsT1.Printf( _T("%s, %d\n%s\nError Code: %d"), __FILE__, __LINE__, wsBfr, dw );
    wxMessageBox( wsT1, _T("Error") );
    LocalFree(lpMsgBuf);
    return false;
  }

  m_lpVersionData = new unsigned char[dwDataSize];
  if (!::GetFileVersionInfo(
#if  wxUSE_UNICODE	
#if wxCHECK_VERSION( 2,9,3 )
     strModuleName.c_str(),
#else
		(WCHAR *)strModuleName.c_str(),
#endif  
#else
		(char *)strModuleName.c_str(),
#endif
		dwHandle, dwDataSize, (void**)m_lpVersionData) )
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

  m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]),
    LOWORD(pTransTable[0]));
#if 1
  // extract the version numbers to make them available
  int n;
  int i;
  size_t nStart = 0;
  long lVal = 0l;
  wxString wsVersion = GetFixedFileVersion();
  for( i = 0, nStart = 0, n = 0;
    i < 4; i++, nStart = n+1 )
  {
    lVal = 0l;
    wxString wsSub;
    n = wsVersion.find( ',', nStart );
    wsSub = wsVersion.substr( nStart, n - nStart );
    if( !wsSub.ToLong( &lVal, 10 ) )
      continue;
    switch( i )
    {
    case 0:
      m_ulMajorVersion = lVal;
      break;
    case 1:
      m_ulMinorVersion = lVal;
      break;
    case 2:
      m_ulBuildNumber = lVal;
      break;
    case 3:
      m_ulSpecialBuild = lVal;
      break;
    default: ;
    }
  }
#endif
  return true;
}
// ------------------------------------------------------------------

wxString wxFileVersion::QueryValue(wxString strValueName,
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
  strBlockName = wxString::Format(_T("\\StringFileInfo\\%08lx\\%s"),
                             dwLangCharset, strValueName.c_str());

  if ( ::VerQueryValue((void **)m_lpVersionData,
#if defined( wxUSE_UNICODE )
    //strBlockName.mb_str(wxConvUTF8),
#if wxCHECK_VERSION( 2,9,3 )
     strBlockName.c_str(),
#else
		(WCHAR *)strBlockName.c_str(),
#endif  
#else
    strBlockName.c_str(),
#endif
                             &lpData, &nQuerySize) )
    strValue = wxString::FromAscii((char*)lpData);
  return strValue;
}
// ------------------------------------------------------------------

bool wxFileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
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
// ------------------------------------------------------------------

wxString wxFileVersion::GetFixedFileVersion()
{
  wxString strVersion;
  VS_FIXEDFILEINFO vsffi;

  if ( GetFixedInfo(vsffi) )
  {
    strVersion = wxString::Format (
      _T("%u,%u,%u,%u"),HIWORD(vsffi.dwFileVersionMS),
      LOWORD(vsffi.dwFileVersionMS),
      HIWORD(vsffi.dwFileVersionLS),
      LOWORD(vsffi.dwFileVersionLS));
  }
  return strVersion;
}
// ------------------------------------------------------------------

wxString wxFileVersion::GetFixedProductVersion()
{
  wxString strVersion;
  VS_FIXEDFILEINFO vsffi;

  if ( GetFixedInfo(vsffi) )
  {
    strVersion = wxString::Format (
      _T("%u,%u,%u,%u"), HIWORD(vsffi.dwProductVersionMS),
      LOWORD(vsffi.dwProductVersionMS),
      HIWORD(vsffi.dwProductVersionLS),
      LOWORD(vsffi.dwProductVersionLS));
  }
  return strVersion;
}
#endif //__WXMSW__

// ------------------------------- eof ------------------------------
