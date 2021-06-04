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

#include "qlogviewer.h"

#include "coresettings.h"
#include "logging.h"
#include "starviewerapplication.h"

// Qt
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QTextCodec>
#include <QFileDialog>

namespace udg {

QLogViewer::QLogViewer(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    readSettings();
    createConnections();
}

QLogViewer::~QLogViewer()
{
    writeSettings();
}

void QLogViewer::updateData()
{
    // \TODO aquest directori s'hauria de guardar en alguna mena de settings o similar
    QFile logFile(udg::UserLogsFile);
    if (!logFile.open(QFile::ReadOnly | QFile::Text))
    {
        ERROR_LOG("No s'ha pogut obrir l'arxiu de logs");
        m_logBrowser->setPlainText(tr("ERROR: No Log file found at this path: %1\nEnvironment variable (logFilePath): %2").arg(udg::UserLogsFile).arg(
                                   QString::fromLocal8Bit(qgetenv("logFilePath"))));
    }
    else
    {
        INFO_LOG("S'ha obert amb èxit l'arxiu de logs [" + udg::getLogFilePath() + "]");
        m_logBrowser->setPlainText(logFile.readAll());
        m_logBrowser->moveCursor(QTextCursor::End);
    }
}

void QLogViewer::createConnections()
{
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(saveLogFileAs()));
}

void QLogViewer::saveLogFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), QString(), tr("Log Files (*.log)"));

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
    {
        return;
    }

    QTextStream logStream(&file);
    logStream << m_logBrowser->document()->toPlainText();
}

void QLogViewer::writeSettings()
{
    Settings settings;
    settings.saveGeometry(CoreSettings::LogViewerGeometry, this);
}

void QLogViewer::readSettings()
{
    Settings settings;
    settings.restoreGeometry(CoreSettings::LogViewerGeometry, this);
}

}
