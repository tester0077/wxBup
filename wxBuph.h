//-----------------------------------------------------------------------------
// Name:         wxBuph.h 
// Purpose:      backup & zip utility main header
//
// Licence:     wxWidgets licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifndef _WX_WXBUP_H_
#define _WX_WXBUP_H_

//-----------------------------------------------------------------------------
// Headers
// ------------------------------------------------------------------
#include "wxBupPreProcDefsh.h" // needs to be first
//-----------------------------------------------------------------------------
#include <wx/snglinst.h>
#include <wx/app.h>             // wxApp
#include <wx/dnd.h>
#include <wx/fileconf.h>
#include <wx/ffile.h>

#include "wxBupFrameh.h" // needs to be first

// ------------------------------------------------------------------
// globals  defined in wxBuFrame.cpp
#if defined( WANT_FILEVERSION )
extern int giMajorVersion;
extern int giMinorVersion;
extern int giBuildNumber;
extern int giOsMajor;
extern int giOsMinor;
#endif

// ------------------------------------------------------------------
#include "wxBupBaseFrame.h"
#include "wxBupFrameh.h"
#include "wxBupConfh.h"
#include "wxBupJobConfigh.h"
#include "wxBupAppDndh.h"
// ------------------------------------------------------------------

extern iniPrefs_t g_iniPrefs;
extern jobPrefs_t g_jobData;
// ------------------------------------------------------------------

// command line processing
#if defined (WANT_CMD_LINE)
#include "wx/cmdline.h"
#endif
#if defined ( WANT_STD_PATHS )
#include <wx/stdpaths.h>
#endif
#if defined( WANT_DBGRPRT )
#include "wx/debugrpt.h"
#include <wx/busyinfo.h>
#endif
#if defined ( WANT_FILEHISTORY )
#include <wx/docview.h>
#endif
#include "wxBupFileLogh.h"

//-------------------------------------------------------------------
// Class definition: MyApp
//-------------------------------------------------------------------

class MyFrame;
// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
  MyApp();
  ~MyApp();
  MyFrame *m_frame;  
  wxString m_wsAppName;
  wxString m_appDir;
  // Override base class virtuals:
  // wxApp::OnInit() is called on application startup and is a good place
  // for the app initialization (doing it here and not in the ctor
  // allows to have an error return: if OnInit() returns false, the
  // application terminates)
  virtual bool OnInit();
  // from http://www.wxwidgets.org/wiki/index.php/Command-Line_Arguments
  virtual int OnExit();
  virtual int OnRun();

#if defined( WANT_SINGLE_INSTANCE )
  wxSingleInstanceChecker *m_checker;
#endif
  //config stuff
  void CreateConfig( void );
  void RestoreConfig ( void  );
  void SaveConfig( bool bDone );
  void MakeConfigCopy( iniPrefs_t& iniPrefsSrc,
    iniPrefs_t& iniPrefsTarg );
  wxFileConfig            *m_pConfig;
  wxWindow                *m_pLogWindow;
  wxStopWatch             m_stopWatch;
  long                    m_lCleanupTime;
  long                    m_lFileCountTime;
  long                    m_lZippingTime;
  long                    m_lBaggingTime;
  void MakeJobConfigCopy( jobPrefs_t& iniPrefsSrc,
    jobPrefs_t& iniPrefsTarg );
  bool JobCompareOldAndNew( jobPrefs_t& iniPrefsOld,
    jobPrefs_t& iniPrefsNew );
#if defined ( WANT_APP_DND )
  bool                    m_bFromDnD;
#endif
#if defined (WANT_CMD_LINE)
  // for command line operation
  // from http://www.wxwidgets.org/wiki/index.php/Command-Line_Arguments
  bool                    m_bConsoleMode;
  bool                    m_bHaveJobFile;
  bool                    m_bQuietMode;     // ask no questions if true
  bool                    m_bAddLog;
  wxString                m_wsJobFileName;
#endif
#if defined( WANT_SOUND )
    wxSound  m_sDone;
#endif // WANT_SOUND
  // job config
  // for now I create the config object in each function and
  // then delete it as the data will be saved in and available
  // from the data structure g_jobData
  bool RestoreJobConfig ( wxString wsJobName );
  bool SaveCloseJobConfig( wxString wsJobName );

  bool IsInternetAvailable();

#if defined( WANT_EXE_LINK_DATE )
  bool RetrieveLinkerTimestamp( wxString& wsDate, wxString& wsYear );
#endif

  // directory for data files - *.log, *.INI
  // returned by: wxStandardPaths::Get().GetUserConfigDir();
  wxString                m_wsConfigDir;
  wxArrayString           m_wasFileNames;
  wxString                m_wsCurLogFileName;

#if defined ( WANT_FILEHISTORY )
  wxFileHistory   *m_pFileHistory;  ///< file history object
#endif
#if defined( WANT_LOG_CHAIN )
  wxLogChain              *m_pLogChain;
  MyFileLog              *m_pAppLog;
  bool                    CreateNewLog();
#endif
#if defined( WANT_DBGRPRT )
  // called when a crash occurs in this application
  virtual void OnFatalException();
  
  // this is where we really generate the debug report
  void GenerateReport(wxDebugReport::Context ctx);

  // if this function is called, we'll use MyDebugReport which would try to
  // upload the (next) generated debug report to its URL, otherwise we just
  // generate the debug report and leave it in a local file
  void UploadReport(bool doIt) { m_uploadReport = doIt; }
#endif

private:    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#if defined( WANT_DBGRPRT )
  bool          m_uploadReport;
  wxBusyInfo    *m_pWaitDialog;
#endif
};

DECLARE_APP( MyApp );

#endif  //_WX_WXBUP_H_
// ------------------------------- eof ------------------------------
