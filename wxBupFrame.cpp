//--------------------------------------------------------------
// Name:        wxBupFrame .cpp
// Purpose:     main frame file for  backup & zip utility
//              
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//--------------------------------------------------------------
// Header of this .cpp file
//--------------------------------------------------------------
#include "wx/msgdlg.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/timer.h"
#include "wx/thread.h"

#include "wxBupFrameh.h" // needs to be first
#include "wxBupConfh.h"

// ------------------------------------------------------------------

#if defined( WANT_FILEVERSION )
#include "fileVerh.h"
extern int giMajorVersion;
extern int giMinorVersion;
extern int giBuildNumber;

extern int giOsMajor;
extern int giOsMinor;
#endif

// -------------------------------------------------------------
#if defined(_MSC_VER ) // from Autohotkey-hummer.ahk
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
// -------------------------------------------------------------

extern MyApp *theApp;
// -------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
// The reason why the menuitems and tools are given the same name in the
// XRC file, is that both a tool (a toolbar item) and a menuitem are designed
// to fire the same kind of event (an EVT_MENU) and thus I give them the same
// ID name to help new users emphasize this point which is often overlooked
// when starting out with wxWidgets.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(wxID_ABOUT,          MyFrame::OnAbout)
  EVT_MENU(wxID_EXIT,           MyFrame::OnExit)
  EVT_MENU(WORKER_EVENT_START,  MyFrame::OnStartWorker)
#if defined( WANT_GRID )
  EVT_GRID_COL_SIZE(            MyFrame::OnGridColSize)
#endif
  EVT_TIMER(TIMER_ID,           MyFrame::OnTimer)
#if defined( EXIT_EVENT )
  EVT_THREAD(WORKER_EVENT_EXIT, MyFrame::OnWorkerEventDone)
#endif
#if defined ( RESTORE_EXIT_EVENT ) 
  EVT_THREAD(RESTORE_WORKER_EVENT_EXIT, MyFrame::OnRestoreEventDone)
#endif
#if defined( LOG_EVENT )
  EVT_THREAD(WORKER_EVENT_LOG,  MyFrame::OnWorkerEventLogMsg)
#endif
#if defined ( WANT_FILEHISTORY )
  EVT_MENU_RANGE( wxID_FILE1, wxID_FILE9, MyFrame::OnMruFile )
#endif
 // job list context menu handlers
  EVT_MENU( wxID_CONTEXT_EDIT_JOB,   MyFrame::OnContextEditJob )
  EVT_MENU( wxID_CONTEXT_RUN_JOB,    MyFrame::OnContextRunJob )
  EVT_MENU( wxID_CONTEXT_DELETE_JOB, MyFrame::OnContextDeleteJob )
  EVT_MENU( wxID_CONTEXT_RENAME_JOB, MyFrame::OnContextRenameJob )
  EVT_MENU( wxID_CONTEXT_SAVE_AS,    MyFrame::OnContextJobSaveAs )
END_EVENT_TABLE()
// ------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

