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

#ifndef UDGQ2DVIEWERCONFIGURATIONSCREEN_H
#define UDGQ2DVIEWERCONFIGURATIONSCREEN_H

#include "ui_q2dviewerconfigurationscreenbase.h"

namespace udg {

/**
    Pantalla de configuració del visor 2D
 */
class Q2DViewerConfigurationScreen : public QWidget, private Ui::Q2DViewerConfigurationScreenBase {
Q_OBJECT
public:
    Q2DViewerConfigurationScreen(QWidget *parent = 0);
    ~Q2DViewerConfigurationScreen();

private:
    /// Inicialitza el widget amb els valors corresponents
    void initialize();
    
    /// Crea les connexions corresponents de signals i slots
    void createConnections();

    /// Updates the checkboxes of the specified modalities group box according to values of the given setting
    void initializeModalitiesGroupBox(const QString &settingName, QModalitiesSelectorGroupBox *groupBox);
    
    /// Actualitza quin radio button del factor de zoom de lupa cal activar
    void initializeMagnifyingGlassToolZoomFactor();

    /// Updates which measurement verbosity radio button should be checked
    void initializeMeasurementsVerbosity();
    
    /// Updates which SUV measurement type radio button should be checked
    void initializeSUVMeasurementType();

private slots:
    /// Es cridaran quan es modifiquin els check box actualitzant els corresponents settings
    void updateSliceScrollLoopSetting(bool enable);
    void updatePhaseScrollLoopSetting(bool enable);
    void updateWheelVolumeScrollSetting(bool enable);
    void updateMouseWraparoundSetting(bool enable);
    void updateReferenceLinesForMRSetting(bool enable);
    void updateReferenceLinesForCTSetting(bool enable);
    void updateModalitiesWithZoomByDefaultSetting(const QStringList &modalities);
    void updateModalitiesWithPropagationByDefaultSetting(const QStringList &modalities);
    void updateMagnifyingGlassZoomFactorSetting();
    void updateMeasurementVerbositySetting();
    void updateSUVMeasurementTypeSetting();
    void updateAutomaticSynchronizationForMRSetting(bool enable);
    void updateAutomaticSynchronizationForCTSetting(bool enable);
};

}

#endif
