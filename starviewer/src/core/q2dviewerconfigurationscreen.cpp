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

#include "q2dviewerconfigurationscreen.h"

#include "coresettings.h"
#include "measurementmanager.h"
#include "standarduptakevaluemeasurehandler.h"

namespace udg {

Q2DViewerConfigurationScreen::Q2DViewerConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    initialize();
    createConnections();
}

Q2DViewerConfigurationScreen::~Q2DViewerConfigurationScreen()
{
}

void Q2DViewerConfigurationScreen::initialize()
{
    Settings settings;
    
    m_sliceScrollLoopCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerSliceScrollLoop).toBool());
    m_phaseScrollLoopCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop).toBool());
    m_wheelVolumeScrollCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerWheelVolumeScroll).toBool());
    m_mouseWraparoundCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerMouseWraparound).toBool());
    m_referenceLinesMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR).toBool());
    m_referenceLinesCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT).toBool());
    m_automaticSynchronizationMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR).toBool());
    m_automaticSynchronizationCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT).toBool());

    initializeModalitiesGroupBox(CoreSettings::ModalitiesWithZoomToolByDefault, m_zoomByDefaultModalitiesGroupBox);
    initializeModalitiesGroupBox(CoreSettings::ModalitiesWithPropagationEnabledByDefault, m_propagationModalitiesByDefaultGroupBox);
    initializeMagnifyingGlassToolZoomFactor();
    initializeMeasurementsVerbosity();
    initializeSUVMeasurementType();
}

void Q2DViewerConfigurationScreen::createConnections()
{
    connect(m_sliceScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updateSliceScrollLoopSetting(bool)));
    connect(m_phaseScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updatePhaseScrollLoopSetting(bool)));
    connect(m_wheelVolumeScrollCheckBox, SIGNAL(toggled(bool)), SLOT(updateWheelVolumeScrollSetting(bool)));
    connect(m_mouseWraparoundCheckBox, SIGNAL(toggled(bool)), SLOT(updateMouseWraparoundSetting(bool)));
    connect(m_referenceLinesMRCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForMRSetting(bool)));
    connect(m_referenceLinesCTCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForCTSetting(bool)));
    connect(m_automaticSynchronizationMRCheckBox,SIGNAL(toggled(bool)), SLOT(updateAutomaticSynchronizationForMRSetting(bool)));
    connect(m_automaticSynchronizationCTCheckBox,SIGNAL(toggled(bool)), SLOT(updateAutomaticSynchronizationForCTSetting(bool)));

    connect(m_zoomByDefaultModalitiesGroupBox, SIGNAL(checkedModalitiesChanged(QStringList)), SLOT(updateModalitiesWithZoomByDefaultSetting(QStringList)));
    connect(m_propagationModalitiesByDefaultGroupBox, SIGNAL(checkedModalitiesChanged(QStringList)), SLOT(updateModalitiesWithPropagationByDefaultSetting(QStringList)));

    connect(m_1point5XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_2XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_4XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_6XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    connect(m_8XZoomFactorRadioButton, SIGNAL(clicked()), SLOT(updateMagnifyingGlassZoomFactorSetting()));
    
    connect(m_minimalVerbosityRadioButton, SIGNAL(clicked()), SLOT(updateMeasurementVerbositySetting()));
    connect(m_minimalExplicitVerbosityRadioButton, SIGNAL(clicked()), SLOT(updateMeasurementVerbositySetting()));
    connect(m_verboseVerbosityRadioButton, SIGNAL(clicked()), SLOT(updateMeasurementVerbositySetting()));
    connect(m_verboseExplicitVerbosityRadioButton, SIGNAL(clicked()), SLOT(updateMeasurementVerbositySetting()));

    connect(m_bodyWeightRadioButton, SIGNAL(clicked()), SLOT(updateSUVMeasurementTypeSetting()));
    connect(m_leanBodyMassRadioButton, SIGNAL(clicked()), SLOT(updateSUVMeasurementTypeSetting()));
    connect(m_bodySurfaceAreaRadioButton, SIGNAL(clicked()), SLOT(updateSUVMeasurementTypeSetting()));
}

void Q2DViewerConfigurationScreen::initializeModalitiesGroupBox(const QString &settingName, QModalitiesSelectorGroupBox *groupBox)
{
    if (!groupBox)
    {
        return;
    }
    
    groupBox->enableAllModalitiesCheckBox(false);
    groupBox->enableOtherModalitiesCheckBox(false);
    groupBox->setExclusive(false);
    
    Settings settings;
    QStringList modalitiesList = settings.getValueAsQStringList(settingName);
    groupBox->checkModalities(modalitiesList);
}

void Q2DViewerConfigurationScreen::initializeMagnifyingGlassToolZoomFactor()
{
    Settings settings;

    QString zoomFactor = settings.getValue(CoreSettings::MagnifyingGlassZoomFactor).toString();

    if (zoomFactor == "1.5")
    {
        m_1point5XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "2")
    {
        m_2XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "4")
    {
        m_4XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "6")
    {
        m_6XZoomFactorRadioButton->setChecked(true);
    }
    else if (zoomFactor == "8")
    {
        m_8XZoomFactorRadioButton->setChecked(true);
    }
    else
    {
        // Si no hi ha cap valor vàlid, l'augment serà 4x per defecte
        m_4XZoomFactorRadioButton->setChecked(true);
    }
}

