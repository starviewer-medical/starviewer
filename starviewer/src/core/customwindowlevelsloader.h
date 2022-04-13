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

#ifndef UDGCUSTOMWINDOWLEVELSLOADER_H
#define UDGCUSTOMWINDOWLEVELSLOADER_H

#include <QFileInfo>
#include <QStringList>

namespace udg {

/**
    Classe encarregada de recuperar els arxius XML que contenen la informació referent als custom window levels
  */
class CustomWindowLevelsLoader {

public:
    CustomWindowLevelsLoader();

    ~CustomWindowLevelsLoader();

    /// Mètode que carrega els arxius XML definits a una adreça per defecte. (Hardcode)
    void loadDefaults();

    /// Mètode que carrega els arxius XML que contenen la informació dels custom window levels. El paràmetre pot ser el path d'un Directori o Fitxer.
    void loadXMLFiles(const QString &path);

private:
    /// Mètodes privats per fer la lectura dels fitxers de custom window levels
    /// Retorna una llista amb els fitxers llegits
    QStringList loadXMLFiles(const QFileInfo &fileInfo);
    /// Retorna el nom del fitxer llegit
    QString loadXMLFile(const QFileInfo &fileInfo);
};

}

#endif
