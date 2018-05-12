/*-----------------------------------------------------------------
 * Name:        wxBupCurl.cpp
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
#include "wxBupPreProcDefsh.h" // needs to be first
#if defined( WANT_CURL )
#include "wxBupCurlh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )
// this block needs to go AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif
// ------------------------------------------------------------------
MyCurlObject::MyCurlObject ( wxString wsUrl, wxString wsUserAgent ) 
{
  wxASSERT( !wsUrl.IsEmpty() );
  wxASSERT( !wsUserAgent.IsEmpty() );
  char buf[100];
  strcpy( buf, (const char*)wsUrl.mb_str(wxConvUTF8) );
  m_curlCode = CURLE_FAILED_INIT;
  m_longHttpCode = 0l;
  m_pCurl = curl_easy_init();
  if( m_pCurl )
  {  
    m_curlCode = curl_easy_setopt( m_pCurl, CURLOPT_URL, buf );
    m_curlCode = curl_easy_setopt( m_pCurl, CURLOPT_WRITEFUNCTION, &MyCurlObject::curlWriter);
    strcpy( buf, (const char*)wsUserAgent.mb_str(wxConvUTF8) );
    m_curlCode = curl_easy_setopt( m_pCurl, CURLOPT_USERAGENT, buf );
    m_curlCode = curl_easy_setopt( m_pCurl, CURLOPT_WRITEDATA, &m_curlBuffer);
  
    m_curlCode = curl_easy_perform( m_pCurl);
  }
};
// ------------------------------------------------------------------
/**
* Get the HTTP status code.
*/
void MyCurlObject::GetHttpCode( long & lCode )
{ 
  curl_easy_getinfo ( m_pCurl, CURLINFO_RESPONSE_CODE, &lCode );
}
// ------------------------------------------------------------------
/**
* Writer call back for libcurl.
*/ 
int MyCurlObject::curlWriter(char *data, size_t size, size_t nmemb, void *buffer)
{
  int result = 0;
  if (buffer != NULL)
  {
    ((std::string *)buffer)->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}
#endif
// ------------------------------- eof ------------------------------
