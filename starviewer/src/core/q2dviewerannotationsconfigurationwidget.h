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

#ifndef UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONWIDGET_H
#define UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONWIDGET_H

#include "ui_q2dviewerannotationsconfigurationwidgetbase.h"

namespace udg {

struct Q2DViewerAnnotationsSettings;

/**
 * @brief The Q2DViewerAnnotationsConfigurationWidget class is the widget to edit 2D viewer annotations configuration for a single modality.
 */
class Q2DViewerAnnotationsConfigurationWidget : public QWidget, private Ui::Q2DViewerAnnotationsConfigurationWidgetBase
{
    Q_OBJECT

public:
    explicit Q2DViewerAnnotationsConfigurationWidget(const QString &modality, QWidget *parent = nullptr);

    /// Sets focus on m_modalityLineEdit and selects all its text, leaving it ready for the user to change its value.
    void editModality();
    /// Accepts modality change. Copies value from m_modalityLineEdit to m_modality and sets m_modalityValidated to true.
    void updateModality();
    /// Rejects modality change. Copies value from m_modality to m_modalityLineEdit and sets m_modalityValidated to false.
    void restoreModality();

    /// Returns the annotations settings represented by the current widgets contents.
    Q2DViewerAnnotationsSettings getAnnotationsSettings() const;

signals:
    /// Emitted when the modality has been changed. Either updateModality() or restoreModality() must be called afterwards.
    void modalityChanged(QString modality);
    /// Emitted when the help button is clicked.
    void helpRequested();

protected:
    /// If the key pressed is Enter/Return or Escape and m_modalityValidated is false, eats the key press. Otherwise, calls QWidget's implementation.
    void keyPressEvent(QKeyEvent *event) override;

private:
    /// Fills the widgets with the given settings.
    void populate(const Q2DViewerAnnotationsSettings &annotationsSettings);

private:
    /// Modality to which the settings apply. When the modality is changed in the corresponding widget, this attribute is updated if the change is accepted
    /// (updateModality()), or otherwise the widget reverts its value from this attribute if the change is not accepted (restoreModality()).
    QString m_modality;
    /// True while waiting for modality change validation. Used to avoid emitting the modalityChanged() signal while already waiting for validation.
    bool m_validatingModality;
    /// Set to true when m_modality and m_modalityLineEdit have the same modality after editing, i.e. when the change has been accepted or there has been no
    /// change. Used to eat Enter and Escape key presses when a change rejected.
    bool m_modalityValidated;
};

} // namespace udg

#endif // UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONWIDGET_H
