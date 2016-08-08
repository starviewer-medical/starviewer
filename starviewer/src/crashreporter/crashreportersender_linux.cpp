/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "crashreportersender.h"

#include <cstdio>
#include <string>
#include <map>

#include <common/linux/http_upload.h>
namespace udg {

bool CrashReporterSender::sendReport(QString url, QString minidumpPath, QHash<QString, QString> &options)
{
    std::map<std::string, std::string> parameters;
    // Afegim els parametres
    Q_FOREACH (QString key, options.keys())
    {
        parameters[key.toStdString()] = options.take(key).toStdString();
    }

    // Enviem el report
    std::string response, error;
    long responseCode;
    std::map<std::string, std::string> files;
    files["upload_file_minidump"] = minidumpPath.toStdString();
    /*bool success = google_breakpad::HTTPUpload::SendRequest(url.toStdString(),
                                                            parameters,
                                                            files,
                                                            "", //proxy
                                                            "", //proxy_user_pwd
                                                            "", //ca_certificate_file
                                                            &response,
                                                            &responseCode,
                                                            &error);
    if (success)
    {
        printf("Successfully sent the minidump file.\n");
    }
    else
    {
        printf("Failed to send minidump: %s\n", error.c_str());
        printf("Response:\n");
        printf("%s\n", response.c_str());
    }

    return success;
*/

}

};
