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

#ifndef UDGCUSTOMWINDOWLEVELSWRITER_H
#define UDGCUSTOMWINDOWLEVELSWRITER_H

class QString;

namespace udg {

/**
    Classe encarregada de llegir els custom window levels del repository
    i crear un fitxer XML per guardar-los.
  */
class CustomWindowLevelsWriter {

public:
    CustomWindowLevelsWriter();

    ~CustomWindowLevelsWriter();

    /// Mètode que crea i escriu un fitxer XML per guardar els custom window levels
    void write();

private:
    // Retorna el directori on s'escriurà el fitxer XML (customwindowlevels.xml)
    QString getPath();

private:
};

}

#endif
