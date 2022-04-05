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

#ifndef UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREEN_H
#define UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREEN_H

#include "ui_q2dviewerannotationsconfigurationscreenbase.h"

namespace udg {

class Q2DViewerAnnotationsConfigurationWidget;

/**
 * @brief The Q2DViewerAnnotationsConfigurationScreen class is the main configuration screen for 2D viewer annotations.
 */
class Q2DViewerAnnotationsConfigurationScreen : public QWidget, private Ui::Q2DViewerAnnotationsConfigurationScreenBase
{
    Q_OBJECT

public:
    explicit Q2DViewerAnnotationsConfigurationScreen(QWidget *parent = nullptr);
    /// Saves the settings.
    ~Q2DViewerAnnotationsConfigurationScreen() override;

private:
    /// Reads the current settings and creates and fills the necessary widgets accordingly.
    void initialize();
    /// Creates signal/slot connections for static widgets.
    void createConnections();

    /// In practice, a private slot. It is called when the given tab changes its modality and checks if the new modality is acceptable. The modality is
    /// acceptable if it's not empty, not called "Default" and not equal to another modality. If the modality is acceptable, updates the tab name and calls
    /// updateModality() on the tab to accept the change, and otherwise calls restoreModality() to revert it.
    void validateModalityChange(Q2DViewerAnnotationsConfigurationWidget *tab, const QString &newModality);

private:
    /// Tab index that must be forced in the tab widget (-1 if none). Used when a modality change is rejected to ensure that the tab that was changed and is
    /// reverted is the current one.
    int m_forceTabIndex;
};

} // namespace udg

#endif // UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREEN_H
