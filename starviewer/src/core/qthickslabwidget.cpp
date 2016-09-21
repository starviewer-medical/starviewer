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

#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"

#include <QMenu>
#include <QWidgetAction>

namespace udg {

QThickSlabWidget::QThickSlabWidget(QWidget *parent)
 : QWidget(parent), m_viewer(nullptr)
{
    setupUi(this);

    QMenu *menu = new QMenu(this);
    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(m_menuWidget);
    menu->addAction(widgetAction);
    m_toolButton->setMenu(menu);

    QStringList items;
    items << tr("MIP") << tr("MinIP") << tr("Average");
    m_projectionModeComboBox->addItems(items);

    this->setEnabled(false);

    connect(m_slabThicknessSlider, &QSlider::valueChanged, [this](int value) { m_slabThicknessLabel->setText(QString::number(value)); });
    connect(m_slabThicknessSlider, &QSlider::sliderPressed, [this] {
        disconnect(m_slabThicknessSlider, &QSlider::valueChanged, this, &QThickSlabWidget::applyThickness);
    });
    connect(m_slabThicknessSlider, &QSlider::sliderReleased, [this] {
        connect(m_slabThicknessSlider, &QSlider::valueChanged, this, &QThickSlabWidget::applyThickness);
        applyThickness(m_slabThicknessSlider->value());
    });
}

QThickSlabWidget::~QThickSlabWidget()
{
}

void QThickSlabWidget::link(Q2DViewer *viewer)
{
    if (!viewer)
    {
        return;
    }

    if (m_viewer)
    {
        removeConnections();
    }

    m_viewer = viewer;

    updateMaximumThickness();
    m_toolButton->setChecked(m_viewer->isThickSlabActive());
    m_projectionModeComboBox->setCurrentIndex(m_viewer->getSlabProjectionMode());   // TODO coupling
    m_slabThicknessSlider->setValue(m_viewer->getSlabThickness());
    setMaximumThicknessEnabled(m_maximumThicknessCheckBox->isChecked());

    createConnections();

    // Enable this widget if the viewer has input and more than one slice; disable it otherwise
    this->setEnabled(m_viewer->hasInput() && m_viewer->getNumberOfSlices() > 1);
}

void QThickSlabWidget::unlink()
{
    removeConnections();
    m_viewer = nullptr;
    this->setEnabled(false);
}

void QThickSlabWidget::createConnections()
{
    Q_ASSERT(m_viewer);

    connect(m_toolButton, &QToolButton::toggled, this, &QThickSlabWidget::setThickSlabEnabled);
    connect(m_projectionModeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &QThickSlabWidget::applyProjectionMode);
    connect(m_slabThicknessSlider, &QSlider::valueChanged, this, &QThickSlabWidget::applyThickness);
    connect(m_maximumThicknessCheckBox, &QCheckBox::toggled, this, &QThickSlabWidget::setMaximumThicknessEnabled);
    connect(m_viewer, &Q2DViewer::volumeChanged, this, &QThickSlabWidget::onVolumeChanged);
    connect(m_viewer, &Q2DViewer::viewChanged, this, &QThickSlabWidget::onViewChanged);
}

void QThickSlabWidget::removeConnections()
{
    Q_ASSERT(m_viewer);

    m_toolButton->disconnect();
    m_projectionModeComboBox->disconnect();
    m_slabThicknessSlider->disconnect(this);
    m_maximumThicknessCheckBox->disconnect();
    m_viewer->disconnect(this);
}

void QThickSlabWidget::updateMaximumThickness()
{
    Q_ASSERT(m_viewer);

    m_slabThicknessSlider->setRange(2, m_viewer->getNumberOfSlices());
}

bool QThickSlabWidget::isThickSlabEnabled() const
{
    return m_toolButton->isChecked();
}

void QThickSlabWidget::setThickSlabEnabled(bool enabled)
{
    Q_ASSERT(m_viewer);

    if (enabled)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        m_viewer->setSlabProjectionMode(m_projectionModeComboBox->currentIndex());  // TODO coupling
        m_viewer->setSlabThickness(m_slabThicknessSlider->value());

        QApplication::restoreOverrideCursor();
    }
    else
    {
        m_viewer->disableThickSlab();
    }
}

void QThickSlabWidget::applyProjectionMode(int projectionMode)
{
    Q_ASSERT(m_viewer);

    if (!isThickSlabEnabled())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_viewer->setSlabProjectionMode(projectionMode);    // TODO coupling

    QApplication::restoreOverrideCursor();
}

void QThickSlabWidget::applyThickness(int thickness)
{
    Q_ASSERT(m_viewer);

    if (!isThickSlabEnabled())
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    m_viewer->setSlabThickness(thickness);

    QApplication::restoreOverrideCursor();
}

void QThickSlabWidget::setMaximumThicknessEnabled(bool enabled)
{
    Q_ASSERT(m_viewer);

    if (enabled)
    {
        m_slabThicknessSlider->setValue(m_slabThicknessSlider->maximum());
        m_slabThicknessSlider->setEnabled(false);
        m_slabThicknessLabel->setEnabled(false);
    }
    else
    {
        m_slabThicknessSlider->setEnabled(true);
        m_slabThicknessLabel->setEnabled(true);
    }

    emit maximumThicknessModeToggled(enabled);
}

void QThickSlabWidget::onVolumeChanged()
{
    Q_ASSERT(m_viewer);

    this->link(m_viewer);
}

void QThickSlabWidget::onViewChanged()
{
    Q_ASSERT(m_viewer);

    this->link(m_viewer);
    setThickSlabEnabled(isThickSlabEnabled());
}

}
