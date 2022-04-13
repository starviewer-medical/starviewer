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

#ifndef UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREENHELP_H
#define UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREENHELP_H

#include "ui_q2dviewerannotationsconfigurationscreenhelpbase.h"

namespace udg {

/**
 * @brief The Q2DViewerAnnotationsConfigurationScreenHelp class is a window to show the help for the Q2DViewerAnnotationsConfigurationScreen.
 */
class Q2DViewerAnnotationsConfigurationScreenHelp : public QWidget, private Ui::Q2DViewerAnnotationsConfigurationScreenHelpBase
{
    Q_OBJECT

public:
    explicit Q2DViewerAnnotationsConfigurationScreenHelp(QWidget *parent = nullptr);

    /// Adds a new variable to the list with its corresponding explanation.
    void addVariable(const QString &variable, const QString &explanation);

protected:
    /// Reimplemented to allow to variable labels to be selected with a single click.
    bool eventFilter(QObject *watched, QEvent *event) override;
};

} // namespace udg

#endif // UDG_Q2DVIEWERANNOTATIONSCONFIGURATIONSCREENHELP_H
