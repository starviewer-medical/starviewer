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

#ifndef UDGHANGINGPROTOCOLSLOADER_H
#define UDGHANGINGPROTOCOLSLOADER_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

namespace udg {

class HangingProtocolsLoader : public QObject {
Q_OBJECT
public:
    HangingProtocolsLoader(QObject *parent = 0);

    ~HangingProtocolsLoader();

    /// Càrrega de hanging protocols per defecte
    void loadDefaults();

    /// Càrrega des d'un directori de hanging protocols o un fitxer XML
    void loadXMLFiles(const QString &path);

private:
    /// Mètodes privats per fer la lectura dels hanging protocols
    QStringList loadXMLFiles(const QFileInfo &fileInfo);
    QString loadXMLFile(const QFileInfo &fileInfo);
};

}

#endif
