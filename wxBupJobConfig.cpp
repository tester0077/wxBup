/*-----------------------------------------------------------------
 * Name:        wxBupJobConfig.cpp
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
#include "wxBupFrameh.h" // needs to be first
#include <wx/config.h>
#include <wx/fileconf.h>
#include "wxBuph.h"
#include "wxBupConfh.h"
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
// externals
// ------------------------------------------------------------------
#undef _USE_REGISTRY__  // use .INI file instead
// ------------------------------------------------------------------
/* Note: the entries in enum iniEnts define the sequence 
 * of data entries here and MUST be in corresponding sequence
 * Note the number of initializers here must match the
 * number of entries specified in the config.h file

 */
jobPrefs_t g_jobData = 
{ 
  CURRENT_JOB_PREF_VERSION,
  {
    { _T("Job"),      // path - INI heading       [HEADING]
      _T("Name"),     // key                      key=false/0/1.23/some string
      eString,        // type of config entry
                      // default INI data
                      // values are only present up to last needed
                      // others are deemed to be set to 0/NULL/0.0
      true,           // bool
      0,              // long
      _T("job"),      // string
      0.0,            // float
                      // current value 
                      //    0, 0, 0, _T(""), 0.0
    },
    { _T("Source"),         _T("Path"),         eString, true, 0,  _T("")                 },
    { _T("Destination"),    _T("Root"),         eString, true, 0,  _T("")                 },
    { _T("Destination"),    _T("DriveLetter"),  eBool,   true,                            },
    { _T("Destination"),    _T("VolName"),      eBool,   false,                           },
    { _T("BagName"),        _T("Base"),         eString, true, 0,  _T("bag")              },
    { _T("Bag"),            _T("Size"),         eString, true, 0,  _T("650 MB CD")        },
    { _T("Exclusions"),     _T("Files"),        eString, true, 0,  _T("wxBupLog.txt")     },
    { _T("Exclusions"),     _T("Dirs"),         eString, true, 0,  _T("")                 },
    { _T("JustMove"),       _T("Files"),        eString, true, 0,  _T("")                 },
    { _T("CopyOnly"),       _T("Wanted"),       eBool,   false                            },
    { _T("Interfering"),    _T("Tasks"),        eString, true, 0,  _T("")                 },
    { _T("EmptyDirs"),      _T("Include"),      eBool,   true,                            },
    { _T("Job"),            _T("enabled"),      eBool,   true                             },
    { _T("SubDirs"),        _T("Include"),      eBool,   true                             },
    { _T("Backup"),         _T("Type"),         eString, true, 0,  _T("Full")             },
    { _T("WhenDone"),       _T("Verify"),       eBool,   true                             },
    { _T("Job"),            _T("Comment"),      eString, true, 0,  _T("")                 }, 
#if defined( WANT_LAST_JOB_DATE )
    { _T("Last"),           _T("Backup"),       eString, true, 0,  _T("unknown")          }, 
#endif
    { _T("EMail"),           _T("Wanted"),       eBool,   true                            },
    { _T("EMail"),           _T("ErrorOnly"),    eBool,   true                            },
    { _T("EMail"),           _T("AddLog"),       eBool,   true                            },
    { _T("EMail"),           _T("SmtpUrl"),      eString, 0,    0,  _T("smtp.telus.net")  },
    { _T("EMail"),           _T("From"),         eString, 0,    0,  _T("you@yourSmtpServer.com")   },
    { _T("EMail"),           _T("To"),           eString, 0,    0,  _T("jecxz114@telus.net") },
    { _T("EMail"),           _T("Subject"),      eString, 0,    0,  _T("wxBup-%d.%d.%d %s Report") },
    //  true => default to serial number, false => use volume name
    { _T("DestDrive"),       _T("DefaultSerNo"), eBool,   true                            },
    //  true => use serial number, false => use volume name
    { _T("DestDrive"),       _T("UseDrvSerNo"),  eBool,   true                            },
    // volume name, if any, not drive letter
    { _T("DestDrive"),       _T("VolName"),      eString, 0,    0,  _T("unknown")         },
    { _T("DestDrive"),       _T("SerialNo"),     eString, 0,    0,  _T("unknown")         },
  }
};

//-----------------------------------------------------------------------------
/**
 * Given a job file path name, fill the global job structure
 * with the data from the file
 */
