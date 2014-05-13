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

#include <QString>

#include "dicomprintjob.h"

namespace udg {

void DicomPrintJob::setPrintPriority(const QString &printPriority)
{
    m_printPriority = printPriority;
}

QString DicomPrintJob::getPrintPriority() const
{
    return m_printPriority;
}

void DicomPrintJob::setMediumType(const QString &mediumType)
{
    m_mediumType = mediumType;
}

QString DicomPrintJob::getMediumType() const
{
    return m_mediumType;
}

void DicomPrintJob::setLabel(const QString &label)
{
    m_label = label;
}

QString DicomPrintJob::getLabel() const
{
    return m_label;
}

void DicomPrintJob::setFilmDestination(const QString &filmDestination)
{
    m_filmDestination = filmDestination;
}

QString DicomPrintJob::getFilmDestination() const
{
    return m_filmDestination;
}

void DicomPrintJob::setDicomPrintPages(QList<DicomPrintPage> dicomPrintPages)
{
    m_listDicomPrintPage = dicomPrintPages;
}

QList<DicomPrintPage> DicomPrintJob::getDicomPrintPages()
{
    return m_listDicomPrintPage;
}

void DicomPrintJob::setNumberOfCopies(int numberOfCopies)
{
    m_numberOfCopies = numberOfCopies;
}

int DicomPrintJob::getNumberOfCopies()
{
    return m_numberOfCopies;
}

}
