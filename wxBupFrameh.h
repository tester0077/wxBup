//-------------------------------------------------------------------
// Name:        wxBupFrameh.h
// Purpose:     main frame header for backup & zip utility
// Author:      A. Wiegert
//
// Copyright:
// Licence:     wxWidgets licence
//-------------------------------------------------------------------

#ifndef _WX_BUP_FRAME_H_
#define _WX_BUP_FRAME_H_

// ------------------------------------------------------------------
#include "wxBupPreProcDefsh.h" // needs to be first
//-----------------------------------------------------------------------------
#if defined( WANT_POWER_BLOCKER )
#include <wx/power.h>
#endif
#include <vector>
#include <algorithm>
#include "wx/dynlib.h"
#include "wx/textfile.h"
#include "wx/progdlg.h"
#include "wx/thread.h"
#include "wx/filehistory.h"
#include "wx/appprogress.h"
#include "wx/sound.h"

#include "wxBupZipListh.h"
#include "wxBuph.h"
#include "wxBupBaseFrame.h"
#include "wxBupJobConfigh.h"
#if defined( WANT_HELP )
#include "wx/cshelp.h"
#include "wx/html/helpctrl.h"
#endif
#include "wxBupBagSizes.h"
#include "wxBupWizTreebookh.h"

#include "wxBupBagLogListh.h"
#include "wxBupWhenDone.h"
// ------------------------------------------------------------------
  // my color ids for the log message event handler
#define  MY_BLACK     1
#define  MY_RED       MY_BLACK + 1
#define  MY_BLUE      MY_RED + 1
#define  MY_GREEN     MY_BLUE + 1

#define MY_SPECIAL_FILE   0x0100
#define MY_FILE_TOO_BIG   0x0200
#define MY_SKIPPED_FILE   0x0400
#define MY_FAILED_FILE    0x0800
#define MY_UDF_LIMIT_FILE 0x1000
#define MY_MISSING_DIR    0x2000
#define MY_ERROR          0x4000
// ------------------------------------------------------------------
// IDs for misc needs
enum
{
  TIMER_ID            = wxID_HIGHEST + 1,

  MY_JOB_TYPE_NONE,         // used to keep track of the job type in progress
  MY_BACKUP_JOB,
  MY_TEST_JOB,
  MY_RESTORE_JOB,

  WORKER_EVENT_START,               // this one gets sent from the worker thread
#if defined ( EXIT_EVENT )
  WORKER_EVENT_EXIT,                // this one gets sent from MyWorkerThread on exit
#endif
#if defined ( RESTORE_EXIT_EVENT )
  RESTORE_WORKER_EVENT_EXIT,        // this one gets sent from MyRestoreThread on exit
#endif
#if defined ( LOG_EVENT )
  WORKER_EVENT_LOG,                 // this one gets sent from MyWorkerThread for
                                    // every  wxLogMessage
#endif
  // menu and control ids
  wxID_CONTEXT_OPEN_JOB,
  wxID_CONTEXT_RUN_JOB,
  wxID_CONTEXT_DRY_RUN,
  wxID_CONTEXT_EDIT_JOB,
  wxID_CONTEXT_RENAME_JOB,
  wxID_CONTEXT_DELETE_JOB,
  wxID_CONTEXT_SAVE_AS,

};

// -------------------------------------------------------------

class MyFrame : public MyFrameBase
{
public:
  MyFrame( wxWindow* parent );
  virtual ~MyFrame();

  wxString          m_wsHomePath;
  int               m_iFlags;
  bool              m_bCompile;
  bool              m_bCheck;
  wxString          m_wsZipChunkBaseFileName;
  bool              m_bRunning;
  bool              m_bHaveDirOrFile;
  bool              m_bCleaningUp;        // disable the 'stop' button during this time
  bool              m_bCountingFiles;     // same while counting the files to be backed up
  int               m_JobTypeInProgress;  // used too keep track of the job type in progress

#if defined( WANT_HELP )
  wxHelpControllerHelpProvider* m_pProvider;
  void OnKeywordSearch(wxCommandEvent & event);
  void OnShowHelpContents(wxHelpEvent & event);
#if defined( WANT_ZIP_HELP )
  wxHtmlHelpController     m_msHtmlHelp;
  wxHtmlHelpController * GetHelpController()
  {
    return &m_msHtmlHelp;
  }
  void OnShowSection_FileMenu(wxCommandEvent & event);
#else
#if defined( _MSC_VER ) // only good for Windows
  wxCHMHelpController     m_msHtmlHelp;

