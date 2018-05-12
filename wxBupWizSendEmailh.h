/*-----------------------------------------------------------------
 * Name:        wxBupWizSendEmailh.h
 * Purpose:
 * Author:      A. Wiegert
 *
 * Copyright:
 * Licence:     wxWidgets licence
 *-------------------------------------------------------------- */
#ifndef _WX_BUP_SEND_EMAIL_H
#define _WX_BUP_SEND_EMAIL_H
// ------------------------------------------------------------------
#include <wx/string.h>
#include <wx/filename.h>
// globals  defined in wxBuFrame.cpp
#if defined( WANT_FILEVERSION )
extern int giMajorVersion;
extern int giMinorVersion;
extern int giBuildNumber;
#endif
// ------------------------------------------------------------------
class wxEmailMessage
{
  wxString m_from;
  wxString m_to;
  wxString m_subject;
  wxString m_message;
  wxString m_payload;
  wxArrayString m_attachements;

  char* m_aschar;
  size_t m_pos;
  size_t m_len;

protected:
  void DoAddAttachment(const wxString& filename, wxString& payload) const;
  bool CheckAttachmentSize( wxString a_wsFilename ) const;
public:
  wxEmailMessage();
  virtual ~wxEmailMessage();

  /**
  * @brief finalize the message and prepare it for send
  */
  bool Finalize();

  char* AsChar() const { return m_aschar + m_pos; }
//    size_t Len() const { return m_len; }
  size_t Len() const { return m_len - m_pos; }  // fixed garbage at end of large attachments
  void Consume(size_t bytes) { m_pos += bytes; }
  bool HasMore() const { return (m_len > m_pos); }

  /**
  * @brief generate a unique message ID
  */
  static wxString GenerateID();

  /**
  * @brief Create the message payload
  */
  wxString PayLoad() const;

  /**
  * @brief add attachment to email
  */
  wxEmailMessage& AddAttachment(const wxString& filename)
  {
    if(m_attachements.Index(filename) == wxNOT_FOUND) {
      m_attachements.Add(filename);
    }
    return *this;
  }

  wxEmailMessage& SetFrom(const wxString& from)
  {
    this->m_from = from;
    return *this;
  }
  wxEmailMessage& SetMessage(const wxString& message)
  {
    this->m_message = message;
    return *this;
  }
  wxEmailMessage& SetSubject(const wxString& subject)
  {
    this->m_subject = subject;
    return *this;
  }
  wxEmailMessage& SetTo(const wxString& to)
  {
    this->m_to = to;
    return *this;
  }
  const wxString& GetFrom() const { return m_from; }
  const wxString& GetMessage() const { return m_message; }
  const wxString& GetSubject() const { return m_subject; }
  const wxString& GetTo() const { return m_to; }
  const void ExpandSubjectVersion( wxString& ar_wsSubjectLine2Expand ) const;
  const void ExpandSubjectMachineName( wxString& ar_wsMachine2Expand ) const;
};

// ------------------------------------------------------------------
#if defined( WANT_CURL )
class wxMailer
{
  wxString m_email;
  wxString m_password;
  wxString m_smtp;

public:
  /**
  * @brief construct a mailer object associated with the sender email address
  * @param email the sender's gmail email
  * @param password the email's password
  * @param smtpURL the smtp URL, for gmail it will be something like:
  * "smtps://smtp.gmail.com:465"
  */
  wxMailer(const wxString& email, const wxString& password, const wxString& smtpURL);
  virtual ~wxMailer();

  /**
  * @brief send an email
  * @param message
  */
  bool Send(const wxEmailMessage& message);
};
#endif
#endif  // _WX_BUP_SEND_EMAIL_H
// ------------------------------- eof ---------------------------
