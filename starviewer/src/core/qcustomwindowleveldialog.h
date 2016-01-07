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

#ifndef UDGQCUSTOMWINDOWLEVELDIALOG_H
#define UDGQCUSTOMWINDOWLEVELDIALOG_H

#include "ui_qcustomwindowleveldialogbase.h"

namespace udg {

/**
    Diàleg per escollir manualment els ajustaments pel window level
  */
class QCustomWindowLevelDialog : public QDialog, private Ui::QCustomWindowLevelDialogBase {
Q_OBJECT
public:
    QCustomWindowLevelDialog(QDialog *parent = 0);

    ~QCustomWindowLevelDialog();

public slots:
    /// Dóna el window level que mostrarà per defecte quan s'obri
    void setDefaultWindowLevel(double window, double level);

signals:
    /// Envia la senyal del window level escollit
    void windowLevel(double window, double level);

private:
    /// Crea les connexions
    void createConnections();

private slots:
    /// Comprova el window level quan diem ok, fa les validacions i envia els senyas pertinents
    void confirmWindowLevel();

};

}

#endif
