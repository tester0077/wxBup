//-------------------------------------------------------------------
// Name:        wxBup.cpp
// Purpose:     main file for  backup & zip utility
// Author:      A. Wiegert
// 
// Copyright:   
// Licence:     wxWidgets licence
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

//------------------------------------------------------------------
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

//-------------------------------------------------------------------
// Header of this .cpp file
//-------------------------------------------------------------------
#include <wx/msgdlg.h>
#include "wxBupFrameh.h"
#include "wxBuph.h"
#if defined (WANT_CMD_LINE)
#include <wx/cmdline.h>
#include <wx/filename.h>
#include "wxBupCmdlineh.h"
#endif
#if defined( WANT_EXE_LINK_DATE )
#include "exeDosHdrh.h"
#endif
#include "wxBupConfh.h"
extern iniPrefs_t g_iniPrefs;
#if defined( WANT_DBGRPRT )
#include "wx/ffile.h"
#include <wx/busyinfo.h>
#include "wx/debugrpt.h"
#include "wxBupDebugRprth.h"
#endif
#include "wxOsVersionh.h"
#include "wxBupFileLogh.h"

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

// version info - to be displayed in 'About' dialog
// should only be defined in the file where it is used
// to avoid extraneous recompiles.
// If the values need to be used in more than one source file,
// the data should be passed around using variables
#if defined( WANT_FILEVERSION )
#include "fileVerh.h"
int giMajorVersion = 0;
int giMinorVersion = 0;
int giBuildNumber = 0;

int giOsMajor = 0;
int giOsMinor = 0;
#endif
//-------------------------------------------------------------------
// wxWidgets macro: Declare the application.
//-------------------------------------------------------------------

// Create a new application object: this macro will allow wxWidgets
// to create the application object during program execution (it's
// better than using a static object for many reasons) and also
// declares the accessor function wxGetApp() which will return the
// reference of the right type (i.e. the_app and not wxApp).
IMPLEMENT_APP(MyApp)
MyApp *theApp;
MyFrame *gpFrame     = (MyFrame *)NULL;

//-------------------------------------------------------------------
// Public methods
//-------------------------------------------------------------------
// ------------------------------------------------------------------
MyApp::MyApp()
{
#if defined( _MSC_VER )
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  // with things setup to identify memory leaks
  // there seem to be some leaks I can't find, but this seems to work
  // at least for some ?? adding this code seems to shift the block
  // numbers; I get the same number of leaks - and the same sizes,
  // but the block number changes -
  // MUST run under exact same conditions for this to work
//  _CrtSetBreakAlloc(11058);
#endif
#if defined( WANT_DBGRPRT )
  // user needs to explicitly enable this
  m_uploadReport = true;//false;

  // call this to tell the library to call our OnFatalException()
  wxHandleFatalExceptions();
#endif
}

// ------------------------------------------------------------------
#if defined( WANT_SINGLE_INSTANCE )
wxSingleInstanceChecker *m_checker;
MyApp::~MyApp()
{
#if defined( WANT_EXIT_LOG )
  logline( _T("MyApp::~MyApp"));
#endif
  delete m_checker;
#if defined( WANT_TASKBAR )
  delete m_pTaskBarIcon;
#endif
//  if ( m_pAppLog )
//    delete m_pAppLog;
//  if ( m_pLogChain )
//    delete m_pLogChain;
//  _CrtDumpMemoryLeaks();
  return;
}
#else
  MyApp::~MyApp(){;}
#endif

// ------------------------------------------------------------------
/**
 * We come here even if user closes app using the system menu red 'x',
 * but we cannot log here, the log window has already been destroyed
 */
int MyApp::OnExit()
{
  // clean up
  SaveConfig( true );
  return 0;
}

// ------------------------------------------------------------------
/* From: wxApp::OnRun();
 */
int MyApp::OnRun()
{
#if 0 // does not work as I want
  // it terminates alright, but never does any work
  if( m_bConsoleMode )
    return 0;
  // else
  //int exitcode = wxApp::OnRun();
#endif
  return wxApp::OnRun();;
}

// ------------------------------------------------------------------
// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
#if defined( WANT_SINGLE_INSTANCE )
  m_checker = NULL;
