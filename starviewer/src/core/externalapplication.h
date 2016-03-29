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

#ifndef EXTERNALAPPLICATION_H
#define EXTERNALAPPLICATION_H

#include <QHash>
#include <QString>

namespace udg {

/**
 * @brief The ExternalApplication class made with an Url and a name.
 *
 * The name, is the name of the external application, this name will be displayed on the external applications menu.
 *
 * The concept of an external application is launching an URL with the system default browser and replacing some URL parameters or fields with information of
 * the currently opened study.
 *
 * Fields are defined on the url with this style: {%fieldName%}.
 *
 * When the application is launched a hash with the key as the field name and the value as the value of the field has to be passed in order to do the
 * substitution.
 *
 */
class ExternalApplication
{
public:
    enum ExternalApplicationType { Url, Command };

    explicit ExternalApplication(QString name = QString(), QString url = QString(), ExternalApplicationType type = Url);
    ~ExternalApplication();

    void setName(QString name);
    QString getName() const;
    void setUrl(QString url);
    /// @return Url with fields {%fieldName%}.
    QString getUrl() const;
    /// @brief Returns an URL with the fields replaced.
    ///
    /// Given a hash map with the field names and its values, the funcion searches for the fields written as {%fieldName%} and replaces them with the value on
    /// the hash map.
    ///
    /// If the type is set to URL, parameters will pass a percent encoding (url encoding) to sanitize them.
    ///
    /// @param replacements Hash map where the key is the field name.
    /// @return URL with the parameters replaced.
    QString getReplacedUrl(const QHash<QString,QString> &replacements) const;
    void setType(ExternalApplicationType type);
    /// @brief Application type, which can be an URL or a command.
    ExternalApplicationType getType() const;

    /// @brief Launch the URL with the system default browser.
    /// 
    /// The launched URL is given by the method getReplacedUrl().
    /// 
    /// If the type is URL, then the system default browser is launched.
    /// 
    /// If the type is Cmd, then the URL is run as a command.
    ///
    /// @param replacements Hash map where the key is the field name.
    /// @return Returns true if launch has been successful.
    bool launch(const QHash<QString, QString> &replacements = QHash<QString, QString>()) const;

private:
    QString m_name;
    QString m_url;
    ExternalApplicationType m_type;

};

}

#endif // EXTERNALAPPLICATION_H
