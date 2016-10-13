#ifndef MGLOGGER_
#define MGLOGGER_

#include <wx/log.h>
#include <wx/ffile.h>
#include <wx/datetime.h>

class MgLogger
{
public:
    MgLogger();
    ~MgLogger();
    void add(const wxString& msg);

private:
    wxLog *logger;
    wxFFile *logFile;
    wxDateTime *logDate;
    wxString logMsg;
    int logDay;
    int logMonth;
    int logYear;
    int logHour;
    int logMin;
    int logSec;

};

#endif // MGLOGGER_
