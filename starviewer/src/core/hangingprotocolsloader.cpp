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

#include "hangingprotocolsloader.h"

#include "logging.h"
#include "hangingprotocol.h"
#include "hangingprotocollayout.h"
#include "hangingprotocolsrepository.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolxmlreader.h"
#include "identifier.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "coresettings.h"
// Qt's
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace udg {

HangingProtocolsLoader::HangingProtocolsLoader(QObject *parent)
 : QObject(parent)
{

}

HangingProtocolsLoader::~HangingProtocolsLoader()
{

}

void HangingProtocolsLoader::loadDefaults()
{
    /// Hanging protocols definits per defecte, agafa el directori de l'executable TODO això podria ser un setting més
    // Path linux
    QString defaultPath = "/etc/xdg/" + OrganizationNameString + "/" + ApplicationNameString + "/hangingprotocols/";
    if (!QFile::exists(defaultPath))
    {
        defaultPath = installationPath() + "/hangingprotocols/";
    }
    if (!QFile::exists(defaultPath))
    {
        defaultPath = sourcePath() + "/hangingprotocols/";
    }

    if (QFile::exists(defaultPath))
    {
        INFO_LOG(QString("Directori a on es van a buscar els hanging protocols per defecte: %1").arg(defaultPath));
        loadXMLFiles(defaultPath);
    }
    else
    {
        INFO_LOG(QString("El directori per defecte dels hanging protocols (%1) no existeix. No es carregaran.").arg(defaultPath));
    }

    /// Hanging protocols definits per l'usuari
    Settings systemSettings;
    QString userPath = systemSettings.getValue(CoreSettings::UserHangingProtocolsPath).toString();
    if (!userPath.isEmpty())
    {
        loadXMLFiles(userPath);
    }
}

void HangingProtocolsLoader::loadXMLFiles(const QString &path)
{
    QFileInfo fileToRead(path);
    QStringList hangingProtocolsLoaded = loadXMLFiles(fileToRead);

    INFO_LOG(QString("Hanging protocols carregats: %1").arg(hangingProtocolsLoaded.join(", ")));
}

QStringList HangingProtocolsLoader::loadXMLFiles(const QFileInfo &fileInfo)
{
    QStringList filesLoaded;

    if (fileInfo.isDir())
    {
        QDir directory(fileInfo.absoluteFilePath());
        foreach (const QFileInfo &file, directory.entryInfoList())
        {
            if ((file.fileName() != ".") && (file.fileName() != ".."))
            {
                filesLoaded << loadXMLFiles(file);
            }
        }
    }
    else
    {
        if (fileInfo.suffix() == "xml")
        {
            QString fileLoaded = loadXMLFile(fileInfo);
            if (!fileLoaded.isEmpty())
            {
                filesLoaded << fileLoaded;
            }
        }
    }
    return filesLoaded;
}

QString HangingProtocolsLoader::loadXMLFile(const QFileInfo &fileInfo)
{
    HangingProtocolXMLReader xmlReader;
    HangingProtocol *hangingProtocol = xmlReader.readFile(fileInfo.absoluteFilePath());

    if (hangingProtocol != NULL)
    {
        Identifier id = HangingProtocolsRepository::getRepository()->addItem(hangingProtocol);
        hangingProtocol->setIdentifier(id.getValue());
        return hangingProtocol->getName();
    }

    return QString();
}

}
