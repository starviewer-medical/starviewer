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
 * @brief The QThickSlabWidget class creates a tool button a side panel with the controls to change the thick slab properties of a Q2DViewer.
 */
class QThickSlabWidget : public QWidget, private Ui::QThickSlabWidgetBase {

    Q_OBJECT

public:
    explicit QThickSlabWidget(QWidget *parent = nullptr);
    ~QThickSlabWidget() override;

    /// Links this widget to the given viewer, i.e. makes this widget control the thick slab properties of that viewer.
    /// If there is already a linked viewer, it is automatically unlinked first.
    void link(Q2DViewer *viewer);

    /// Unlinks this widget from the linked viewer.
    void unlink();

    /// Returns the width of the button and options widget when it's not foldable.
    int getFixedWidth() const;
    /// Makes the widget foldable or not.
    void setFoldable(bool foldable);

signals:
    /// Emitted when the maximum thickness checkbox changes its status.
    void maximumThicknessModeToggled(bool checked);
    /// Emitted when it should be ensured that the whole widget is visible in a scroll area.
    void ensureVisible();

protected:
    /// Reimplemented to set the height of the arrow button to the same as the main button and to calculate the fixed width.
    void showEvent(QShowEvent *event) override;

private:
    /// Shows the options widget. If the parameter is true, ensureVisible() is emitted.
    void showOptions(bool emitEnsureVisible);
    /// Hides the options widget.
    void hideOptions();

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

    /// If thick slab is enabled, applies the projection mode in the given index to the linked viewer.
    void applyProjectionMode(int index);

    /// If thick slab is enabled, applies the given thickness to the linked viewer.
    void applyThickness(double thickness);

    /// Sets the thickness to the maximum possible value, keeping it at the maximum even when the direction of projection changes.
    void setMaximumThicknessEnabled(bool enabled);

    /// Resets values after the linked viewer has changed its volume.
    void onVolumeChanged();

    /// Resets values to those of the viewer after it has changed its view.
    void onViewChanged();

private:
    /// The linked viewer.
    Q2DViewer *m_viewer;
    /// True if this widget is being shown for the first time. Used to adjust the height of the arrow button the first time.
    bool m_firstShow;
    /// True if the options widget is being shown for the first time.
    bool m_firstOptionsShow;

    /// Width of the full widget when it's not foldable. Equal to the width of the main button plus the options widget.
    int m_fixedWidth;
    /// If true, the options widget can be shown or hidden with an arrow button; if false, the options widget is always visible and the arrow button not.
    bool m_foldable;

};

}

#endif
