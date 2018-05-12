/////////////////////////////////////////////////////////////////////////////
// Name:        wxBupConfig.cpp
// Purpose:     config part of wxBup  backup & zip utility
// Author:      A. Wiegert based on Julian Smart's  code
// Modified by:
// Created:     Mar 15/07
// RCS-ID:      $Id: $
// Copyright:   (c) A. Wiegert
// Licence:     wxWidgets license
/////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------
// Note __VISUALC__ is defined by wxWidgets, not by MSVC IDE
// and thus won't be defined until some wxWidgets headers are included
#if defined( _MSC_VER ) 
#  if defined ( _DEBUG )
// this statement NEEDS to go BEFORE all headers
#    define _CRTDBG_MAP_ALLOC
#  endif
#endif
#include "wxBupPreProcDefsh.h"   // needs to be first

// ------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif
#include "wxBupFrameh.h" // needs to be first
#include <wx/config.h>
#include <wx/fileconf.h>
#include "wxBuph.h"
#include "wxBupConfh.h"

// ------------------------------------------------------------------

#ifdef __WIN32__
    // this is not supported by native control
    #define NO_VARIABLE_HEIGHT
#endif
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// only good for MSVC
// this block needs to go AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
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
iniPrefs_t g_iniPrefs = 
{ 
  CURRENT_PREF_VERSION,
  {
    { _T("LAST_DIR"), // path - INI heading       [HEADING]
      _T("Path"),     // key                      key=false/0/1.23/some string
      eString,        // type of config entry
                      // default INI data
                      // values are only present up to last needed
                      // others are deemed to be set to 0/NULL/0.0
      true,           // bool
      0,              // long
//      0,              // index #
      _T("c:/"),      // string
      0.0,            // float
                      // current value 
  //    0, 0, 0, 0, _T(""), 0.0
    },

    { _T("MainFrame"),      _T("x"),   eLong,   0,    10  },
    { _T("MainFrame"),      _T("y"),   eLong,   0,    20  },
    { _T("MainFrame"),      _T("w"),   eLong,   0,   300  },
    { _T("MainFrame"),      _T("h"),   eLong,   0,   500  },
    { _T("LogWindow"),      // path - INI heading       [HEADING]
        _T("Wanted"),       // key                      key=false/0/1.23/some string
        eBool,              // type of config entry
                            // default INI data
                            // values are only present up to last needed
                            // others are deemed to be set to 0/NULL/0.0
            true,           // bool
            0,              // long
            _T(""),         // string
            0.0,            // float
                            // current value
            true },
    { _T("Sash"),           _T("Position"),     eLong,   0,  250               },
    { _T("OutFile"),        _T("Path"),         eString, true, 0, _T("c:/")    },
#if defined( WANT_GRID )
    // results grid col widths, as string of numbers with | separator
    { _T("ResultsGrid"),    _T("Cols"),         eString, 0,    0,  _T("80|120|120|80|120|80")  },
    { _T("ResultsGrid"),    _T("Labels"),       eString, 0,    0,  _T("File name|Path|Size")   },
#endif
  
    { _T("LogFile"),        _T("Wanted"),       eBool,   false                 },
    { _T("Log"),            _T("Verbosity"),    eLong,   0,    4               },
    { _T("NewLog"),         _T("Frequency"),    eLong,   0,    0               },
    { _T("LogDir"),         _T("Path"),         eString, 0,    0,  _T("")      },
    { _T("LogDir"),         _T("UseDefault"),   eBool,   true                  },
    { _T("LastOptionDlg"),  _T("Tab"),          eLong,   0,    0               },
    { _T("LogWindow"),      _T("ClearAtStart"), eBool,   true                  },
    { _T("LogWindow"),      _T("ClearAtTest"),  eBool,   true                  },
    
    { _T("LastJob"),        _T("File"),         eString, 0,    0,  _T("")      },
    { _T("LastJob"),        _T("LoadAtStart"),  eBool,   true                  },
    { _T("JobDialog"),      _T("w"),            eLong,   0,  300               },
    { _T("JobDialog"),      _T("h"),            eLong,   0,  500               },
    { _T("JobDlgSash"),     _T("Position"),     eLong,   0,  250               },
    // reserve space to fiddle the 4.7 DVD size without recompiling
    // 10 MB wasn't enough
    { _T("4.7GB_DVD"),      _T("Reserve"),      eLong,   0,   15*1024*1024     },
    { _T("Floppy"),         _T("Reserve"),      eLong,   0,   15*1024          },

    { _T("RestoreSource"),   _T("Last"),         eString, 0,   0,  _T("")      },
    { _T("RestoreTarget"),   _T("Last"),         eString, 0,   0,  _T("")      },
    { _T("TargetDir"),       _T("ClearAtStart"), eBool,   true                 },
    { _T("TargetDir"),       _T("Compare"),      eBool,   true                 },
    { _T("SummaryUpdate"),   _T("Seconds"),      eLong,   0,  10               },
    { _T("BaseDir"),         _T("Name"),         eString, 0,   0,  _T("wxbupTemp") },
    { _T("JobSash"),         _T("Position"),     eLong,   0, 250               },
    { _T("GlobalTempDir"),   _T("Use"),          eBool,   true                 },
    { _T("GlobalTempDir"),   _T("Path"),         eString, 0,   0,  _T("")      },
    { _T("Toolbar"),         _T("Text"),         eBool,   false                },
    { _T("Exclude"),         _T("Files"),        eString, 0,   0,  
          _T("desk*.ini;*.lnk;*Thumbs.db;*hpoth*.*|")      },
    { _T("Exclude"),         _T("Dirs"),         eString, 0,   0,  _T("")      },
    { _T("MoveOnly"),        _T("List"),         eString, 0,   0,  
          _T("*.mov;*.jpg;*.avi;*.iso;*.pdf;*.zip|")                           },
    { _T("DefaultBag"),      _T("Size"),         eLong,   0,   5,              }, // 650 MB
    { _T("DestDelete"),      _T("Warn"),         eBool,   true                 },
    { _T("Check"),           _T("NewVersion"),   eBool,   true                 },
    { _T("JobEdit"),         _T("Showpath"),     eBool,   true                 },
    { _T("Sound"),           _T("Feedback"),     eBool,   true                 },
    { _T("WhenDone"),        _T("Action"),       eLong,   0,  0                },
    { _T("WhenDone"),        _T("Enabled"),      eBool,   false                },
    { _T("SoundFile"),       _T("Error"),        eString, 0,   0,  _T("railroad_crossing_bell-Brylon_Terry-7.wav") },
    { _T("SoundFile"),       _T("Warn"),         eString, 0,   0,  _T("Ring10.wav")      },
    { _T("SoundFile"),       _T("Done"),         eString, 0,   0,  _T("foghorn-daniel_simon-short.wav") },
    { _T("EmailDefault"),    _T("Use"),          eBool,   true                 },
    { _T("EmailDefault"),    _T("Server"),       eString, 0,   0,  _T("smtp://smtp.your.isp")      },
    { _T("EmailDefault"),    _T("From"),         eString, 0,   0,  _T("your.name@your.isp")        },
    { _T("EmailDefault"),    _T("To"),           eString, 0,   0,  _T("addressee@destination.isp") },
    { _T("EmailDefault"),    _T("Subject"),      eString, 0,   0,  _T("wxBup Report from PCxxxx")  },
  }
};

