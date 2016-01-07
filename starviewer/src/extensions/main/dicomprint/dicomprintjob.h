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

#ifndef UDGDICOMPRINTJOB_H
#define UDGDICOMPRINTJOB_H

#include <QString>
#include <QList>

#include "dicomprintpage.h"

namespace udg {

/**
    Implementació de la interfície PrintJob per impressores Dicom en DCMTK.
    Un PrintJob serà un objecte que contindrà la informació necessària per fer la impressió (Prioritat, Pàgines a imprimir...)
  */
class DicomPrintJob {
public:
    /// Afegim la Prioritat que tindrà la impressió (High, Medium, Low)
    void setPrintPriority(const QString &printPriority);

    /// Retorna la Prioritat que tindrà la impressió (High, Medium, Low)
    QString getPrintPriority() const;

    /// Afegim el Mitjà en el qual es realitzarà la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    void setMediumType(const QString &mediumType);

    /// Retorna el Mitjà en el qual es realitzarà la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    QString getMediumType() const;

    /// Afegim informació addicional a la impressió.
    void setLabel(const QString &label);

    /// Retorna la informació addicional de la impressió.
    QString getLabel() const;

    /// Afegim el destí de la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    void setFilmDestination(const QString &filmDestination);

    /// Retorna el destí de la impressió. (Aquest vindrà definit per les capacitats de la impressora).
    QString getFilmDestination() const;

    /// Especifica les PrintPage del PrintJob
    void setDicomPrintPages(QList<DicomPrintPage> dicomPrintPages);

    /// Retorna les PrintPage del PrintJob
    QList<DicomPrintPage> getDicomPrintPages();

    /// Especifica/retorna el número de pàgines que s'han d'implementar del JOB
    void setNumberOfCopies(int numberOfCopies);
    int getNumberOfCopies();

private:
    QString m_printPriority;
    QString m_mediumType;
    QString m_label;
    QString m_filmDestination;
    QList<DicomPrintPage> m_listDicomPrintPage;
    int m_numberOfCopies;
};
};
#endif
