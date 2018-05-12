/*-----------------------------------------------------------------
 * Name:        wxBupFrameUtils.cpp
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *---------------------------------------------------------------- */

/*----------------------------------------------------------------
 * Standard wxWidgets headers
 *---------------------------------------------------------------- */


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
// Note __VISUALC__ is defined by wxWidgets, not by MSVC IDE
// and thus won't be defined until some wxWidgets headers are included
#if defined( _MSC_VER ) 
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
 * Set the main frame title. 
 * Originally, the format string "%s - %s - Version %d.%d - Build: %d"
 * was hard coded into the wxCrafter/wxFB - and still is part of wxSkCr/wxSkFb
 * GUI patterns, but to allow display of the current Location wslf file 
 * name as part of the title, I needed to rework this code
 */
void MyFrame::SetMainFrameTitle( wxString wsLocation )
{
#if defined( WANT_VERSION_IN_TITLE )  
  wxString wsCompiler = _T("unknown");
  if (_MSC_VER == 1600)
    wsCompiler = _T("2010");
  else if (_MSC_VER == 1700)
    wsCompiler = _T("2012");
  else if (_MSC_VER == 1800)
    wsCompiler = _T("2013");
  else if (_MSC_VER == 1900)
    wsCompiler = _T("2015");
  wxString wsName = _("Backup & Zip - ") + wsCompiler;
  // Set up the main frame title with version & build info
  // Note: the base frame has the main frame title set to:
  // "%s - %s - Version %d.%d - Build: %d"
  wxString wsFrameTitle = GetLabel();
  if ( !wsFrameTitle.IsEmpty() )
  {
    wxString wsT;
    wsT.Printf( _T("%s - %s - Version %d.%d - Build: %d"), 
      wxGetApp().m_wsAppName, wsName,
      giMajorVersion, giMinorVersion, giBuildNumber );
    if ( !wsLocation.IsEmpty() )
      wsT += _T("  -  ") + wsLocation;
    SetLabel( wsT );
  }
#endif
}

// ------------------------------------------------------------------
/**
 * Adapted from:
 http://stackoverflow.com/questions/3898840/converting-a-number-of-bytes-into-a-file-size-in-c
 */

#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char     *sizes[]   = { "EB", "PB", "TB", "GB", "MB", "KB", "B" };
static const unsigned long long  exbibytes = 1024ULL * 1024ULL * 1024ULL *
                                             1024ULL * 1024ULL * 1024ULL;

wxString MyFrame::CalculateSize(unsigned long long ullSize )
{   
  wxString wsRes;
  unsigned long long  multiplier = exbibytes;
  int i;

  for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
  {   
    if (ullSize < multiplier)
      continue;
    if (ullSize % multiplier == 0)
    {
      wsRes.Printf( _T("%llu %s"), ullSize / multiplier, sizes[i]);
    }
    else
    {
      wsRes.Printf( _T("%.2Lf %s"), (long double)( ullSize / multiplier), sizes[i]);
    }
    return wsRes;
  }
  return wxEmptyString;
}

// ------------------------------------------------------------------
void MyFrame::OnWhenDoneClicked(wxCommandEvent& event)
{
  g_iniPrefs.data[IE_WHEN_DONE_ENABLED].dataCurrent.bVal = event.IsChecked();
}

// ------------------------------- eof ------------------------------
