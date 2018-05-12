/*-----------------------------------------------------------------
 * Name:        wxBupWorkerh.h
 * Purpose:     header for main backup/zip thread
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_WORKER_H
#define _WX_BUP_WORKER_H
#include "wxBuph.h"
#include "wxBupFrameh.h"
// ------------------------------------------------------------------
// worker thread
// ----------------------------------------------------------------------------

class MyWorkerThread : public wxThread
{
public:
  MyWorkerThread( MyFrame *frame );

  // thread execution starts here
  virtual void *Entry();

  // called when the thread exits - whether it terminates normally or is
  // stopped with Delete() (but not when it is Kill()ed!)
  void OnExit();

  MyFrame *m_frame;
  wxString                m_wsCurRootPath;        // since we can have multiple sources, this
                                                  // is expected to contain the current root
#if defined( WANT_BACKUP_TYPE )
  bool                    m_bFullBackup;          // true if full backup requested, flase otherwise
#endif

  bool GetFileSize( wxString wsFileFullPath, unsigned long long& ullFileSize );
  bool ZipNBackupFile( wxString wsSrcFile, wxString wsDestRoot, size_t i );
  bool SplitFilesIntoBags();
  bool MyFileMove( const wxString &a_rwsSrc, wxString &a_rwsDest );
  bool WorkerCountFiles( wxString wsPath, wxString mask, wxVector<wxString>& files,
    unsigned long& a_ulDirs );
  bool MakeBackupDestnPath( wxString wsSourceFile, wxString wsDestRoot, wxString &wsDestPath );
};

#endif  // _WX_BUP_WORKER_H
// ------------------------------- eof ---------------------------
