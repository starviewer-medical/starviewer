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

#ifndef UDGCUSTOMWINDOWLEVELSREADER_H
#define UDGCUSTOMWINDOWLEVELSREADER_H

#include <QXmlSchemaValidator>
#include <QList>

class QXmlStreamReader;

namespace udg {

class WindowLevel;

/**
    Classe encarregada de llegir els fitxers XML que contenen la llista de custom window levels.
    També és l'encarregada de crear els diferent objectes WindowLevel.
  */
class CustomWindowLevelsReader {

public:
    CustomWindowLevelsReader();

    ~CustomWindowLevelsReader();

    /// Mètode que llegeix un fitxer XML i retorna un objecte WindowLevel.
    QList<WindowLevel*> readFile(const QString &path);

private:
    /// Validador que s'utilitza a l'hora de llegir els fitxers
    QXmlSchemaValidator m_customWindowLevelsSchemaValidator;
};

}

#endif