bool MyApp::RestoreJobConfig ( wxString wsJobName )
{
  int x, y;
  x = sizeof( g_jobData.data );
  y = sizeof( entry );
  // bombs here, no idea why
  wxASSERT( sizeof(g_jobData.data)/sizeof( entry ) == IEJ_JOB_MAX );

  // restore the control's values from the config

  // NB: in this program, the config object is already created
  // at this moment because we had called Get() from MyApp::OnInit().
  // However, if you later change the code and don't create it
  // before this line, it won't break anything - unlike if you
  // manually create wxConfig object with Create() or in any
  // other way (then you must be sure to create it before using it!).
  wxString wsFileName;

  wsFileName = wsJobName;

  wxFileConfig *pConfig = new wxFileConfig(
      m_wsAppName,
      wxEmptyString,                // vendor name
      wsFileName,                   // local filename
      _T(""),                       // global filename
      wxCONFIG_USE_LOCAL_FILE /*| wxCONFIG_USE_RELATIVE_PATH*/ );
//  wxConfigBase::Set( pConfig );

  // see if we have an entry PrefVersion & whether it is recent enough
  // presumably the code will be backwards commpatible, i.e.
  // it can read old files and convert them to the new version
  if( pConfig->Exists( _T("/PrefVersion") )
    && (pConfig->Read( _T("/PrefVersion"), &g_jobData.lPrefVer )
    && ( g_jobData.lPrefVer <= CURRENT_PREF_VERSION ) ) )
  {
    wxString wsT;
    wsT.Printf( _T("Found version %ld, looking for %ld" ), 
      g_jobData.lPrefVer, CURRENT_JOB_PREF_VERSION );
    wxASSERT_MSG( true, wsT );
  }
  if( pConfig->Read( _T("/PrefVersion"), &g_jobData.lPrefVer)
      && g_jobData.lPrefVer < CURRENT_PREF_VERSION )
  {
    int answer = wxMessageBox(
      _T("The configuration file version is incompatible\n")
      _T("with the current application.\n")
      _T("OK to overwrite?"), _T("Confirm"), wxYES_NO );
    if ( answer == wxYES )
    {
#if 1
      wxASSERT_MSG( false, _T("Something is wrong!! ;-)"));
#else
      // this does not make any sense since we would recreate 
      // a app config file, nit a job config
      pConfig->DeleteAll();
      CreateConfig();
#endif
    }
    // simply carry on if user won't let us update the config file
    // it WILL complain the next time as well, until ...
    //else
    //  return;
  }
  pConfig->SetPath(_T("/"));

  wxString wsKey;
  bool  bOK = false;
  wxString wsS = _T("");
  for( int i = 0; i < IEJ_JOB_MAX; i++ )
  {
    wxASSERT( !g_jobData.data[i].wsPathStr.IsEmpty()
      && !g_jobData.data[i].wsKeyStr.IsEmpty() );
    wsKey.Printf( _T("/%s/%s"), g_jobData.data[i].wsPathStr,
      g_jobData.data[i].wsKeyStr );
    wxString wsDef((const wxChar*)g_jobData.data[i].dataDefault.wsVal );
    switch( g_jobData.data[i].eType )
    {
      case eBool:
        bOK = pConfig->Read( wsKey, &(g_jobData.data[i].dataCurrent.bVal),
          g_jobData.data[i].dataDefault.bVal );
        break;
      case eLong:
        bOK = pConfig->Read( wsKey, &(g_jobData.data[i].dataCurrent.lVal),
          g_jobData.data[i].dataDefault.lVal );
        break;
      case eString:
        bOK = pConfig->Read( wsKey, &g_jobData.data[i].dataCurrent.wsVal,
          wsDef );
        break;
      case eFloat:
        double f;
        bOK = pConfig->Read( wsKey, &f, g_jobData.data[i].dataDefault.fVal );
        if( bOK )
          g_jobData.data[i].dataCurrent.fVal = f;
        break;
    }
  }
  // avoid memory leaks
  delete pConfig;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * We can save the data and close the file because the data will remain
 * in the data structure.
 */
bool MyApp::SaveCloseJobConfig( wxString a_wsJobName )
{
  wxFileConfig *pConfig = new wxFileConfig(
      m_wsAppName,
      wxEmptyString,                // vendor name
      a_wsJobName,                  // local filename
      _T(""),                       // global filename
      wxCONFIG_USE_LOCAL_FILE /*| wxCONFIG_USE_RELATIVE_PATH*/ );
   wxConfigBase::Set( pConfig );

  if ( pConfig == NULL )
    return false;
  pConfig->SetPath(_T("/"));
  wxString wsKey;
  for( int i = 0; i < IEJ_JOB_MAX; i++ )
  {
    wsKey.Printf( _T("%s/%s"), g_jobData.data[i].wsPathStr,
         g_jobData.data[i].wsKeyStr );
    switch( g_jobData.data[i].eType )
    {
      case eBool:
         pConfig->Write( wsKey,
           (bool)(g_jobData.data[i].dataCurrent.bVal) );
         break;
      case eLong:
         pConfig->Write( wsKey,
           (int)g_jobData.data[i].dataCurrent.lVal );
         break;
      case eString:
         pConfig->Write( wsKey, g_jobData.data[i].dataCurrent.wsVal );
         break;
      case eFloat:
         pConfig->Write( wsKey, (float)g_jobData.data[i].dataCurrent.fVal );
         break;
    }
  }
  pConfig->Flush();// needed to ensure data is written
  // avoid memory leaks
  delete pConfig;
  return true;
}

// ------------------------------------------------------------------

/**
  * Make a copy of the Config data structure. The copy is intended to 
  * be passed to a dialog which might change data. Using the copy
  * will allow the user to save the data on exit or
  * abandon the changes if he choses 'Cancel'
  */
void MyApp::MakeJobConfigCopy( jobPrefs_t& jobPrefsOld,
    jobPrefs_t& jobPrefsNew )
{
  jobPrefsNew.lPrefVer = jobPrefsOld.lPrefVer;
  for( int i = 0; i < IEJ_JOB_MAX; i++ )
  {
    jobPrefsNew.data[i].wsPathStr = jobPrefsOld.data[i].wsPathStr;
    jobPrefsNew.data[i].wsKeyStr = jobPrefsOld.data[i].wsKeyStr;
    jobPrefsNew.data[i].eType = jobPrefsOld.data[i].eType;
    switch( jobPrefsOld.data[i].eType )
    {
      case eBool:
        jobPrefsNew.data[i].dataCurrent.bVal =
          jobPrefsOld.data[i].dataCurrent.bVal;
        break;
      case eLong:
        jobPrefsNew.data[i].dataCurrent.lVal =
          jobPrefsOld.data[i].dataCurrent.lVal;
        break;
      case eString:
        jobPrefsNew.data[i].dataCurrent.wsVal =
          jobPrefsOld.data[i].dataCurrent.wsVal;
        break;
      case eFloat:
        jobPrefsNew.data[i].dataCurrent.fVal =
          jobPrefsOld.data[i].dataCurrent.fVal;
        break;
    }
  }
}

// ------------------------------------------------------------------
/**
  * Compare the old and new/edited data.
  * Any differences => change, so return when the first one is detected.
  */
bool MyApp::JobCompareOldAndNew( jobPrefs_t& jobPrefsOld,
    jobPrefs_t& jobPrefsNew )
{
  wxString wsT;
  jobPrefsNew.lPrefVer = jobPrefsOld.lPrefVer;
  for( int i = 0; i < IEJ_JOB_MAX; i++ )
  {
    jobPrefsNew.data[i].wsPathStr = jobPrefsOld.data[i].wsPathStr;
    jobPrefsNew.data[i].wsKeyStr = jobPrefsOld.data[i].wsKeyStr;
    jobPrefsNew.data[i].eType = jobPrefsOld.data[i].eType;
    switch( jobPrefsOld.data[i].eType )
    {
      case eBool:
        if( jobPrefsNew.data[i].dataCurrent.bVal !=
          jobPrefsOld.data[i].dataCurrent.bVal )
          return false;
        break;
      case eLong:
        if ( jobPrefsNew.data[i].dataCurrent.lVal !=
          jobPrefsOld.data[i].dataCurrent.lVal )
          return false;
        break;
      case eString:
        wsT = jobPrefsNew.data[i].dataCurrent.wsVal;
        // only use case-insensitive compare for path related strings
        switch( i )
        {
        case IEJ_SOURCE_PATHS:
        case IEJ_DESTN_ROOT_PATH:
          if ( !wsT.MakeLower().IsSameAs(
            jobPrefsOld.data[i].dataCurrent.wsVal.MakeLower() ) )
            return false;
          break;
        default:
          if ( !wsT.IsSameAs( jobPrefsOld.data[i].dataCurrent.wsVal ) )
            return false;
          break;
        }
        break;
      case eFloat:
        if ( jobPrefsNew.data[i].dataCurrent.fVal !=
          jobPrefsOld.data[i].dataCurrent.fVal )
          return false;
        break;
    }
  }
  return true;
}
// ------------------------------- eof ------------------------------
