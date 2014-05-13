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

#ifndef UDGDIAGNOSISTEST_H
#define UDGDIAGNOSISTEST_H

#include "diagnosistestresult.h"
#include <QObject>

namespace udg {

class DiagnosisTest : public QObject {
public:
    /// Destructor per defecte. És virtual per que les subclasses puguin reimplementar-lo
    virtual ~DiagnosisTest(){}
    /// Mètode principal de la interfície. No està implementat, i es força a les subclasses a fer-ho.
    virtual DiagnosisTestResult run() = 0;
    /// Retorna la descripció del test
    virtual QString getDescription() = 0;
protected:
    /// Constructor per defecte protected perque no es pugui instanciar objectes DiagnosisTest
    DiagnosisTest(QObject *parent = 0) : QObject(parent){}
};

}

#endif
