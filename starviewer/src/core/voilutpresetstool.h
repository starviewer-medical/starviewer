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

#ifndef UDGVOILUTPRESETSTOOL_H
#define UDGVOILUTPRESETSTOOL_H

#include "tool.h"

#include <QStringList>
#include <QMap>

namespace udg {

class Q2DViewer;
class VoiLutPresetsToolData;

/**
    Tool per canviar window levels predefinits
  */
class VoiLutPresetsTool : public Tool {
Q_OBJECT
public:
    VoiLutPresetsTool(QViewer *viewer, QObject *parent = 0);
    ~VoiLutPresetsTool();

    void handleEvent(unsigned long eventID);

    /// Ens retorna el nombre de presets que té carregats
    int getNumberOfPresets() const;

    void setToolData(ToolData *toolData);

private:
    /// Aplica el window level predeterminat per la tecla donada
    /// @param key Caràcter rebut
    void applyPreset(char key);

private slots:
    /// Actualitza la llista de window/level definits per defecte
    void updateVoiLutData();

private:
    /// Dades de window level amb tots els presets
    VoiLutPresetsToolData *m_myToolData;

    /// Llista de presets predefinits
    QStringList m_standardPresets;

    /// Llista de presets per defecte del volum actual
    QStringList m_defaultPresets;

    /// Índex de l'últim preset per defecte escollit
    int m_defaultPresetsIndex;

    /// Aquest mapa ens donarà la relació entre el caràcter premut i l'índex corresponent dels presets
    QMap <char, int> m_characterIndexMap;
};

}

#endif
