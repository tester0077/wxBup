/*-----------------------------------------------------------------
 * Name:        wxBupDestPathValidatorh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_DEST_VALIDATOR_H
#define _WX_BUP_DEST_VALIDATOR_H

#include "wxBupFrameh.h"
// ------------------------------------------------------------------
/**
 * Create my own text validator patterned after the original wxTextValidator.
 *
 * After reviewing the comments by Vadim in http://trac.wxwidgets.org/ticket/14579
 * I have decided to implemt my own validator.
 * My latest tests show that I could get at the OnChar routine in the edit dialog
 * but then I would have to spend  some effort to get the current string from the
 * derived text validator I had started with.
 * Making my own validator seems to be the better and simpler way.
 * The idea is to use OnChar() to recalculate the max data length and give the user 
 * more specific and timely feedback as he entrs text.
 * The max length feedback is too late and I am sure would cause frustration.
 */
// ------------------------------------------------------------------


/**
    Styles used by MyCellTextValidator.

    Note that when you specify more styles in wxTextValidator the validation checks
    are performed in the order in which the styles of this enumeration are defined.
*/
enum MyCellTextValidatorStyle
{
    /// No filtering takes place.
    wxFILTER_NONE_CELL,
#if 0
    /// Empty strings are filtered out.
    /// If this style is not specified then empty strings are accepted
    /// only if they pass the other checks (if you use more than one wxTextValidatorStyle).
    wxFILTER_EMPTY,

    /// Non-ASCII characters are filtered out. See wxString::IsAscii.
    wxFILTER_ASCII,

    /// Non-alpha characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isalpha
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_ALPHA,

    /// Non-alphanumeric characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isalnum
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_ALPHANUMERIC,

    /// Non-numeric characters are filtered out.
    /// Uses the wxWidgets wrapper for the standard CRT function @c isdigit
    /// (which is locale-dependent) on all characters of the string.
    wxFILTER_DIGITS,

    /// Non-numeric characters are filtered out.
    /// Works like @c wxFILTER_DIGITS but allows also decimal points,
    /// minus/plus signs and the 'e' or 'E' character to input exponents.
    /// Note that this is not the same behaviour of wxString::IsNumber().
    wxFILTER_NUMERIC,

    /// Use an include list. The validator checks if the user input is on
    /// the list, complaining if not. See wxTextValidator::SetIncludes().
    wxFILTER_INCLUDE_LIST,

    /// Use an include list. The validator checks if each input character is
    /// in the list (one character per list element), complaining if not.
    /// See wxTextValidator::SetCharIncludes().
    wxFILTER_INCLUDE_CHAR_LIST,

    /// Use an exclude list. The validator checks if the user input is on
    /// the list, complaining if it is. See wxTextValidator::SetExcludes().
    wxFILTER_EXCLUDE_LIST,

    /// Use an exclude list. The validator checks if each input character is
    /// in the list (one character per list element), complaining if it is.
    /// See wxTextValidator::SetCharExcludes().
    wxFILTER_EXCLUDE_CHAR_LIST
#endif
};


// the class definition MUST come from
class MyCellTextValidator : public wxValidator 
{
public:
    MyCellTextValidator(long style = wxFILTER_NONE, wxString *val = NULL);
    MyCellTextValidator(const MyCellTextValidator& val);

    virtual ~MyCellTextValidator(){}

    // Make a clone of this validator (or return NULL) - currently necessary
    // if you're passing a reference to a validator.
    // Another possibility is to always pass a pointer to a new validator
    // (so the calling code can use a copy constructor of the relevant class).
    virtual wxObject *Clone() const { return new MyCellTextValidator(*this); }
    bool Copy(const MyCellTextValidator& val);

    // Called when the value in the window must be validated.
    // This function can pop up an error message.
    virtual bool Validate(wxWindow *parent);

    // Called to transfer data to the window
    virtual bool TransferToWindow();

    // Called to transfer data from the window
    virtual bool TransferFromWindow();

    // Filter keystrokes
    void OnChar(wxKeyEvent& event);

    // ACCESSORS
    inline long GetStyle() const { return m_validatorStyle; }
    void SetStyle(long style);
    void SetMaxLen( long lLen );
    void SetCurLen( long lLen );

    wxTextEntry *GetTextEntry();

    void SetCharIncludes(const wxString& chars);
    void SetIncludes(const wxArrayString& includes) { m_includes = includes; }
    inline wxArrayString& GetIncludes() { return m_includes; }

    void SetCharExcludes(const wxString& chars);
    void SetExcludes(const wxArrayString& excludes) { m_excludes = excludes; }
    inline wxArrayString& GetExcludes() { return m_excludes; }

    bool HasFlag(wxTextValidatorStyle style) const
        { return (m_validatorStyle & style) != 0; }

protected:

    // returns true if all characters of the given string are present in m_includes
    bool ContainsOnlyIncludedCharacters(const wxString& val) const;

    // returns true if at least one character of the given string is present in m_excludes
    bool ContainsExcludedCharacters(const wxString& val) const;

    // returns the error message if the contents of 'val' are invalid
    virtual wxString IsValid(const wxString& val) const;

protected:
    long                m_validatorStyle;
    wxString*           m_stringValue;
    wxArrayString       m_includes;
    wxArrayString       m_excludes;
    long                m_lMaxLen;
    long                m_lCurLen;
    // intended to help with the <Enter> problem, but does NOT - yet
    int                 m_iLastKeyCode;

private:
    wxDECLARE_NO_ASSIGN_CLASS(MyCellTextValidator);
    DECLARE_DYNAMIC_CLASS(MyCellTextValidator)
    DECLARE_EVENT_TABLE()
};

#endif  // _WX_BUP_DEST_VALIDATOR_H
// ------------------------------- eof ---------------------------

