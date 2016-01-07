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

#include "qwidgetselectpacstostoredicomimage.h"

#include <QMessageBox>

#include "pacsdevice.h"
#include "starviewerapplication.h"

namespace udg {

QWidgetSelectPacsToStoreDicomImage::QWidgetSelectPacsToStoreDicomImage(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    createConnections();

    /// Indiquem que només volem que es mostrin els PACS que tinguin configurat el servei Store
    m_pacsNodeList->setFilterPACSByService(PacsDeviceManager::PacsWithStoreServiceEnabled);
    m_pacsNodeList->setShowQueryPacsDefaultHighlighted(false);
}

void QWidgetSelectPacsToStoreDicomImage::createConnections()
{
    connect(m_pacsNodeList, SIGNAL(pacsSelectionChanged()), SLOT(updateStoreButton()));
    connect(m_storeButton, SIGNAL(clicked()), SLOT(storeImagesToSelectedPacs()));
    connect(m_cancelButton, SIGNAL(clicked()), SLOT(close()));
}

QList<PacsDevice> QWidgetSelectPacsToStoreDicomImage::getSelectedPacsToStoreDicomImages()
{
    return m_pacsNodeList->getSelectedPacs();
}

void QWidgetSelectPacsToStoreDicomImage::updateStoreButton()
{
    m_storeButton->setDisabled(m_pacsNodeList->getSelectedPacs().isEmpty());
}

void QWidgetSelectPacsToStoreDicomImage::storeImagesToSelectedPacs()
{
    emit selectedPacsToStore();
    close();
}

void QWidgetSelectPacsToStoreDicomImage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    m_pacsNodeList->clearSelection();
    m_pacsNodeList->refresh();
}

}
