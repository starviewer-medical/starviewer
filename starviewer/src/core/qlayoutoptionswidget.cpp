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

#include "qlayoutoptionswidget.h"

#include "coresettings.h"
#include "studylayoutconfigsettingsmanager.h"
#include "studylayoutconfig.h"
#include "studylayoutconfigsloader.h"

namespace udg {

QLayoutOptionsWidget::QLayoutOptionsWidget(const QString &modality, QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    m_modality = modality;
    initialize();
}

QLayoutOptionsWidget::~QLayoutOptionsWidget()
{
}

void QLayoutOptionsWidget::initialize()
{
    populateMaxViewersComboBox();
    setupConnections();
    updateDisplayedSettings();
}

void QLayoutOptionsWidget::populateMaxViewersComboBox()
{
    // TODO Should be filled according with the possible results returned by OptimalViewersGridEstimator
    m_maxViewersComboBox->addItem(tr("As many as possible"));
    m_maxViewersComboBox->addItem("2");
    m_maxViewersComboBox->addItem("4");
    m_maxViewersComboBox->addItem("6");
    m_maxViewersComboBox->addItem("8");
    m_maxViewersComboBox->addItem("12");
    m_maxViewersComboBox->addItem("16");
    m_maxViewersComboBox->addItem("18");
    m_maxViewersComboBox->addItem("20");
    m_maxViewersComboBox->addItem("24");
    m_maxViewersComboBox->addItem("28");
    m_maxViewersComboBox->addItem("30");
    m_maxViewersComboBox->addItem("32");
    m_maxViewersComboBox->addItem("36");
    m_maxViewersComboBox->addItem("40");
    m_maxViewersComboBox->addItem("42");
    m_maxViewersComboBox->addItem("48");
    m_maxViewersComboBox->setMaxVisibleItems(m_maxViewersComboBox->count());
}

void QLayoutOptionsWidget::setupConnections()
{
    connect(m_restoreDefaultsButton, SIGNAL(clicked()), SLOT(restoreAutomaticLayoutSettingsDefaults()));
    // Automatic settings update
    connect(m_unfoldBySeriesRadioButton, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_unfoldByImagesRadioButton, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_leftToRightRadioButton, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_topToBottomRadioButton, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_localizerCheckBox, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_surveyCheckBox, SIGNAL(clicked()), SLOT(updateAutomaticLayoutSettings()));
    connect(m_maxViewersComboBox, SIGNAL(currentIndexChanged(int)), SLOT(updateAutomaticLayoutSettings()));

    connect(m_applyHangingProtocolAsFirstOptionCheckBox, SIGNAL(clicked(bool)), SLOT(updateHangingProtocolSetting(bool)));
}

void QLayoutOptionsWidget::setStudyLayoutConfig(const StudyLayoutConfig &config)
{
    switch (config.getUnfoldType())
    {
        case StudyLayoutConfig::UnfoldSeries:
            m_unfoldBySeriesRadioButton->setChecked(true);
            break;

        case StudyLayoutConfig::UnfoldImages:
            m_unfoldByImagesRadioButton->setChecked(true);
            break;
    }

    switch (config.getUnfoldDirection())
    {
        case StudyLayoutConfig::LeftToRightFirst:
            m_leftToRightRadioButton->setChecked(true);
            break;

        case StudyLayoutConfig::TopToBottomFirst:
            m_topToBottomRadioButton->setChecked(true);
            break;
    }

    QList<StudyLayoutConfig::ExclusionCriteriaType> criteria = config.getExclusionCriteria();
    m_localizerCheckBox->setChecked(config.getExclusionCriteria().contains(StudyLayoutConfig::Localizer));
    m_surveyCheckBox->setChecked(config.getExclusionCriteria().contains(StudyLayoutConfig::Survey));

    int maxNumberOfViewers = config.getMaximumNumberOfViewers();
    if (maxNumberOfViewers == 0)
    {
        m_maxViewersComboBox->setCurrentIndex(0);
    }
    else
    {
        int index = m_maxViewersComboBox->findText(QString::number(maxNumberOfViewers));
        m_maxViewersComboBox->setCurrentIndex(index);

        // TODO Error check missing! What to do if value is not in the list? Figure out closer value from the list?
    }
}

void QLayoutOptionsWidget::updateDisplayedSettings()
{
    // Hanging protocol preference
    Settings settings;
    m_applyHangingProtocolAsFirstOptionCheckBox->setChecked(settings.getValueAsQStringList(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption).contains(m_modality));

    // Automatic layout options
    StudyLayoutConfigSettingsManager settingsManager;
    QList<StudyLayoutConfig> layoutConfigList = settingsManager.getConfigList();
    foreach (const StudyLayoutConfig &config, layoutConfigList)
    {
        if (config.getModality() == m_modality)
        {
            setStudyLayoutConfig(config);
            break;
        }
    }
}

void QLayoutOptionsWidget::updateAutomaticLayoutSettings()
{
    StudyLayoutConfig currentConfig;

    currentConfig.setModality(m_modality);

    StudyLayoutConfig::UnfoldType unfoldBy;
    if (m_unfoldBySeriesRadioButton->isChecked())
    {
        unfoldBy = StudyLayoutConfig::UnfoldSeries;
    }
    else if (m_unfoldByImagesRadioButton->isChecked())
    {
        unfoldBy = StudyLayoutConfig::UnfoldImages;
    }
    currentConfig.setUnfoldType(unfoldBy);
    
    StudyLayoutConfig::UnfoldDirectionType direction;
    if (m_leftToRightRadioButton->isChecked())
    {
        direction = StudyLayoutConfig::LeftToRightFirst;
    }
    else if (m_topToBottomRadioButton->isChecked())
    {
        direction = StudyLayoutConfig::TopToBottomFirst;
    }
    currentConfig.setUnfoldDirection(direction);

    if (m_localizerCheckBox->isChecked())
    {
        currentConfig.addExclusionCriteria(StudyLayoutConfig::Localizer);
    }
    if (m_surveyCheckBox->isChecked())
    {
        currentConfig.addExclusionCriteria(StudyLayoutConfig::Survey);
    }
    
    currentConfig.setMaximumNumberOfViewers(m_maxViewersComboBox->currentText().toInt());
    
    StudyLayoutConfigSettingsManager settingsManager;
    settingsManager.updateItem(currentConfig);
}

void QLayoutOptionsWidget::restoreAutomaticLayoutSettingsDefaults()
{
    StudyLayoutConfig currentConfig = StudyLayoutConfigsLoader::getDefaultConfigForModality(m_modality);
    StudyLayoutConfigSettingsManager settingsManager;
    settingsManager.updateItem(currentConfig);
    setStudyLayoutConfig(currentConfig);
}

void QLayoutOptionsWidget::updateHangingProtocolSetting(bool enable)
{
    Settings settings;
    QStringList currentModalities = settings.getValueAsQStringList(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption);
    
    if (enable)
    {
        // Add
        if (!currentModalities.contains(m_modality))
        {
            currentModalities << m_modality;
        }
    }
    else
    {
        // Remove
        currentModalities.removeAll(m_modality);
    }
    
    settings.setValue(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption, currentModalities.join(";"));
}

} // End namespace udg
