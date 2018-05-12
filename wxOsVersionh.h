/*-----------------------------------------------------------------
 * Name:        wxOsVersionh.h
 * Purpose:     Obtain version information for a specific file.
 *              Can be used to get the OS version info, if the file queried is
 *              a system file such as kernel32.dll
 * Author:      A. Wiegert
 * From:        https://forums.wxwidgets.org/viewtopic.php?t=8193
 *              with several crucial adaptations
 * Copyright:
 * Licence:     wxWidgets licence
 * 
 * Notes:
 * MS advocates some other functions to get information as to what
 * OS features an application can expect; see:
 * https://msdn.microsoft.com/en-ca/library/windows/desktop/ms724429%28v=vs.85%29.aspx
 *    **** There is a difference *****
 * For instance if an applicatiion is set for some compatibility mode
 *-------------------------------------------------------------- */
#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#include <wx/wx.h>

#ifdef __WXMSW__

class wxOsVersion
{

public:
  wxOsVersion();

public:
  bool    Open(wxString strModuleName=wxEmptyString);
  void    Close();

  wxString QueryValue(wxString strValueName, unsigned long dwLangCharset = 0);

  wxString GetFileDescription()  {return QueryValue(wxT("FileDescription")); };
  wxString GetFileVersion()      {return QueryValue(wxT("FileVersion"));     };
  wxString GetInternalName()     {return QueryValue(wxT("InternalName"));    };
  wxString GetCompanyName()      {return QueryValue(wxT("CompanyName"));     };
  wxString GetLegalCopyright()   {return QueryValue(wxT("LegalCopyright"));  };
  wxString GetOriginalFilename() {return QueryValue(wxT("OriginalFilename"));};
  wxString GetProductName()      {return QueryValue(wxT("ProductName"));     };
  wxString GetProductVersion()   {return QueryValue(wxT("ProductVersion"));  };

  bool     GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
  wxString GetFixedFileVersion();
  wxString GetFixedProductVersion();

protected:
  unsigned char*  m_lpVersionData;
  unsigned long   m_dwLangCharset;

public:
  ~wxOsVersion();
};

#endif //__WXMSW__
#endif  // __FILEVERSION_H_
// ------------------------------- eof ---------------------------
