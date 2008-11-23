/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "crashreportersender.h"

#include <string>
#include <map>

#include "../main/src_breakpad/common/linux/http_upload.h"

namespace udg {

bool CrashReporterSender::sendReport(QString url, QString minidumpPath, QHash<QString,QString> &options)
{
    std::map<std::string, std::string> parameters;
    // Afegim els parametres
    foreach(QString key, options.keys())
    {
        parameters[key.toStdString()] = options.take(key).toStdString();
    }

    // Enviem el report
    std::string response, error;
    bool success = google_breakpad::HTTPUpload::SendRequest(url.toStdString(),
                                                            parameters,
                                                            minidumpPath.toStdString(),
                                                            "upload_file_minidump",
                                                            "", //proxy
                                                            "", //proxy_auth
                                                            &response,
                                                            &error);

    if (success) {
        printf("Successfully sent the minidump file.\n");
    } else {
        printf("Failed to send minidump: %s\n", error.c_str());
        printf("Response:\n");
        printf("%s\n", response.c_str());
    }
    
    return success;
    
}


};