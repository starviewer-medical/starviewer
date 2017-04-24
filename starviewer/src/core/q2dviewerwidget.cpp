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

#include "q2dviewerwidget.h"
#include "volume.h"
#include "logging.h"
#include "statswatcher.h"
#include "synchronizetool.h"
#include "toolproxy.h"
#include "image.h"
#include "qfusionbalancewidget.h"
#include "qfusionlayoutwidget.h"
#include "series.h"

#include <QAction>
#include <QMenu>
#include <QWidgetAction>

namespace udg {

Q2DViewerWidget::Q2DViewerWidget(QWidget *parent)
 : QStackedWidget(parent)
{
    setupUi(this);
    setAutoFillBackground(true);

    // Creació de l'acció del boto de sincronitzar.
    m_synchronizeButtonAction = new QAction(0);
    m_synchronizeButtonAction->setIcon(QIcon(":/images/icons/emblem-symbolic-link.svg"));
    m_synchronizeButtonAction->setText(tr("Enable manual synchronization in this viewer"));
    m_synchronizeButtonAction->setStatusTip(m_synchronizeButtonAction->text());
    m_synchronizeButtonAction->setCheckable(true);
    m_synchronizeButton->setDefaultAction(m_synchronizeButtonAction);
    m_synchronizeButton->setEnabled(false);

    // Set up fusion balance controls
    m_fusionBalanceWidget = new QFusionBalanceWidget(this);
    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(m_fusionBalanceWidget);
    QMenu *menu = new QMenu(this);
    menu->addAction(widgetAction);
    m_fusionBalanceToolButton->setMenu(menu);
    m_fusionBalanceToolButton->setMenuPosition(QEnhancedMenuToolButton::Above);
    m_fusionBalanceToolButton->setMenuAlignment(QEnhancedMenuToolButton::AlignRight);
    m_fusionBalanceToolButton->hide();

    // Set up fusion layout widget
    m_fusionLayoutWidget = new QFusionLayoutWidget(this);
    connect(m_2DView, SIGNAL(anatomicalViewChanged(AnatomicalPlane)), m_fusionLayoutWidget, SLOT(setCurrentAnatomicalPlane(AnatomicalPlane)));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x1FirstRequested()), SLOT(requestFusionLayout2x1First()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x1SecondRequested()), SLOT(requestFusionLayout2x1Second()));
    connect(m_fusionLayoutWidget, SIGNAL(layout3x1Requested()), SLOT(requestFusionLayout3x1()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x3FirstRequested()), SLOT(requestFusionLayout2x3First()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x3SecondRequested()), SLOT(requestFusionLayout2x3Second()));
    connect(m_fusionLayoutWidget, SIGNAL(layout3x3Requested()), SLOT(requestFusionLayout3x3()));
    widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(m_fusionLayoutWidget);
    menu = new QMenu(this);
    menu->addAction(widgetAction);
    connect(m_fusionLayoutWidget, SIGNAL(layout2x1FirstRequested()), menu, SLOT(close()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x1SecondRequested()), menu, SLOT(close()));
    connect(m_fusionLayoutWidget, SIGNAL(layout3x1Requested()), menu, SLOT(close()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x3FirstRequested()), menu, SLOT(close()));
    connect(m_fusionLayoutWidget, SIGNAL(layout2x3SecondRequested()), menu, SLOT(close()));
    connect(m_fusionLayoutWidget, SIGNAL(layout3x3Requested()), menu, SLOT(close()));
    m_fusionLayoutToolButton->setMenu(menu);
    m_fusionLayoutToolButton->setMenuPosition(QEnhancedMenuToolButton::Above);
    m_fusionLayoutToolButton->setMenuAlignment(QEnhancedMenuToolButton::AlignRight);
    m_fusionLayoutToolButton->hide();

    createConnections();
    m_viewText->setText(QString());

    this->setSliderBarWidgetsEnabledFromViewerStatus();

    m_statsWatcher = new StatsWatcher("Q2DViewerWidget", this);
    m_statsWatcher->addClicksCounter(m_synchronizeButton);
    m_statsWatcher->addSliderObserver(m_slider);
}

Q2DViewerWidget::~Q2DViewerWidget()
{
}

