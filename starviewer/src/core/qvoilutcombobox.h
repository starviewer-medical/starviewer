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

#ifndef UDGQVOILUTCOMBOBOX_H
#define UDGQVOILUTCOMBOBOX_H

#include <QComboBox>

namespace udg {

// FWD declarations
class QCustomWindowLevelDialog;
class VoiLutPresetsToolData;
class VoiLut;

/**
    Combo box personalitzat per a escollir valors de window level predeterminats i personalitzats
  */
class QVoiLutComboBox : public QComboBox {
Q_OBJECT
public:
    QVoiLutComboBox(QWidget *parent = 0);
    ~QVoiLutComboBox();

    /// Li assignem la font de dades a partir de la qual obté els valors de window level
    /// @param windowLevelData Les dades en sí
    void setPresetsData(VoiLutPresetsToolData *voiLutData);

    /// Neteja el contingut del combo box i elimina qualsevol referència a un WindowLevelPresetsToolData
    void clearPresets();

public slots:
    /// Selecciona el preset indicat en el combo, però no l'activa
    /// @param preset Descripció del preset
    void selectPreset(const QString &preset);
    void selectPreset(const VoiLut &preset);

private slots:
    /// Adds/removes a preset from the list
    void addPreset(const VoiLut &preset);
    void removePreset(const VoiLut &preset);
    
    /// Seleccionem el window level que volem aplicar com a actiu
    void setActiveVoiLut(const QString &text);

    /// Sets a custom window level to the VOI LUT data.
    void setCustomWindowLevel(double width, double center);

private:
    /// Afegeix o elimina un preset de la llista
    /// @param preset Preset a afegir o eliminar
    void removePreset(const QString &preset);
    
    /// Omple el combo a partir de les dades de presets. Neteja les dades que hi pugui haver anteriorment
    void populateFromPresetsData();

    /// Desconnecta totes les connexions
    void disconnectPresetsData();

private:
    /// Diàleg per escollir un window level personalitzat
    QCustomWindowLevelDialog *m_customWindowLevelDialog;

    /// Les dades dels valors pre-establerts
    VoiLutPresetsToolData *m_presetsData;

    /// Ens guardem el preset actual per reestablir-lo si l'usuari clica l'editor de WW/WL ja que sinó quedaria la fila de l'editor seleccionada.
    QString m_currentSelectedPreset;
};

}

#endif
