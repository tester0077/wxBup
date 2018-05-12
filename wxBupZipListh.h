/*-----------------------------------------------------------------
 * Name:        wxBupZipListh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_ZIP_LIST_H
#define _WX_BUP_ZIP_LIST_H
#include "wx/wx.h"
#include <list>

class MyZipListElement
{
public:
  wxString            m_wsZipFileName;  // file name, complete with (parts of original?) path
  wxString            m_wsSrcFullPath;  //<< complete file path of source, needed for move only
                                        //<< because source & dest'n path won't be the same
  unsigned long long  m_ullSrcSize;     // size of original file
  unsigned long long  m_ullZipSize;     // size of zipped file
  

  MyZipListElement(
    wxString            a_wsZipFileName,
    wxString            a_wsSrcFullPath,
    unsigned long long  a_ullSrcSize,
    unsigned long long  a_ullZipSize ) 
  {                 // initialize all members
    m_wsZipFileName     = a_wsZipFileName;
    m_wsSrcFullPath     = a_wsSrcFullPath;
    m_ullSrcSize        = a_ullSrcSize;
    m_ullZipSize        = a_ullZipSize;
  };
};

#endif  // _WX_BUP_ZIP_LIST_H
// ------------------------------- eof ---------------------------
