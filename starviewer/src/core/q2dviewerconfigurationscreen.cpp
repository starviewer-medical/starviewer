#include "q2dviewerconfigurationscreen.h"

#include "coresettings.h"

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
    m_referenceLinesMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForMR).toBool());
    m_referenceLinesCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerReferenceLinesForCT).toBool());
}

void Q2DViewerConfigurationScreen::createConnections()
{
    connect(m_sliceScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updateSliceScrollLoopSetting(bool)));
    connect(m_phaseScrollLoopCheckBox, SIGNAL(toggled(bool)), SLOT(updatePhaseScrollLoopSetting(bool)));
    connect(m_referenceLinesMRCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForMRSetting(bool)));
    connect(m_referenceLinesCTCheckBox, SIGNAL(toggled(bool)), SLOT(updateReferenceLinesForCTSetting(bool)));
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

}
