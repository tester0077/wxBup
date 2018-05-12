/*-----------------------------------------------------------------
 * Name:        wxBupWizTreebook.cpp
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *---------------------------------------------------------------- */

/*----------------------------------------------------------------
 * Standard wxWidgets headers
 *---------------------------------------------------------------- */
// Note __VISUALC__ is defined by wxWidgets, not by MSVC IDE
// and thus won't be defined until some wxWidgets headers are included
#if defined( _MSC_VER )
#  if defined ( _DEBUG )
 // this statement NEEDS to go BEFORE all headers
#    define _CRTDBG_MAP_ALLOC
#  endif
#endif
#include "wxBupPreProcDefsh.h"   // needs to be first

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

/* For all others, include the necessary headers
 * (this file is usually all you need because it
 * includes almost all "standard" wxWidgets headers) */
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
// ------------------------------------------------------------------
#include "wxBupFrameh.h" // needs to be first
#include "wxBupWizTreebookh.h"

#include "wxBupWizSrcDnDh.h"
#include <wx/tokenzr.h>
#if defined( _MSC_VER ) // only good for Windows
#include <TlHelp32.h>
#include <Shlwapi.h>
#endif
#include <wx/dir.h>
//#include <VersionHelpers.h> // only available under MSVC 2015
#if defined( WANT_DIRTY_VALIDATE )
#include "wxBupJobValidateh.h"
#endif
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to go AFTER all headers
#include <crtdbg.h>
#  ifdef _DEBUG
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  endif
#endif

// ------------------------------------------------------------------
MyBupJobDataDlg::MyBupJobDataDlg( MyFrame* parent,
  wxString wsConfigDir, wxString wsJobFile, bool bIsEditing )
    : wxBupJobDataDlgBase(parent )
{
  GetGenericDirCtrlSrcPicker()->ShowHidden( true );
  m_wsConfigDir   = wsConfigDir;
  m_wsJobFile     = wsJobFile;    // job file path & name
  m_pParent       = parent;
  m_bIsEditing    = bIsEditing;
  m_bHaveSourceListSelection = false;
  m_bFoundClient = false;
  // show the first page - Job name
  m_treebookJob->SetSelection( 0 );
  // and give proper field the focus
  m_textCtrlTaskName->SetFocus();
  // associate drop targets with the controls
  // seems to cause a memory leak of 56 bytes, but all my efforts at deleting
  // the memory allocated here have made things worse causing
  // a crash on exit - writing past heap buffer or something like it
  // more test: I can remove this allocation & still get the leak,
  // though the dropped file wont be accepted without this.
  // conclusion: it must be a different allocation and this one is taken
  // care of somewhere
  m_listBoxSource->SetDropTarget( new MyDnDFile( m_listBoxSource ) );

  // set the default destination directory to the one last used
  m_dirPickerDestinationDir->SetInitialDirectory(
    m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );

  // fill the 'bag' size choice box from the list of enums
  // ignore the 'unknown' case
  // clear out old data, just in case
  m_choiceWizOutputBagSize->Clear();
  int iSel = 0;

  // limit the list entries to those implemented for now
  for ( int i = EBupBagSizeTypes::eBagSize_NoSplit; i <= EBupBagSizeTypes::eBagSize_4_7GB
    /*EBupBagSizeTypes::eBagSize_25GB*/; i++ )
  {
    if ( m_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal.IsSameAs( EBupBagSizeTypes::AsWxName(i) ) )
      iSel = i;
    m_choiceWizOutputBagSize->Append( EBupBagSizeTypes::AsWxName(i) );
  }
  this->Connect(wxEVT_TREEBOOK_PAGE_CHANGING, wxBookCtrlEventHandler(MyBupJobDataDlg::OnTreebookWizPageChanging), NULL, this);
  this->Connect(wxEVT_TREEBOOK_PAGE_CHANGED, wxBookCtrlEventHandler(MyBupJobDataDlg::OnTreebookWizPageChanged), NULL, this);
  Fit();
}

// ------------------------------------------------------------------

MyBupJobDataDlg::~MyBupJobDataDlg()
{
  this->Disconnect(wxEVT_TREEBOOK_PAGE_CHANGING, wxBookCtrlEventHandler(MyBupJobDataDlg::OnTreebookWizPageChanging), NULL, this);
  this->Disconnect(wxEVT_TREEBOOK_PAGE_CHANGED, wxBookCtrlEventHandler(MyBupJobDataDlg::OnTreebookWizPageChanged), NULL, this);
}

// ------------------------------------------------------------------

