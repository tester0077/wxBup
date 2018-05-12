/*-----------------------------------------------------------------
 * Name:        wxBupTestRestoreThreadh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_TEST_RESTORE_THREAD_H
#define _WX_BUP_TEST_RESTORE_THREAD_H
// ------------------------------------------------------------------
//#include "wx/dynarray.h"

#include "wxBuph.h"
#include "wxBupFrameh.h"
#include "wxBupBagLogListh.h"


// ------------------------------------------------------------------

class MyRestoreThread : public wxThread
{
public:
  MyRestoreThread(MyFrame *frame);

  // thread execution starts here
  virtual void *Entry();

  // called when the thread exits - whether it terminates normally or is
  // stopped with Delete() (but not when it is Kill()ed!)
  virtual void OnExit();

public:
  MyFrame               *m_frame;
  unsigned long         m_ulTotalFilesToCheck;
  unsigned long         m_ulTotalFiles;
  unsigned long         m_ulTotalFilesChecked;
  unsigned long         m_ulCheckedFilesGood;
  unsigned long         m_ulCheckedFilesFailed;
  bool                  m_bRestoring;
  wxString              m_wsBagLogPath;

  bool RemoveTestRestoreDirAndFiles( wxString wsDir );
  void RestoreOrTestOneBag( wxString wsBagLog );
  bool CompareZippedFile( wxString a_wsOrigFile, wxString a_wsTestFile );
  bool Compare2Files( wxString a_wsOrigFile, wxString a_wsTestFile );
  bool Restore1ZippedFile(const wxString& aZipFile, const wxString& aTargetDir);
  bool Restore1File( wxString a_wsOrigFile, wxString a_wsDestFile );
  bool ExtractZipFiles(const wxString& aZipFile, const wxString& aTargetDir);

  std::list<MyBagLogDataEl>               m_ThreadBagList;
  void SetThreadNextBagData( unsigned long ulTotalFilesToCheck, wxString wsBagLogPath );

  bool TestRestoreCountFiles( wxString wsPath, wxString mask, wxVector<wxString>& files,
    unsigned long& a_ulDirs );
};
#endif  // _WX_BUP_TEST_RESTORE_THREAD_H
// ------------------------------- eof ---------------------------
