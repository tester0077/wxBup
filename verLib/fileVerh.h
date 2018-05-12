//From:
//http://wxforum.shadonet.com/viewtopic.php?t=8193&highlight=fileversion
/*
 * usage:
 * char szExeName[MAX_PATH];
 * GetModuleFileName(NULL, szExeName, sizeof (szExeName));
 *
 */
#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#include <wx/wx.h>

#ifdef __WXMSW__

class wxFileVersion
{

public:
  wxFileVersion();

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

  unsigned long GetMajorVersion() { return m_ulMajorVersion; };
  unsigned long GetMinorVersion() { return m_ulMinorVersion; };
  unsigned long GetBuildNumber()  { return m_ulBuildNumber; };
  unsigned long GetSpecialBuild() { return m_ulSpecialBuild; };
  bool     GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
  wxString GetFixedFileVersion();
  wxString GetFixedProductVersion();

protected:
  unsigned char*  m_lpVersionData;
  unsigned long   m_dwLangCharset;
  unsigned long   m_ulMajorVersion;
  unsigned long   m_ulMinorVersion;
  unsigned long   m_ulBuildNumber;
  unsigned long   m_ulSpecialBuild;

public:
  ~wxFileVersion();
};

#endif //__WXMSW__
#endif  // __FILEVERSION

// ------------------------------- eof ------------------------------