#endif
  theApp = this;
  ::wxInitAllImageHandlers(); // needed by wxCrafter
  m_wsAppName = GetAppName();
  m_stopWatch.Pause();
  m_lCleanupTime = m_lFileCountTime = 
    m_lZippingTime = m_lBaggingTime = 0l;
#if defined (WANT_CMD_LINE)
  m_bHaveJobFile = false;
  m_bQuietMode = false;
  m_bAddLog = true;
#endif
#if defined ( WANT_APP_DND )
  m_bFromDnD = false;
#endif
  m_pAppLog = NULL;
  m_pLogChain = NULL;
  // ------------------------------------------------------------------

#if defined ( WANT_STD_PATHS )
  // needed ahead of config
  // for Win 7 at least we have to write the file to the user
  // accessible AppData area because the executable directory
  // is off-limits
  // This means we have to create the data directory wxBup in 
  // the user's Home directory
  // XP:     c:/Documents and Settings/userName/Application data
  // Vista:  c:/Users/UserName/AppData/Roaming
  // Win7    c:/Users/UserName/AppData/Roaming
  // Other info says it would be better to have the in AppData/LocalLow
  // but the Inno installer has no way to set this directory based on 
  // OS, so I will simply stay with ..../roaming
  // This code sets up the main config directory, though the variable
  // is not used throughout because it is local to the app.
  // Instead similar code is used to get the same information
  // where needed
  m_wsConfigDir = wxStandardPaths::Get().GetUserConfigDir() + 
    _T("\\") + m_wsAppName;

  // MUST ensure the config directory exists, just in case user
  // or someone else deletes it; otherwise app cannot function
  // Sometimes fails because a file with the same name exists, 
  // offer to delete the file and start over
  bool firstTry = true;
tryAgain:
  if (!::wxDirExists(m_wsConfigDir))
  {
    if (::wxMkDir(m_wsConfigDir) != 0)
    {
      wxFileName wfnConfig(m_wsConfigDir);
      if (firstTry && wfnConfig.FileExists())
      {
        wxString wsT;
        wsT.Printf(_T("Can't create directory %s\nA file with the name '%s' existsts\n") +
          _("Delete the file and create the directory?"),
          wfnConfig.GetFullPath(), wfnConfig.GetName());
        int iResponse = wxMessageBox(wsT, _T("Confirm"), wxICON_QUESTION | wxYES_NO);
        if (iResponse == wxYES)
        {
          firstTry = false;
          ::wxRemoveFile(m_wsConfigDir);
          goto tryAgain;
        }
        else
          return false;
      }
      else
      {
        wxMessageBox(_T("Can't create: ") + m_wsConfigDir
          + _("\nCannot continue - Exiting") + _("\nHint: Perhaps a file by the name ")
          + m_wsAppName + _(" existsts in ") + wxStandardPaths::Get().GetUserConfigDir(),
          _T("Error"), wxOK);
        return false;
      }
    }
  }
#endif
#if defined( WANT_SINGLE_INSTANCE )
  // Cant run debug version while another version, even release, is running
  // because they both use the same data directory etc.
  const wxString wsName = wxString::Format( _T("%s-%s"), m_wsAppName, wxGetUserId() );

  m_checker = new wxSingleInstanceChecker( wsName );
  if ( m_checker->IsAnotherRunning() )
  {
    wxMessageBox( _("Another program instance is already running, aborting."),
      _("Error"), wxOK | wxICON_ERROR );
    delete m_checker; // OnExit() won't be called if we return false         
    m_checker = NULL;
    return false;
  }
