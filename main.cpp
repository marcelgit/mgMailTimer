/////////////////////////////////////////////////////////////////////////////
// Name:        mgMailTimer.cpp
// Purpose:     Send e-mail from files in directory
// Author:      Marcelino Gisbert
// Modified by:
// Created:     23/09/16
// Copyright:   (c) Marcelino Gisbert
// Licence:     wxWindows licence
// Thanks to Eran Ifrah for SendEmailGUI
// Thanks to https://curl.haxx.se/mail/lib-2012-04/0021.html
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================
#undef __CASA__  // Para las pruebas en casa

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include <wx/string.h>
    #include <wx/filesys.h>
    #include <wx/filename.h>
    #include <wx/fs_arc.h>
    #include <wx/stopwatch.h>
    #include <wx/utils.h>
    #include "wxMailer.h"
    #include "MgLogger.h"
    #include "MgDocument.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
//#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../marce.xpm"
//#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
MgLogger *mglog;
void sendMail(const MgDocument* cmv);
void ShowOutput(const wxString& cmd,
                const wxArrayString& output,
                const wxString& title);
void convertPclToPdf(const wxString& docu);
bool getFileToSend(wxString& docu);
void aTrabajar();
void moveFileToFinalFolder(const wxString& docu);

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnSend(wxCommandEvent& event);
    void aTrabajar();

private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT,
    ID_TIMER = wxID_HIGHEST,
    MENU_ID_SEND
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_MENU(MENU_ID_SEND, MyFrame::OnSend)
    //EVT_TIMER(ID_TIMER, MyFrame::OnTimer)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("mgMailTimer App");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(marce));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(MENU_ID_SEND, "&Send\tAlt-S", "Send e-mail");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#else // !wxUSE_MENUS
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
#endif // wxUSE_MENUS/!wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("mgMailTimer running!");
#endif // wxUSE_STATUSBAR

    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, ID_TIMER);
    wxTimer *timer = new wxTimer(this, ID_TIMER);
    timer->Start(5000);  // 5 segundos
    
    mglog = new MgLogger;
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    if (mglog)
    {
        wxLog::FlushActive();
    }
    wxLog::EnableLogging(false);
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s Beta 2!\n"
                    "\n"
                    "This is the mailer spy\n"
                    "running with %s\n"
                    "under %s.",
                    wxTheApp->GetAppName(),
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About mgMailTimer",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MyFrame::OnTimer(wxTimerEvent& event)
{
    // Cada 5 segundos se procesa un archivo.
    aTrabajar();
}
void MyFrame::OnSend(wxCommandEvent& WXUNUSED(event))
{
    aTrabajar();
}

void MyFrame::aTrabajar()
{
//    static int contador = 0;
//    SetStatusText(wxString::Format("Paso: %4d", contador++, 1));
//    wxMessageBox(wxT("Procesar un archivo"), "Intervalo", wxOK | wxICON_INFORMATION, this);

    Unbind(wxEVT_TIMER, &MyFrame::OnTimer, this, ID_TIMER);
    wxBeginBusyCursor();
    wxString docu;
    if ( getFileToSend(docu) )
    {
        //#ifndef __CASA__
            convertPclToPdf(docu);
        //#endif
        MgDocument *cmv = new MgDocument(docu);
        sendMail(cmv);
        moveFileToFinalFolder(docu);
    }
    wxEndBusyCursor();
    Bind(wxEVT_TIMER, &MyFrame::OnTimer, this, ID_TIMER);
}

void convertPclToPdf(const wxString& docu)
{
    //pcl6 -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=%1tmp.pdf  -J"@PJL SET PAPER=A4" %1
    //wxString docu("F07191233420430001");
    wxString cmd(wxString::Format("pcl6 -dNOPAUSE -sDEVICE=pdfwrite -sOutputFile=%s.pdf  -J\"@PJL SET PAPER=A4\" %s.pcl", docu, docu));
    if (mglog)
        mglog->add(wxString::Format("\"%s\" is running please wait...", cmd));

    wxStopWatch sw;
    wxArrayString output, errors;
    int code = wxExecute(cmd, output, errors);
    if (mglog)
        mglog->add(wxString::Format("Command \"%s\" terminated after %ldms; exit code %d.",
                    cmd, sw.Time(), code));

    ShowOutput(cmd, output, wxT("Output"));
    ShowOutput(cmd, errors, wxT("Errors"));
}

