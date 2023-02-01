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

#include "externalapplication.h"

#include "logging.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

namespace udg {


ExternalApplication::ExternalApplication(QString name, QString url, ExternalApplicationType type)
{
    this->setName(name);
    this->setUrl(url);
    this->setType(type);
}

ExternalApplication::~ExternalApplication()
{

}

void ExternalApplication::setName(QString name)
{
    m_name = name;
}

void ExternalApplication::setUrl(QString url)
{
    m_url = url;
}

void ExternalApplication::setType(ExternalApplicationType type)
{
    m_type = type;
}

QString ExternalApplication::getName() const
{
    return m_name;
}

QString ExternalApplication::getUrl() const
{
    return m_url;
}

ExternalApplication::ExternalApplicationType ExternalApplication::getType() const
{
    return m_type;
}

QString ExternalApplication::getReplacedUrl(const QHash<QString,QString> &replacements) const
{
    if (m_url.isNull()) 
    {
        return QString();
    }

    QString replacedUrl;
    int begin = 0;
    while(true) 
    {
        int fieldStart = m_url.indexOf("{%", begin);
        if (fieldStart < 0) //start delimiter not found
        {
            break;
        }
        replacedUrl += m_url.midRef(begin, fieldStart-begin); //The string from where we where to the current delimiter
        fieldStart += 2; //Skip the two characters of the delimiter

        int fieldEnd = m_url.indexOf("%}", fieldStart);
        if (fieldEnd < 0) //end delimiter not found
        {
            break;
        }

        const QString &fieldName = m_url.mid(fieldStart, fieldEnd-fieldStart);
        if (this->getType() == ExternalApplicationType::Url)
        {
            replacedUrl += QUrl::toPercentEncoding(replacements[fieldName]);
        }
        else
        {
            replacedUrl += replacements[fieldName];
        }
        fieldEnd += 2; //Skip the two characters of the delimiter
        begin = fieldEnd;
    }
    replacedUrl += m_url.midRef(begin);
    return replacedUrl;
}

bool ExternalApplication::launch(const QHash<QString,QString> &replacements) const
{
    bool success = false;
    const QString& replacedUrl = this->getReplacedUrl(replacements);
    if (this->getType() == Url)
    {
        INFO_LOG("Opening URL " + replacedUrl);
        const QUrl& url = QUrl(replacedUrl);
        success = QDesktopServices::openUrl(url);
    }
    else if (this->getType() == Command)
    {
        if ((success = QProcess::startDetached(replacedUrl))) //Ninja code: assignation inside an if condition.
        {
            INFO_LOG("Running command " + replacedUrl);
        }
        else
        {
            ERROR_LOG("Command " + replacedUrl + " could not be started.");
        }
    }
    return success;
}

}
