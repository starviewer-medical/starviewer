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

#ifndef UDGQTHICKSLABWIDGET_H
#define UDGQTHICKSLABWIDGET_H

#include "ui_qthickslabwidgetbase.h"

namespace udg {

class Q2DViewer;

/**
 * @brief The QThickSlabWidget class creates a tool button with a dropdown panel with the controls to change the thick slab properties of a Q2DViewer.
 */
class QThickSlabWidget : public QWidget, private Ui::QThickSlabWidgetBase {

    Q_OBJECT

public:
    explicit QThickSlabWidget(QWidget *parent = nullptr);
    virtual ~QThickSlabWidget();

    /// Links this widget to the given viewer, i.e. makes this widget control the thick slab properties of that viewer.
    /// If there is already a linked viewer, it is automatically unlinked first.
    void link(Q2DViewer *viewer);

    /// Unlinks this widget from the linked viewer.
    void unlink();

signals:
    /// Emitted when the maximum thickness checkbox changes its status.
    void maximumThicknessModeToggled(bool checked);

private:
    /// Creates the signal-slot connections.
    void createConnections();
    /// Removes the signal-slot connections.
    void removeConnections();

    /// Updates the maximum thickness allowed by the slider to match the number of slices of the linked viewer in the current direction of projection.
    void updateMaximumThickness();

    /// Returns true if thick slab is enabled by this widget.
    bool isThickSlabEnabled() const;

private slots:
    /// Enables or disables thick slab on the linked viewer.
    void setThickSlabEnabled(bool enabled);

    /// If thick slab is enabled, applies the given projection mode to the linked viewer.
    void applyProjectionMode(int projectionMode);

    /// If thick slab is enabled, applies the given thickness to the linked viewer.
    void applyThickness(int thickness);

    /// Sets the thickness to the maximum possible value, keeping it at the maximum even when the direction of projection changes.
    void setMaximumThicknessEnabled(bool enabled);

    /// Resets values after the linked viewer has changed its volume.
    void onVolumeChanged();

    /// Resets values to those of the viewer after it has changed its view.
    void onViewChanged();

private:
    /// The linked viewer.
    Q2DViewer *m_viewer;

};

}

#endif
