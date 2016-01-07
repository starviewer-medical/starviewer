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

#ifndef UDGSYNCHRONIZETOOLDATA_H
#define UDGSYNCHRONIZETOOLDATA_H

#include "tooldata.h"

namespace udg {

/**
    Classe per guardar les dades de la tool de sincronització.
  */
class SynchronizeToolData : public ToolData {
Q_OBJECT
public:
    SynchronizeToolData();
    ~SynchronizeToolData();

    /// Obtenir l'increment de llesca
    double getIncrement();

    /// Obtenir la vista que s'ha incrementat
    QString getIncrementView();

public slots:
    /// Per canviar les dades de sincronitzacio de llesques
    void setIncrement(double value, QString view);

signals:
    /// Signal que s'emet quan les dades de l'increment de llesca canvien
    void sliceChanged();

private:
    /// Dades per la sincronització de l'slicing. Distància recorreguda
    double m_increment;

    /// Vista on ha de tenir efecte l'increment
    QString m_incrementView;
};

}

#endif