  void OnMenuShowHelpContents( wxCommandEvent& event );
  void OnShowSection_FileMenu(wxCommandEvent & event);
  // can't overload the next function
//  void OnDialogContextHelp(wxCommandEvent & event);
  wxHelpController * GetHelpController()
  {
    return &m_msHtmlHelp;
  }
#endif
  wxString        m_wsCurHelpFile;
#endif
#endif
#if defined( WANT_LOG )
  wxLog     *m_pLogOld;
  wxWindow* m_pLogWindow; ///< ptr to log window when not shown
  wxLogTextCtrl* m_pLogWin;
#endif
#if defined( WANT_LOG_CHAIN )
  void OnDeleteLogFiles( wxCommandEvent& event );
#endif

  size_t FormatSize(double dNumber, wxString &wsOutput );
  double GetDepth(double dNumber, int& iDepth) ;

  // accessors for MyWorkerThread (called in its context!)
  bool IsWorkerCancelled();
  bool IsWorkerRunning();
  // same for test/restore thread
  bool RunTest( wxString wsTestDir );
  void TestOneBag( wxString wsBagLogPath );
  void SetupOneBag( wxString wsBagLogPath );
  void StartRestoreThread( bool bRestore );

  bool TestCancelled();
  // new job grid
  void InitializeJobContentsGrid();
  void ResizeJobContentsGrid();
  void OnJobContentsGridSize(wxSizeEvent& event);
#if defined( WANT_GRID )
  void InsertItemInReportView(long i,
    wxString fileName, wxString path, wxString sizeStr );
  void InitializeResultsGrid();
  void OnGridSize(wxSizeEvent& event);
  void ResizeResultsDataGrid();
  void RestoreColWidths();
  void SaveColWidths();
  void RestoreColLabels();
  void SaveColLabels();

  wxArrayLong     m_walColWidths;
  wxArrayString   m_wasColLabels;
#endif
  bool RemoveDirAndFiles( wxString wsDir );
  bool MyFileCopy( const wxString &a_rwsSrc, wxString &a_rwsDest, bool bFailIfExists );
  bool InsertFileInZipList( wxString wsDestFullPath, wxString wsSrcFullPath,
    unsigned long long a_ullSrcFileSize, unsigned long long ullZipFileSize );
  void OpenOptionsDlg();
  void DumpZipList();
  void SetNextBagData( unsigned long ulTotalFilesToCheck, wxString wsBagLogPath );

  void TicksToString( long lTicks, wxString& wsT );
  // job files
  // file operations
  wxFileConfig    *m_pJobConf;
#if defined( WANT_LAST_JOB_DATE )
  bool GetLastBackupDate( wxString wsFileName, wxString & wsDate );
#endif
//  void SaveJob();
//  void RestoreJob( wxString wsFileNameAndPath );
  bool GetFileFromUser( wxString &wsJobFile );
  void MyOpenFile(wxString wsFilePathAndName );
  bool OpenExistingFile(wxString wsFilePathAndName );
  void SetMainFrameTitle( wxString wsLocation );

  void SetBagSize();
  void ExpandPath( wxString & wsFilePath2Expand );
  // Job list
  void FillJobList();
  // context menu
  void OnJobListRightClick(wxContextMenuEvent& event);
  void ShowContextMenu(const wxPoint& pos);

  void OnContextEditJob( wxCommandEvent& WXUNUSED( event ) );
  void OnContextRunJob( wxCommandEvent& WXUNUSED( event ) );
  void OnContextDeleteJob( wxCommandEvent& WXUNUSED( event ) );
  void OnContextRenameJob( wxCommandEvent& WXUNUSED( event ) );
  void OnContextJobSaveAs( wxCommandEvent& WXUNUSED( event ) );

  void DeleteJobFile();
  bool SaveJobAs( wxString wsOldFullPath, wxString& wsNewFullPath );

  // email feedback
  void SendEmail();
  void ComposeAsciiMessage(wxString & ar_wsMessage );
  bool m_bSendingMailInProgress;
#if defined( WANT_POWER_BLOCKER )
  wxPowerResourceBlocker  *m_pShutDownBlocker;
#endif