void Q2DViewerWidget::updateViewerSliceAccordingToSliderAction(int action)
{
    switch (action)
    {
        case QAbstractSlider::SliderMove:
        case QAbstractSlider::SliderPageStepAdd:
        case QAbstractSlider::SliderPageStepSub:
            m_2DView->setSlice(m_slider->sliderPosition());
            break;

        default:
            break;
    }
}

void Q2DViewerWidget::createConnections()
{
    connect(m_slider, SIGNAL(actionTriggered(int)), SLOT(updateViewerSliceAccordingToSliderAction(int)));
    connect(m_2DView, SIGNAL(sliceChanged(int)), m_slider, SLOT(setValue(int)));
    connect(m_2DView, SIGNAL(sliceChanged(int)), SLOT(updateProjectionLabel()));
    connect(m_2DView, SIGNAL(anatomicalViewChanged(AnatomicalPlane)), SLOT(updateProjectionLabel()));
    connect(m_2DView, SIGNAL(viewChanged(int)), SLOT(resetSliderRangeAndValue()));
    connect(m_2DView, &Q2DViewer::slabThicknessChanged, this, &Q2DViewerWidget::resetSliderRangeAndValue);

    // Quan seleccionem l'slider, també volem que el viewer quedi com a actiu/seleccionat
    connect(m_slider, SIGNAL(sliderPressed()), SLOT(setAsActiveViewer()));

    connect(m_2DView, SIGNAL (selected()), SLOT(emitSelectedViewer()));
    connect(m_2DView, SIGNAL(volumeChanged(Volume*)), SLOT(updateInput(Volume*)));

    connect(m_synchronizeButtonAction, SIGNAL(toggled(bool)), SLOT(enableSynchronization(bool)));

    connect(m_2DView, SIGNAL(viewerStatusChanged()), SLOT(setSliderBarWidgetsEnabledFromViewerStatus()));

    connect(m_fusionBalanceWidget, SIGNAL(balanceChanged(int)), m_2DView, SLOT(setFusionBalance(int)));
    connect(m_2DView, SIGNAL(volumeChanged(Volume*)), SLOT(resetFusionOptions()));

    connect(m_2DView, SIGNAL(doubleClicked()), SLOT(emitDoubleClicked()));
}

void Q2DViewerWidget::updateProjectionLabel()
{
    m_viewText->setText(m_2DView->getCurrentAnatomicalPlaneLabel());
}

void Q2DViewerWidget::setSliderBarWidgetsEnabledFromViewerStatus()
{
    if (m_2DView->getViewerStatus() == Q2DViewer::VisualizingVolume)
    {
        this->setSliderBarWidgetsEnabled(true);
    }
    else
    {
        this->setSliderBarWidgetsEnabled(false);
    }
}

void Q2DViewerWidget::setInput(Volume *input)
{
    m_2DView->setInput(input);
    updateProjectionLabel();
}

void Q2DViewerWidget::setInputAsynchronously(Volume *input, QViewerCommand *command)
{
    if (input)
    {
        m_2DView->setInputAsynchronously(input, command);
        updateProjectionLabel();
    }
}

void Q2DViewerWidget::updateInput(Volume *input)
{
    Q_UNUSED(input)
    m_synchronizeButton->setEnabled(true);
    m_slider->setMaximum(m_2DView->getMaximumSlice());
    updateProjectionLabel();
}

void Q2DViewerWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    setAsActiveViewer();
}

void Q2DViewerWidget::setAsActiveViewer()
{
    m_2DView->setActive(true);
}

void Q2DViewerWidget::emitSelectedViewer()
{
    emit selected(this);
}

void Q2DViewerWidget::setSelected(bool option)
{
    // Per defecte li donem l'aspecte de background que té l'aplicació en general
    // TODO podríem tenir a nivell d'aplicació centralitzat el tema de
    // gestió de les diferents paletes de l'aplicació
    QBrush brush = QApplication::palette().window();
    if (option)
    {
        // Si seleccionem el widget, li canviem el color de fons
        brush.setColor(QColor(85, 160, 255));
    }
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Active, QPalette::Window, brush);
    setPalette(palette);
    m_2DView->setActive(option);
}

Q2DViewer* Q2DViewerWidget::getViewer()
{
    return m_2DView;
}

bool Q2DViewerWidget::hasPhases()
{
    return m_2DView->hasPhases();
}

void Q2DViewerWidget::setDefaultAction(QAction *synchronizeAction)
{
    m_synchronizeButton->setDefaultAction(synchronizeAction);
}

