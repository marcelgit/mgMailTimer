#include "MgLogger.h"

MgLogger::MgLogger()
{
    logFile = new wxFFile(wxT("mgMailTimer.log"), wxT("a"));
    logger = new wxLogStderr(logFile->fp());
    wxLog::SetActiveTarget(logger);
    //logMsg = new wxString;
    logDate = new wxDateTime( wxDateTime::Today() );
    wxString msg;
    msg = wxT("Inicio de registro");
    add(msg);
}

MgLogger::~MgLogger()
{
    logger->Flush();
    logFile->Close();
    delete logFile;
    delete logger;
    //delete logMsg;
    delete logDate;
}

void MgLogger::add(const wxString& msg)
{
    logDay  = logDate->GetDay();
    logMonth= logDate->GetMonth() + 1;
    logYear = logDate->GetYear();
    logHour = logDate->GetHour();
    logMin  = logDate->GetMinute();
    logSec  = logDate->GetSecond();
    logMsg = wxString::Format("%02d/%02d/%04d - %s", 
                                logDay,
                                logMonth,
                                logYear,
                                msg);
    wxLogMessage(logMsg);
}
