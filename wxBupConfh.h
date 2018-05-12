//////////////////////////////////////////////////////////////////
// Name:        wxBupConfh.h
// Purpose:     wxWidgets based backup & zip utility - Config header
// Author:      A. Wiegert
// Modified by:
// Created:
// Licence:     wxWidgets license
//////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !defined( __WX_P_CONF_H )
#define __WX_P_CONF_H

// Pref version
#define CURRENT_PREF_VERSION  3

enum entryType 
{ eBool, eLong, eString, eFloat };

typedef struct
{
  bool  bVal;
  long  lVal;
  wxString wsVal;
  float fVal;
} iniData;


typedef struct entry_
{
  wxString  wsPathStr;
  wxString  wsKeyStr;
  entryType eType;
  iniData   dataDefault;
  iniData   dataCurrent;
} entry;

// ------------------------------------------------------------------ 
/* Note: these entries define the sequence in iniPrefs
 */
enum iniEnts
{
  IE_LAST_DIR = 0,              // Last job dir

  IE_FRAME_X,
  IE_FRAME_Y,
  IE_FRAME_W,
  IE_FRAME_H,
  IE_SHOW_LOG,                  // show the log window at start
  IE_SASH_POS,

  IE_OUT_FILENAME,
#if defined( WANT_GRID )
  IE_GRID_COL_WIDTHS,           // mail grid col widths w1|w2|w3.. |wn
  IE_GRID_COL_LABELS,           // mail grid col Labels l1|l2|l3.. |ln
#endif
  IE_LOG_FILE_WANTED,           // write log to a file
  IE_LOG_VERBOSITY,             // level of log detail
  IE_LOG_NEW_LOG,               // when to start new log: ignore - do nothing, every job or every day
  IE_LOG_DIR_PATH,              // path to log file directory
  IE_USE_LOG_DEF_DIR,           // use the default log directory
  IE_LAST_OPTION_TAB,           // last used tab in options dialog
  IE_LOG_CLEAR_AT_START,        // clear the log window at start of a job
  IE_LOG_CLEAR_AT_TEST,         // clear the log window at start of a test
  
  IE_LAST_JOB_FILE,             // last selected job file with path
  IE_LAST_JOB_LOAD_AT_START,    // load the last used job file at the start
  IE_EDIT_JOB_DLG_H,            // edit dialog height
  IE_EDIT_JOB_DLG_W,            // edit dialog width
  IE_EDIT_JOB_DLG_SASH,         // edit dialog sash position - in files panel

  IE_BAG_4_7_RESERVE,           // reserve space for 4.7 GB DVDs
  IE_BAG_FLOPPY_RESERVE,        // reserve space for floppies

  IE_LAST_RESTORE_SOURCE,       // directory with the data to be restored or tested
  IE_LAST_RESTORE_TARGET,       // directory to hold the restored data
  IE_CLEAR_TARGET,              // clear the target directory at start
  IE_COMPARE_TARGET_ORIG,       // compare the target directory with original directory
  IE_SUMMARY_UPDATE_INT,        // interval (in secs) for summary update
  IE_BASE_DIR,                  // string to use for the base directory
  IE_JOB_SASH_POS,              // position of vertical sash in job pane of main frame
  IE_USE_GLOBAL_DEFAULT,        // use the default global temporary directory
  IE_GLOBAL_TEMP_DIR,           // Path to the global temp dir
  IE_TOOLBAR_TEXT,              // do we want tool bar text as well as icons?
  IE_EXCLUDE_FILES_LIST,        // string listing all file exclude patterns "*.??|*.bak ... )
  IE_EXCLUDE_DIRS_LIST,         // string listing all dirs exclude patterns "*.??|*.bak ... )
  IE_MOVE_AS_IS_LIST,           // string listing all 'move only' patterns "*.jpg|*.zip ... )
  IE_DEFAULT_BAG_SIZE,          // default bag size to use for new jobs - as enum
  IE_DELETE_DEST_WARN,          // do we want a warning if destination contains data
  IE_CHECK4NEW_VERSION,         // check for new version at startup
  IE_EDIT_SHOWFULLPATH,         // show the full path in job edit dialog
  IE_SOUND_FEEDBACK,            // sound feedback
  IE_WHEN_DONE,                 // what to do when done
  IE_WHEN_DONE_ENABLED,         // is the 'when done' action enabled
  IE_SOUND_FILE_ERROR,          // sound file to play on fatal error
  IE_SOUND_FILE_WARN,           // sound file to play on warning - non-fatal error
  IE_SOUND_FILE_DONE,           // sound file to play on completion
                                // app wide email defaults
  IE_EMAIL_DFLT_USE,            // use defaults
  IE_EMAIL_DFLT_SERVER,         // server
  IE_EMAIL_DFLT_FROM,           // Addressee
  IE_EMAIL_DFLT_TO,             // Sender email address
  IE_EMAIL_DFLT_SUBJECT,        // subject
  
  IE_MAX            // keep as last - determines size of struct iniPrefs
};
typedef struct _iniPrefs_t
{
  long      lPrefVer;   // magic number
  entry     data[IE_MAX];
} iniPrefs_t; 

#endif   // __WX_P_CONF_H

// ---------------------- eof ---------------------------------------