void Q2DViewerWidget::enableSynchronization(bool enable)
{
    if (!enable)
    {
        m_synchronizeButtonAction->setIcon(QIcon(":/images/icons/emblem-symbolic-unlink.svg"));
        m_synchronizeButtonAction->setText(tr("Enable manual synchronization in this viewer"));
        m_synchronizeButtonAction->setStatusTip(m_synchronizeButtonAction->text());
    }
    else
    {
        m_synchronizeButtonAction->setIcon(QIcon(":/images/icons/emblem-symbolic-link.svg"));
        m_synchronizeButtonAction->setText(tr("Disable manual synchronization in this viewer"));
        m_synchronizeButtonAction->setStatusTip(m_synchronizeButtonAction->text());
    }
    
    if (m_synchronizeButtonAction->isChecked() != enable)
    {
        // Ens han invocat el mètode directament, no s'ha fet clicant el botó
        // Això farà invocar aquest mètode de nou passant per "l'else"
        m_synchronizeButtonAction->setChecked(enable);
    }
    else
    {
        SynchronizeTool *synchronizeTool = dynamic_cast<SynchronizeTool*>(getViewer()->getToolProxy()->getTool("SynchronizeTool"));
        if (synchronizeTool)
        {
            synchronizeTool->setEnabled(enable);
            emit manualSynchronizationStateChanged(enable);
        }
        else
        {
            DEBUG_LOG("El viewer no té registrada l'eina de sincronització, per tant no es pot activar la sincronització");
            // TODO deixar el botó en estat "un-checked"?
        }
    }
}

void Q2DViewerWidget::enableSynchronizationButton(bool enable)
{
    m_synchronizeButton->setEnabled(enable);
}

void Q2DViewerWidget::resetSliderRangeAndValue()
{
    m_slider->setMinimum(m_2DView->getMinimumSlice());
    m_slider->setMaximum(m_2DView->getMaximumSlice());
    m_slider->setValue(m_2DView->getCurrentSlice());
}

void Q2DViewerWidget::setSliderBarWidgetsEnabled(bool enabled)
{
    m_slider->setEnabled(enabled);
    m_synchronizeButtonAction->setEnabled(enabled);
    m_viewText->setEnabled(enabled);
}

void Q2DViewerWidget::resetFusionOptions()
{
    if (m_2DView->getNumberOfInputs() == 2 && m_2DView->getViewerStatus() == QViewer::VisualizingVolume)
    {
        m_fusionBalanceWidget->setBalance(50);
        m_fusionBalanceWidget->setFirstVolumeModality(m_2DView->getInput(0)->getModality());
        m_fusionBalanceWidget->setSecondVolumeModality(m_2DView->getInput(1)->getModality());
        m_fusionLayoutWidget->setCurrentAnatomicalPlane(m_2DView->getCurrentAnatomicalPlane());
        m_fusionLayoutWidget->setModalities(m_2DView->getInput(0)->getModality(), m_2DView->getInput(1)->getModality());
        m_fusionBalanceToolButton->show();
        m_fusionLayoutToolButton->show();
    }
    else
    {
        m_fusionBalanceToolButton->hide();
        m_fusionLayoutToolButton->hide();
    }
}

void Q2DViewerWidget::emitDoubleClicked()
{
    emit doubleClicked(this);
}

void Q2DViewerWidget::requestFusionLayout2x1First()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout2x1FirstRequested(m_2DView->getInputs(), m_2DView->getCurrentAnatomicalPlane());
    }
}

void Q2DViewerWidget::requestFusionLayout2x1Second()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout2x1SecondRequested(m_2DView->getInputs(), m_2DView->getCurrentAnatomicalPlane());
    }
}

void Q2DViewerWidget::requestFusionLayout3x1()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout3x1Requested(m_2DView->getInputs(), m_2DView->getCurrentAnatomicalPlane());
    }
}

void Q2DViewerWidget::requestFusionLayout2x3First()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout2x3FirstRequested(m_2DView->getInputs());
    }
}

void Q2DViewerWidget::requestFusionLayout2x3Second()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout2x3SecondRequested(m_2DView->getInputs());
    }
}

void Q2DViewerWidget::requestFusionLayout3x3()
{
    if (m_2DView->getNumberOfInputs() == 2)
    {
        emit fusionLayout3x3Requested(m_2DView->getInputs());
    }
}

}
