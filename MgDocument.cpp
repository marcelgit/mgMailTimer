#include <wx/ffile.h>
#include <wx/arrstr.h>
#include "MgDocument.h"

MgDocument::MgDocument(const wxString& docu)
{
    docuFile = docu;
    wxString nameAndExt = wxString::Format("%s.%s", docu, wxT("cmv"));
    wxFFile fileCmv( nameAndExt, "r");
    wxString dataCmv;
    if ( !fileCmv.IsOpened() || !fileCmv.ReadAll(&dataCmv) )
    {
        // MyLog error al leer cmv
//        wxLogMessage("Error opening \"%s\".\n", nameAndExt);
        return;
    }
    size_t offset = 0;
    while (offset < dataCmv.Len())
    {
        wxString sub_line = GetSubLine(dataCmv, offset);
        interpretaLinea(sub_line);
    }
}

wxString MgDocument::GetSubLine(wxString &from, size_t &offset)
{

    //Coded by Torben Bruchhaus @ 2014
    //Copies whatever content starts at "offset" and stops
    //at first encountered line break, returning the copied,
    //adjusting "offset" to the beginning of the next line.

    //We do not care about empty strings
    if (from.Len() == 0) return from;

    //Handy variable
    wxString res;

    for (size_t i = offset; i < from.Len(); i++)
    {

        //Get current char
        wxUniChar cc = from.GetChar(i);

        if ((cc == '\n') || (cc == '\r'))
        {

            //Line break character found - adjust the offset to
            //whatever comes after "\n" or "\r" or "\r\n" or "\n\r"

            offset = i + 1;

            if (offset < from.Len() - 1)
            {

                if ((cc == '\r') && (from.GetChar(offset) == '\n')) offset++; //CR+LF
                else if ((cc == '\n') && (from.GetChar(offset) == '\r')) offset++; //LF+CR

            }

            //Return sub line
            return res;

        } else res << cc; //Add char to result

    }

    //No line breaks found. Set "offset" to length of "from"
    offset = from.Len();

    //Return result
    return res;

}

void MgDocument::interpretaLinea(const wxString& sub_line)
{
    wxChar sep('=');
    wxArrayString arrLine = wxSplit(sub_line, sep);
    docuBody = wxT("Un archivo adjunto.\r\n");
    if ( arrLine[0] == wxT("E_FROM") )
    {
        docuFrom = arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_TO") )
    {
        docuTo = arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_CC") )
    {
        docuCC = arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_ASUNTO") )
    {
        docuSubject = arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_CUERPO") )
    {
        docuBody += arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_SERVIDOR") )
    {
        docuServer = arrLine[1];
    }
    else if ( arrLine[0] == wxT("E_PASSWORD") )
    {
        docuPassword = decriptPassword(arrLine[1]);
    }
    else if ( arrLine[0] == wxT("E_USUARIO") )
    {
        docuSender = arrLine[1];
    }
}

wxString MgDocument::decriptPassword(const wxString& str)
{
    wxString res;
    res.Clear();
    int i = -1;
    //size_t s = 1;
    //for( wxChar car = str.begin(); str.end(); )
    for( wchar_t car : str )
    {
        if ( car > 33)
        {
            wxChar carstr(car+i);
            //res.Append(car+i, s);
            res<<carstr;
            if ( i < 0)
                i = 1;
            else
                i = -1;
        }
    }
    return res;
}