void Q2DViewerConfigurationScreen::initializeMeasurementsVerbosity()
{
    MeasurementManager::MeasurementDisplayVerbosityType verbosity = MeasurementManager::getConfiguredDisplayVerbosity();

    switch (verbosity)
    {
        case MeasurementManager::Minimal:
            m_minimalVerbosityRadioButton->setChecked(true);
            break;

        case MeasurementManager::MinimalExplicit:
            m_minimalExplicitVerbosityRadioButton->setChecked(true);
            break;

        case MeasurementManager::Verbose:
            m_verboseVerbosityRadioButton->setChecked(true);
            break;

        case MeasurementManager::VerboseExplicit:
            m_verboseExplicitVerbosityRadioButton->setChecked(true);
            break;
    }
}

void Q2DViewerConfigurationScreen::initializeSUVMeasurementType()
{
    StandardUptakeValueMeasureHandler suvHandler;
    StandardUptakeValueMeasureHandler::FormulaType formula = suvHandler.getPreferredFormula();

    switch (formula)
    {
        case StandardUptakeValueMeasureHandler::BodyWeight:
            m_bodyWeightRadioButton->setChecked(true);
            break;

        case StandardUptakeValueMeasureHandler::LeanBodyMass:
            m_leanBodyMassRadioButton->setChecked(true);
            break;

        case StandardUptakeValueMeasureHandler::BodySurfaceArea:
            m_bodySurfaceAreaRadioButton->setChecked(true);
            break;
    }
}

void Q2DViewerConfigurationScreen::updateSliceScrollLoopSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerSliceScrollLoop, enable);
}

void Q2DViewerConfigurationScreen::updatePhaseScrollLoopSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerPhaseScrollLoop, enable);
}

void Q2DViewerConfigurationScreen::updateWheelVolumeScrollSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerWheelVolumeScroll, enable);
}

void Q2DViewerConfigurationScreen::updateMouseWraparoundSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerMouseWraparound, enable);
}

void Q2DViewerConfigurationScreen::updateReferenceLinesForMRSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR, enable);
}

void Q2DViewerConfigurationScreen::updateReferenceLinesForCTSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT, enable);
}

void Q2DViewerConfigurationScreen::updateAutomaticSynchronizationForMRSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR, enable);
}

void Q2DViewerConfigurationScreen::updateAutomaticSynchronizationForCTSetting(bool enable)
{
    Settings settings;

    settings.setValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT, enable);
}

void Q2DViewerConfigurationScreen::updateModalitiesWithZoomByDefaultSetting(const QStringList &modalities)
{
    Settings settings;
    QString modalitiesWithZoom = modalities.join(";");
    settings.setValue(CoreSettings::ModalitiesWithZoomToolByDefault, modalitiesWithZoom);
}

void Q2DViewerConfigurationScreen::updateModalitiesWithPropagationByDefaultSetting(const QStringList &modalities)
{
    Settings settings;
    QString modalitiesWithPropagation = modalities.join(";");
    settings.setValue(CoreSettings::ModalitiesWithPropagationEnabledByDefault, modalitiesWithPropagation);
}

void Q2DViewerConfigurationScreen::updateMagnifyingGlassZoomFactorSetting()
{
    QString zoomFactor;

    if (m_1point5XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "1.5";
    }
    else if (m_2XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "2";
    }
    else if (m_4XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "4";
    }
    else if (m_6XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "6";
    }
    else if (m_8XZoomFactorRadioButton->isChecked())
    {
        zoomFactor = "8";
    }
    else
    {
        // Si no hi ha cap botó marcat, l'augment serà 4x per defecte. Tot i això, això no hauria de passar mai
        zoomFactor = "4";
    }

    Settings settings;
    settings.setValue(CoreSettings::MagnifyingGlassZoomFactor, zoomFactor);
}

void Q2DViewerConfigurationScreen::updateMeasurementVerbositySetting()
{
    MeasurementManager::MeasurementDisplayVerbosityType verbosity = MeasurementManager::Minimal;

    if (m_minimalVerbosityRadioButton->isChecked())
    {
        verbosity = MeasurementManager::Minimal;
    }
    else if (m_minimalExplicitVerbosityRadioButton->isChecked())
    {
        verbosity = MeasurementManager::MinimalExplicit;
    }
    else if (m_verboseVerbosityRadioButton->isChecked())
    {
        verbosity = MeasurementManager::Verbose;
    }
    else if (m_verboseExplicitVerbosityRadioButton->isChecked())
    {
        verbosity = MeasurementManager::VerboseExplicit;
    }

    Settings settings;
    settings.setValue(CoreSettings::MeasurementDisplayVerbosity, MeasurementManager::getMeasurementDisplayVerbosityTypeAsQString(verbosity));
}

void Q2DViewerConfigurationScreen::updateSUVMeasurementTypeSetting()
{
    StandardUptakeValueMeasureHandler suvHandler;

    if (m_bodyWeightRadioButton->isChecked())
    {
        suvHandler.setPreferredFormula(StandardUptakeValueMeasureHandler::BodyWeight);
    }
    else if (m_leanBodyMassRadioButton->isChecked())
    {
        suvHandler.setPreferredFormula(StandardUptakeValueMeasureHandler::LeanBodyMass);
    }
    else if (m_bodySurfaceAreaRadioButton->isChecked())
    {
        suvHandler.setPreferredFormula(StandardUptakeValueMeasureHandler::BodySurfaceArea);
    }
}

}
