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

#include "starviewerapplicationcommandline.h"

#include "logging.h"
#include "starviewerapplication.h"

namespace udg {

namespace {

const QString AccessionNumberOption("accessionnumber");

}

StarviewerApplicationCommandLine::StarviewerApplicationCommandLine(QObject *parent)
    : QObject(parent)
{
    m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    m_parser.setApplicationDescription(ApplicationNameString);

#ifndef STARVIEWER_LITE
    // Starviewer Lite can't connect to PACS so it won't have these options
    QCommandLineOption accessionNumberOption(AccessionNumberOption, tr("Retrieve the study with the given Accession Number from the query default PACS."),
                                             "accessionNumber");
    m_parser.addOption(accessionNumberOption);
#endif
}

const QCommandLineParser& StarviewerApplicationCommandLine::getStarviewerApplicationCommandLineParser() const
{
    return m_parser;
}

bool StarviewerApplicationCommandLine::parse(QStringList arguments, QString &errorText)
{
    if (m_parser.parse(arguments))
    {
        return true;
    }
    else
    {
        errorText = m_parser.errorText();
        return false;
    }
}

bool StarviewerApplicationCommandLine::parseAndRun(QStringList arguments, QString &errorText)
{
    if (m_parser.parse(arguments))
    {
        if (m_parser.isSet(AccessionNumberOption))
        {
            addOptionToListToProcess(qMakePair(RetrieveStudyByAccessionNumber, m_parser.value(AccessionNumberOption)));
        }
        else // no option
        {
            addOptionToListToProcess(qMakePair(OpenBlankWindow, QString()));
        }

        emit newOptionsToRun();
        return true;
    }
    else
    {
        errorText = m_parser.errorText();
        return false;
    }
}

bool StarviewerApplicationCommandLine::parseAndRun(const QString &arguments, QString &errorText)
{
    return parseAndRun(arguments.split(";"), errorText);
}

bool StarviewerApplicationCommandLine::takeOptionToRun(QPair<StarviewerApplicationCommandLine::StarviewerCommandLineOption, QString> &optionAndValue)
{
    QMutexLocker locker(&m_mutexCommandLineOptionListToProcess);

    if (!m_commandLineOptionListToProcess.isEmpty())
    {
        optionAndValue = m_commandLineOptionListToProcess.takeFirst();
        return true;
    }
    else
    {
        return false;
    }
}

bool StarviewerApplicationCommandLine::parseAndRun(const QString &arguments)
{
    QString errorText;

    if (parseAndRun(arguments, errorText))
    {
        return true;
    }
    else
    {
        ERROR_LOG("Invalid command line arguments: " + errorText);
        return false;
    }
}

void StarviewerApplicationCommandLine::addOptionToListToProcess(QPair<StarviewerCommandLineOption, QString> optionAndValue)
{
    QMutexLocker locker(&m_mutexCommandLineOptionListToProcess);
    m_commandLineOptionListToProcess.append(optionAndValue);
}

}
