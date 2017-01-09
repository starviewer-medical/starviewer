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

#include "qpdfextension.h"

#include "dicomtagreader.h"
#include "encapsulateddocument.h"
#include "logging.h"
#include "patient.h"

#include <QDesktopServices>
#include <QFile>
#include <QUrl>

namespace udg {

QPdfExtension::QPdfExtension(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(m_listWidget, &QListWidget::itemActivated, this, &QPdfExtension::openPdf);
}

QPdfExtension::~QPdfExtension()
{
}

void QPdfExtension::setPatient(Patient *patient)
{
    m_listWidget->clear();

    if (!patient)
    {
        DEBUG_LOG("Received a null patient");
        return;
    }

    int count = 0;

    foreach (Study *study, patient->getStudies())
    {
        foreach (Series *series, study->getSeries())
        {
            foreach (EncapsulatedDocument *document, series->getEncapsulatedDocuments())
            {
                if (document->getMimeTypeOfEncapsulatedDocument() == "application/pdf")
                {
                    count++;
                    QListWidgetItem *item = new QListWidgetItem(QIcon(":/images/icons/mime-pdf.svg"), document->getDocumentTitle(), m_listWidget);
                    if (document->getDocumentTitle().isEmpty())
                    {
                        item->setText(tr("Document %1").arg(count));
                    }
                    item->setData(Qt::UserRole, document->getPath());
                    if (count == 1)
                    {
                        item->setSelected(true);
                    }
                }
            }
        }
    }
}

void QPdfExtension::showEvent(QShowEvent*)
{
    m_listWidget->setFocus();
}

void QPdfExtension::openPdf(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();

    DICOMTagReader dicomTagReader(path);
    QByteArray pdfData = dicomTagReader.getValueAttributeAsByteArray(DICOMEncapsulatedDocument);

    QFile pdfFile(path + ".pdf");
    if (!pdfFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        ERROR_LOG("Can't write to file " + pdfFile.fileName());
        return;
    }
    pdfFile.write(pdfData);
    pdfFile.close();

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(pdfFile.fileName())))
    {
        ERROR_LOG("Can't open file " + pdfFile.fileName());
    }
}

} // namespace udg
