#ifndef WXBUPEDITJOB_H
#define WXBUPEDITJOB_H
#include "wxBupBaseFrame.h"

class wxBupEditJob : public wxBupEditJobBase
{
public:
    wxBupEditJob(wxWindow* parent);
    virtual ~wxBupEditJob();
protected:
    virtual void OnSelDestn(wxCommandEvent& event);
    virtual void OnSelSource(wxCommandEvent& event);
};
#endif // WXBUPEDITJOB_H
