/*-----------------------------------------------------------------
 * Name:        wxBupUtils.cpp
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
#include "wxBupFrameh.h"
#include <wx/tokenzr.h>
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
// ------------------------------------------------------------------
/**
 * We have to make sure we have no duplicate sources. The logic chokes
 * if we have duplicate sources because after one source file is moved
 * it will be no longer available for a second move.
 */
bool MyFrame::Check4DuplicateSources( wxString a_wsSources )
{
  wxArrayString wasSources;
  // first, split the concatenated source paths
  // Note: we DON'T handle the case where files which fit one of the
  // exclusion masks are specifically added.
  // they will be backed up in any case.
  wxStringTokenizer tokenizer( a_wsSources, ";");
  while ( tokenizer.HasMoreTokens() )
  {
    wxString wsToken = tokenizer.GetNextToken();
    // process token here
    wasSources.Add( wsToken );
  }
  // sort the entries; result => identical entries will be adjacent
  wasSources.Sort();
  size_t i;
  for (i = 0; i < wasSources.GetCount() - 1; i++)
  {
    if (wasSources[i].MakeLower().IsSameAs(wasSources[i+1].MakeLower()))
    {
      return false;
    }
  }
  return true;
}

// ------------------------------------------------------------------
/**
 * From: http://www.codeproject.com/Tips/67577/Check-for-an-active-Internet-connection.aspx
 * "I have seen many different ways to check if the computer has an active internet connection. 
 * The best one, IMO, is to check for the presence of the default route in the IP forwarding 
 * table that is maintained by windows. 
 * This can be checked by using GetIPForwardTable function found in the iphlpapi.dll library. 
 * The default route is present if one of the entries in the table has a forwarding destination 
 * of 0.0.0.0."
 * I don't know how well this works as I have not really tested this to any useful degree
 */
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi")

bool MyApp::IsInternetAvailable()
{
  bool bIsInternetAvailable = false;
  // Get the required buffer size
  DWORD dwBufferSize = 0;
  if (ERROR_INSUFFICIENT_BUFFER == GetIpForwardTable(NULL, &dwBufferSize, false))
  {
    BYTE *pByte = new BYTE[dwBufferSize];
    if (pByte)
    {
      PMIB_IPFORWARDTABLE pRoutingTable =
        reinterpret_cast<PMIB_IPFORWARDTABLE>(pByte);
      // Attempt to fill buffer with routing table information
      if (NO_ERROR == GetIpForwardTable(pRoutingTable, &dwBufferSize, false))
      {
        DWORD dwRowCount = pRoutingTable->dwNumEntries; // Get row count
        // Look for default route to gateway
        for (DWORD dwIndex = 0; dwIndex < dwRowCount; ++dwIndex)
        {
          if (pRoutingTable->table[dwIndex].dwForwardDest == 0)
          { // Default route designated by 0.0.0.0 in table
            bIsInternetAvailable = true; // Found it
            break; // Short circuit loop
          }
        }
      }
      delete[] pByte; // Clean up. Just say "No" to memory leaks
    }
  }
  return bIsInternetAvailable;
}

// ------------------------------------------------------------------
// Adapted from: 
// http://www.codeproject.com/Articles/3275/Format-File-Sizes-in-Human-Readable-Format

size_t MyFrame::FormatSize(double dNumber, wxString &wsOutput ) 
{
  int iDepth = 0;
  wxString wsSuffix = _T("error");

  double dResult = GetDepth(dNumber, iDepth);
  wsOutput.Printf( _T("%.01f"), dResult);
  switch (iDepth) 
  {
    case 0: 
      wsSuffix = _T(" B");
      break;
    case 1: 
      wsSuffix = _T(" KB");
      break;
    case 2: 
      wsSuffix = _T(" MB");
      break;
    case 3: 
      wsSuffix = _T(" GB");
      break;
    case 4:
      wsSuffix = _T(" TB");
      break;
    case 5:
      wsSuffix = _T(" PB");
      break;
    case 6:
      wsSuffix = _T(" EB");
      break;
  }
  wsOutput.Append( wsSuffix );
  return wsOutput.Length();
}

// ------------------------------------------------------------------

double MyFrame::GetDepth(double dNumber, int& iDepth) 
{
  if (dNumber < 1024.0) 
  {
    return dNumber;
  } 
  else 
  {
    return GetDepth(dNumber / 1024.0, ++iDepth);
  }
} 

// ------------------------------------------------------------------
void MyFrame::ExpandPath( wxString & ar_wsFilePath2Expand )
{
#if defined( WANT_DATE_DIR )
  wxString wsTest = ar_wsFilePath2Expand;
  // expand any macros found in the dest'n directory string
  // currently we only allow {date} => yyyymmdd
  /*possible additions - LeoBackup for more details if necessary
  {year}, {month}, {monthName}, ShortMonth, week, day, dayName, shortDay,
  hour, minute,
  */
  int iFirstOpenBrace = wsTest.First( '{' );
  int iFirstCloseBrace = wsTest.First( '}' );
  int iDateStr;
  wxString wsDate;
  // open < close
  if ( iFirstOpenBrace != wxNOT_FOUND )
  {
    if (iFirstCloseBrace == wxNOT_FOUND )
    {
      wxFAIL_MSG( _("Unmatched braces") );
    }
    wxASSERT( iFirstOpenBrace < iFirstCloseBrace );
    // find the expected string "{date}"
    iDateStr = wsTest.find( _T("{date}") );
    wxASSERT( iDateStr  != wxNOT_FOUND );
    if (iDateStr == wxNOT_FOUND) // something is wrong
    {
      ;
    }
    else
    { 
      wxDateTime today = wxDateTime::Today();
      wsDate.Printf( _T("%04d%02d%02d"), today.GetCurrentYear(), 
        today.GetCurrentMonth() + 1, today.GetDay() );
      wsTest.replace( iFirstOpenBrace, iFirstCloseBrace - iFirstOpenBrace + 1,
        wsDate );
    }
  }
  ar_wsFilePath2Expand = wsTest;
#endif
}

// ------------------------------- eof ------------------------------
