#ifndef MGDOCUMENT_H
#define MGDOCUMENT_H

/**
 * @class MgDocument
 * @author Alex
 * @date 10/09/2016
 * @file MgDocument.h
 * @brief Interpreta el archivo cmv
 * Ejemplo de fichero cmv
 * IMPRIMIR=NO
 * SELECCION=NO
 * EMAIL=SI
 * E_PLANTILLA=marce
 * E_TO=direcciondelcliente(at)dominiodelcliente(dot)com
 * E_FROM=administracion@pataleta.com
 * E_CC=
 * E_ASUNTO="Envio Facturas"
 * E_CUERPO=
 * E_SERVIDOR=smtp://mail.pataleta.com:587
 * E_USUARIO=administracion@pataleta.com
 * E_PASSWORD=encriptada
 * E_ENCRIPTADO=SI
 * VERSION=65
 * FAX=NO
 */
 
 #include <wx/string.h>
 
class MgDocument
{
public:
    MgDocument(const wxString& docu);
    wxString GetSubLine(wxString &from, size_t &offset);
    void interpretaLinea(const wxString& sub_line);
    wxString decriptPassword(const wxString& str);
    
    wxString docuFile;
    wxString docuTo;
    wxString docuCC;
    wxString docuFrom;
    wxString docuSender;
    wxString docuServer;
    wxString docuPassword;
    wxString docuSubject;
    wxString docuBody;
};

#endif // MGDOCUMENT_H
