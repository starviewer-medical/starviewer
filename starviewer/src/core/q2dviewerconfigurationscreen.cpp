#include "q2dviewerconfigurationscreen.h"

#include "coresettings.h"

namespace udg {

Q2DViewerConfigurationScreen::Q2DViewerConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    initialize();
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
    m_automaticSynchronizationMRCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForMR).toBool());
    m_automaticSynchronizationCTCheckBox->setChecked(settings.getValue(CoreSettings::EnableQ2DViewerAutomaticSynchronizationForCT).toBool());

    initializeModalitiesWithZoomByDefault();
    initializeMagnifyingGlassToolZoomFactor();
}

void Q2DViewerConfigurationScreen::initializeModalitiesWithZoomByDefault()
{
    Settings settings;
    
    QStringList modalitiesWithZoomList = settings.getValue(CoreSettings::ModalitiesWithZoomToolByDefault).toString().split(";");

    if (modalitiesWithZoomList.contains("CR"))
    {
        m_zoomCRCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("CT"))
    {
        m_zoomCTCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("DX"))
    {
        m_zoomDXCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("ES"))
    {
        m_zoomESCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("MG"))
    {
        m_zoomMGCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("MR"))
    {
        m_zoomMRCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("NM"))
    {
        m_zoomOPCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("PT"))
    {
        m_zoomPTCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("RF"))
    {
        m_zoomRFCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("SC"))
    {
        m_zoomSCCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("US"))
    {
        m_zoomUSCheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("XA"))
    {
        m_zoomXACheckBox->setChecked(true);
    }
    if (modalitiesWithZoomList.contains("XC"))
    {
        m_zoomXCCheckBox->setChecked(true);
    }
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

void Q2DViewerConfigurationScreen::updateModalitiesWithZoomByDefaultSetting()
{
    QString modalitiesWithZoom;
    
    if (m_zoomCRCheckBox->isChecked())
    {
        modalitiesWithZoom += "CR;";
    }

    if (m_zoomCTCheckBox->isChecked())
    {
        modalitiesWithZoom += "CT;";
    }

    if (m_zoomDXCheckBox->isChecked())
    {
        modalitiesWithZoom += "DX;";
    }

    if (m_zoomESCheckBox->isChecked())
    {
        modalitiesWithZoom += "ES;";
    }

    if (m_zoomMGCheckBox->isChecked())
    {
        modalitiesWithZoom += "MG;";
    }

    if (m_zoomMRCheckBox->isChecked())
    {
        modalitiesWithZoom += "MR;";
    }

    if (m_zoomNMCheckBox->isChecked())
    {
        modalitiesWithZoom += "NM;";
    }

    if (m_zoomOPCheckBox->isChecked())
    {
        modalitiesWithZoom += "OP;";
    }

    if (m_zoomPTCheckBox->isChecked())
    {
        modalitiesWithZoom += "PT;";
    }

    if (m_zoomRFCheckBox->isChecked())
    {
        modalitiesWithZoom += "RF;";
    }

    if (m_zoomSCCheckBox->isChecked())
    {
        modalitiesWithZoom += "SC;";
    }

    if (m_zoomUSCheckBox->isChecked())
    {
        modalitiesWithZoom += "US;";
    }

    if (m_zoomXACheckBox->isChecked())
    {
        modalitiesWithZoom += "XA;";
    }

    if (m_zoomXCCheckBox->isChecked())
    {
        modalitiesWithZoom += "XC;";
    }

    Settings settings;
    settings.setValue(CoreSettings::ModalitiesWithZoomToolByDefault, modalitiesWithZoom);
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
        // Si no hi ha cap botó marcat, l'augment serà 4x per defecte. Tot i així, això no hauria de passar mai
        zoomFactor = "4";
    }

    Settings settings;
    settings.setValue(CoreSettings::MagnifyingGlassZoomFactor, zoomFactor);
}

void Q2DViewerConfigurationScreen::applyChanges()
{
    updateSliceScrollLoopSetting(m_sliceScrollLoopCheckBox->isChecked());
    updatePhaseScrollLoopSetting(m_phaseScrollLoopCheckBox->isChecked());
    updateReferenceLinesForMRSetting(m_referenceLinesMRCheckBox->isChecked());
    updateReferenceLinesForCTSetting(m_referenceLinesCTCheckBox->isChecked());
    updateModalitiesWithZoomByDefaultSetting();
    updateMagnifyingGlassZoomFactorSetting();
    updateAutomaticSynchronizationForMRSetting(m_automaticSynchronizationMRCheckBox->isChecked());
    updateAutomaticSynchronizationForCTSetting(m_automaticSynchronizationCTCheckBox->isChecked());
}

}
