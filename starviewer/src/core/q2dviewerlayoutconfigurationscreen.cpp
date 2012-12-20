#include "q2dviewerlayoutconfigurationscreen.h"

#include "coresettings.h"
#include "studylayoutconfigsettingsmanager.h"
#include "studylayoutconfig.h"
#include "studylayoutconfigsloader.h"

namespace udg {

Q2DViewerLayoutConfigurationScreen::Q2DViewerLayoutConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    initialize();
}

Q2DViewerLayoutConfigurationScreen::~Q2DViewerLayoutConfigurationScreen()
{
}

void Q2DViewerLayoutConfigurationScreen::initialize()
{
    m_applyHPOnModalitiesGroupBox->enableAllModalitiesCheckBox(false);
    m_applyHPOnModalitiesGroupBox->enableOtherModalitiesCheckBox(false);
    m_applyHPOnModalitiesGroupBox->setExclusive(false);

    populateMaxViewersComboBox();
    initializeModalitiesToApplyHangingProtocolsByDefault();
    initializeAutomaticLayoutSettings();
}

void Q2DViewerLayoutConfigurationScreen::populateMaxViewersComboBox()
{
    // TODO S'hauria d'omplir d'acord amb el que ens pot retornar OptimalViewersGridEstimator
    m_maxViewersComboBox->addItem(tr("As much as possible"));
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

void Q2DViewerLayoutConfigurationScreen::initializeModalitiesToApplyHangingProtocolsByDefault()
{
    Settings settings;
    
    m_applyHPOnModalitiesGroupBox->checkModalities(settings.getValue(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption).toString().split(";", QString::SkipEmptyParts));
}

void Q2DViewerLayoutConfigurationScreen::initializeAutomaticLayoutSettings()
{
    StudyLayoutConfigSettingsManager settingsManager;

    m_modalityComboBox->clear();
    QList<StudyLayoutConfig> layoutConfigList = settingsManager.getConfigList();
    foreach (const StudyLayoutConfig &config, layoutConfigList)
    {
        m_modalityComboBox->addItem(config.getModality());
    }
    connect(m_modalityComboBox, SIGNAL(activated(QString)), SLOT(updateLayoutSettingsWidgetForModality(QString)));
    updateLayoutSettingsWidgetForModality(m_modalityComboBox->currentText());

    connect(m_updateButton, SIGNAL(clicked()), SLOT(updateSelectedAutomaticLayoutSettings()));
    connect(m_restoreDefaultsButton, SIGNAL(clicked()), SLOT(restoreDefaultsForSelectedAutomaticLayoutSettings()));
}

void Q2DViewerLayoutConfigurationScreen::setCurrentStudyLayoutConfig(const StudyLayoutConfig &config)
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

        // TODO Falta error checking! Si el valor no està a la lista què fem? Trobar el valor més proper dels que hi ha a la llista?
    }
}

void Q2DViewerLayoutConfigurationScreen::applyChanges()
{
    Settings settings;
    
    settings.setValue(CoreSettings::ModalitiesToApplyHangingProtocolsAsFirstOption, m_applyHPOnModalitiesGroupBox->getCheckedModalities().join(";"));
}

void Q2DViewerLayoutConfigurationScreen::updateLayoutSettingsWidgetForModality(const QString &modality)
{
    StudyLayoutConfigSettingsManager settingsManager;
    QList<StudyLayoutConfig> layoutConfigList = settingsManager.getConfigList();
    foreach (const StudyLayoutConfig &config, layoutConfigList)
    {
        if (config.getModality() == modality)
        {
            setCurrentStudyLayoutConfig(config);
            break;
        }
    }
}

void Q2DViewerLayoutConfigurationScreen::updateSelectedAutomaticLayoutSettings()
{
    StudyLayoutConfig currentConfig;

    currentConfig.setModality(m_modalityComboBox->currentText());

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

void Q2DViewerLayoutConfigurationScreen::restoreDefaultsForSelectedAutomaticLayoutSettings()
{
    StudyLayoutConfig currentConfig = StudyLayoutConfigsLoader::getDefaultConfigForModality(m_modalityComboBox->currentText());
    StudyLayoutConfigSettingsManager settingsManager;
    settingsManager.updateItem(currentConfig);
    setCurrentStudyLayoutConfig(currentConfig);
}

}
