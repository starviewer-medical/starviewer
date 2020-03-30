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

#include "diagnosistestresultwriter.h"

#include "diagnosistest.h"
#include "logging.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace udg {

namespace {

QJsonDocument getJsonDocument(const QList<QPair<QString, QStringList>> &informations, const QList<QPair<DiagnosisTest*, DiagnosisTestResult>> &diagnosisTests)
{
    QJsonArray documentArray;

    foreach (auto pair, informations)
    {
        QJsonArray array;

        foreach (const QString &detail, pair.second)
        {
            array.append(detail);
        }

        QJsonObject object
        {
            {"state", "info"},
            {"description", pair.first},
            {"details", array}
        };

        documentArray.append(object);
    }

    foreach (auto pair, diagnosisTests)
    {
        const DiagnosisTestResult &result = pair.second;
        QString state;

        switch (result.getState())
        {
            case DiagnosisTestResult::Ok:
                state = "correct";
                break;
            case DiagnosisTestResult::Warning:
                state = "warning";
                break;
            case DiagnosisTestResult::Error:
                state = "error";
                break;
        }

        QJsonObject object
        {
            {"state", state},
            {"description", pair.first->getDescription()}
        };

        if (result.getState() != DiagnosisTestResult::Ok)
        {
            QJsonArray problemsArray;

            foreach (const DiagnosisTestProblem &problem, result.getErrors() + result.getWarnings())
            {
                QString problemState;

                switch (problem.getState())
                {
                    case DiagnosisTestProblem::Ok:
                        problemState = "ok";
                        break;
                    case DiagnosisTestProblem::Warning:
                        problemState = "warning";
                        break;
                    case DiagnosisTestProblem::Error:
                        problemState = "error";
                        break;
                    case DiagnosisTestProblem::Invalid:
                        problemState = "invalid";
                        break;
                }

                QJsonObject problemObject
                {
                    {"state", problemState},
                    {"description", problem.getDescription()},
                    {"solution", problem.getSolution()}
                };

                problemsArray.append(problemObject);
            }

            object.insert("problems", problemsArray);
        }

        documentArray.append(object);
    }

    return QJsonDocument(documentArray);
}

}

void DiagnosisTestResultWriter::setInformations(QList<QPair<QString, QStringList>> informations)
{
    m_informations = std::move(informations);
}

void DiagnosisTestResultWriter::setDiagnosisTests(QList<QPair<DiagnosisTest*, DiagnosisTestResult>> diagnosisTests)
{
    m_diagnosisTests = std::move(diagnosisTests);
}

void DiagnosisTestResultWriter::write(const QString &pathFile) const
{
    QFile file(pathFile);

    if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
    {
        ERROR_LOG(QString("Can't open the file %1 to write: %2").arg(pathFile).arg(file.errorString()));
        return;
    }

    this->write(file);
}

void DiagnosisTestResultWriter::write(QIODevice &ioDevice) const
{
    if (ioDevice.isOpen() && !ioDevice.isWritable())
    {
        ioDevice.close();
    }

    if (!ioDevice.isOpen())
    {
        if (!ioDevice.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
        {
            ERROR_LOG(QString("Can't open the device to write: %1").arg(ioDevice.errorString()));
            return;
        }
    }

    if (ioDevice.write(getJsonDocument(m_informations, m_diagnosisTests).toJson()) == -1)
    {
        ERROR_LOG("An error ocurred while writing to the device: " + ioDevice.errorString());
    }
}

}