// ------------------------------------------------------------------
void MyApp::CreateConfig( void )
{
  int x, y;
  x = sizeof( g_iniPrefs.data );
  y = sizeof( entry );
  wxASSERT( sizeof(g_iniPrefs.data)/sizeof( entry) == IE_MAX );
  // By default, the config data goes to registry under Windows

  // we're using wxConfig's "create-on-demand" feature: it will create the
  // config object when it's used for the first time. It has a number of
  // advantages compared with explicitly creating our wxConfig:
  //  1) we don't pay for it if we don't use it
  //  2) there is no danger to create it twice

// ====== .INI file ends up in
  // Home directory
  // XP:     c:/Documents and Settings/userName/Application data/wxSr/
  // Vista:  c:/Users/UserName/AppData/Roaming/wxSr/
  // Win7    c:/Users/UserName/AppData/Roaming/wxSr/
  wxString wsFileName;

  wsFileName = m_wsConfigDir + _T("\\") + m_wsAppName + _T(".ini");

   m_pConfig = new wxFileConfig(
      m_wsAppName,
      wxEmptyString,                // vendor name
      wsFileName,                   // local filename
      _T(""),                       // global filename
      wxCONFIG_USE_LOCAL_FILE /*| wxCONFIG_USE_RELATIVE_PATH*/ );
   wxConfigBase::Set(m_pConfig);

   // see if we have an entry PrefVersion & whether it is recent enough
   if( m_pConfig->Exists( _T("/PrefVersion") )
     && (m_pConfig->Read( _T("/PrefVersion"), &g_iniPrefs.lPrefVer )
     && (g_iniPrefs.lPrefVer <= CURRENT_PREF_VERSION ) ) )
   {
     ;
   }
   // either found no .INI file or a newer version
   // set defaults and save
   else
   {
      g_iniPrefs.lPrefVer = CURRENT_PREF_VERSION;
      // read any existing entries, with a default, in
      // case they are missing
      //pConfig->Read( "key", &Settings.value, default_value);

      // might be an idea to delete the file here to clean
      // out any cruft
      m_pConfig->DeleteAll();
      m_pConfig->Write( _T("/PrefVersion"), g_iniPrefs.lPrefVer );
      wxString wsData;
      for( int i = 0; i < IE_MAX; i++ )
      {
        wxASSERT_MSG( !g_iniPrefs.data[i].wsPathStr.IsEmpty() &&
        !g_iniPrefs.data[i].wsKeyStr.IsEmpty(), _T("NULL entries in iniPrefs.data[]") );
      wsData.Printf( _T("%s/%s"), g_iniPrefs.data[i].wsPathStr,
        g_iniPrefs.data[i].wsKeyStr );
      switch( g_iniPrefs.data[i].eType )

        {
        case eBool:
          m_pConfig->Write( wsData,
            // Note: current == default - supposed to, but may not
            (bool)(g_iniPrefs.data[i].dataDefault.bVal) );
          break;
        case eLong:
          m_pConfig->Write( wsData,
             g_iniPrefs.data[i].dataDefault.lVal );
          break;
        case eString:
          m_pConfig->Write( wsData,
              g_iniPrefs.data[i].dataDefault.wsVal );
          g_iniPrefs.data[i].dataCurrent.wsVal =
              g_iniPrefs.data[i].dataDefault.wsVal;
          break;
        case eFloat:
          m_pConfig->Write( wsData,
               (float)g_iniPrefs.data[i].dataDefault.fVal );
          break;
        }
      }
      m_pConfig->Flush();
   }
}
//-----------------------------------------------------------------------------

