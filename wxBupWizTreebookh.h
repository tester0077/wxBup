/*-----------------------------------------------------------------
 * Name:        wxBupWizTreebookh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_TREEBOOK_H
#define _WX_BUP_TREEBOOK_H

#include "wxBupFrameh.h" 

#include "wxBupConfh.h"
// ------------------------------------------------------------------
class MyFrame;

class MyBupJobDataDlg : public wxBupJobDataDlgBase
{
struct ProcessEntry 
{
   wxString name;
   long pid;
};

public:
  MyBupJobDataDlg( MyFrame* parent, wxString wsConfigDir, 
    wxString wsJobFile, bool bIsEditing );
  virtual ~MyBupJobDataDlg();
  wxString                m_wsConfigDir;
  wxString                m_wsJobFile;  ///< job file path & name
  MyFrame                 *m_pParent;
  iniPrefs_t              m_iniPrefs;
  jobPrefs_t              m_jobData;
  bool                    m_bIsEditing;
  bool                    m_bHaveSourceListSelection; // set to true if we have a selection
  EBupBagSizeTypes        m_eBagSizes;  ///< enumeration of all 'bag' sizes supported
  bool                    m_bFoundClient;
  void OnBeginDrag( wxTreeEvent& event );

  // process stuff
  // return vector of processes currently running in the system
  void GetProcessList(std::vector<ProcessEntry> &proclist);

private:
  bool TransferDataToWindow( void );
  bool TransferDataFromWindow( void );

  void OnWizSrcDel( wxCommandEvent& event );
  void OnWizDestnDirChanged( wxFileDirPickerEvent& event );
  void OnDestnDriveIdChanged(wxCommandEvent& event);
  void SetDestnDriveInfo( wxString a_wsVolName, wxString a_wsSerNo );
  bool CheckDestDriveInfo();
  void OnSourceTreeKeyDown(wxTreeEvent& event);
  
  void OnBackupTypeClicked(wxCommandEvent& event);
  void UpdateBackupType( int iButton );
  void OnJobOptionEmailWanted(wxCommandEvent& event);

  // OnUpdateUI handlers for the wizard
  void OnUpdateUIButtonSrcDel(wxUpdateUIEvent& event );

  void OnTreebookWizPageChanging(wxBookCtrlEvent& event);
  void OnTreebookWizPageChanged(wxBookCtrlEvent& event);

  void OnSourceListSelect( wxCommandEvent& event );
  void OnTreebookJobPageChanged(wxTreebookEvent& event);

  void OnNotificationSendTestMessage(wxCommandEvent& event);

  bool Check4Overlap( wxString &wsSrcDir, wxString DestDir );
  // Destination media info
  bool GetMediaInfo( wxString wsPathName, bool &bIsFixed, 
    bool &bIsDVD, wxString &wsVolName,
    wxString &wsSerNo );
};

#endif  // _WX_BUP_TREEBOOK_H
// ------------------------------- eof ---------------------------

