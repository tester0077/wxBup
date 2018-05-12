/*-----------------------------------------------------------------
 * Name:        wxBupWizEmailMessageIdh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_MESSAGE_ID_H
#define _WX_BUP_MESSAGE_ID_H

// ------------------------------------------------------------------

#include <wx/string.h>

class wxEmailMessageID
{
public:
  wxString m_value;

protected:
  wxString toHexString(const wxString& value) const;
  wxString fromHexString(const wxString& hexString) const;
  wxString XOR(const wxString& str, const wxChar KEY) const;

public:
  wxEmailMessageID(const wxString& value);
  wxEmailMessageID();
  virtual ~wxEmailMessageID();

  wxString Decrypt(const wxChar byte = 's') const;
  wxString Encrypt(const wxChar byte = 's') const;
};

#endif  // _WX_BUP_MESSAGE_ID_H
// ------------------------------- eof ---------------------------