void MyApp::RestoreConfig ( void )
{
  int x, y, w, h;
  x = sizeof(g_iniPrefs.data);
  y = sizeof( entry);
  w = x/y;
  wxASSERT( sizeof(g_iniPrefs.data)/sizeof( entry) == IE_MAX );

  // restore the control's values from the config

  // NB: in this program, the config object is already created
  // at this moment because we had called Get() from MyApp::OnInit().
  // However, if you later change the code and don't create it
  // before this line, it won't break anything - unlike if you
  // manually create wxConfig object with Create() or in any
  // other way (then you must be sure to create it before using it!).
  m_pConfig = (wxFileConfig *)wxConfigBase::Get();

  if( m_pConfig->Read( _T("/PrefVersion"), &g_iniPrefs.lPrefVer)
      && g_iniPrefs.lPrefVer < CURRENT_PREF_VERSION )
  {
    int answer = wxMessageBox(
      _T("The configuration file version is incompatible\n")
      _T("with the current application.\n")
      _T("OK to overwrite?"), _T("Confirm"), wxYES_NO );
    if ( answer == wxYES )
    {
      m_pConfig->DeleteAll();
      CreateConfig();
    }
    // simply carry on if user won't let us update the config file
    // it WILL complain the next time as well, until ...
    //else
    //  return;
  }
  // SetPath() understands ".."
  m_pConfig->SetPath(_T("/MainFrame"));

  // restore dialog position on screen
  x = m_pConfig->Read(_T("x"), 50);
  y = m_pConfig->Read(_T("y"), 50);
  w = m_pConfig->Read(_T("w"), 350);
  h = m_pConfig->Read(_T("h"), 200);

  wxGetApp().m_frame->SetSize( x, y, w, h );
  m_pConfig->SetPath(_T("/"));

  wxString wsKey;
  bool  bOK = false;
  wxString wsS = _T("");
  for( int i = 0; i < IE_MAX; i++ )
  {
    wxASSERT( !g_iniPrefs.data[i].wsPathStr.IsEmpty()
      && !g_iniPrefs.data[i].wsKeyStr.IsEmpty() );
    wsKey.Printf( _T("/%s/%s"), g_iniPrefs.data[i].wsPathStr,
      g_iniPrefs.data[i].wsKeyStr );
    wxString wsDef((const wxChar*)g_iniPrefs.data[i].dataDefault.wsVal );
    switch( g_iniPrefs.data[i].eType )
    {
      case eBool:
        bOK = m_pConfig->Read( wsKey, &(g_iniPrefs.data[i].dataCurrent.bVal),
          g_iniPrefs.data[i].dataDefault.bVal );
        break;
      case eLong:
        bOK = m_pConfig->Read( wsKey, &(g_iniPrefs.data[i].dataCurrent.lVal),
          g_iniPrefs.data[i].dataDefault.lVal );
        break;
      case eString:
        bOK = m_pConfig->Read( wsKey, &g_iniPrefs.data[i].dataCurrent.wsVal,
          wsDef );
        break;
      case eFloat:
        double f;
        bOK = m_pConfig->Read( wsKey, &f, g_iniPrefs.data[i].dataDefault.fVal );
        if( bOK )
          g_iniPrefs.data[i].dataCurrent.fVal = f;
        break;
    }
  }
#if defined ( WANT_FILEHISTORY )
  // clean out any MRU files which no longer exist
  m_pFileHistory->Load( *m_pConfig );
  wxString wsFile;
  size_t i;
  size_t n = wxGetApp().m_pFileHistory->GetCount();
  for ( i = 0; i < n; i++ )
  {
    wsFile = wxGetApp().m_pFileHistory->GetHistoryFile( i );
    if ( wxFile::Exists( wsFile ) )
    {
      wxGetApp().m_pFileHistory->AddFileToHistory( wsFile );
    }
    else
    {
      wxGetApp().m_pFileHistory->RemoveFileFromHistory( i );
      // seems we also need to decrement n if the file is missing
      n--;
    }
  }
#endif
}