  size_t                  m_szNFilesProcessed;
  size_t                  m_szNTotalFiles;
  unsigned long long      m_ullTotalZipsSize;     // actual size of all zip file, in bytes
  EBupBagSizeTypes        m_eSelectedBagSize;     // !! needed to initialize EBupBagSizeTypes::m_Map
  unsigned long long      m_ullSelectedBagSize;
  wxArrayString           *m_pwasFiles;
  unsigned long           m_ulDirs;
  unsigned long           m_ulBags;
  wxTimer                 m_timer;
  wxString                m_wsTotalTime;
  wxString                m_wsAllTimes;
  bool                    m_bEditJob;
  wxFileName              m_wfnDest;              // destination directory to hold the backed up data

  wxArrayString           m_wasBags;              // all bag log paths
  unsigned long           m_ulMediumAllocUnit;    // size of the allocation unit on the specified
                                                  // medium, in bytes
  // these length are expressed as # of TCHARs => wchar_t = 2 bytes
  unsigned long           m_ulMaxSrcPathCompLen;  // max length of source path components
  unsigned long           m_ulMaxDestPathCompLen; // max length of destn path components
  unsigned long           m_ulMaxDestPathLen;     // max length of destn path found - max: 511 chars
  unsigned long           m_ulMaxDestFileNameLen; // max length of destn path found - max: 127 chars
  wxString                m_wsLongestDestFileName;
  wxString                m_wsLongestDestPath;
  unsigned long long      m_ullTotalSrcBytes;     // total size of source in bytes
  unsigned long long      m_ullTotalZipBytes;     // total size of zip in bytes
  wxVector<wxString>      m_vFiles;
  void                    SelectLineFromMouse( wxPoint ptMouse );

  wxString CalculateSize(unsigned long long ullSize );
  std::list<MyBagLogDataEl>               m_FrameBagList;

  // process stuff
  // return vector of processes currently running in the system
  struct ProcessEntry {
     wxString name;
     long pid;
  };
  void GetProcessList(std::vector<ProcessEntry> &proclist);
  void DisplayProcessList();

  // drive info
  struct DriveInfoEntry
  {
     wxString m_wsDriveLetter;
     wxString m_wsVolumeName;
     wxString m_wsSerialNo;
     bool     m_bIsDVD;
  };
  void GetDriveInfo(std::vector<DriveInfoEntry> &driveInfolist );
  std::vector<DriveInfoEntry> m_DriveInfo;

#if defined ( WANT_FILEHISTORY )
  void OnMruFile( wxCommandEvent & event );
  wxMenu                  *m_fileRecent;          ///< file history menu item
#endif
  std::list<MyZipListElement>               m_zipList;
  unsigned int            m_uiSrcDirLevelDepth;   // max depth of source directory levels
  unsigned int            m_uiZipDirLevelDepth;   // max depth of zip directory levels

  bool Cancelled();
  // protects m_bCancelled
  wxCriticalSection m_critsectWork;
  wxCriticalSection m_critsectTest;
  // was the worker thread cancelled by user?
  bool                    m_bCancelled;
  wxCriticalSection       m_csCancelled;        // protects m_bCancelled

  // Note: this is intended to display progress in main task bar icon for this application
  bool                    m_bHaveAppProgress;
  wxAppProgressIndicator *m_pAppProgress;
  wxAppProgressIndicator* GetAppProgressInd() { return m_pAppProgress; }

  bool RunWizard( bool bIsEditing );

  bool Check4Update( bool bFromUser, wxString& ar_wsCurrentVersion, wxString& ar_wsLatestVersion );

  // make sure we have no duplicate sources
  bool Check4DuplicateSources(  wxString a_wsSources );

#if defined( WANT_SOUND )
  bool CreateSound(wxSound& snd) const;
  wxSound*    m_pSound;
  wxString    m_wsSoundFile;
  wxString    m_wsSoundRes;
#if defined( WANT_MEMORY_SOUND )
  bool        m_bUseMemory;
#endif
#endif

private:    //////////////////////////////////////////////
#if defined( WANT_DBGRPRT )
  void OnCauseException( wxCommandEvent& event );
  void baz(const wxString& s);
  void foo(int n);
#endif
  // Event handlers - these functions should _not_ be virtual)
  void OnExit(wxCommandEvent& event);
  void OnAbout( wxCommandEvent &WXUNUSED(event) );
  void OnMenuItemLogClear( wxCommandEvent& event );
  void OnMenuItemLogShow( wxCommandEvent& event );
  void OnSelJobFile( wxCommandEvent& event );
  void OnStop( wxCommandEvent& event );
  //void OnStart( wxCommandEvent& event );
  void OnIdle( wxIdleEvent &ev );
  void OnTimer( wxTimerEvent& evenet );
  void OnEditJobFile(wxCommandEvent& event);
  void OnDeleteFileCLick(wxCommandEvent& event);
  void OnNewJobFile(wxCommandEvent& event);
  void OnRestoreJob(wxCommandEvent& event);
  void OnRenameFile(wxCommandEvent& event);
  void OnSaveAsFile(wxCommandEvent& event);
  void OnCheck4Update(wxCommandEvent& event);
  void OnOptions( wxCommandEvent& event );
  void OnDumpZipList(wxCommandEvent& event);
  void OnWhenDoneClicked(wxCommandEvent& event);

