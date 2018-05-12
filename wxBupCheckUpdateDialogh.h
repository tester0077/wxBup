/*-----------------------------------------------------------------
 * Name:        wxBupCheckUpdateDialogh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_CHECKUPDATE_DLG_H
#define _WXBUP_CHECKUPDATE_DLG_H

// ------------------------------------------------------------------
#include "wxBupBaseFrame.h"
// ------------------------------------------------------------------
class MyBupUpdateDlg : public MyBupUpdateDlgBase
{
public:
  MyBupUpdateDlg( wxWindow* parent );

  wxString            m_wsDlgAppName;   // set once dialog is created
};

#endif  // _WXBUP_CHECKUPDATE_DLG_H
// ------------------------------- eof ---------------------------
