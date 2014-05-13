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

#ifndef UDGQCINECONTROLLER_H
#define UDGQCINECONTROLLER_H

#include "ui_qcinecontrollerbase.h"

namespace udg {

class QViewerCINEController;
class QViewer;
/**
    Widget per controlar el CINE en els QViewer
  */
class QCINEController : public QWidget, private Ui::QCINEControllerBase {
Q_OBJECT
public:
    QCINEController(QWidget *parent = 0);

    ~QCINEController();

    void setQViewer(QViewer *viewer);

protected slots:
    void updateVelocityLabel(int value);
    void updateLoopStatus(bool enabled);

protected:
    QViewerCINEController *m_cineController;
};

}

#endif