//-----------------------------------------------------------------------------
void MyApp::SaveConfig( bool bDone )
{
  if ( m_pConfig == NULL )
    return;
  m_pConfig->SetPath(_T("/"));
  wxString wsKey;
  for( int i = 0; i < IE_MAX; i++ )
  {
    wsKey.Printf( _T("%s/%s"), g_iniPrefs.data[i].wsPathStr,
         g_iniPrefs.data[i].wsKeyStr );
    switch( g_iniPrefs.data[i].eType )
    {
      case eBool:
         m_pConfig->Write( wsKey,
           (bool)(g_iniPrefs.data[i].dataCurrent.bVal) );
         break;
      case eLong:
         m_pConfig->Write( wsKey,
           (int)g_iniPrefs.data[i].dataCurrent.lVal );
         break;
      case eString:
         m_pConfig->Write( wsKey, g_iniPrefs.data[i].dataCurrent.wsVal );
         break;
      case eFloat:
         m_pConfig->Write( wsKey, (float)g_iniPrefs.data[i].dataCurrent.fVal );
         break;
    }
  }
#if defined ( WANT_FILEHISTORY )
  // saves the files to the root of the .INI file
  // as file1=..., file2=...,  ... file9=...
  m_pFileHistory->Save( *m_pConfig );
  // now we can delete the file history - it has been saved
  delete m_pFileHistory;
#endif
  m_pConfig->Flush();// needed to ensure data is written
  // avoid memory leaks
  if ( bDone )
    delete m_pConfig;
}

// ------------------------------------------------------------------

/**
  * Make a copy of the Config data structure. The copy is intended to 
  * be passed to a dialog which might change data. Using the copy
  * will allow the user to save the data on exit or
  * abandon the changes if he choses 'Cancel'
  */
void MyApp::MakeConfigCopy( iniPrefs_t& iniPrefsSrc,
    iniPrefs_t& iniPrefsTarg )
{
  iniPrefsTarg.lPrefVer = iniPrefsSrc.lPrefVer;
  for( int i = 0; i < IE_MAX; i++ )
  {
    iniPrefsTarg.data[i].wsPathStr = iniPrefsSrc.data[i].wsPathStr;
    iniPrefsTarg.data[i].wsKeyStr = iniPrefsSrc.data[i].wsKeyStr;
    iniPrefsTarg.data[i].eType = iniPrefsSrc.data[i].eType;
    switch( iniPrefsSrc.data[i].eType )
    {
      case eBool:
        iniPrefsTarg.data[i].dataCurrent.bVal =
          iniPrefsSrc.data[i].dataCurrent.bVal;
        break;
      case eLong:
        iniPrefsTarg.data[i].dataCurrent.lVal =
          iniPrefsSrc.data[i].dataCurrent.lVal;
        break;
      case eString:
        iniPrefsTarg.data[i].dataCurrent.wsVal =
          iniPrefsSrc.data[i].dataCurrent.wsVal;
        break;
      case eFloat:
        iniPrefsTarg.data[i].dataCurrent.fVal =
          iniPrefsSrc.data[i].dataCurrent.fVal;
        break;
    }
  }
}

// ---------------------------- eof -------------------------------