// Called automagically by InitDialog() to set up the data
// prior to the wizard being shown
// Note: since this overrides a predefined function, it MUST have
// the name and return type as given here
bool MyBupJobDataDlg::TransferDataToWindow( void )
{
  wxStringTokenizer tokenizer;
  wxString wsT;

  // things that need to be done in any case
  // Properties page

  // fill the three combo boxes in any case
  // these exclusion masks can contain several mask strings, separated by ';'
  // so we need a different separator for each line in the combobox
  wxString wsToken;

  m_comboBoxWizFileExclusions->Clear();
  tokenizer.SetString( m_iniPrefs.data[IE_EXCLUDE_FILES_LIST].dataCurrent.wsVal, "|");
  while ( tokenizer.HasMoreTokens() )
  {
    wsToken = tokenizer.GetNextToken();
    // process token here
    m_comboBoxWizFileExclusions->Append( wsToken );
  }
  m_comboBoxWizDirExclusions->Clear();
  tokenizer.SetString( m_iniPrefs.data[IE_EXCLUDE_DIRS_LIST].dataCurrent.wsVal, "|");
  while ( tokenizer.HasMoreTokens() )
  {
    wsToken = tokenizer.GetNextToken();
    // process token here
    m_comboBoxWizFileExclusions->Append( wsToken );
  }
  // see comments above; they also apply to the 'move as is' masks
  m_comboBoxMoveAsIs->Clear();
  tokenizer.SetString( m_iniPrefs.data[IE_MOVE_AS_IS_LIST].dataCurrent.wsVal, "|");
  while ( tokenizer.HasMoreTokens() )
  {
    wsToken = tokenizer.GetNextToken();
    // process token here
    m_comboBoxMoveAsIs->Append( wsToken );
  }
  // explain the difference between backup type available
  m_staticTextBackupTypeExplain->SetLabel(
    _("For 'Full' backup, ALL Files are backed up no matter the state ") +
    _("of the 'archive' bit. All 'archive' bits are reset\n" ) +
    _("For 'differential' backup, only files with the 'archive' bit set ") +
    _("are backed up. The 'archive' bit is NOT changed" ) );
  // setup the page labels
  if ( m_bIsEditing )
  {
    // set the dialog panel titles
    m_staticTextTaskGeneral->SetLabel( _("Editing - General") );
    m_staticTextTaskFiles->SetLabel( _("Editing - Files") );
    m_staticTextTaskProperties->SetLabel( _("Editing - Properties") );
    m_staticTextTaskFilters->SetLabel( _("Editing - Filters") );
    m_staticTextTaskZipEncrypt->SetLabel( _("Editing - ZipEncrypt") );
    m_staticTextInterferingTasks->SetLabel( _("Editing - Interfering Tasks") );
    m_staticTextNotification->SetLabel( _("Editing - Notification") );
    m_staticTextWhenDone->SetLabel( _("Editing - When Done") );
  }
  else
  {
    m_staticTextTaskGeneral->SetLabel( _("New Job - General") );
    m_staticTextTaskFiles->SetLabel( _("New Job - Files") );
    m_staticTextTaskProperties->SetLabel( _("New Job - Properties") );
    m_staticTextTaskFilters->SetLabel( _("New Job - Filters") );
    m_staticTextTaskZipEncrypt->SetLabel( _("New Job - ZipEncrypt") );
    m_staticTextInterferingTasks->SetLabel( _("New Job - Interfering Tasks") );
    m_staticTextNotification->SetLabel( _("New Job - Notification") );
    m_staticTextWhenDone->SetLabel( _("New Job - When Done") );
    // set the initial dest directory
    m_dirPickerDestinationDir->SetInitialDirectory( ::wxGetUserHome() );
  }
  if ( m_bIsEditing )
  {
    // General page // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // get the file name
    wxFileName wfnLastFile( m_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal );
    wxString wsLastFileName = wfnLastFile.GetFullName();
    wxString wsJobName = wfnLastFile.GetName();
    m_textCtrlTaskName->SetValue( wsJobName );
    m_checkBoxJobEnabled->SetValue(
      m_jobData.data[IEJ_JOB_ENABLED].dataCurrent.bVal );
    m_checkBoxJobIncludeSubDirs->SetValue(
      m_jobData.data[IEJ_INCL_SUB_DIRS].dataCurrent.bVal);
    m_textCtrlJobComments->WriteText(
      m_jobData.data[IEJ_JOB_COMMENT].dataCurrent.wsVal );
    // backup type
    bool bIsFull =
      m_jobData.data[IEJ_BACKUP_TYPE].dataCurrent.wsVal.MakeUpper().IsSameAs( _T("FULL") );
    int iSel = 0;
    if ( ! bIsFull  )
      iSel = 1;
    m_radioBoxBackupType->SetSelection( iSel );
    UpdateBackupType( iSel );

    // Files page // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // clear the destn drive path explanation  for now
    m_staticTextDestnNotes->SetLabel( wxEmptyString );
    // what if the drive or path is no longer valid
    // that is handled when we change to the 'Files' page
    m_dirPickerDestinationDir->SetPath(
      m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );

    // first, split the concatenated source paths
    // Note: we DON'T handle the case where files which fit one of the
    // exclusion masks are specifically added.
    // they will be backed up in any case.
    m_listBoxSource->Clear();  // in case we call it again
#if defined( _DEBUG )
    int i = m_listBoxSource->GetCount();
#endif

    wxStringTokenizer tokenizer(
      m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal, _T(";") );
    while ( tokenizer.HasMoreTokens() )
    {
      wsToken = tokenizer.GetNextToken();
      // process token here
      if( !wsToken.IsEmpty() )
      m_listBoxSource->Append( wsToken );
    }
#if defined( _DEBUG )
    i = m_listBoxSource->GetCount();
#endif

    // Properties page  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    m_textCtrlWizZipChunkBaseFileName->SetValue(
      m_jobData.data[IEJ_BAG_NAME_BASE].dataCurrent.wsVal );
    // bag size combo is set in ctor, just select appropriate value here
    // make up for the 'unknown' case
    m_choiceWizOutputBagSize->SetSelection(
      EBupBagSizeTypes::AsEnum(
        m_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal ) - 1 );
    // include empty directories in backup?
    // some app such as the SVN repo contain lots of empty directories
    // not sure if they are really needed  or if they are recreated
    // if a set of data was to be restored
    m_checkBoxIncludeEmptyDirs->SetValue(
      m_jobData.data[IEJ_INCL_MT_DIRS].dataCurrent.bVal );

    // Filters page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // select the item in the 'File' exclusion list, if any
    if ( m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxWizFileExclusions->SetValue( wxEmptyString );
    }
    else
    {
      iSel = m_comboBoxWizFileExclusions->FindString(
         m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal );
      if( iSel == wxNOT_FOUND )
      {
        // display the value
        m_comboBoxWizFileExclusions->SetValue(
          m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal );
        // and add it to the top of the list
        m_comboBoxWizFileExclusions->Insert(
          m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal, 0 );
      }
      else
      {
        // is this enough or do I need to copy the value to the text box??
        m_comboBoxWizFileExclusions->SetSelection( iSel );
      }
    }
    // select the item in the 'Dirs' exclusion list, if any
    if ( m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxWizDirExclusions->SetValue( wxEmptyString );
    }
    else
    {
      iSel = m_comboBoxWizDirExclusions->FindString(
         m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal );
      if( iSel == wxNOT_FOUND )
      {
        // display the value
        m_comboBoxWizDirExclusions->SetValue(
          m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal );
        // and add it to the top of the list
        m_comboBoxWizDirExclusions->Insert(
          m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal, 0 );
      }
      else
      {
        // is this enough or do I need to copy the value to the text box??
        m_comboBoxWizDirExclusions->SetSelection( iSel );
      }
    }
    // same for the 'move as is' list
    // select the item in the exclusion list, if any
    if ( m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxMoveAsIs->SetValue( wxEmptyString );
    }
    else
    {
      iSel = m_comboBoxMoveAsIs->FindString(
        m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal );
      if( iSel == wxNOT_FOUND )
      {
        // display the value
        m_comboBoxMoveAsIs->SetValue(
          m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal );
        // and add it to the top of the list
        m_comboBoxMoveAsIs->Insert(
          m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal, 0 );
      }
      else
      {
        // is this enough or do I need to copy the value to the text box??
        m_comboBoxMoveAsIs->SetSelection( iSel );
      }
    }
    // ZIP page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // 0 = Copy only, 1 = Zip
    m_radioBoxWizZipOrMove->SetSelection(
      !m_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal );
    // Interfering Tasks page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // nothing as yet
    // Email Notification  page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    m_checkEmailWanted->SetValue( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_checkBoxNotifyOnError->SetValue( m_jobData.data[IEJ_EMAIL_ERROR_ONLY].dataCurrent.bVal );
    m_checkBoxAddLogFile->SetValue( m_jobData.data[IEJ_EMAIL_ADD_LOG].dataCurrent.bVal );

    m_textCtrlNotificationServer->SetValue( m_jobData.data[IEJ_EMAIL_SMTP].dataCurrent.wsVal );
    m_textCtrlNotificationFrom->SetValue( m_jobData.data[IEJ_EMAIL_FROM].dataCurrent.wsVal );
    m_textCtrlNotificationTo->SetValue( m_jobData.data[IEJ_EMAIL_TO].dataCurrent.wsVal );
    wsT.Printf( m_jobData.data[IEJ_EMAIL_SUBJECT].dataCurrent.wsVal,
      giMajorVersion, giMinorVersion, giBuildNumber, wxGetHostName() );
    m_textCtrlNotificationSubject->SetValue( wsT );

    m_checkBoxNotifyOnError->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_checkBoxAddLogFile->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_textCtrlNotificationServer->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_textCtrlNotificationFrom->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_textCtrlNotificationTo->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    m_textCtrlNotificationSubject->Enable( m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal );
    // When Done page      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // verify backup?
    m_checkBoxRunVerify->SetValue( m_jobData.data[IEJ_VERIFY_JOB].dataCurrent.bVal );
  }
  else    // for new job files - use defaults
  {
    wxString wsT;
    // general page
    wsT = g_jobData.data[IEJ_JOB_NAME].dataDefault.wsVal;
    m_textCtrlTaskName->SetValue( wsT );
    // files page

    // properties page
    // base name
    wsT = g_jobData.data[IEJ_BAG_NAME_BASE].dataDefault.wsVal;
    m_textCtrlWizZipChunkBaseFileName->SetValue( wsT );
    m_choiceWizOutputBagSize->SetSelection(
      g_iniPrefs.data[IE_DEFAULT_BAG_SIZE].dataCurrent.lVal );
    // include empty dirs
    m_checkBoxIncludeEmptyDirs->SetValue(
      g_jobData.data[IEJ_INCL_MT_DIRS].dataDefault.bVal );

    // filters page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // select the top most 'File' entry - for now
    if ( m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxWizFileExclusions->SetValue( wxEmptyString );
    }
    else
    {
      m_comboBoxWizFileExclusions->SetSelection( 0 );
    }
    // select the top most 'Dir' entry - for now
    if ( m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxWizDirExclusions->SetValue( wxEmptyString );
    }
    else
    {
      m_comboBoxWizDirExclusions->SetSelection( 0 );
    }
    if ( m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal.IsEmpty() )
    {
      m_comboBoxMoveAsIs->SetValue( wxEmptyString );
    }
    else
    {
      m_comboBoxMoveAsIs->SetSelection( 0 );
    }
    // zip page
    bool bCopyOnly = g_jobData.data[IEJ_COPY_ONLY_WANTED].dataDefault.bVal;
    m_radioBoxWizZipOrMove->SetSelection( bCopyOnly ? 0 : 1 );

    // interfering tasks page
    // check boxes set before the dialog is opened
  }
  long lIndex = 1l;
  wxPlatformInfo wpi;
  wxOperatingSystemId woiTest = wpi.GetOperatingSystemId();
  int iOsMajor = wpi.GetOSMajorVersion();
  wxArchitecture warch = wpi.GetArchitecture();
  #if defined( _MSC_VER ) // only good for Windows
  //
  // Note mosBackup can find Thunderbird on Hummer, Black-10, but finds none on Leno-10 because there isn't any
  // using regedit, I can find (on Hummer) thunderbird at
  // HKLM\Software\RegisteredApplications -> Software\Client\Mail\Mozilla\Thunderbird\Capabilities
  // HKLM\SOFTWARE\WOW6432Node\Classes\CLSID\......\LocalServer32 ->
  //               c:\Program Files (x86)\Mozilla Thunderbird\thunderbird.exe /MAPIStartup
  // HKLM\SOFTWARE\Wow6432Node\Clients\Mail\Mozilla Thunderbird
  // lots under news as well
  // HKLM\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersiion\App Paths\thunderbird.exe
  // also uninstall ....
  // HKLM\SOFTWARE\WOW6432Node\Mozilla\Mozilla Thunderbird
  //                                   \ lots os other Mozilla stuff - likely Moz options etc
  // HKLM\Software\WOW6432Node\RegisteredApplications -> Software\Client\Mail\Mozilla\Thunderbird\Capabilities

  // HKU\S-----\SOFTWARE\Client\Mail
  // also under eCOSM - Mailwasher stuff
  // all on Hummer, where no other user has any mail client registered
  //SOFTWARE\mozilla\Mozilla Thunderbird
  // preliminary work for  an E-Mail page
  m_textCtrlWizEmailClients->Clear();
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
  {
    m_textCtrlWizEmailClients->AppendText( _("Registered E-mail Clients\n") );
    wxRegKey key( "HKLM\\SOFTWARE\\Clients\\Mail"  );
    if ( key.Exists()  )
    {
      key.Open( wxRegKey::Read );
      bool bIsOpen = key.IsOpened();
      if (  key.IsOpened() )
      {
        // Get the number of subkeys and enumerate them.
        size_t subkeys;
        key.GetKeyInfo(&subkeys, NULL, NULL, NULL);
        wxString key_name;
        {
          key.GetFirstKey(key_name, lIndex );
          for(size_t i = 0; i < subkeys; i++)
          {
            wxString wsTT;
            wsTT.Printf( _("Sub key %d: %s\n"), i, key_name );
            m_textCtrlWizEmailClients->AppendText( wsTT );
            key.GetNextKey(key_name, lIndex );
          }
        }
        key.Close();
      }
    }
  }
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
  {
    m_textCtrlWizEmailClients->AppendText( _("\nRegistered E-mail Clients - WOW6432\n") );
    wxRegKey key2( wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Clients\\MAIL\\" );
    if ( key2.Exists()  )
    {
      key2.Open( wxRegKey::Read );
      bool bIsOpen = key2.IsOpened();
      // Read the value back.
      if (  key2.IsOpened() )
      {
        // Get the number of subkeys and enumerate them.
        size_t subkeys2;
        key2.GetKeyInfo(&subkeys2, NULL, NULL, NULL);
        wxString key_name2;
        key2.GetFirstKey(key_name2, lIndex );
        for(size_t i = 0; i < subkeys2; i++)
        {
          wxString wsTT;
          wsTT.Printf( _("Sub key: %s\n"), key_name2 );
          m_textCtrlWizEmailClients->AppendText( wsTT );
          key2.GetNextKey(key_name2, lIndex );
        }
        key2.Close();
      }
    }
  }
  wxString wsTT;
  m_bFoundClient = false;
  if ( g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal > 4 )
  {
    m_textCtrlWizEmailClients->AppendText( _("\nCurrent User E-mail Clients\n") );
    // subkeys 0, values 4, default not set
    //wxRegKey key3( wxRegKey::HKCU,
    //  "SOFTWARE\\Clients\\StartMenuInternet\\FIREFOX.EXE\\InstallInfo" );
    // subkeys: 2 , value: 0, default not set
    //wxRegKey key3( wxRegKey::HKCU, "SOFTWARE\\Clients" );
    // subkeys 0, values 1; default is set - string = Mozilla Thunderbird
    wxRegKey key3( wxRegKey::HKCU, "SOFTWARE\\Clients\\Mail" );
    // seems to be case-insensitive  ------------^
    if ( key3.Exists()  )
    {
      key3.Open( wxRegKey::Read );
      bool bIsOpen = key3.IsOpened();
      if (  key3.IsOpened() )
      {
        // Get the number of subkeys and enumerate them.
        size_t subkeys3;
        size_t subValues;
        wxString wsT;
        // returns # of values minus default; it needs to be gotten separately
        // except when default is the only one??
        // It seems every key does have a default value, but it might not be set
        // to any value; all reg editors do show a defaylt value
        key3.GetKeyInfo(&subkeys3, NULL, &subValues, NULL);
        wsTT.Printf( _("sub keys: %d, values: %d\n"),subkeys3, subValues );
        m_textCtrlWizEmailClients->AppendText( wsTT );
        wxString key_name3;
        key3.GetFirstKey(key_name3, lIndex );
        for(size_t i = 0; i < subkeys3; i++)
        {
          wsTT.Printf( _("Sub key %d: %s\n"), i, key_name3 );
          m_textCtrlWizEmailClients->AppendText( wsTT );
          key3.GetNextKey(key_name3, lIndex );
        }
        // try for values -
        key3.GetFirstValue(key_name3, lIndex );

        for(size_t i = 0; i < subValues; i++)
        {
          wsTT.Printf( _("Value %d: %s\n"), i, key_name3 );
          m_textCtrlWizEmailClients->AppendText( wsTT );
          key3.GetNextValue(key_name3, lIndex );
        }
        wsT = key3.QueryDefaultValue();
        wsTT.Printf( _("Default value: %s\n\n"), wsT );
        m_textCtrlWizEmailClients->AppendText( wsTT );
        if ( !wsTT.IsEmpty() )
        {
          m_bFoundClient = true;
        }
        key3.Close();
      }
    }
    if ( ! m_bFoundClient )
      m_textCtrlWizEmailClients->AppendText( _("No e-mail client found!\n") );
  }
#endif
  wxString wsName;
  std::vector<ProcessEntry> proclist;
  GetProcessList( proclist );
//  int iSize = proclist.size();
  ProcessEntry pe;
  bool bFound = false;
  int iNClients = 0;
  m_textCtrlWizEmailClients->AppendText( _("--------\n") );
  for ( unsigned int i = 0; i < proclist.size(); i++ )
  {
    pe = proclist.at( i );
    if( pe.name.IsSameAs( _T("thunderbird.exe") ) )
    {
      bFound = true;
      iNClients++;
      //wsTT.Printf( _("ThunderBird is running\n") );
      m_textCtrlWizEmailClients->AppendText( _("ThunderBird is running\n") );
    }
    if( pe.name.IsSameAs( _T("MailClient.exe") ) )
    {
      bFound = true;
      iNClients++;
      m_textCtrlWizEmailClients->AppendText( _("eM-Client is running\n") );
    }
  }
  wsTT.Printf( _("%d e-mail client(s) running"), iNClients );

  m_textCtrlWizEmailClients->AppendText( wsTT );
  return true;
}