void ShowOutput(const wxString& cmd,
                         const wxArrayString& output,
                         const wxString& title)
{
    size_t count = output.GetCount();
    if ( !count )
        return;

    if (mglog)
    {
        mglog->add(wxString::Format(wxT("--- %s of '%s' ---"),
                                        title.c_str(), cmd.c_str()));
        for ( size_t n = 0; n < count; n++ )
        {
            mglog->add(output[n]);
        }
        mglog->add(wxString::Format(wxT("--- End of %s ---"),
                                        title.Lower().c_str()));
    }
}


void sendMail(const MgDocument* cmv)
{
    wxString mailSender(cmv->docuSender);           //"administracion@pataleta.com");
    wxString mailPassword(cmv->docuPassword);   //"correo");
    wxString mailServer(cmv->docuServer);       //"smtp://mail.pataleta.com:587");
    wxString mailFrom(cmv->docuFrom);           //"administracion@pataleta.com");
    wxString mailTo(cmv->docuTo);               //"marcegis@yahoo.es");
    wxString mailCC(cmv->docuCC);               //marcelinator@gmail.com");
    wxString mailBCC("");
    wxString mailMessage(cmv->docuBody);        //"Cuerpo del mensaje");
    wxString mailSubject(cmv->docuSubject);     //"Asunto del correo");
    wxString mailAttach(wxString::Format("%s.pdf", cmv->docuFile));         //"F07191233420430001.pdf");
    //wxString mailAttach("examenfisica4eso2.pdf");
    if(mglog)
    {
        mglog->add(wxString::Format("Sender: %s", mailSender));
        mglog->add(wxString::Format("Server: %s", mailServer));
        mglog->add(wxString::Format("To: %s", mailTo));
        mglog->add(wxString::Format("CC: %s", mailCC));
        mglog->add(wxString::Format("BCC: %s", mailBCC));
        mglog->add(wxString::Format("Message: %s", mailMessage));
        mglog->add(wxString::Format("Subject: %s", mailSubject));
        mglog->add(wxString::Format("Attach: %s", mailAttach));
    }

    wxMailer mailer(mailSender, mailPassword, mailServer);
    wxEmailMessage message;
    message.SetFrom(mailFrom)
        .SetTo(mailTo)
        .SetMessage(mailMessage)
        .SetSubject(mailSubject);
//        .SetCC(mailCC)
//        .SetBCC(mailBCC)
        
    //if(!GetFilePickerAttachment()->GetPath().IsEmpty()) {
    //    message.AddAttachment(GetFilePickerAttachment()->GetPath());
    //}
    message.AddAttachment(mailAttach);

    message.Finalize(); // Must finalize the message before sending it
    if(mailer.Send(message)) {
        //::wxMessageBox("Message Sent Succesfully!");
        if(mglog)
            mglog->add(wxT("Message Sent Succesfully!"));
    }
}

bool getFileToSend(wxString& docu)
{
    bool isOK = false;
    #ifdef __CASA__
        wxString path(wxT("C:\\Microven\\EMAIL"));
    #else
        wxString path(wxT("D:\\Microven\\EMAIL"));
    #endif
    wxString patron = wxT("*.cmv");
    wxString ficheroCMVencontrado("");
    wxFileSystem fs;    // = new wxFileSystem();
    wxFileName fn;
    fs.AddHandler(new wxArchiveFSHandler);
    fs.ChangePathTo(path, true);
    if (fs.HasHandlerForPath(path))
    {
        ficheroCMVencontrado = fs.FindFirst(patron, wxFILE);
        if (ficheroCMVencontrado.Length() > 4)
        {
            fn.Assign(ficheroCMVencontrado);
            docu = path + fn.GetPathSeparator() + fn.GetName();
            isOK = true;
        }
    }
    return isOK;
}

void moveFileToFinalFolder(const wxString& docu)
{
    wxString pdf = docu + wxT(".pdf");
    wxString cmv = docu + wxT(".cmv");
    wxString pcl = docu + wxT(".pcl");
    wxFileName fn;
    fn.Assign(pdf);
    wxString finalPath = fn.GetPath() +
                         fn.GetPathSeparator() +
                         wxT("Enviados") +
                         fn.GetPathSeparator();
    wxString destination;
    destination = finalPath + fn.GetName() + wxT(".pdf");
    wxCopyFile(pdf, destination, true);
    destination = finalPath + fn.GetName() + wxT(".cmv");
    wxCopyFile(cmv, destination, true);
    wxRemoveFile(cmv);
    wxRemoveFile(pcl);
    wxRemoveFile(pdf);
}