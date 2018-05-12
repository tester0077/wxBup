/*-----------------------------------------------------------------
 * Name:        wxBupCurlh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_CURL_H
#define _WX_BUP_CURL_H
// this #define seems to allow linking, though it also causes
// warning LNK4098: defaultlib 'MSVCRT' conflicts with use of other libs; use /NODEFAULTLIB:library
// but only for the debug version - it seems the library is compiled for 'release'
// as indicated by the path.
#define CURL_STATICLIB  
#if defined( WANT_CURL )
// avoids linker warnings & errors for curl_easy... functions
// though I haven't been able to figure out how
#include "wx/protocol/http.h"
#include "curl/curl.h"
// ------------------------------------------------------------------
class MyCurlObject
{
  public:
  MyCurlObject( wxString url, wxString wsUserAgent ); 
  ~MyCurlObject() { curl_easy_cleanup( m_pCurl ); };

  static int curlWriter(char *data, size_t size, size_t nmemb, void *buffer);

  std::string getData () { return m_curlBuffer; }
  CURLcode GetReturnCode() { return m_curlCode; };
  wxString GetErrorString() { return curl_easy_strerror( m_curlCode ); };
  void GetHttpCode( long & lCode );

protected:
  CURL *      m_pCurl;
  std::string m_curlBuffer;
  CURLcode    m_curlCode;
  wxString    m_wsErrorString;
  long        m_longHttpCode;
};
#endif
#endif  // _WXBUP_CURL_H
// ------------------------------- eof ---------------------------
