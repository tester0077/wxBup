/*-----------------------------------------------------------------
 * Name:        wxBupFileLogh.h
 * Purpose:     Setup logging to a file
 *              Adapted from code at:
 * from: https://forums.wxwidgets.org/viewtopic.php?f=1&t=42551&p=172671&hilit=settimestamp#p172671
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_FILE_LOG_H
#define _WXBUP_FILE_LOG_H
#include <wx/log.h>
#include <wx/datetime.h>
#include <wx/ffile.h>

// ------------------------------------------------------------------
class MyFileLog : public wxLog
{   
public:       
  MyFileLog(const wxString& fileName);

  ~MyFileLog();

  virtual void Flush();
  bool IsLogFileOK() const { return m_logFile.IsOpened() && !m_logFile.Error(); }
  wxString GetLogFileName() const { return m_logFile.GetName(); } 
  void OpenLog( const wxString& fileName );
  void CloseLog();

protected:
  virtual void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info);
private:
  wxFFile  m_logFile;
};

#endif  // _WXBUP_FILE_LOG_H
// ------------------------------- eof ---------------------------

