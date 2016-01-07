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

#ifndef UDGQLAYOUTOPTIONSWIDGET_H
#define UDGQLAYOUTOPTIONSWIDGET_H

#include "ui_qlayoutoptionswidgetbase.h"

namespace udg {

class StudyLayoutConfig;

/**
    Widget to configure the layout options of a concrete modality.
    Modality should be given as a parameter on the constructor.
 */
class QLayoutOptionsWidget : public QWidget, private Ui::QLayoutOptionsWidgetBase {
Q_OBJECT
public:
    QLayoutOptionsWidget(const QString &modality, QWidget *parent = 0);
    ~QLayoutOptionsWidget();

private:
    /// Initializes the widget with the corresponding values
    void initialize();

    /// Fills combo box with the proper values
    void populateMaxViewersComboBox();

    /// Sets up signal-slot connections
    void setupConnections();

    void setStudyLayoutConfig(const StudyLayoutConfig &config);

    /// Updates the displayed settings according to the current modality
    void updateDisplayedSettings();

private slots:
    /// Updates the automatic layout settings according to the modified values on the widget
    void updateAutomaticLayoutSettings();

    /// Updates the automatic layout settings to its defaults
    void restoreAutomaticLayoutSettingsDefaults();

    /// Updates the hanging protocol setting according to current modality
    void updateHangingProtocolSetting(bool enable);

private:
    /// The modality to be configured
    QString m_modality;
};

} // End namespace udg

#endif
