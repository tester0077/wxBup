/*-----------------------------------------------------------------
 * Name:        wxBupWizSrcDnDh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WXBUP_WIZ_SRC_DND_H
#define _WXBUP_WIZ_SRC_DND_H

#include "wxBupFrameh.h" 
// ------------------------------------------------------------------
class MyDnDFile : public wxFileDropTarget
{
public:
    MyDnDFile(wxListBox *pOwner = NULL) { m_pOwner = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:
    wxListBox *m_pOwner;
};

#endif  // _WXBUP_WIZ_SRC_DND_H
// ------------------------------- eof ---------------------------

