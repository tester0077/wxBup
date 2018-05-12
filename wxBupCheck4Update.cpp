//--------------------------------------------------------------
// Name:        wxBupCheck4Update.cpp
// Purpose:
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//--------------------------------------------------------------

//--------------------------------------------------------------
// Standard wxWidgets headers
//--------------------------------------------------------------
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

// For all others, include the necessary headers (this file is
// usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wxBupBaseFrame.h"
#include "wxBupFrameh.h"
#include "wxBupCurlh.h"
#include "wxBupCheckUpdateDialogh.h"
// -------------------------------------------------------------
#if defined( _MSC_VER )
// this block needs to go AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif

// ------------------------------------------------------------------
/**
 * Check if we have the latest version.
 * Get the string from the download site and advise user
 * if a more recent version is available.
 * This rotine is called from the OnIdle() handler when the application
 * first starts up.
 * To allow the user to call for a check, we'll return the status
 * so we can advise the user if there is no newer version
 * NOTE: the version php file MUST NOT be UTF-8 encoded - else 
 * the server returns a BAD string
 * We need to check if the internet is available here, but
 * we only care about internet availability if we are running the
 * release version.
 * When we run the debug version, we only need the local server.
 * for HTTP status codes, see:
 * https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 */

bool MyFrame::Check4Update(bool bFromUser,
  wxString& ar_wsCurrentVersion, wxString& ar_wsLatestVersion)
{
  wxString wsT;
  wxString wsUrl;
  long     lHttpCode;

  wsUrl = CHECK_4_UPDATE_URL;

  if ( wxGetApp().IsInternetAvailable())
  {
#if defined( WANT_CURL )
    // need to setup the dialog early, so I can get at the URL
    MyBupUpdateDlg dlg(this);
    wxString wsUrl = dlg.GetHyperLinkBupUpdatePage()->GetURL();
    
    wxString wsVersion;
    MyCurlObject MyCurl(wsUrl, wxGetApp().m_wsAppName);
    wxString wsVersionHtml = MyCurl.getData();
    CURLcode code = MyCurl.GetReturnCode();
    MyCurl.GetHttpCode( lHttpCode );
    if (code != CURLE_OK)
    {
      wsT.Printf(
        _("Checking for Updates\nGot error %d from libcurl interface!\n%s"),
        code, curl_easy_strerror(code));
      wxMessageBox(wsT, "Error", wxOK);
      return false;
    }
    if ( !wsVersionHtml.IsEmpty() && ( lHttpCode == 200 ) )
    {
      // extract & clean up the string
      // first find the 'Latest Version" line
      int iStart = wsVersionHtml.Find(_T("Latest Version"));
      wsVersion = wsVersionHtml.Mid(iStart);
      wsVersion = wsVersion.AfterFirst(':');
      wsVersion = wsVersion.BeforeFirst('<');
      wsVersion = wsVersion.Trim(true);
      wsVersion = wsVersion.Trim(false);
      ar_wsLatestVersion = wsVersion;
      ar_wsCurrentVersion.Printf(_T("%d.%d.%d"),
        giMajorVersion, giMinorVersion, giBuildNumber);
      // now convert & test against current version
      wxString wsMajor = wsVersion.BeforeFirst('.');
      wxString wsMinor = wsVersion.AfterFirst('.');
      wxString wsBuild = wsMinor.AfterFirst('.');
      wsMinor = wsMinor.BeforeFirst('.');
      long lMajor;
      long lMinor;
      long lBuild;
      wsMajor.ToLong(&lMajor);
      wsMinor.ToLong(&lMinor);
      wsBuild.ToLong(&lBuild);

      wxString wsTVersion = dlg.GetStaticTextVersion()->GetLabel();
      wsT.Printf(wsTVersion, (int)lMajor, (int)lMinor, (int)lBuild);
      if ((lMajor > giMajorVersion) ||
        ((lMajor == giMajorVersion) && (lMinor > giMinorVersion)) ||
        ((lMajor == giMajorVersion) && (lMinor == giMinorVersion) && (lBuild > giBuildNumber)))
      {
        dlg.GetStaticTextVersion()->SetLabel(wsT);
        if (dlg.ShowModal() == wxID_OK)
          return true;
      }
    }
    else if ( lHttpCode == 404 ) 
    {
      // we got an empty string, let user know
#if defined( _DEBUG )
      wsT.Printf(
        _("The Update Check URL \"%s\" was not found"), wsUrl );
      wxMessageBox(wsT, "Error", wxOK);
#else
      // do nothing for release - don't advertise the URL
#endif
      return false;
    }
    else
    {
#if defined( _DEBUG )
      // we got an empty string, let user know
      wsT.Printf(
        _("Checking for Updates\nGot an empty response from libcurl interface!") );
      wxMessageBox(wsT, "Error", wxOK);
#else
      // do nothing for release - don't advertise the URL
#endif
      return false;
    }
#endif
  }
  else if ( bFromUser )
  {
    wsT.Printf(
      _("Cannot connect to the internet to check for updates!\n") +
      _("Perhaps the server is down. Please try again later.") );
    wxMessageBox(wsT, "Error", wxOK);
    // just advise user and pretend there is nothing to do
    // to avoid a second error dialog about the version update
    return true;  
  }
  return false;
}

// ------------------------------------------------------------------
/**
 * Allow user to manually check for a newer version.
 * Here we only need to handle the case where the user has
 * the latest available version.
 * The alternate case, i. e. a newer version is available,  is handled by the 
 * called function.
 */
void MyFrame::OnCheck4Update(wxCommandEvent& event)
{
  wxString wsCurrentVersion;
  wxString wsLatestVersion;
  if( ! MyFrame::Check4Update( true, wsCurrentVersion, wsLatestVersion ) )
  {
    wxString wsT;
    wsT.Printf( _("No newer version found!\n") +
      _("Your version is: %s.\n") +
      //_("Latest version:  %s\n") +
      _("\n\nPlease note:\n") +
      _("Automatic checking for updates at startup can be\n") +
      _("enabled or disabled in the 'Options' dialog"),
      wsCurrentVersion /*, wsLatestVersion */);
      wxMessageBox(wsT, "Notice", wxOK );;
  }
  event.Skip();
}

// ------------------------------- eof -------------------------
