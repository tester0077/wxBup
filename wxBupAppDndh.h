/*-----------------------------------------------------------------
 * Name:        wxBupAppDndh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_APP_DND_H
#define _WX_BUP_APP_DND_H

// ------------------------------------------------------------------
#include "wxBupPreProcDefsh.h" // needs to be first
#include "wxBupBaseFrame.h"
#include <wx/dnd.h>
// ------------------------------------------------------------------
#if defined ( WANT_APP_DND )
// Drag'n'Drop class
class MyFrame;
class DnDFile : public wxFileDropTarget
{
public:
    DnDFile(MyFrame *pOwner) { m_pOwner = pOwner; }

    virtual bool OnDropFiles(wxCoord x, wxCoord y,
                             const wxArrayString& filenames);

private:
    MyFrame *m_pOwner;
};
#endif
#endif  // _WX_BUP_APP_DND_H
// ------------------------------- eof ---------------------------

