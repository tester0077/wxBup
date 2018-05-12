//-----------------------------------------------------------------------------
// Name:        wxBupOptionsh.h
// Purpose:     
// Copyright:   
// Licence:     wxWidgets licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#ifndef _WXID_OPTIONS_H_
#define _WXID_OPTIONS_H_
//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include <wx/frame.h>
#include <wx/fontdlg.h>
#include <wx/dnd.h>
#include <wx/fileconf.h>
#include <wx/docview.h>

#include "wxBupBaseFrame.h"
#include "wxBupBagSizes.h"
// ------------------------------------------------------------------
class MyOptionsDialog : public MyBaseOptionsDialog
{
public:
  MyOptionsDialog( wxWindow* parent );
  bool TransferDataToWindow( void );
  bool TransferDataFromWindow( void );
  void OnOptDlgLog_SelLogFilePathClick( wxCommandEvent &event );
  void OnOptDlgLog_DefPathClick( wxCommandEvent& WXUNUSED(event) );
  void OnOptDlgLog_Log2FileClick( wxCommandEvent& WXUNUSED(event) );
  void OnOptDlgLog_ClearLogAtStart(wxCommandEvent& event);
  void OnOptDlgGen_UseDefaultTempDirClick( wxCommandEvent& event );
  void OnOptDlgSnd_WantSound(wxCommandEvent& event);

  iniPrefs_t          m_iniPrefs;
  EBupBagSizeTypes    m_eBagSizes;      ///< enumeration of all 'bag' sizes supported
  wxString            m_wsDlgAppName;   // set once dialog is created
};

#endif  // _WXID_OPTIONS_H_
// ------------------------------- eof ------------------------------