#endif
#if 1
  /* We need to check the OS version early since we use the information
  * to handle differences in OS directory structure & names.
  * If we fail here, no point going on!!
  */
  /* this is unreliable and likely out-of-date compared to
   * real world from Microsoft
   * need to use some MS specific code; 
   * see: https://msdn.microsoft.com/en-ca/library/windows/desktop/ms724429%28v=vs.85%29.aspx
   */
  ::wxGetOsVersion( &giOsMajor, &giOsMinor );
  wxOsVersion wfvOS;
  if( !wfvOS.Open( _T("kernel32.dll") ) )
  {
    // we need to check, just in case, though if kernel32.dll is not found, we
    // likely won't get this far.
    wxMessageBox( _T("Cannot find 'kernel32.dll' to get OS version!")
      _T("\nExiting!"), _T("Error"),  wxOK | wxICON_ERROR );
    return false;
  }
  wxString wsVersion =  wfvOS.GetFileVersion();
  // now we need to parse this string, typically looks:	"10.0.10586.0 (th2_release.151029-1700)"	
  // all we want is first two numbers
  wxString wsRest;
  long lMajor, lMinor;
  wxString wsOsMajor = wsVersion.BeforeFirst( '.', &wsRest );
  wxString wsOsMinor = wsRest.BeforeFirst( '.' );
  wsOsMajor.ToLong( &lMajor );
  wsOsMinor.ToLong( &lMinor );
  giOsMajor = (int)lMajor;
  giOsMinor = (int)lMinor;
  //             XP                 Win 10
  if ( giOsMajor < 5 || giOsMajor > 10 )
  {
    // we need to check, the app is not set to handle other OSes
    wxMessageBox( _T("Application not setup to handle OS version ") + wsOsMajor +
      _T("\nExiting!"), _T("Error"),  wxOK | wxICON_ERROR );
    return false;
  }
