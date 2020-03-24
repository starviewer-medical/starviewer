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

#ifndef UDGDIAGNOSISTESTRESULTWRITER_H
#define UDGDIAGNOSISTESTRESULTWRITER_H

#include "diagnosistestresult.h"

#include <QList>
#include <QPair>

namespace udg {

class DiagnosisTest;

/**
 * @brief The DiagnosisTestResultWriter class writes diagnosis tests results and some information in a JSON file.
 */
class DiagnosisTestResultWriter
{
public:
    /// Sets the informations (system information and other metadata) to write.
    void setInformations(QList<QPair<QString, QStringList>> informations);
    /// Sets the diagnosis tests and corresponding results to write.
    void setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult>> diagnosisTests);

    /// Writes the informations and results to a file with the given path.
    void write(const QString &pathFile) const;
    /// Writes the informations and results to the given I/O device.
    void write(QIODevice &ioDevice) const;

private:
    QList<QPair<QString, QStringList>> m_informations;
    QList<QPair<DiagnosisTest*, DiagnosisTestResult>> m_diagnosisTests;
};

}

#endif
