/*-----------------------------------------------------------------
 * Name:        wxBupCheckUpdateDialog.cpp
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
#include "wxBupCheckUpdateDialogh.h"
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
/**
 * Note: this URL is NOT the downlaod page -
 * very misleading
 */
// Constructor
MyBupUpdateDlg::MyBupUpdateDlg( wxWindow* parent ) : 
    MyBupUpdateDlgBase( parent  )
{
#if defined (_DEBUG )
  m_hyperLinkBupUpdatePage->SetURL( 
    _T("http://hummer/wxBupVer/wxBupversion.php") );
  // disbale the link for the DEBUG version
  m_staticText1152->Disable();
  m_hyperLinkBupUpdatePage->Disable();
  m_staticText1156->Disable();
  m_staticText1158->Disable();
#else
  m_hyperLinkBupUpdatePage->SetURL( 
    _T("http://www.columbinehoney.net/wxBupVer/wxBupversion.php") );
#endif

}

// ------------------------------- eof ------------------------------
