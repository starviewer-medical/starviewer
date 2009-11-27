#include "qwidgetselectpacstostoredicomimage.h"

#include <QMessageBox>

#include "pacsdevice.h"
#include "starviewerapplication.h"

namespace udg
{

QWidgetSelectPacsToStoreDicomImage::QWidgetSelectPacsToStoreDicomImage(QWidget *parent)
{
    setupUi( this );
    createConnections();
}

void QWidgetSelectPacsToStoreDicomImage::createConnections()
{
    connect(m_storeButton, SIGNAL(clicked()), SLOT(storeImagesToSelectedPacs()));  
    connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
}

QList<PacsDevice> QWidgetSelectPacsToStoreDicomImage::getSelectedPacsToStoreDicomImages()
{
    return m_pacsNodeList->getSelectedPacs();
}


void QWidgetSelectPacsToStoreDicomImage::storeImagesToSelectedPacs()
{
    if (m_pacsNodeList->getSelectedPacs().count() > 0)
    {
        emit selectedPacsToStore();

        close();
    }
    else
    {
        QMessageBox::warning(this, ApplicationNameString, "You have to select at least one PACS to store the images");
    }

}

void QWidgetSelectPacsToStoreDicomImage::showEvent(QShowEvent *event)
{
    m_pacsNodeList->clearSelection();
	m_pacsNodeList->refresh();
}

}