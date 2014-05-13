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

#include "qthickslabwidget.h"

#include "q2dviewer.h"
#include "volume.h"
#include "logging.h"

namespace udg {

QThickSlabWidget::QThickSlabWidget(QWidget *parent)
 : QWidget(parent), m_currentViewer(0)
{
    setupUi(this);
    // Omplim el combo amb els valors que volem
    m_projectionModeComboBox->clear();
    QStringList items;
    items << tr("Disabled") << tr("MIP") << tr("MinIP") << tr("Average");
    m_projectionModeComboBox->addItems(items);
    m_slabThicknessSlider->setEnabled(false);
    m_slabThicknessLabel->setEnabled(false);
    m_maximumThicknessCheckBox->setEnabled(false);
    m_projectionModeComboBox->setEnabled(false);
}

QThickSlabWidget::~QThickSlabWidget()
{
}

void QThickSlabWidget::link(Q2DViewer *viewer)
{
    if (!viewer)
    {
        DEBUG_LOG("Al tanto, ens passen un viewer NUL! Sortida immediata!");
        return;
    }

    if (!m_currentViewer)
    {
        m_currentViewer = viewer;
        // Si no teníem cap viewer fins ara creem les connexions necessàries
        connect(m_projectionModeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(applyProjectionMode(int)));
        connect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), SLOT(updateThicknessLabel(int)));
        connect(m_maximumThicknessCheckBox, SIGNAL(toggled(bool)), this, SLOT(enableVolumeMode(bool)));
    }
    else
    {
        // Primer deslinkem qualsevol altre viewer que tinguéssim linkat anteriorment
        disconnectSignalsAndSlots();
    }
    // Posem a punt el widget d'acord amb les dades del viewer
    m_currentViewer = viewer;
    // Aquests seran els valors per defecte si el thickslab no està activat
    int slabThickness = 2, projectionMode = 0;
    // Llavors cal reflexar els valors adequats
    if (m_currentViewer->isThickSlabActive())
    {
        slabThickness = m_currentViewer->getSlabThickness();
        projectionMode = m_currentViewer->getSlabProjectionMode() + 1;
    }
    updateMaximumThickness();
    m_slabThicknessSlider->setValue(slabThickness);
    m_projectionModeComboBox->setCurrentIndex(projectionMode);

    // Creem els vincles
    connect(m_currentViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_currentViewer, SIGNAL(viewChanged(int)), SLOT(onViewChanged()));
    connect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), SLOT(applyThickSlab()));

    // Enable the combo if the viewer has input and more than one slice; disable it otherwise
    if (m_currentViewer->hasInput() && m_currentViewer->getNumberOfSlices() > 1)
    {
        m_projectionModeComboBox->setEnabled(true);
    }
    else
    {
        m_projectionModeComboBox->setEnabled(false);
    }
}

void QThickSlabWidget::unlink()
{
    disconnectSignalsAndSlots();
    disconnect(m_projectionModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(applyProjectionMode(int)));
    m_slabThicknessSlider->setValue(2);
    m_projectionModeComboBox->setCurrentIndex(0);
    m_projectionModeComboBox->setEnabled(false);
    m_slabThicknessSlider->setEnabled(false);
    m_slabThicknessLabel->setEnabled(false);
    m_maximumThicknessCheckBox->setChecked(false);
    m_maximumThicknessCheckBox->setEnabled(false);
    m_currentViewer = 0;
}

void QThickSlabWidget::setProjectionMode(int mode)
{
    m_projectionModeComboBox->setCurrentIndex(mode);
}

void QThickSlabWidget::applyProjectionMode(int comboItem)
{
    emit projectionModeChanged(comboItem);
    QString projectionType = m_projectionModeComboBox->itemText(comboItem);
    if (projectionType == tr("Disabled"))
    {
        // Desconnectem qualsevol possible connexió
        disconnect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(applyThickSlab()));
        disconnect(m_slabThicknessSlider, SIGNAL(sliderPressed()), this, SLOT(turnOnDelayedUpdate()));
        disconnect(m_slabThicknessSlider, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));

        m_currentViewer->disableThickSlab();
        m_slabThicknessSlider->setEnabled(false);
        m_slabThicknessLabel->setEnabled(false);
        m_maximumThicknessCheckBox->setEnabled(false);
        // Restaurem manualment els valors per defecte
        m_slabThicknessSlider->setValue(2);
        updateThicknessLabel(2);
    }
    else
    {
        updateMaximumThickness();
        m_slabThicknessSlider->setEnabled(true);
        m_maximumThicknessCheckBox->setEnabled(true);

        if (!m_maximumThicknessCheckBox->isChecked())
        {
            m_slabThicknessLabel->setEnabled(true);
        }
        else
        {
            m_slabThicknessLabel->setEnabled(false);
        }

        // Fem que si avancem d'un en un el valor d'slab (amb teclat o amb la roda del ratolí)
        // s'actualitzi amb el signal valueChanged()
        // Si el valor es canvia arrossegant l'slider, canviem el comportament i no apliquem el nou
        // valor de thickness fins que no fem el release
        // Ho fem així, ja que al arrossegar es van enviant senyals de valueChanged i això feia que
        // la resposta de l'interfície fos una mica lenta, ja que calcular el nou slab és costós
        // TODO si el procés de l'slab anés amb threads no tindríem aquest problema
        turnOffDelayedUpdate();
        connect(m_slabThicknessSlider, SIGNAL(sliderPressed()), SLOT(turnOnDelayedUpdate()));

        // TODO ara fem la conversió a id d'enter, però en un futur anirà tot amb Strings
        int projectionModeID = -1;
        if (projectionType == tr("MIP"))
        {
            projectionModeID = 0;
        }
        else if (projectionType == tr("MinIP"))
        {
            projectionModeID = 1;
        }
        else if (projectionType == tr("Average"))
        {
            projectionModeID = 2;
        }

        // TODO per a donar sensació d'espera, canviem el cursor abans d'aplicar el thickslab i el restaurem quan s'acaba el procés. S'hauria de fer de manera
        // més centralizada per tal de que si es crida des de qualsevol lloc, es facin aquestes accions sobre el cursor, és a dir, que no calgui programar això
        // en cada lloc on s'apliqui thickslab.
        // Al canviar de mode de projecció haurem de recalcular
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_currentViewer->setSlabProjectionMode(projectionModeID);
        if (m_maximumThicknessCheckBox->isChecked())
        {
            m_slabThicknessSlider->setValue(m_currentViewer->getNumberOfSlices());
        }
        else
        {
            m_currentViewer->setSlabThickness(m_slabThicknessSlider->value());
        }

        QApplication::restoreOverrideCursor();

        // Cal mantenir l'slider disabled si el checkbox de màxim thickness està habilitat,
        // ja que al aplicar una projecció nova es pot tornar a habilitar al fer el setValue()
        if (m_maximumThicknessCheckBox->isChecked())
        {
            m_slabThicknessSlider->setEnabled(false);
        }
    }
}