  // OnUpdateUI handlers
  // Menu
  void OnUpdateUILogWanted(wxUpdateUIEvent& event);
  void OnUpdateUILogClear(wxUpdateUIEvent& event);

  void OnUpdateUiMenuHelp(wxUpdateUIEvent& event);

  void OnUpdateUiMenuBackup(wxUpdateUIEvent& event);
  void OnUpdateUIMenuDump(wxUpdateUIEvent& event );
  void OnUpdateUiMenuEdit(wxUpdateUIEvent& event);
  void OnUpdateUINewJob(wxUpdateUIEvent& event);
  void OnUpdateUiMenuSel(wxUpdateUIEvent& event);
  void OnUpdateUiMenuTest(wxUpdateUIEvent& event);
  void OnUpdateUiMenuRestore(wxUpdateUIEvent& event);
  void OnUpdateUiMenuDelLog(wxUpdateUIEvent& event);
  void OnUpdateUiMenuOpenLog(wxUpdateUIEvent& event);
  void OnUpdateUiMenuOptions(wxUpdateUIEvent& event);
  void OnUpdateUiMenuCrash(wxUpdateUIEvent& event);
  // Toolbar
  void OnUpdateUITbNewJob(wxUpdateUIEvent& event);
  void OnUpdateUITbOpenJob(wxUpdateUIEvent& event);
  void OnUpdateUITbDeleteJob(wxUpdateUIEvent& event);
  void OnUpdateUITbEditJob(wxUpdateUIEvent& event);
  void OnUpdateUITbBackupJob(wxUpdateUIEvent& event);
  void OnUpdateUITbRestoreJob(wxUpdateUIEvent& event);
  void OnUpdateUITbTestJob(wxUpdateUIEvent& event);

  void OnUpdateUiTbStop(wxUpdateUIEvent& event);
  void OnUpdateUiTbHelp(wxUpdateUIEvent& event);

  // Job List text control
  void OnJobsSelectLine(wxMouseEvent& event);
  void SelectJobLine( long lJobSelLineCaretPos );
  bool SelectJobLineByText( wxString wsText );
  void OnJobListChar(wxKeyEvent& event);

  // toolbar
  void ToggleHorizText();

  bool            m_bJobSelected;
  long            m_lSelectedLine;              // currently selected line in the job list text ctrl
                                                // -1 if no  line selected
  long            m_lNumberOfLines;
  long            m_lJobSelLineAbsPos;          // absolute position of click in the buffer string for HitTest
  bool            m_bCurrentConfigNeedsSave;    // dirty = true, clean = false
  int             m_iNIdleCalls;
  bool            m_bSplitterSetDone;

  void UpdateSummary( wxString wsJobFileName, wxString wsJobFilePath,
    wxString wsTimeBreakout );

  // worker thread
  void OnStartWorker(wxCommandEvent& event);
  // test & restore thread
  void OnTestJob(wxCommandEvent& event);

#if defined( EXIT_EVENT )
  void OnWorkerEventDone(wxThreadEvent& event);
#endif
#if defined ( RESTORE_EXIT_EVENT )
  void OnRestoreEventDone(wxThreadEvent& event);
#endif
#if defined( LOG_EVENT )
  void OnWorkerEventLogMsg( wxThreadEvent& event);
#endif
  void OnMenuItemClearLogWindow( wxCommandEvent& WXUNUSED(event) );
  void OnMenuItemDeleteLogFiles( wxCommandEvent& WXUNUSED( event ) );
  void OnMenuItemOpenLogFiles(wxCommandEvent& WXUNUSED( event ));

  // was the worker thread cancelled by user?
  bool                                      m_bStopScan;
  bool                                      m_bFoundHelpFile;

  // did we have any errors - needed for email feedback
  bool                                      m_bHadError;
  // were any files skipped
  bool                                      m_bSkippedFiles;
  unsigned long long                        m_ullTicks;

  // Any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif    // _WX_BUP_FRAME_H_
// ------------------------------- eof ------------------------------