// Constructor
MyFrame::MyFrame( wxWindow* parent ) 
  : MyFrameBase( parent ), m_timer( this, TIMER_ID)
{
  m_bCheck = false;
  m_bSendingMailInProgress = m_bRunning = m_bCancelled = false;
  m_bHaveDirOrFile = false;
  m_bEditJob = false;
  m_bCleaningUp = m_bCountingFiles = false;
  m_bJobSelected = m_bSplitterSetDone = false;
  m_bHadError = m_bSkippedFiles = false;
  m_ulMaxSrcPathCompLen = m_ulMaxDestPathCompLen = 
    m_ulMaxDestPathLen = m_ulMaxDestFileNameLen = 0l;
  m_ullTotalZipsSize = 0ll;
  m_ullSelectedBagSize = 0ll;
  m_ulDirs = m_ulBags = m_ulMediumAllocUnit = 0l;
  m_pJobConf = NULL;
  m_ullTotalSrcBytes = m_ullTotalZipBytes = 0ll;
  m_uiSrcDirLevelDepth = m_uiZipDirLevelDepth = 0;
  m_szNTotalFiles = 0;
  m_pAppProgress = new  wxAppProgressIndicator(this);
  m_JobTypeInProgress = MY_JOB_TYPE_NONE;
  m_iNIdleCalls = 0;
  m_wsTotalTime = m_wsAllTimes = wxEmptyString;
#if defined( WANT_POWER_BLOCKER )
  m_pShutDownBlocker = NULL;
#endif
  if( m_pAppProgress->IsAvailable() )
  {
    m_pAppProgress->SetRange( 100 );
    m_bHaveAppProgress = true;
  }
  else
    m_bHaveAppProgress = false;
  // note with the rc stuff part of the executable
  // it seems to serve the same purpose as the XPM way
  SetIcon(wxICON(appicon));  

  m_wsZipChunkBaseFileName = _T("");
#if defined( WANT_FILEVERSION )
//#include "fileVerh.h"
  wxFileVersion fVer;
  // open the executable's version info
  bool bErr = fVer.Open();
  if( bErr == false )
  {
    wxString wsT;
    wsT.Printf( _("Cannot find the .EXE file version, exiting !") );
    wxMessageBox( wsT, _("Error") );
    //return;
  }
  wxString wsFV = fVer.GetFixedFileVersion();
  // and close the 
//    fVer.Close();  
  giMajorVersion = fVer.GetMajorVersion();
  giMinorVersion =  fVer.GetMinorVersion();
  giBuildNumber  = fVer.GetBuildNumber();
    //  fVer.GetSpecialBuild();
#endif
  m_bFoundHelpFile = false;
#if defined( WANT_HELP )
#if defined( WANT_ZIP_HELP )
  //wxFileSystem::AddHandler(new wxZipFSHandler);
  //m_pProvider = new wxHelpControllerHelpProvider;
  //wxHelpProvider::Set( m_pProvider );
  m_pProvider->SetHelpController(GetHelpController());
  if( !GetHelpController()->AddBook(wxFileName(_T("wx??Help.zip"))))
  {
    wxLogError( _("Cannot initialize the Help system."));
  }
#else
  // help file now is in the %USER%/AppData/Roaming   for Vista
//  wxString wsCurPath = wxStandardPaths::Get().GetResourcesDir();
  wxFileName wfnHelpFile;
  wfnHelpFile.AssignDir( wxGetApp().m_wsConfigDir );
  // and construct the help file name
  wxString wsHelpFile = wxGetApp().m_wsAppName;
  wfnHelpFile.SetName( wsHelpFile + _T("Help") );
  wfnHelpFile.SetExt( _T("chm") );
  m_wsCurHelpFile = wfnHelpFile.GetFullPath();
  if( ! ::wxFileExists( m_wsCurHelpFile ) )
  {
    wxLogError( _("Cannot find the Help file %s."), m_wsCurHelpFile);
    m_bFoundHelpFile  = false;
  }
  else
    m_bFoundHelpFile  = true;
  m_pProvider = new wxHelpControllerHelpProvider;
  wxHelpProvider::Set( m_pProvider );
  m_pProvider->SetHelpController(GetHelpController());
  // Note: this code only checks if the basic MS Help DLLs are available
  // not whether or not the help file can be found
  if( !GetHelpController()->Initialize( m_wsCurHelpFile ) )
  {
    wxLogError( _("Cannot initialize the MS HTML Help system."));
  }
#endif
#endif
#if defined ( WANT_FILEHISTORY )
  // set up the MRU stuff for the File menu
  m_fileRecent = new wxMenu();
  theApp->m_pFileHistory = new wxFileHistory();
  theApp->m_pFileHistory->UseMenu( m_fileRecent );
  // clean up the file history; 
  // remove any files which can no longer be found.
  size_t iCount = theApp->m_pFileHistory->GetCount();
  wxString wsFileName;
  for ( size_t i = 0; i < iCount; i++ )
  {
    wsFileName = theApp->m_pFileHistory->GetHistoryFile( i );
    if ( ::wxFileExists( wsFileName ) )
    {
      theApp->m_pFileHistory->AddFileToHistory( wsFileName );
    }
  }
  int n = m_menuFile->GetMenuItemCount();
  m_menuFile->InsertSeparator( n );
  m_menuFile->Insert( n + 1, wxID_ANY, _T("Recent files"), m_fileRecent );
#endif
#if defined( WANT_LOG )
  // set up Log Window
  // send text to it via something like:
  // wxLogStatus(_T("Caret is at (%d, %d)"), m_xCaret, m_yCaret);
#if 1
  m_pLogWin = new wxLogTextCtrl( m_textCtrlLog );
  m_pLogOld = wxLog::SetActiveTarget( m_pLogWin );
#else
//  m_pLogOld = wxLog::SetActiveTarget( new wxLogTextCtrl( m_textCtrlLog ) );
#endif
#endif
  // handles the restoration of the splitter window sashes on start up
  // from the *.INI file.
  // This handler is removed once the update is finished and
  // the splitter window sash positions have been set
  Connect( wxEVT_IDLE, (wxObjectEventFunction)&MyFrame::OnIdle );
#if defined( WANT_DBGRPRT )
#ifndef _DEBUG
  // remove the crash menu item if in release mode
  int iCrashId = m_menubarMain->FindMenu( _T("Crash") );
  wxASSERT( iCrashId != wxNOT_FOUND );

  wxString wsMenu;
  for ( size_t i = 0; i < m_menubarMain->GetMenuCount(); i ++ )
  {
    wsMenu = m_menubarMain->GetMenuLabelText( i );
    if ( wsMenu.IsSameAs( _T("Crash") ) )
      m_menubarMain->Remove( i );
  }
#endif
#endif
#if defined( WANT_SOUND )
  m_pSound = NULL;
  m_wsSoundFile = wxEmptyString;
#if defined( WANT_MEMORY_SOUND )
  m_bUseMemory = false;//true;
#endif
#endif
  InitializeJobContentsGrid();

//  GetDriveInfo( m_DriveInfo );
  GetGaugeProgress()->SetValue( 0 );
  UpdateSummary( wxEmptyString, wxEmptyString, wxEmptyString );
  
  DisplayProcessList();
}

