#include "q2dviewerlayoutconfigurationscreen.h"

#include "qlayoutoptionswidget.h"

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
    QStringList modalities;
    modalities << "CR" << "CT" << "DX" << "ES" << "MG" << "MR" << "NM" << "OP" << "PT" << "RF" << "SC" << "US" << "XA" << "XC";
    foreach (const QString &modality, modalities)
    {
        m_modalitiesTabWidget->addTab(new QLayoutOptionsWidget(modality, m_modalitiesTabWidget), modality);
    }
}

}
