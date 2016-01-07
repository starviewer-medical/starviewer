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

#ifndef UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H
#define UDGQ2DVIEWERLAYOUTCONFIGURATIONSCREEN_H

#include "ui_q2dviewerlayoutconfigurationscreenbase.h"

namespace udg {

/**
    Pantalla de configuració de layouts del visor 2D
 */
class Q2DViewerLayoutConfigurationScreen : public QWidget, private Ui::Q2DViewerLayoutConfigurationScreenBase {
Q_OBJECT
public:
    Q2DViewerLayoutConfigurationScreen(QWidget *parent = 0);
    ~Q2DViewerLayoutConfigurationScreen();

private:
    /// Inicialitza el widget amb els valors corresponents
    void initialize();
};

} // End namespace udg

#endif