void QThickSlabWidget::applyThickSlab()
{
    // TODO per a donar sensació d'espera, canviem el cursor abans d'aplicar el thickslab i el restaurem quan s'acaba el procés. S'hauria de fer de manera més
    // centralizada per tal de que si es crida des de qualsevol lloc, es facin aquestes accions sobre el cursor, és a dir, que no calgui programar això en cada
    // lloc on s'apliqui thickslab.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_currentViewer->setSlabThickness(m_slabThicknessSlider->value());
    QApplication::restoreOverrideCursor();
}

void QThickSlabWidget::turnOnDelayedUpdate()
{
    disconnect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(applyThickSlab()));
    connect(m_slabThicknessSlider, SIGNAL(sliderReleased()), SLOT(onSliderReleased()));
}

void QThickSlabWidget::turnOffDelayedUpdate()
{
    disconnect(m_slabThicknessSlider, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
    connect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), SLOT(applyThickSlab()));
}

void QThickSlabWidget::onSliderReleased()
{
    applyThickSlab();
    turnOffDelayedUpdate();
}

void QThickSlabWidget::updateMaximumThickness()
{
    m_slabThicknessSlider->setRange(2, m_currentViewer->getNumberOfSlices());
}

void QThickSlabWidget::updateThicknessLabel(int value)
{
    m_slabThicknessLabel->setText(QString::number(value));
}

void QThickSlabWidget::reset()
{
    m_currentViewer->disableThickSlab();
    this->link(m_currentViewer);
}

void QThickSlabWidget::onViewChanged()
{
    // Si no tenim cap projecció aplicada, no cal fer res
    if (m_projectionModeComboBox->currentText() == tr("Disabled"))
    {
        return;
    }

    // Disconnect valueChanged from applyThickSlab while changing the maximum because if it's reduced it can change the slider value, leading to an undesired
    // thickness (see #1982)
    disconnect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(applyThickSlab()));
    updateMaximumThickness();
    connect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), SLOT(applyThickSlab()));

    // Quan canviem de vista, sempre que tinguem la opció marcada, mantenim el thickness màxim per aquella vista
    if (m_maximumThicknessCheckBox->isChecked())
    {
        // Cal forçar que es faci el thickSlab des del viewer, ja que si canviem de vista i el nombre de llesques
        // a renderitzar és el mateix, no s'emetrà la senyal "valueChanged()" de l'slider i no s'aplicarà el thickSlab
        m_currentViewer->setSlabThickness(m_currentViewer->getNumberOfSlices());
        m_slabThicknessSlider->setValue(m_currentViewer->getNumberOfSlices());
    }
    else
    {
        int currentSlabThickness = m_currentViewer->getSlabThickness();
        if (currentSlabThickness > 1)
        {
            // Update the value of the slider with the current slab thickness
            m_slabThicknessSlider->setValue(m_currentViewer->getSlabThickness());
        }
        else
        {
            m_projectionModeComboBox->setCurrentIndex(0);
        }
    }
}

void QThickSlabWidget::enableVolumeMode(bool enable)
{
    if (enable)
    {
        m_slabThicknessSlider->setValue(m_currentViewer->getNumberOfSlices());
        m_slabThicknessSlider->setEnabled(false);
        m_slabThicknessLabel->setEnabled(false);
    }
    else
    {
        m_slabThicknessSlider->setEnabled(true);
        m_slabThicknessLabel->setEnabled(true);
    }

    emit maximumThicknessModeToggled(enable);
}

void QThickSlabWidget::disconnectSignalsAndSlots()
{
    if (m_currentViewer)
    {
        disconnect(m_currentViewer, 0, this, 0);
        disconnect(m_currentViewer, 0, m_slabThicknessSlider, 0);
        disconnect(m_slabThicknessSlider, SIGNAL(valueChanged(int)), this, SLOT(applyThickSlab()));
    }
}

}