// ------------------------------------------------------------------

/**
 * Check to make sure we have a usable job file.
 */

bool MyBupJobDataDlg::TransferDataFromWindow( void )
{
#if defined (_DEBUG )
  wxString wsDbg;
#endif
  // general page   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  wxString wsName = GetTextCtrlTaskName()->GetValue();
  if( wsName.IsEmpty() )
  {
    wxMessageBox( _("The job name cannot be blank!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTextCtrlTaskName()->SetFocus();
    return false;
  }
  // check for invalid chars: \ / : * ? " < > |
  size_t i;
  wxString wsInvalid( _T("\\/:*?\"<>") );
  i = wsName.find_first_of( wsInvalid );
  if( i != wxString::npos )
  {
    wxMessageBox( _("The job name cannot contain any of ") + wsInvalid,
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTextCtrlTaskName()->SetFocus();
    return false;
  }
  wxFileName wfnJobFile;
  wfnJobFile.SetPath( wxGetApp().m_wsConfigDir );
  wfnJobFile.SetName( wsName );
  wfnJobFile.SetExt( _T("wbj") );
  m_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal =
    wfnJobFile.GetFullPath();
  if( ! wfnJobFile.IsOk() )
  {
    wxMessageBox( wfnJobFile.GetFullName() + _(" is not a valid file name!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTextCtrlTaskName()->SetFocus();
    return false;
  }
  else
  {
    // this entry consists of nothing but the name, no path no extension
    m_jobData.data[IEJ_JOB_NAME].dataCurrent.wsVal = wfnJobFile.GetName();
    m_iniPrefs.data[IE_LAST_JOB_FILE].dataCurrent.wsVal =
      wfnJobFile.GetFullPath();
  }

  bool bTemp = m_checkBoxJobEnabled->GetValue();
  m_jobData.data[IEJ_JOB_ENABLED].dataCurrent.bVal = bTemp;

  bTemp = m_checkBoxJobIncludeSubDirs->GetValue();
  m_jobData.data[IEJ_INCL_SUB_DIRS].dataCurrent.bVal = bTemp;

  // backup type
  int iSel = m_radioBoxBackupType->GetSelection();
  m_jobData.data[IEJ_BACKUP_TYPE].dataCurrent.wsVal =
    (iSel == 0 ) ? _T("Full") : _T("Differential");


  // Files page   // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  unsigned int uiNItems = GetListBoxSource()->GetCount();
  if( uiNItems == 0 )
  {
    wxMessageBox( _("Must have some source files or directories to backup!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTreebookJob()->SetSelection( 1 );
    GetGenericDirCtrlSrcPicker()->SetFocus();
    return false;
  }
  wxString wsT;
  wxString wsAll;
  wxString wsLast;
  wxArrayString wasSourceAllDirs;   // array of all source paths selected by user
  wxArrayString wasSourceDir;
  unsigned int uiNLines = GetListBoxSource()->GetCount();
  m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal = wxEmptyString;
#if defined (_DEBUG )
  wsDbg = m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal;
#endif
  // check for duplicate entries
  wxFileName wfnSrcDir;
  for (unsigned int i = 0; i < uiNLines; i++)
  {
    wsT = GetListBoxSource()->GetString( i );
    if( wsT.IsEmpty() )
      wxLogWarning( _T("%s %d Source line %d is empty"),
        __FILE__, __LINE__, i );
    wxASSERT(  !wsT.IsEmpty() );
    if ( wsT.MakeLower().IsSameAs( wsLast ) ) // we have a duplicate
    {
      wxString wsTT;  // avoid confusion with outer loop wsT
      wsTT.Printf(  _("%s is a duplicate!\nIt will be removed!"), wsLast );
      wxMessageBox( wsTT, wxT("Notice"), wxICON_ERROR | wxOK, this);
      GetListBoxSource()->Delete( i-- );
      uiNLines--; // adjust count
      continue;
    }
    wsLast = wsT;
    // assuming all string are dirs, rather than files,
    // string all source directory together, into one
    // long string, with ';' as separator - see below
    m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal += wsT;
#if defined (_DEBUG )
    wsDbg = m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal;
#endif
    // add separator for all but last entry
    if( i < uiNLines - 1 )
      m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal += _T(";");
#if defined (_DEBUG )
    wsDbg = m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal;
#endif
  }
#if defined (_DEBUG )
  wsDbg = m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal;
#endif
  wxString wsDestPath = m_dirPickerDestinationDir->GetPath();
  if ( wsDestPath.IsEmpty() )
  {
    wxMessageBox( _("You must specify a destination directory!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTreebookJob()->SetSelection( 1 );
    m_dirPickerDestinationDir->SetFocus();
    return false;
  }
  wxFileName wfnDestDir;
  wfnDestDir.AssignDir( m_dirPickerDestinationDir->GetPath() );
  if ( !wfnDestDir.IsDir() )
  {
    wxMessageBox( _("Destination MUST be a directory!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTreebookJob()->SetSelection( 1 );
    m_dirPickerDestinationDir->SetFocus();
    return false;
  }
  m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal =
    m_dirPickerDestinationDir->GetPath();
  // check if the destination drive & path is acceptable
  if ( ! CheckDestDriveInfo() )
  {
    GetTreebookJob()->SetSelection( 1 );
    GetDirPickerDestinationDir()->SetFocus();
    return false; // get user to fix it
  }
  // should also check if the directory exists already
  // ask if we are to continue, because we will need to
  // wipe the directory clean later.
  // But note: the dir picker does NOT allow the user to pick
  // a nonexistent directory!!!!!!!
  // also NOTE: the user can modify the path directly !!!!!!!
  // but, if user just created an empty directory, it will exist, so we
  // need to check if it has any files or directories
  wxDir wdDest( m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );
  if ( wfnDestDir.Exists() && (wdDest.HasFiles() || wdDest.HasSubDirs() ) )
  {
    if ( m_iniPrefs.data[IE_DELETE_DEST_WARN].dataCurrent.bVal )
    {
      wxString wsT;
      wsT.Printf(  _("Destination directory %s exists and appears to contain data!\n")  +
        _("It will be necessary to delete all of its contents!\n") +
        _("Did you, perhaps, forget to specify a  new subdirectory for this backup?\n") +
        _("Continue?"),wfnDestDir.GetFullPath() );
      int iAnswer = wxMessageBox( wsT, _("Confirm"), wxICON_QUESTION | wxYES_NO, this);
      if( iAnswer != wxYES )
      {
        GetTreebookJob()->SetSelection( 1 );
        GetDirPickerDestinationDir()->SetFocus();
        return false;
      }
    }
  }
  // check for overlapping source & dest paths
  // refuse to accept the 'bad' destination and
  // ask user to fix the destination, if we find overlap
  wxString wsSrcDir;
  bool bRet = Check4Overlap( wsSrcDir, wfnDestDir.GetPath() );
  if( bRet )  // we have path overlap
  {
    wxString wsT1;
    wsT1.Printf(  _("Destination directory path %s\n")  +
      _("is a sibling or child of the source directory %s\n") +
      _("Please select a different destination directory!\n"),
      wfnDestDir.GetFullPath(), wsSrcDir );
    wxMessageBox( wsT1, _("Notice"), wxOK | wxICON_INFORMATION, this);
    GetTreebookJob()->SetSelection( 1 );
    GetDirPickerDestinationDir()->SetFocus();
    return false;
  }
  // Save the serial number or volume name save already

  // save the latest job comment
  m_jobData.data[IEJ_JOB_COMMENT].dataCurrent.wsVal = m_textCtrlJobComments->GetValue();

  // Properties page      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  // we need a base directory - the rest are optional
  // or used later as given
  wsT = GetTextCtrlWizZipChunkBaseFileName()->GetValue();
  if( wsT.IsEmpty() )
  {
    wxMessageBox( _("You MUST provide a base name!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTreebookJob()->SetSelection( 2 );
    GetTextCtrlWizZipChunkBaseFileName()->SetFocus();
    return false;
  }
  m_jobData.data[IEJ_BAG_NAME_BASE].dataCurrent.wsVal = wsT;

  // also make sure user has selected a bag size
  // make up for the 'unknown' case
  // Note: the 'unknow' case is never inserted into the choice
  // box; any invalid value ( < 0 )implies a problem.
  iSel = GetChoiceWizOutputBagSize()->GetSelection();
  if( iSel < 0 )
  {
    wxMessageBox( _("You must select a valid 'bag' size!"),
      wxT("Notice"), wxICON_ERROR | wxOK, this);
    GetTreebookJob()->SetSelection( 2 );
    GetChoiceWizOutputBagSize()->SetFocus();
    return false;
  }
  // make up for the 'unknown' case
  m_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal =
    EBupBagSizeTypes::AsWxName( iSel + 1 );
  // include empty dirs
  bTemp = m_checkBoxIncludeEmptyDirs->GetValue();
  m_jobData.data[IEJ_INCL_MT_DIRS].dataCurrent.bVal = bTemp;

  // Filters page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  // Exclusion 'File' list choice box
  m_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal =
    m_comboBoxWizFileExclusions->GetValue();
  // Exclusion 'Dir' list choice box
  m_jobData.data[IEJ_EXCLUSION_LIST_DIRS].dataCurrent.wsVal =
    m_comboBoxWizDirExclusions->GetValue();
  // Move-as-Is choice box
  m_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal =
    m_comboBoxMoveAsIs->GetValue();
  // Zip page         // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  m_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal =
    (m_radioBoxWizZipOrMove->GetSelection() == 0 );
  // interfering tasks page  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  wxArrayInt waiCheckedTasks;
  unsigned int uiNTasks;
  uiNTasks = m_checkListBoxTasks2Kill->GetCheckedItems( waiCheckedTasks );
  m_jobData.data[IEJ_INTERFERING_TASKS].dataCurrent.wsVal = _T("");
  for ( unsigned int i = 0; i < uiNTasks; i++ )
  {
    m_jobData.data[IEJ_INTERFERING_TASKS].dataCurrent.wsVal +=
      m_checkListBoxTasks2Kill->GetString( i ) + _T(";");
  }

  // Notification  page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  // I am assuming that, if the fields are disabled, the data cannot be changed
  m_jobData.data[IEJ_EMAIL_WANTED].dataCurrent.bVal = m_checkEmailWanted->GetValue();
  m_jobData.data[IEJ_EMAIL_ERROR_ONLY].dataCurrent.bVal = m_checkBoxNotifyOnError->GetValue();
  m_jobData.data[IEJ_EMAIL_ADD_LOG].dataCurrent.bVal = m_checkBoxAddLogFile->GetValue();

  m_jobData.data[IEJ_EMAIL_SMTP].dataCurrent.wsVal  = m_textCtrlNotificationServer->GetValue();
  m_jobData.data[IEJ_EMAIL_FROM].dataCurrent.wsVal = m_textCtrlNotificationFrom->GetValue();
  m_jobData.data[IEJ_EMAIL_TO].dataCurrent.wsVal = m_textCtrlNotificationTo->GetValue();
  m_jobData.data[IEJ_EMAIL_SUBJECT].dataCurrent.wsVal = m_textCtrlNotificationSubject->GetValue();

  // When Done page     // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  // verify backup?
  m_jobData.data[IEJ_VERIFY_JOB].dataCurrent.bVal =
    m_checkBoxRunVerify->GetValue();
  return true;
}

// ------------------------------------------------------------------
/**
 * From the documentation:
 * The user changed the directory selected in the control either using
 * the button or using text control (see wxDIRP_USE_TEXTCTRL; note that
 * in this case the event is fired only if the user's input is valid,
 * e.g. an existing directory path [was chosen]).
 * No point worrying about the destination path until user is done
 * inputting or modifying the path.
 * BUT, since we assume that the user can select only an existing path,
 * we have to analyse the content of the text control and make sure it
 * meets those conditions: exist & is writable
 *
 * Since I cannot see the character the user  might have entered, I
 * pretty well seem to have to depend on the validator code and
 * act only on the 'enter' key to verify the path as valid or complain
 * and make him/her correct the path.
 But NOTE: pressing the 'Enter' key will open a dir selection dialog
 */

void MyBupJobDataDlg::OnWizDestnDirChanged(wxFileDirPickerEvent& event)
{
  event.Skip();
  wxString wsPath = event.GetPath();    // does not include any trailing slashes
  /**
   * This makes no sense and causes a lot of issues while user is trying to setup
   * a path. I constantly complains because the partial path does not exist.
   */
#if 0
  // no point worrying about the destination path until user is done
  // inputting or moddifying
#if 0
  // try to figure out if call is because of button or other user input
  // no luck so far
  wxEventType evt = event.GetEventType();
  wxEventCategory evc = event.GetEventCategory();
  long evl = event.GetExtraLong();
  int evi = event.GetInt();
  wxString wsOldPath = m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
  wxString wsEv = event.GetString();
#endif
  // handle user editing of the destination path
  wxString wsVolName = _("Unknown");
  wxString wsSerNo  = _("Unknown");
  wxString wsDir;
  wxString wsVol;
  wxString wsT;
  bool bIsFixed = true;
  bool bIsDVD = true;
  // need to strip or expand??? any macros first
  wxFileName wfnDestDir = m_dirPickerDestinationDir->GetDirName();
  wsDir =  wfnDestDir.GetPath();
  wsDir = wsDir.BeforeFirst( '{' ); // strip any macros
  wsVol = wfnDestDir.GetVolume();
  // user can change volume either by delete & insert or by insert & delete
  if ( wsVol.IsEmpty() || (wsVol.Length() > 1) )
  {
    // need to verify whether the user input is a valid directory or macro
    // currently the only macros is {date}
    // get both serial # & volume name
    SetDestnDriveInfo( wsVolName, wsSerNo );
    m_staticTextDestnNotes->SetLabel( wxEmptyString );
    return;
  }
  else
  {
    // make sure it is a valid drive
    wxFileName wfnDrive;
    wfnDrive.SetPath( wsVol + _T(":\\") );
    wxString wsS = wfnDrive.GetFullPath();
    if( !wfnDrive.IsDirWritable() || !wfnDrive.DirExists() )
    {
      wsT.Printf( _("Either drive %s does not exist or\nyou do not have write permissions\nPlease change the path!"),
        wsS.MakeUpper() );
      // don't just log it, user might not see it because the log window
      // might be hidden under the  job dialog
      wxMessageBox( wsT,  _("Error"), wxOK | wxICON_ERROR );
      wxLogMessage( wsT );
      return;
    }
    // else - we have a volume, which is all we need for the media info
    bool bError = GetMediaInfo( wsDir, bIsFixed, bIsDVD, wsVolName, wsSerNo );
    if ( bError )
    {
      wsT.Printf( _(" Error from GetMediaInfo() "));
      // don't just log it, user might not see it because the log window
      // might be hidden under the  job dialog
      wxMessageBox( wsT,  _("Error"), wxOK | wxICON_ERROR );
      return;
    }
    SetDestnDriveInfo( wsVolName, wsSerNo );

    // if it doe not exist, offer to create it
    wsT = wfnDestDir.GetFullPath();
    if( ! ::wxDirExists( wsDir ) )  // need to use the path without any macros
    {
      wsT.Printf( _("Directory %s does not exist!\nCreate it?"), wsDir );
      // don't just log it, user might not see it because the log window
      // might be hidden under the  job dialog
      // don't just log it, user might not see it because the log window
      // is hidden under the  job dialog
      int iAnswer = wxMessageBox( wsT,  _("Notice"), wxYES_NO | wxICON_QUESTION  );
      if ( iAnswer != wxYES )
      {
        return;
      }
      else
      {
        // MUST use the 'long' way to make sure all intermediary direcries are also created
        wxFileName wfnNewDir( wsDir );
        bool bRet = wfnNewDir.Mkdir( wxPOSIX_USER_WRITE, wxPATH_MKDIR_FULL );
        wxASSERT_MSG( bRet, _("MkDir failed: ") + wsDir );
        //::wxMkDir( wsDir );
        wfnDestDir = wfnNewDir; // update wfnDestDir for the resr of this routine
        //fall through and save the path
      }
    }
    // the path exists and see whether we have write permissions to it
    if( ! wfnDestDir.IsDirWritable() )
    {
      wsT.Printf( _("Directory %s exist but you do not have write permission\nPlease fix before proceeding!"),
        wfnDestDir.GetFullPath() );
      // don't just log it, user might not see it because the log window
      // might be hidden under the  job dialog
      // don't just log it, user might not see it because the log window
      // is hidden under the  job dialog
      wxMessageBox( wsT,  _("Notice"), wxOK | wxICON_ERROR  );
      {
        return;
      }
    }
  }
  m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal = wfnDestDir.GetPath();
#endif
  // keep focus on the control user is editing
  m_dirPickerDestinationDir->SetFocus();
}

// ------------------------------------------------------------------
void MyBupJobDataDlg::SetDestnDriveInfo(  wxString a_wsVolName, wxString a_wsSerNo )
{
  wxString wsLabel;
  // show the ser no & vol name for the selected drive
  wsLabel =  m_radioBoxDestnMedia->GetString( 0 );
  wsLabel = wsLabel.BeforeFirst( ':' );
  m_radioBoxDestnMedia->SetString( 0, wsLabel + _T(": ") + a_wsSerNo );
  wsLabel =  m_radioBoxDestnMedia->GetString( 1 );
  wsLabel = wsLabel.BeforeFirst( ':' );
  m_radioBoxDestnMedia->SetString( 1, wsLabel + _T(": ") + a_wsVolName );
#if 1
  m_jobData.data[IEJ_DEST_SER_NO].dataCurrent.wsVal = a_wsSerNo;
  m_jobData.data[IEJ_DEST_VOL_NAME].dataCurrent.wsVal = a_wsVolName;
#else
  if ( m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal )
  {
    m_jobData.data[IEJ_DEST_USE_VOL_OR_SER_NO_STRING].dataCurrent.wsVal = a_wsSerNo;
  }
  else
  {
    m_jobData.data[IEJ_DEST_USE_VOL_OR_SER_NO_STRING].dataCurrent.wsVal = a_wsVolName;
  }
#endif
}

// ------------------------------------------------------------------
/**
 * Check if there are any issues with the destination path.
 * return true if all is as expected
 * return false if not
 */
bool MyBupJobDataDlg::CheckDestDriveInfo()
{
  bool bAllOK = true;
  bool bDriveOk = false;
  bool bDirExists = false;
  wxString wsT;
  wxString wsMacro;
  wxString wsVol;
  wxString wsDest = m_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
  /* Here we typically have a path ...\DD{macro}
   * We need to strip off DD{macro} before we check the path for existence
   * because that part of the path is intended to be expanded and created
   * by the application to make up the full destination path
   */
  // make typing a little easier
  size_t n = wsDest.Replace( _T("\\"), _T("/") );
  int iBrace= wsDest.find( '{' );
  if( iBrace != wxNOT_FOUND )
  {
    // we have a macro, so also remove any parts of the destination directory
    // back to the last forward slash which terminates the parent directory
    wsMacro = wsDest.Mid( iBrace ); // get macro before trimming the string
    wsDest  = wsDest.SubString( 0, iBrace - 1 );
    wsT = wsDest.AfterLast( '/' );
    size_t l1 = wsT.Len();
    size_t ll = wsDest.Len();
    wsDest = wsDest.Mid( 0, ll - l1 );
  }
  wxFileName wfnDest;
  bool bIsFixed = false;
  bool bIsDVD = false;
  wfnDest.AssignDir( wsDest );
  wsVol = wfnDest.GetVolume() +_T(":\\");
  // now check if the path exists
  if ( !wfnDest.DirExists() )
  {
    bAllOK = false;
    // does not exist
    // check if it is the volume - we know the full path does not exist
    wsVol = wfnDest.GetVolume() +_T(":\\");
    if( wxDirExists(  wsVol ) )
    {
      bIsFixed = false;
      bIsDVD = false;
      wxString wsVolName;
      wxString wsSerNo;
      // but make sure we use the clean dest'n
      bool bError = GetMediaInfo( wsDest, bIsFixed, bIsDVD, wsVolName, wsSerNo );
      if ( bError )
      {
        bAllOK = false;
        wxASSERT( true ); // stop us in our tracks if we get here
      }
      bDriveOk = true;
      // make it easier to read
      wsDest.Replace( _T("/"), _T("\\") );
      /* the drive exists, but not the base path
       * This likely means the user is editing a path from an old job file
       * and moving the destination to a new drive where the path might not exist
       * offer to create the path on the new drive.
       * During creation of a new job file, the user can ONLY select an existing
       * path, in this case, help him out
       */
      wsT.Printf(
        _("The destination drive '%s' exist,\n") +
        _("but the base destination path '%s' does NOT!\n") +
        _("Very likely the destination is/was intended\n") +
        _("to be on a USB removable drive\n") +
        _("Do you want the new path to be created?"), wsVol, wsDest );
      // don't just log it, user might not see it because the log window
      // is hidden under the  job dialog
      int iAnswer = wxMessageBox( wsT,  _("Notice"), wxYES_NO | wxICON_QUESTION  );
      if ( iAnswer != wxYES )
      {
        return false;
      }
      else
      {
        ::wxMkDir( wsDest, wxS_DIR_DEFAULT );
        return true;
      }
    }
    else // the drive wsVol does not exist, cannot carry on
    {
      wsT.Printf(
        _("The destination drive '%s' does NOT exist,\n") +
        _("Please chose a different drive!"), wsVol );
      // don't just log it, user might not see it because the log window
      // is hidden under the  job dialog
      int iAnswer = wxMessageBox( wsT,  _("Error"), wxOK | wxICON_ERROR  );
      return false;
    }
  }
  else
  {
    bDirExists = true;
  }
  // but if it does, we need to check whether we have the right drive.
  // get both serial # & volume name

  wxString wsVolNameWanted;
  wxString wsSerNoWanted;
  wxString wsVolName;
  wxString wsSerNo;
  wxString wsLabel;
  bIsFixed = bIsDVD = false;
  // but make sure we use the clean dest'n
  bool bError = GetMediaInfo( wsDest, bIsFixed, bIsDVD, wsVolName, wsSerNo );
  if ( bError )
  {
    bAllOK = false;
    wxASSERT( true ); // stop us in our tracks if we get here
  }
  // no problems getting the info for the given drive
  // now check if it is the one we wanted
  bool bHAveProblem = true;
  if ( m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal )
  {
    // serial number
    wsSerNoWanted = m_jobData.data[IEJ_DEST_SER_NO].dataCurrent.wsVal;
    if( wsSerNoWanted.IsSameAs( wsSerNo ) )
      bHAveProblem = false;
  }
  else
  {
    //volume name
    wsVolNameWanted = m_jobData.data[IEJ_DEST_VOL_NAME].dataCurrent.wsVal;
    if( wsVolNameWanted.IsSameAs( wsVolName ) )
      bHAveProblem = false;
  }
  if ( bHAveProblem )
  {
    // neither volume name nor serial number were specified
    // it must be  an 'old' job file ?
    if( wsSerNoWanted.IsSameAs( _T("unknown") ) ||
      wsVolNameWanted.IsSameAs( _T("unknown") ) )
    {
      bAllOK = true;
      wsT.Printf( _("This appears to be an old job file!\n") +
        _("The currently selected destination drive is %s.\n") +
        _("Please select whether the destination drive is to be identified by\n") +
        _(" its serial number or by the drive letter\n") +
        _("The default selection will be the serial number for removable drives\n")  +
        _("and drive letter for fixed drives!\n") +
        _("For more details, please see the help file."), wsVol );
      // don't just log it, user might not see it because the log window
      // might be hidden under the  job dialog
      wxMessageBox( wsT,  _("Notice"), wxOK | wxICON_INFORMATION  );
    }
  }
  if( bAllOK )
  {
    // now act on the default
    // true => use serial number, false => use volume name
    m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal = !bIsFixed;
#if 0
    if ( bIsFixed )
    {
      m_radioBoxDestnMedia->Enable( false );
      wsT.Printf( _("Dest Drive: %s is a fixed drive\nThe Volume Label will be used!"), wsVolName );
      //m_staticTextDestnNotes->SetLabel( wsT );
      // Use the volume name - drive letter
      // that will be all hard disk have often enough.
      SetDestnDriveInfo( wsVolName, wsSerNo );
    }
    else
#endif
    {
      m_radioBoxDestnMedia->Enable( true ); // just in case
      // it is a removable drive; set the default to the volume name
      if ( m_jobData.data[IEJ_DEST_DEFAULT_SERNO_OR_VOL].dataCurrent.bVal ) // true => serial number
      {
        m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal = true;
      }
      else  // false => volume label/name
      {
        m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal = false;
      }
      // selection in radio box depends on value of
      // m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal
      wsT.Printf( _("The Dest drive letter '%s' will NOT be used!\n") +
        _("Please chose either the Volume name or serial number "), wsVol );
      SetDestnDriveInfo( wsVolName, wsSerNo );
      m_staticTextDestnNotes->SetLabel( wsT +
        _("\nYou may change this option anytime before saving!") );
    }
  }
  else
  {
    // show user the current info
    SetDestnDriveInfo( wsVolName, wsSerNo );
    // describe issue for user in the adjacent text ctrl m_textCtrlDestnNotes
    if ( bDirExists )
      wsT.Printf( _("Dest Drive: %s exists\nDest Path   %s exists!"), wsVol, wsDest );
    else
      wsT.Printf( _("Dest Drive: %s exists\nDest Path   %s does NOT exist!"), wsVol, wsDest );
    m_staticTextDestnNotes->SetLabel( wsT );
  }
//  Fit();  // makes the dialog way too big
  return bAllOK;
}

// ------------------------------------------------------------------
/**
 * The user changed the destination ID - serial number <-> volume name
 */
void MyBupJobDataDlg::OnDestnDriveIdChanged(wxCommandEvent& event)
{
  int iSel = event.GetSelection();
  m_jobData.data[IEJ_DEST_USE_SERNO_OR_VOL].dataCurrent.bVal = ( iSel == 0 );
  m_staticTextDestnNotes->SetLabel(
    _("Please chose either the Volume name or serial number\n") +
    _("You may change this option anytime before saving!") );
  event.Skip();
}

// ------------------------------------------------------------------

void MyBupJobDataDlg::OnWizSrcDel( wxCommandEvent& WXUNUSED(event ) )
{
  m_bHaveSourceListSelection = false;
  int iSel = GetListBoxSource()->GetSelection();
  if ( wxNOT_FOUND == iSel )
    return; // nothing selected, just ignore the click
  wxString wsT = GetListBoxSource()->GetString( iSel );
  GetListBoxSource()->Delete( iSel );
  // this removes the selected item and leaves nothing selected
  // so we ought to select the item above the one just deleted
  unsigned int uiNItems = GetListBoxSource()->GetCount();
  if ( uiNItems )
  {
    GetListBoxSource()->SetSelection( std::max( 0, iSel - 1 ) );
    m_bHaveSourceListSelection = true;
  }
}

// ------------------------------------------------------------------
void MyBupJobDataDlg::OnTreebookWizPageChanging(wxBookCtrlEvent& event)
{
  event.Skip();
}

// ------------------------------------------------------------------

void MyBupJobDataDlg::OnTreebookWizPageChanged(wxBookCtrlEvent& event)
{
  event.Skip();
}

// ------------------------------------------------------------------
void MyBupJobDataDlg::OnSourceListSelect(wxCommandEvent& event)
{
  m_bHaveSourceListSelection = true;
  event.Skip();
}

// ------------------------------------------------------------------
void MyBupJobDataDlg::OnSourceTreeKeyDown(wxTreeEvent& event)
{
  event.Skip();
}

// ------------------------------------------------------------------

void MyBupJobDataDlg::OnUpdateUIButtonSrcDel(wxUpdateUIEvent& event)
{
  event.Enable( m_bHaveSourceListSelection );
}

// ------------------------------------------------------------------
/**
 * When pages change, we need to check if we landed on the 'Files' page
 * and if so, we need to check again that the destination drive and
 * path in the file selector correspond to the desired path
 */
void MyBupJobDataDlg::OnTreebookJobPageChanged(wxTreebookEvent& event)
{
  event.Skip();
  if( event.GetSelection() == 1 )
  {
    // we landed on the Files' page
    // make sure the destination drive & path are OK
    // sometimes when using USN=B drives for the destiantion
    // the drive letters can change and we don't want backups to
    // seemingly disappear because they did not end up on
    // the expected drive/location
    // skip for now on page change
    // error will come up when user tries to close the job dialog
    // if there is an issue with the drive & path
    // but only makes sense if field is not empty as it will be
    // at the start of editing
    wxString wfnDestPath = m_dirPickerDestinationDir->GetPath();
    if ( /* m_bIsEditing && */ ! wfnDestPath.IsEmpty() )
    {
      CheckDestDriveInfo(); // ignore the return value
    }
  }
}

// ------------------------------------------------------------------

void MyBupJobDataDlg::GetProcessList(std::vector<ProcessEntry> &proclist)
{
#ifdef __WXMSW__

#if 0
   if (!IsWindowsXPOrGreater())
    {
      wxMessageBox( _("You need at least Windows XP") ); //, "Version Not Supported", MB_OK);
      return;
    }
#else
   OSVERSIONINFO osver;
   // this function has been deprecated
   // Check to see if were running under Windows95 or
   // Windows NT.
   osver.dwOSVersionInfoSize = sizeof( osver ) ;
   if ( !GetVersionEx( &osver ) ) {
      return;
   }

   if ( osver.dwPlatformId != VER_PLATFORM_WIN32_NT ) {
      return;
   }
#endif
   //get child processes of this node
   HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
   if (!hProcessSnap) {
      return;
   }

   //Fill in the size of the structure before using it.
   PROCESSENTRY32 pe;
   memset(&pe, 0, sizeof(pe));
   pe.dwSize = sizeof(PROCESSENTRY32);

   // Walk the snapshot of the processes, and for each process,
   // kill it if its parent is pid.
   if (!Process32First(hProcessSnap, &pe)) {
      // Can't get first process.
      CloseHandle (hProcessSnap);
      return;
   }

   do {
      ProcessEntry entry;
      entry.name = pe.szExeFile;
      entry.pid = (long)pe.th32ProcessID;
      proclist.push_back(entry);
   } while (Process32Next (hProcessSnap, &pe));
   CloseHandle (hProcessSnap);
#else
   //GTK and other
   wxArrayString output;
#if defined( _MSC_VER )
#if defined (__WXGTK__)
   ExecuteCommand( wxT("ps -A -o pid,command  --no-heading"), output);
#elif defined (__WXMAC__)
   // Mac does not like the --no-heading...
   ExecuteCommand(wxT("ps -A -o pid,command "), output);
#endif
#endif
   for (size_t i=0; i< output.GetCount(); i++) {
      wxString line = output.Item(i);
      //remove whitespaces
      line = line.Trim().Trim(false);

      //get the process ID
      ProcessEntry entry;
      wxString spid = line.BeforeFirst(wxT(' '));
      spid.ToLong( &entry.pid );
      entry.name = line.AfterFirst(wxT(' '));

      if (entry.pid == 0 && i > 0) {
         //probably this line belongs to the provious one
         ProcessEntry e = proclist.back();
         proclist.pop_back();
         e.name << entry.name;
         proclist.push_back( e );
      } else {
         proclist.push_back( entry );
      }
   }
#endif
}

// ------------------------------------------------------------------
/**
 * Called when the backup type radio box in the job edit dialog is clicked.
 */
void MyBupJobDataDlg::OnBackupTypeClicked(wxCommandEvent& event)
{
  int iButton = event.GetInt();
  UpdateBackupType( iButton );
}

// ------------------------------------------------------------------
/* Explain the differnce betweenn full and differential backup.
 * Now done when dialog is built up
 */
void MyBupJobDataDlg::UpdateBackupType( int iButton )
{
}

// ------------------------------------------------------------------
/**
 * set the enabled status of the email message fields
 */
void MyBupJobDataDlg::OnJobOptionEmailWanted(wxCommandEvent& event)
{
  m_checkBoxAddLogFile->Enable( event.IsChecked() );
  m_checkBoxNotifyOnError->Enable( event.IsChecked() );
  m_textCtrlNotificationServer->Enable( event.IsChecked() );
  m_textCtrlNotificationFrom->Enable( event.IsChecked() );
  m_textCtrlNotificationTo->Enable( event.IsChecked() );
  m_textCtrlNotificationSubject->Enable( event.IsChecked() );
  event.Skip();
}

// ------------------------------------------------------------------
/**
 * Check if any source and destination directories overlap.
 * the destination cannot be a sibling or child of any source directory,
 * otherwise we end up with loop - eventually
 * return true if we have overlap,
 *        false, otherwise
 */
bool MyBupJobDataDlg::Check4Overlap( wxString &wsSrcDir, wxString wsDestDir )
{
  wxArrayString wasDestDirs;
  wxArrayString wasSrcDirs;
  wxFileName wfnDest;
  wxFileName wfnSrc;
  // we need to strip any 'templates off the destination
  wxString wsT = wsDestDir.BeforeFirst( '{' );
  wfnDest.AssignDir( wsT );
  wasDestDirs = wfnDest.GetDirs();
  wxStringTokenizer tokenizer(
    m_jobData.data[IEJ_SOURCE_PATHS].dataCurrent.wsVal, ";" );
  size_t n;
  size_t i;
  while ( tokenizer.HasMoreTokens() )
  {
    wxString wsToken = tokenizer.GetNextToken();
    // process token here
    bool bIsDir = ::wxDirExists( wsToken );
    bool bIsFile = ::wxFileExists( wsToken );
    if( bIsFile )
      continue;   // ignore files
    else if ( bIsDir )
    {
      wfnSrc.AssignDir( wsToken );
      wasSrcDirs = wfnSrc.GetDirs();
      n = std::min( wfnSrc.GetDirCount(), wfnDest.GetDirCount() );
      for( i = 0; i < n; i++ )
      {
        // for Windows, we ignore case
        if ( !wasDestDirs[i].MakeLower().IsSameAs( wasSrcDirs[i].MakeLower() ) )
        {
          break; // mismatch, break out of inner loop
        }
      }
      if ( n == i )
      {
        wsSrcDir = wsToken;
        return true; // no point carrying on, get user to fix the bad path
      }
    }
  }
  return false;
}

// ------------------------------- eof ------------------------------
