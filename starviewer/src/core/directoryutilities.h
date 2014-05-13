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

#ifndef UDGDIRECTORYUTILITIES_H
#define UDGDIRECTORYUTILITIES_H

#include <QObject>

class QString;
class QDir;

namespace udg {

/**
    Classe que encapsula operacions d'utilitat amb directoris
  */
class DirectoryUtilities : public QObject {
Q_OBJECT
public:
    DirectoryUtilities();
    ~DirectoryUtilities();

    /// Esborra el contingut del directori i el directori passat per paràmetres
    /// @param directoryPath Path del directori a esborrar
    /// @param deleteRootDirectory Indica si s'ha d'esborrar només el contingu del directori o també el directori arrel passat per paràmetre. Si fals només
    /// s'esborra el contingut, si és cert s'esborra el contingut i el directori passat per paràmetre
    /// @return Indica si l'operacio s'ha realitzat amb èxit
    bool deleteDirectory(const QString &directoryPath, bool deleteRootDirectory);

    /// Copia el directori origen al directori destí
    static bool copyDirectory(const QString &sourceDirectory, const QString &sourceDestination);
    
    /// Ens indica si un directori està buit
    bool isDirectoryEmpty(const QString &directoryPath);

signals:
    void directoryDeleted();

private:
    bool removeDirectory(const QDir &dir, bool deleteRootDirectory);
};

}

#endif
