/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "crashreportersender.h"

#include <Windows.h> // No sé si és necessari
#include <map>
#include <string>
#include "../main/src_breakpad/common/windows/http_upload.h"


namespace udg {


bool CrashReporterSender::sendReport(QString url, QString minidumpPath, QHash<QString,QString> &options)
{
    std::map<std::wstring, std::wstring> parameters;
    // Afegim els parametres
    foreach(QString key, options.keys())
    {
        parameters[key.toStdWString()] = options.take(key).toStdWString();
    }
    
    // Enviem el report
    std::wstring reportCode;
    int httpResponse = 0;
    bool success = google_breakpad::HTTPUpload::SendRequest(url.toStdWString(),
                                                            parameters,
                                                            minidumpPath.toStdWString(),
                                                            L"upload_file_minidump",
                                                            &reportCode,
                                                            &httpResponse);
    
    return success;
    
}


};