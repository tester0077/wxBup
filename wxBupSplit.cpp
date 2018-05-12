/*-----------------------------------------------------------------
 * Name:        wxBupSplit.cpp
 * Purpose:     split the files in the zip list among as many 'bags' 
 *              of the given maximum size as necessary
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
#include "wxBuph.h"
#include "wxBupFrameh.h"
#include "wxBupConfh.h"
#include "wxBupWorkerh.h"
#include "wxBupExclusionsh.h"
// ------------------------------------------------------------------
#if defined( _MSC_VER )  // from Autohotkey-hummer.ahk
// this block needs to AFTER all headers
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif
#endif
extern jobPrefs_t g_jobData;
// ------------------------------------------------------------------
/**
 * The sources are all in the zip list m_zipList in order of size,
 * starting with the largest at the front.
 * To start with, we simply divide the files among a new set of bag 
 * lists, where, for now at least, the bag lists will have the same
 * elements as the zip list.
 * I will keep track of the bag lists in a std::vector = dynamic array
 * If a bag log  is wanted, it will be named after the bag name with the
 * extension 'log' and record the original file path and name.
 * return true only if all went well
 */
bool MyWorkerThread::SplitFilesIntoBags()
{
  wxString wsT;
  wxFile   wfBagLog;
  wxString wsBagLogPathNName;
  wxString wsDestBagRoot;
  unsigned long long ullDiskSize = 0ll;
  wxFileName wfnFileName;
  // setup move only exclusions
  wxString wsMoveOnly = g_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal;
  FileExclusions fileMoveOnly( wsMoveOnly );

  bool bFound = false;
  bool bTooBig = false;
  bool bFirstTime = true;
  size_t szN = 0;
  m_frame->m_wasBags.Clear();
  // iterate over the list in STL syntax
  // to find files which are too big for the 'bag' size
  // Note: if we erase an item, we already get the next 'iter' doing another
  // 'iter++' will skip that next item!!!!
  for (std::list<MyZipListElement>::iterator iter = m_frame->m_zipList.begin(); 
    iter != m_frame->m_zipList.end();  /* see note above */)
  {  
    // check if we were asked to exit
    if ( TestDestroy() || wxGetApp().m_frame->TestCancelled() )
        return false;
    wxString wsMsg;
    wxString wsT1;
    // try to calculate actual size on disk
    // for some media, such as e-mail attachments the granularity is 0
    // for others, the granularity is the sector size of the selected medium
    if ( m_frame->m_ulMediumAllocUnit > 0 )
    {
      ullDiskSize = ((iter->m_ullZipSize / m_frame->m_ulMediumAllocUnit) + 1 ) * 
        m_frame->m_ulMediumAllocUnit;
    }
    else
    {
      ullDiskSize = iter->m_ullZipSize;
    }
    wxString wsT = iter->m_wsZipFileName;
    if( ullDiskSize > m_frame->m_ullSelectedBagSize )
    {
      if ( !bFound )
      {
        bFound = true;
        bTooBig = true;
        if ( bFirstTime )
        {
          wsT1.Printf(  _("The following files are too big for ") +
            g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
          wsMsg = wsT1 + _(" - They will be skipped!!!!"); 
          bFirstTime = false;
        }
        else
          wsMsg = wxEmptyString;
        {
          wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
          wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 | MY_FILE_TOO_BIG );  // verbosity
          event.SetString( wsMsg );
          wxQueueEvent( m_frame, event.Clone() );
        }
      }
      wfnFileName.Assign( iter->m_wsSrcFullPath ); 
      wsT.Printf( _("%s, %s"), iter->m_wsZipFileName,
        wfnFileName.GetHumanReadableSize() );
      wsMsg += wsT;
      {
        wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
        // send a log message to the main frame log window
        if ( bTooBig )
        {
          event.SetInt( MY_RED );
          event.SetExtraLong( 5 | MY_FILE_TOO_BIG );    // verbosity
        }
        else
        {
          event.SetInt( MY_BLACK );   // my color enums
          event.SetExtraLong( 0 );    // verbosity
        }
        
        event.SetString( wsT );
        wxQueueEvent( m_frame, event.Clone() );
      }
      // and update the total zip size
      // MUST use the old iterator BEFORE it is changed by the 'erase'
      m_frame->m_ullTotalZipsSize -= iter->m_ullZipSize;
      // now remove this item from the list
      // which also does 'iter++' in effect
      iter = m_frame->m_zipList.erase( iter );
    }
    else 
    {
      bTooBig = false;
      iter++;
    }
  }
  // estimate # of 'bags' required for the give total size
  unsigned long long ullNBags;
  ullNBags = (m_frame->m_ullTotalZipsSize / 
    m_frame->m_ullSelectedBagSize) + 1;

  m_frame->m_ulBags = (unsigned long )ullNBags;
  wxString wsTotalSize;
  wxString wsBagSize;
  m_frame->FormatSize( (double)m_frame->m_ullTotalZipsSize, wsTotalSize );
  m_frame->FormatSize( (double)m_frame->m_ullSelectedBagSize, wsBagSize );
  wsT.Printf( _("Estimated number of 'bags': %ld (%s/%s)"), 
    m_frame->m_ulBags, wsTotalSize,  wsBagSize );
    //m_frame->m_ullTotalZipsSize, m_frame->m_ullSelectedBagSize );
  wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
  // send a log message to the main frame log window
  event.SetInt( MY_BLACK ); // my color enums
  event.SetExtraLong( 0 );  // verbosity
  event.SetString( wsT );
  wxQueueEvent( m_frame, event.Clone() );
  m_frame->GetGaugeProgress()->SetValue( 0 );
  // and the taskbar icon's as well
  m_frame->GetAppProgressInd()->SetValue( 0 );
  // now do the actual splitting
  // in this scenario, I will remove each file from the main list
  // as it is moved to the bag directory
  unsigned int uiNBags = 1;
  size_t szNFilesBagged = 0;
  while ( ! m_frame->m_zipList.empty() )
  {
    // check if we were asked to exit
    if ( TestDestroy() || wxGetApp().m_frame->TestCancelled() )
        return false;
    wxString wsBase = g_jobData.data[IEJ_BAG_NAME_BASE].dataCurrent.wsVal;
    wxString wsBag;
    // limit the bag base name to 8 chars
    wsBase = wsBase.Left( 8 );
    wsBag.Printf( _T("%s%02d"), wsBase, uiNBags );
    // make a sanity check & trap in case the # of actual bags the
    // app attempts to create exceeds the estimate by a preset limit of 20
    if( uiNBags >= ( m_frame->m_ulBags + 20 ) )
    {
      wxString wsT1;
      wsT1.Printf( _("SplitFiles: the actual number of 'bags' (%ld) exceeds\n" ) +
        _("the estimated number of %ld\nSomething is wrong!"), uiNBags, m_frame->m_ulBags );
      wxMessageBox( wsT, 
        _("Error"), wxOK |wxICON_ERROR );
      wxThreadEvent event1( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      event1.SetInt( MY_RED );   // my color enums
      event1.SetExtraLong( 0 );  // verbosity
      event1.SetString( wsT1 );
      wxQueueEvent( m_frame, event1.Clone() );
      break;
    }
#if defined( WANT_DATE_DIR )
    wxString wsDest = m_frame->m_wfnDest.GetPath();
#else
    wxString wsDest = g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal;
#endif
    // make sure the destination path is treated as a directory
    wxFileName wfnDestBag;
    wfnDestBag.AssignDir( wsDest );
    wxString wsTest = wfnDestBag.GetFullPath();
    wfnDestBag.AppendDir( wsBag );
    wsTest = wfnDestBag.GetFullPath();
    if ( wfnDestBag.DirExists() )
    {
      if ( ! wxGetApp().m_bQuietMode )
      {
        int answer = wxMessageBox( _("Directory ") + wsTest +
          _(" already exists!\nOverwrite all files?"), 
          _("Confirm"), wxYES_NO | wxCANCEL |wxICON_QUESTION );
        if (answer != wxYES)
        {
          wsT.Printf( 
            _("Directory %s already exists.\n") +
            _("User declined overwriting it!\nCleaning up & Aborting job"),
            wfnDestBag.GetFullPath() );
          wxThreadEvent  event( wxEVT_THREAD, WORKER_EVENT_LOG );
          // send a log message to the main frame log window
          event.SetInt( MY_RED );   // my color enums
          event.SetExtraLong( 5 );  // verbosity
          event.SetString( wsT );
          wxQueueEvent( m_frame, event.Clone() );
          return false;
        }
      }
      //else
      // this will remove all files and sub directories
      m_frame->GetStatusBar()->SetStatusText( _("Cleaning up: ") + wfnDestBag.GetFullPath() );
      m_frame->RemoveDirAndFiles( wfnDestBag.GetFullPath() );
      m_frame->GetStatusBar()->SetStatusText( _T(""), 0 );
    }
    // create the base dir
    if( !wfnDestBag.Mkdir( 0777, wxPATH_MKDIR_FULL ) )
    {
      wsT = _("Can't create the directory:\n") + wfnDestBag.GetFullPath();
      wxMessageBox(  wsT, _("Error"), wxOK | wxICON_EXCLAMATION );
      wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
      // send a log message to the main frame log window
      event.SetInt( MY_RED );   // my color enums
      event.SetExtraLong( 5 );  // verbosity
      event.SetString( wsT );
      wxQueueEvent( m_frame, event.Clone() );
      return false;
    }
    // start the bag log if it is wanted
    // the bag log is always wanted/needed
    // no testing of backup integrity without it
    {
      bool bRet;
      wsBagLogPathNName =  wfnDestBag.GetFullPath() + wsBag + _T(".log" );
      bRet = wxFile::Exists( wsBagLogPathNName );
      if( bRet )
      {
        // we have problems
        wxLogError( _T("%s %d '%s' does not exist!"), 
          __FILE__, __LINE__, wsBagLogPathNName );
        wxASSERT( bRet == false );
      }
      bRet = wfBagLog.Create( wsBagLogPathNName ); 
      if ( !bRet )
        wxLogError( _T("%s %d Cannot create: %s"), 
          __FILE__, __LINE__, wsBagLogPathNName );
      wxASSERT( bRet );
      
      // add a header
      wxString wsDate;
      wxString wsTime;
      wxString wsT2;

      wxString wsPC = wxGetHostName();
      wxString wsUser = wxGetUserName();
 //     wsUser = wxGetUserId();
      wxDateTime dt = wxDateTime::Now();
      wsDate = dt.FormatISODate();  
      wsTime = dt.FormatISOTime();
      wfBagLog.Write( wsDate + _T(" ") + wsTime );
      wsT2.Printf( _(" - Bag %d of %d\r\n"), uiNBags, m_frame->m_ulBags );
      wfBagLog.Write( wsT2 ); 
      wsT2.Printf( _T("PC: %s User: %s wxBUP ver. %d.%d, build %d, verbosity: %ld\r\n"), 
        wsPC, wsUser, giMajorVersion,
        giMinorVersion, giBuildNumber, g_iniPrefs.data[IE_LOG_VERBOSITY].dataCurrent.lVal );
      wfBagLog.Write( wsT2 );
      wsT2.Printf( _("%s, Exclusions: \"%s\"\r\nMoveOnly: \"%s\"\r\nBag size: %s\r\n\r\n" ), 
        g_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal ? _("Moved") : _("Zipped"),
        g_jobData.data[IEJ_EXCLUSION_LIST_FILES].dataCurrent.wsVal, 
        g_jobData.data[IEJ_JUST_MOVE_LIST].dataCurrent.wsVal,
        g_jobData.data[IEJ_BAG_TYPE].dataCurrent.wsVal );
      wfBagLog.Write( wsT2 );
      wfBagLog.Write( _("Comment: ") + g_jobData.data[IEJ_JOB_COMMENT].dataCurrent.wsVal +
        _T("\r\n\r\n") );
      wfBagLog.Write( _("Original data location:\r\n========================\r\n" ) );
    }
    wxString wsZipFile;
    wxFileName wfnDestFile = wfnDestBag;
    wxFileName wfnZipFile;
    unsigned long long ullCurBagSize = 0ll;
    unsigned long long ullCurBagRoomLeft;
    
    bool bRet;
    bool bCopied = false;
    std::list<MyZipListElement>::iterator iter = m_frame->m_zipList.begin();
    while ( iter != m_frame->m_zipList.end() )
    {  
      bCopied = false;
      if ( fileMoveOnly.Matches( iter->m_wsSrcFullPath ))
      {
        bCopied = true;
      }
      ullCurBagRoomLeft = (m_frame->m_ullSelectedBagSize - ullCurBagSize );
      if ( m_frame->m_ulMediumAllocUnit > 0 )
      {
        ullDiskSize = ((iter->m_ullZipSize / m_frame->m_ulMediumAllocUnit) + 1 ) * 
          m_frame->m_ulMediumAllocUnit;
      }
      else
      {
        ullDiskSize = iter->m_ullZipSize;
      }
#ifdef _DEBUG
      wsZipFile = iter->m_wsZipFileName;
#endif // _DEBUG
      if( ullDiskSize < (m_frame->m_ullSelectedBagSize - ullCurBagSize ) )
      {
        ullCurBagSize += ullDiskSize;  
        wsZipFile = iter->m_wsZipFileName;
        // set the destination path & file name
        wfnZipFile.Assign( iter->m_wsZipFileName );
        wfnDestFile.SetFullName( wfnZipFile.GetFullName() );
        // must also look after any extra bag directory components
        wxString wsPath;
        wxArrayString wasBagDirs;
        wasBagDirs = wfnDestBag.GetDirs();
        wsPath = wfnZipFile.GetFullPath();
        wxArrayString wasZipDirs;
        wasZipDirs = wfnZipFile.GetDirs();
        wsPath = wfnZipFile.GetFullPath();
#if defined _DEBUG
        wxString wsD = wfnZipFile.GetFullName();
        if( wsD.Lower().IsSameAs( _T("unzip_exe.zip") ) )
        {
          int jjj = 0;
          jjj++;
        }
#endif
        // for wsPath = wfnZipFile.GetFullPath();
        // we have wsPath:   "D:\bupTestBinMe1.44\UTILS\UNZIP_EXE.zip"
        // m_frame->m_wfnDest \------------------/
        // we want:           "D:\bupTestBinMe1.44\BagNamexx\UTILS\UNZIP_EXE.zip"
        // wsBagLog.path      \-----------------------------/
        // we need to strip dirs of g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal
        // - which includes the trailing slash
        // and replace them with dirs from wsBagLog.path
        //                    "D:\bupTestBinMe1.44\BagNamexx\UTILS\UNZIP_EXE.zip"
        //                                         \---------------------------?
        // the final destination path will only include
        //                    
        wxString wsTT;
        wxFileName wfnFinalDestFile;
        wxFileName wfnFinalDestBag;
#if defined( WANT_DATE_DIR )
        wxFileName wfnDest( m_frame->m_wfnDest );
#else
        wxFileName wfnDest( g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );
#endif
        wxFileName wfn2Fix( wfnZipFile );
#if defined _DEBUG
        wsTT = wfn2Fix.GetFullPath();
#endif
        for ( size_t i = 0; i < wfnDest.GetDirCount(); i++ )
        {
          wfn2Fix.RemoveDir( 0 );
#if defined _DEBUG
          wsTT = wfn2Fix.GetFullPath();
#endif
        }
        // removed the special dir from the path
        wfn2Fix.RemoveDir( 0 );
#if defined _DEBUG
        wsTT = wfn2Fix.GetFullPath();
        wsDest = wfn2Fix.GetFullPath();
#endif
        // now save the final bag directory
        wfnFinalDestBag = wfn2Fix;
        wasBagDirs = wfnDestBag.GetDirs();
        for ( size_t i = wasBagDirs.GetCount(); i; i-- )
        {
          wfn2Fix.PrependDir( wasBagDirs[i-1] );
#if defined _DEBUG
          wsTT = wfn2Fix.GetFullPath();
#endif
        }
#if defined _DEBUG
        wsTT = wfn2Fix.GetFullPath();
        wsDest = wfn2Fix.GetFullPath();
        wxString wsFileName = wfn2Fix.GetFullName();
        if( wsFileName.IsSameAs( _T("Sigmaringen-1867-1454525974bsb10020620.pdf") ) )
          int i = 0;
#endif
        wfnDestFile = wfn2Fix;
        wsDest = wfnDestFile.GetFullPath();
        // ensure the destination directory tree exists
        if ( !wfnDestFile.DirExists() )
        {
          if( !wfnDestFile.Mkdir( 0777, wxPATH_MKDIR_FULL ) )
          {
            wsT = _("Can't create the directory:\n") + wfnDestFile.GetFullPath();
            wxMessageBox(  wsT, _("Error"), wxOK | wxICON_EXCLAMATION );
            wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
            // send a log message to the main frame log window
            event.SetInt( MY_RED );   // my color enums
            event.SetExtraLong( 5 );  // verbosity
            event.SetString( wsT );
            wxQueueEvent( m_frame, event.Clone() );
            continue;
          }
        }
        /* keep track of the longest destination path
         * this can only be done here
         */
        if( wfnFinalDestBag.GetFullPath().Length() > m_frame->m_ulMaxDestPathLen )
        {
          // play it safe
          wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
          m_frame->m_ulMaxDestPathLen = wfnFinalDestBag.GetFullPath().Length();
          m_frame->m_wsLongestDestPath = wfnFinalDestBag.GetFullPath();
        }
#if defined( WANT_NAME_LEN_CHECK_IN_SPLIT )  
        should be done during initial file count
        if ( wfnFinalDestBag.GetFullName().Length() > m_frame->m_ulMaxDestFileNameLen )
        {
          // play it safe
          wxCriticalSectionLocker lock( wxGetApp().m_frame->m_critsectWork );
          m_frame->m_ulMaxDestFileNameLen = wfnFinalDestBag.GetFullName().Length();
          m_frame->m_wsLongestDestFileName = wfnFinalDestBag.GetFullName();
        }
#endif
        if ( g_jobData.data[IEJ_COPY_ONLY_WANTED].dataCurrent.bVal )
        {
          bRet = m_frame->MyFileCopy( iter->m_wsSrcFullPath, wsDest, true );
          if( bRet == false )
          {
            wsT.Printf( _("BupSlit: MyCopyFile returned error for %s to %s\nStop?"),
              iter->m_wsSrcFullPath, wsDest );
            int iAnswer = wxMessageBox(  wsT, _("Error"), wxYES_NO | wxICON_ERROR );
            if( iAnswer == wxYES )
            {
              wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
              wxGetApp().m_frame->m_bCancelled = true;
              return false;
            }
            iter++;
            continue; // ignore this file & go to next one
          }
        }
        else
        {
          wxFileName wfnSrc( wsZipFile );
          size_t szZip = wsZipFile.Length();
          size_t szDest = wsDest.Length();
          wxASSERT( wfnSrc.Exists() );
          wxFileName wfnDest( wsDest );
          wxASSERT( !wfnDest.Exists() );
          bRet = MyFileMove( wsZipFile, wsDest );
          if( bRet == false )
          {
            wsT.Printf( _("%s, %d - MyMoveFile returned an error for %s to %s\n") +
              _("Zip len: %d, Zip len: %d\nStop?"), 
              __FILE__, __LINE__, wsZipFile, wsDest, szZip, szDest );
            wxThreadEvent event( wxEVT_THREAD, WORKER_EVENT_LOG );
            // send a log message to the main frame log window
            event.SetInt( MY_RED );   // my color enums
            event.SetExtraLong( 5 | MY_ERROR );  // verbosity
            event.SetString( wsT );
            wxQueueEvent( m_frame, event.Clone() );
            wxLogMessage( wsT );
            int iAnswer = wxMessageBox(  wsT, _("Error"), wxYES_NO | wxICON_ERROR );
            if( iAnswer == wxYES )
            {
              wxCriticalSectionLocker lock(wxGetApp().m_frame->m_csCancelled);
              wxGetApp().m_frame->m_bCancelled = true;
              return false;
            }
            // not sure if this is really needed or not //????
//            m_frame->m_zipList.erase( iter );	// remove it from the list
            iter++;
            continue; // ignore this file & go to next one
            // but NOTE: this results in an infinite loop since the file stays in the list
          }
        }
        wxString wsTest;
        //if ( g_jobData.data[IEJ_BAG_LOG_WANTED].dataCurrent.bVal )
        // the bag log is always wanted/needed
        // no testing of backup integrity without it
        {
          // add both source & target
          // but make the destination relative to bag directory
          // g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal
#if defined( WANT_DATE_DIR )
          wfnDestFile.MakeRelativeTo( m_frame->m_wfnDest.GetPath() );
#else
          wfnDestFile.MakeRelativeTo( g_jobData.data[IEJ_DESTN_ROOT_PATH].dataCurrent.wsVal );
#endif
#if defined( _DEBUG )
          wxString wsBag = wfnDestFile.GetPath();
#endif
          wxString wsCopyOrZip = bCopied ? _T("Copied: ") : _T("Zipped: ");
          wsTest = wsCopyOrZip + iter->m_wsSrcFullPath + _T(" | ") + wfnDestFile.GetFullPath();
          wfBagLog.Write( wsTest + _T("\r\n" ) ); 
          m_frame->SetStatusText( wfnDestFile.GetFullPath(), 1 );
        }
        int i = m_frame->m_zipList.size();
        iter = m_frame->m_zipList.erase( iter );
        i = m_frame->m_zipList.size();
        wxString wsCount;
        float fPercent = 
          ((float)(szNFilesBagged) * 100 ) / (float)(m_frame->m_szNTotalFiles);
        wsCount.Printf( _("Bagging: %Id/%Id  - %3.1f%%"), szNFilesBagged, 
          m_frame->m_szNTotalFiles, fPercent );
        // update the progress gauge for user feedback
        m_frame->GetGaugeProgress()->SetValue( 
          __min( 100, (szNFilesBagged * 100)/m_frame->m_szNTotalFiles ) );
        // and the taskbar icon's as well
        m_frame->GetAppProgressInd()->SetValue(
          __min( 100, (szNFilesBagged * 100)/m_frame->m_szNTotalFiles ) );
        m_frame->SetStatusText( wsCount, 0 );
        szNFilesBagged++;
      }
      else
      {
        ++iter; // try the next file
      }
    }
    // the log was started in the proper directory, just flush & close the file
    wfBagLog.Flush(); 
    wfBagLog.Close(); 
    // start a new 'bag'
    m_frame->m_wasBags.Add( wsBagLogPathNName );
    uiNBags++;
  }
  // clean up status field 1
  m_frame->SetStatusText( wxEmptyString, 1 );
  return true;
}

// ------------------------------- eof ------------------------------