//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

// -------------------------------------------------------------
/**
 * We need to call 
 * Destroy();
 * instead of 
 * Close(true);  // so the main frame dtor will be called
 * when the application is closed from the File menu or with Alt-F4
 * This function is NOT called when the user closes the application
 * using the red 'x' on the system menu. 
 * That action calls the main frame dtor directly.
 */
void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
  Destroy();
}
//-----------------------------------------------------------------------------

MyFrame::~MyFrame()
{
  // save the frame position
  int x, y, w, h;

  GetSize(&w, &h);
  g_iniPrefs.data[IE_FRAME_W].dataCurrent.lVal = w;
  g_iniPrefs.data[IE_FRAME_H].dataCurrent.lVal = h;

  GetPosition(&x, &y);
  g_iniPrefs.data[IE_FRAME_X].dataCurrent.lVal = x;
  g_iniPrefs.data[IE_FRAME_Y].dataCurrent.lVal = y;

  g_iniPrefs.data[IE_SASH_POS].dataCurrent.lVal =
    m_splitterMain->GetSashPosition();
  // checking the menu item state is unreliable at this point
  // could it be changed by OnUpdateUI??
//  g_iniPrefs.data[IE_SHOW_LOG].dataCurrent.bVal =
//    m_menuItemLogShow->IsChecked();
  g_iniPrefs.data[IE_JOB_SASH_POS].dataCurrent.lVal =
    m_splitterJob->GetSashPosition();
#if 1
  // handle the new 'when done' choice box in main frame tool bar
  g_iniPrefs.data[IE_WHEN_DONE].dataCurrent.lVal = m_choiceWhenDone->GetSelection();
#endif
#if defined( WANT_LOG )
  // now logging works, does not crash on exit
  // and does not result in a memory leak.
  delete wxLog::SetActiveTarget( m_pLogOld );
#endif
#if defined( WANT_GRID )
  SaveColWidths();
  SaveColLabels();
#endif
  delete m_pAppProgress;
  if ( m_pJobConf != NULL )
  { 
    delete m_pJobConf;
    m_pJobConf = NULL; 
  }
#if defined( WANT_SOUND )
  if ( m_pSound )
    delete m_pSound;
#endif
}

// -------------------------------------------------------------
// It now does what it needs to to do with
// multi-threaded programming
void MyFrame::OnStop( wxCommandEvent& event )
{
  wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );

  m_bStopScan = m_bCancelled = true;
  m_bRunning = false;
#if defined( WANT_POWER_BLOCKER )
  if( m_pShutDownBlocker )
  {
    delete m_pShutDownBlocker;
    m_pShutDownBlocker = NULL;
  }
#endif
  // feedback for user
  m_textCtrlLog->SetDefaultStyle(wxTextAttr(*wxRED));
  wxLogMessage( _("User pressed 'Stop'\nCleaning up! ....") );
  m_textCtrlLog->SetDefaultStyle(wxTextAttr( wxTextAttr(*wxBLACK) ));
  m_gaugeProgress->SetValue(0);
}
#if defined( WANT_GRID )
// ------------------------------------------------------------------ 
void MyFrame::InsertItemInReportView(long i, 
                                     wxString fileName,
                                     wxString path,
                                     wxString sizeStr )
{

  wxFileName wfnFile( fileName );
  m_grid->InsertRows( i );
  m_grid->SetCellValue( i, 0, wfnFile.GetFullName() );
  m_grid->SetCellValue( i, 1, path );

  m_grid->SetCellAlignment( wxALIGN_RIGHT, i, 2 );
  m_grid->SetCellValue( i, 2, sizeStr );

  m_grid->MakeCellVisible( i, 0 );

  ResizeResultsDataGrid();
}
#endif
#if 0
// ------------------------------------------------------------------
bool MyFrame::Cancelled()
{
  wxCriticalSectionLocker lock(m_csCancelled);
  return m_bCancelled;
}
#endif
// ------------------------------- eof ------------------------------