#endif
// ------------------------------------------------------------------
#if defined (WANT_CMD_LINE)
  // Parse command line
  wxString wsCmdFilename;
  wxCmdLineParser cmdParser(g_cmdLineDesc, argc, argv);
  int res;
  {
    wxLogNull log;
    // Pass false to suppress auto Usage() message
    res = cmdParser.Parse(false);
  }
  m_bConsoleMode = cmdParser.Found( _T("C"));
  if( m_bConsoleMode )
    wxMessageOutput::Set( new wxMessageOutputStderr ); 
  // Check if the user asked for command-line help
  if (res == -1 || res > 0 
    || cmdParser.Found( _T("h")) || cmdParser.Found( _T("?"))  )
  {
    cmdParser.Usage();
    return false;
  }
  // Check for job file
  wxString wsJobFile;
  if (cmdParser.Found( _T("B"), &wsJobFile ) )
  {
    m_bHaveJobFile = true;
    m_wsJobFileName = wsJobFile;
  }
  // Check for bag name
  wxString wsBagBase = _T("bag");
  if (cmdParser.Found( _T("b"), &wsBagBase ) )
  {
    wxMessageBox( _("'bagname' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // copy only
  bool bCopyOnly = false;
  if (cmdParser.Found( _T("c") ) )
  {
    wxMessageBox( _("'copyonly' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
    bCopyOnly = true;
  }
  // Check for destination root
  wxString wsDestRoot;
  if (cmdParser.Found( _T("d"), &wsDestRoot ) )
  {
    wxMessageBox( _("'destroot' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // Check for exclude list
  wxString wsExcludeList;
  if (cmdParser.Found( _T("e"), &wsExcludeList ) )
  {
    wxMessageBox( _("'excludes' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // Check for omit bag log - switch MUST be negated
  wxCmdLineSwitchState clsState = cmdParser.FoundSwitch( _T("l") );
  if ( clsState == wxCMD_SWITCH_NOT_FOUND )
  {
    m_bAddLog = true;
  }
  else
  {
    m_bAddLog = ( clsState == wxCMD_SWITCH_ON );
    wxMessageBox( _("'log' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // Check for 'move-as-is' list
  wxString wsMoveOnlyList;
  if (cmdParser.Found( _T("m"), &wsMoveOnlyList ) )
  {
    wxMessageBox( _("'moveonly' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // get job name
  wxString wsJobName;   // ext wbj will be appended;
  if (cmdParser.Found( _T("n"), &wsJobName ) )
  {
    wxMessageBox( _("'name' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  m_bQuietMode = cmdParser.Found( _T("q"));
  // source list
  wxString wsSrcList;   
  if (cmdParser.Found( _T("s"), &wsSrcList ) )
  {
    wxMessageBox( _("'sources' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // media type => bag size
  wxString wsBagType;   
  if (cmdParser.Found( _T("t"), &wsBagType ) )
  {
    wxMessageBox( _("'type' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
  // berbosity 0 - 5
  long lVerbosity;   
  if (cmdParser.Found( _T("v"), &lVerbosity ) )
  {
    // wxLogError causes an issue because crit section
    // is not initialized yet
//    wxLogError( _("-v is not implemented yet!"));
    wxMessageBox( _("'verbosity' option is not implemented yet"), _("Notice"));
    // nothing to do
    return false;
  }
#if defined( WANT_CMDLINE_V )
  // I can't see any reason for this option to be useful.
  // If someone want to know the current version, let him 
  // open up the app; the version will be dispalyed in the 
  // title bar, or else in the 'About' box
  // Check if the user asked for the version
  // when runing, this displays a dialog box with
  // the message
  if (cmdParser.Found( _T("V")))
  {
#ifndef __WXMSW__
    wxLog::SetActiveTarget(new wxLogStderr);
#endif
    wxString msg;
    wxString date(wxString::FromAscii(__DATE__));
    msg.Printf(
      _("%s, - Version %d.%d - Build: %d, %s"),
      m_wsAppName,
      giMajorVersion, giMinorVersion, giBuildNumber,
      (const wxChar*) date);
#if defined ( MY_LOG )
    wxLogMessage(msg);
#endif
    return false;
  }
#endif
#endif

  // Make an instance of your derived frame. Passing NULL (the default value
  // of MyFrame's constructor is NULL) as the frame doesn't have a parent
  // since it is the main application window.
  gpFrame = new MyFrame( NULL );
  m_frame = gpFrame;
  m_wsAppName = GetAppName();

  CreateConfig(); //create this first - for .INI file
  RestoreConfig();
  // now we can check all drives because we can provide feedback 
  // for the user, it the verboisty is set hign enough
  m_frame->GetDriveInfo(  m_frame->m_DriveInfo );
#if defined( WANT_GRID )
  m_frame->RestoreColLabels();
  m_frame->RestoreColWidths();
  m_frame->InitializeResultsGrid();
#endif

  wxString wsT;
#if defined( WANT_VERSION_IN_TITLE )  
#if defined( WANT_NEW_TITLE )
  m_frame->SetMainFrameTitle( wxEmptyString );
#else
  // Set up the main frame title with version & build info
  // Note: the base frame has the main frame title set to:
  // "%s - %s - Version %d.%d - Build: %d"
  wxString wsFrameTitle = m_frame->GetLabel();
  wsT.Printf( wsFrameTitle, m_wsAppName, _("Backup & Zip" ),
    giMajorVersion, giMinorVersion, giBuildNumber );
  m_frame->SetLabel( wsT );
#endif
#endif
#if 1
  // we can now set this up
  // handle the new when done choice box in main frame tool bar
  if( (g_iniPrefs.data[IE_WHEN_DONE].dataCurrent.lVal >= 0 ) &&
       (g_iniPrefs.data[IE_WHEN_DONE].dataCurrent.lVal < 
       (long)m_frame->GetChoiceWhenDone()->GetCount() ) )
  {
    m_frame->GetChoiceWhenDone()->SetSelection( 
      g_iniPrefs.data[IE_WHEN_DONE].dataCurrent.lVal );
  }
  else
    m_frame->GetChoiceWhenDone()->SetSelection( 0 );
  m_frame->GetCheckBoxWhenDone()->SetValue(
    g_iniPrefs.data[IE_WHEN_DONE_ENABLED].dataCurrent.bVal );
#endif
#if defined( WANT_LOG ) // want log window etc
  m_frame->m_pLogWindow = 
      m_frame->GetSplitterMain()->GetWindow2();
  bool bShowLog = 
    g_iniPrefs.data[IE_SHOW_LOG].dataCurrent.bVal;
  if( !bShowLog )
  {
    m_frame->GetSplitterMain()->Unsplit();
  }
  theApp->m_frame->Layout();
#if defined( WANT_LOG_CHAIN )
  g_iniPrefs.data[IE_LOG_DIR_PATH].dataDefault.wsVal =
    g_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal = m_wsConfigDir;
  m_wsCurLogFileName = g_iniPrefs.data[IE_LOG_DIR_PATH].dataCurrent.wsVal + _T("\\") + WS_LOG_FILE_NAME;
  if ( g_iniPrefs.data[IE_LOG_FILE_WANTED].dataCurrent.bVal )
  {
    CreateNewLog();
  }
#endif    // WANT_LOG_CHAIN
#endif    // WANT_LOG
  // only set the default, if the path is empty
  if ( g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal.IsEmpty() )
  {
    g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal =
      wxStandardPaths::Get().GetTempDir();
  }
  else  // verify the path is usable
  {
    wxFileName wfnTemp;
    wfnTemp.AssignDir( g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal );
    if( !wfnTemp.IsOk() )
    {
      wxMessageBox( g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal +
        _(" is not a valid path"), _("Error"), wxOK | wxICON_HAND );
      return false;
    }
    if( !wfnTemp.IsAbsolute() ) // MUST be absolute
    {
      wxMessageBox( _("The current default path:\n" ) + 
        g_iniPrefs.data[IE_GLOBAL_TEMP_DIR].dataCurrent.wsVal +
        _("\nis not a valid path"), _("Error"), wxOK | wxICON_HAND );
//      return false;
    }
  }

  // populate the Job list now that we have read the .INI file
  // needed mainly to filter out .XML files which are NOT job
  // files. such as the DTD spec file.
  m_frame->FillJobList();
  // Show the frame as it's created initially hidden.
  m_frame->Show(true);
  if( wxGetApp().m_bConsoleMode )
  {
    // after some discussion on the forum, iconizing works
    // and gives the user the option to restore the app 
    // to allow monitoring progress
    wxTopLevelWindow * tw = (wxTopLevelWindow *)wxApp::GetTopWindow();
    tw->Iconize( true );
  }
#if defined ( WANT_APP_DND )
  // associate drop targets with the main dialog
  m_frame->SetDropTarget(new DnDFile( m_frame ));
#endif
  /**
   * Note, we only care about internet availability if we are running the
   * release version.
   * When we run the debug versiion, we only need the local server.
   * the called routine will perform the check for internet
   */
  wxString wsCurrentVersion;
  wxString wsLatestVersion;
  if( g_iniPrefs.data[IE_CHECK4NEW_VERSION].dataCurrent.bVal )
    m_frame->Check4Update( false, wsCurrentVersion, wsLatestVersion );

  // Return true to tell program to continue (false would terminate).
  return true;
}

// ------------------------------------------------------------------

#if defined( WANT_EXE_LINK_DATE )
#include <wx/stdpaths.h>
// ------------------------------------------------------------------
/**
 * RetrieveLinkerTimestamp - get the linker time stamp from the current executable.
 * Used to display the build date & copyright year in the 'About box.
 * The old way used the compile macro __DATE__, but that froze the build date
 * to the last time the file with the 'OnAbout() function was compiled,
 * not what I wanted.
 * Taken from:
http://www.codinghorror.com/blog/2005/04/determining-build-date-the-hard-way.html
 */

#define BYTE_BFR_SIZE 2048
#define PE_EXE_LINK_TIME  0x100

bool MyApp::RetrieveLinkerTimestamp( wxString& wsDate, wxString& wsYear )
{
  BYTE cByteBfr[BYTE_BFR_SIZE];
  size_t szBytesRead;
  long lSecondsSince1970;
  unsigned long ulNewHdrOffset;
  
  wsDate = wxEmptyString;
  wxString wsExePath = wxStandardPaths::Get().GetExecutablePath();
  wxFile wfExe( wsExePath );
  if ( !wfExe.IsOpened() )
    return false;
  szBytesRead = wfExe.Read( cByteBfr, BYTE_BFR_SIZE );
  if( szBytesRead < BYTE_BFR_SIZE )
    return false;
  DosExeHdr pDosExeHdr( cByteBfr, BYTE_BFR_SIZE );
  ulNewHdrOffset = pDosExeHdr.GetNewHdrOffset( cByteBfr );
  // get the EXE link date & time as seconds since Jan 1, 1970
  // located at NewHdrOffset + 8 bytes
  lSecondsSince1970 = *(long *)(cByteBfr + ulNewHdrOffset + 8);
  wxDateTime wdtExe( (time_t)lSecondsSince1970 );    // link time of .EXE
  wsYear.Printf( _T("%d"), wdtExe.GetYear() );  // Copyright year
  // Note in wx 2.9.4 format "%e" - format day without leading '0' - bombs
  // under Windows
  wsDate = wdtExe.Format( _T("%B %#d, %Y") );// _T("%B %e, %Y") );
  return true;
}

#endif  //#if defined( WANT_EXE_LINK_DATE )

// ------------------------------- eof ------------------------------
