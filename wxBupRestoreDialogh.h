/*-----------------------------------------------------------------
 * Name:        wxBupRestoreDialogh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_RESTORE_DIALOG_H
#define _WXBUP_RESTORE_DIALOG_H

// ------------------------------------------------------------------
#include "wxBupFrameh.h" 

class wxBupRestoreDlg : public wxBupRestoreDlgBase
{
public:
  wxBupRestoreDlg( MyFrame* parent );
  virtual ~wxBupRestoreDlg();

  MyFrame                 *m_pParent;
  iniPrefs_t              m_iniPrefs;
  bool                    m_bIsRestore;
  bool                    m_bIsTest;

private:
  bool TransferDataToWindow( void );
  bool TransferDataFromWindow( void );
};

#endif  // _WXBUP_RESTORE_DIALOG_H
// ------------------------------- eof ---------------------------
