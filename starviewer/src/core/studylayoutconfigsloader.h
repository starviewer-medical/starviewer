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

#ifndef UDGSTUDYLAYOUTCONFIGSLOADER_H
#define UDGSTUDYLAYOUTCONFIGSLOADER_H

class QString;

namespace udg {

class StudyLayoutConfig;

class StudyLayoutConfigsLoader {
public:
    StudyLayoutConfigsLoader();
    ~StudyLayoutConfigsLoader();

    /// Carrega els valors que corresponen per defecte o que ja estiguin configurats per l'usuari
    void load();

    /// Retorna la configuració de fàbrica d'un layout corresponent a una modalitat
    static StudyLayoutConfig getDefaultConfigForModality(const QString &modality);
};

} // End namespace udg
#endif
