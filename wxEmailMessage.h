#ifndef WXEMAILMESSAGE_H
#define WXEMAILMESSAGE_H

#include <wx/string.h>
#include <wx/filename.h>

class wxEmailMessage
{
    wxString m_from;
    wxString m_to;
    wxString m_cc;
    wxString m_bcc;
    wxString m_subject;
    wxString m_message;
    wxString m_payload;
    wxArrayString m_attachements;

    char* m_aschar;
    size_t m_pos;
    size_t m_len;

protected:
    void DoAddAttachment(const wxString& filename, wxString& payload) const;

public:
    wxEmailMessage();
    virtual ~wxEmailMessage();

    /**
     * @brief finalize the message and prepare it for send
     */
    void Finalize();

    char* AsChar() const { return m_aschar + m_pos; }
    size_t Len() const { return m_len; }
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

    /**
     * @brief set sender
     */
    wxEmailMessage& SetFrom(const wxString& from)
    {
        this->m_from = from;
        return *this;
    }

    /**
     * @brief set message body
     */
    wxEmailMessage& SetMessage(const wxString& message)
    {
        this->m_message = message;
        return *this;
    }

    /**
     * @brief set subject
     */
    wxEmailMessage& SetSubject(const wxString& subject)
    {
        this->m_subject = subject;
        return *this;
    }

    /**
     * @brief set firts receiver
     */
    wxEmailMessage& SetTo(const wxString& to)
    {
        this->m_to = to;
        return *this;
    }

    /**
     * @brief set another receiver
     */
    wxEmailMessage& SetCC(const wxString& cc)
    {
        this->m_cc = cc;
        return *this;
    }

    /**
     * @brief set a hide receiver
     */
    wxEmailMessage& SetBCC(const wxString& bcc)
    {
        this->m_bcc = bcc;
        return *this;
    }
    const wxString& GetFrom() const { return m_from; }
    const wxString& GetMessage() const { return m_message; }
    const wxString& GetSubject() const { return m_subject; }
    const wxString& GetTo() const { return m_to; }
    const wxString& GetCC() const { return m_cc; }
    const wxString& GetBCC() const { return m_bcc; }
};

#endif // WXEMAILMESSAGE_H
