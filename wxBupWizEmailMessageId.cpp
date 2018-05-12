/*-----------------------------------------------------------------
 * Name:        wxBupWizEmailMessageId.cpp
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
#include "wxBupWizEmailMessageIdh.h"
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

wxEmailMessageID::wxEmailMessageID() {}

// ------------------------------------------------------------------

wxEmailMessageID::~wxEmailMessageID() {}

// ------------------------------------------------------------------

wxString wxEmailMessageID::Decrypt(const wxChar byte) const
{
  wxString dec = fromHexString(m_value);
  dec = XOR(dec, byte);
  return dec;
}

// ------------------------------------------------------------------

wxString wxEmailMessageID::Encrypt(const wxChar byte) const
{
  wxString enc = XOR(m_value, byte);
  enc = toHexString(enc);
  return enc;
}

// ------------------------------------------------------------------

wxEmailMessageID::wxEmailMessageID(const wxString& value)
  : m_value(value)
{
}

// ------------------------------------------------------------------

wxString wxEmailMessageID::toHexString(const wxString& value) const
{
  wxString output;
  for(size_t i = 0; i < value.length(); ++i) 
  {
    wxChar buf[5] = { 0, 0, 0, 0, 0 };
    wxSprintf(buf, "%04X", (int)value[i]);
    output << buf;
  }
  return output;
}

// ------------------------------------------------------------------

wxString wxEmailMessageID::fromHexString(const wxString& hexString) const
{
  wxString output;
  size_t cnt = hexString.length() / 4;
  for(size_t i = 0; cnt > i; ++i) 
  {
    wxString dec = hexString.Mid(i * 4, 4);
    int hexVal;
    wxSscanf(dec, "%X", &hexVal);
    output << (wxChar)hexVal;
  }
  return output;
}

// ------------------------------------------------------------------

wxString wxEmailMessageID::XOR(const wxString& str, const wxChar KEY) const
{
  wxString output;
  for(size_t i = 0; i < str.length(); ++i) 
  {

    wxChar ch = str[i];
    wxChar x = (ch ^ KEY);
    output << x;
  }
  return output;
}

// ------------------------------- eof ------------------------------
