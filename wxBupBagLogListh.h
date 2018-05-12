/*-----------------------------------------------------------------
 * Name:        wxBupBagLogListh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_BAGLOG_LIST_H
#define _WXBUP_BAGLOG_LIST_H

// ------------------------------------------------------------------
#include "wx/dynarray.h"
#include "wxBupPreProcDefsh.h" // needs to be first
// ------------------------------------------------------------------

class MyBagLogDataEl
{
public:
  unsigned long m_ulTotalFilesToCheck;
  wxString      m_wsBagLogPath;
  
  MyBagLogDataEl(
    unsigned long long  ulTotalFilesToCheck,
    wxString            wsBagLogPath ) 
  {                 // initialize all members
    m_ulTotalFilesToCheck = ulTotalFilesToCheck;
    m_wsBagLogPath        = wsBagLogPath;
  };
};

#endif  // _WXBUP_BAGLOG_LIST_H
// ------------------------------- eof ---------------------------

