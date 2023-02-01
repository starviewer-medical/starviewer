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

#include <QRegularExpression>

namespace udg {

namespace {

const QString StudyInstanceUidOption("studyinstanceuid");
const QString AccessionNumberOption("accessionnumber");
const QString UrlOption("url");

}

StarviewerApplicationCommandLine::StarviewerApplicationCommandLine(QObject *parent)
    : QObject(parent)
{
    m_parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    m_parser.setApplicationDescription(ApplicationNameString);

#ifndef STARVIEWER_LITE
    // Starviewer Lite can't connect to PACS so it won't have these options
    QCommandLineOption studyInstanceUidOption(StudyInstanceUidOption, tr("Retrieve the study with the given Study Instance UID from the query default PACS."),
                                              "studyInstanceUid");
    m_parser.addOption(studyInstanceUidOption);

    QCommandLineOption accessionNumberOption(AccessionNumberOption, tr("Retrieve the study with the given Accession Number from the query default PACS."),
                                             "accessionNumber");
    m_parser.addOption(accessionNumberOption);

    QCommandLineOption urlOption(UrlOption, tr("Retrieve the study specified by the given URL from the query default PACS. "
                                               "Read the help for the supported URL formats."), "url");
    m_parser.addOption(urlOption);
#endif
}

const QCommandLineParser& StarviewerApplicationCommandLine::getStarviewerApplicationCommandLineParser() const
{
    return m_parser;
}

bool StarviewerApplicationCommandLine::parse(QStringList arguments, QString &errorText)
{
    bool ok;

    if ((ok = m_parser.parse(arguments)))   // assign and check in this and other ifs below
    {
        auto parseStudyInstanceUid = [&](const QString &studyInstanceUid) -> bool {
            static const QRegularExpression StudyInstanceUidRegex("^[0-9.]{1,64}$");
            QRegularExpressionMatch match = StudyInstanceUidRegex.match(studyInstanceUid);

            if (match.hasMatch())
            {
                return true;
            }
            else
            {
                errorText = tr("Invalid Study Instance UID: “%1”").arg(studyInstanceUid);
                return false;
            }
        };

        auto parseAccessionNumber = [&](const QString &accessionNumber) -> bool {
            static const QRegularExpression AccessionNumberRegex(R"(^[^\\]{1,16}$)");
            QRegularExpressionMatch match = AccessionNumberRegex.match(accessionNumber);

            if (match.hasMatch())
            {
                return true;
            }
            else
            {
                errorText = tr("Invalid Accession Number: “%1”").arg(accessionNumber);
                return false;
            }
        };

        if (m_parser.isSet(StudyInstanceUidOption))
        {
            QString studyInstanceUid = m_parser.value(StudyInstanceUidOption);

            if ((ok = parseStudyInstanceUid(studyInstanceUid)))
            {
                addOptionToListToProcess(qMakePair(RetrieveStudyByUid, studyInstanceUid));
            }
        }
        else if (m_parser.isSet(AccessionNumberOption))
        {
            QString accessionNumber = m_parser.value(AccessionNumberOption);

            if ((ok = parseAccessionNumber(accessionNumber)))
            {
                addOptionToListToProcess(qMakePair(RetrieveStudyByAccessionNumber, accessionNumber));
            }
        }
        else if (m_parser.isSet(UrlOption))
        {
            static const QRegularExpression StudyInstanceUidUrlRegex("^starviewer://studyinstanceuid/([^/?#]+)/?(?:[?#]|$)");
            static const QRegularExpression AccessionNumberUrlRegex("^starviewer://accessionnumber/([^/?#]+)/?(?:[?#]|$)");
            QString url = m_parser.value(UrlOption);
            QRegularExpressionMatch studyInstanceUidMatch = StudyInstanceUidUrlRegex.match(url);
            QRegularExpressionMatch accessionNumberMatch = AccessionNumberUrlRegex.match(url);

            if (studyInstanceUidMatch.hasMatch())
            {
                QString studyInstanceUid = studyInstanceUidMatch.captured(1);

                if ((ok = parseStudyInstanceUid(studyInstanceUid)))
                {
                    addOptionToListToProcess(qMakePair(RetrieveStudyByUid, studyInstanceUid));
                }
            }
            else if (accessionNumberMatch.hasMatch())
            {
                QString accessionNumber = accessionNumberMatch.captured(1);

                if ((ok = parseAccessionNumber(accessionNumber)))
                {
                    addOptionToListToProcess(qMakePair(RetrieveStudyByAccessionNumber, accessionNumber));
                }
            }
            else
            {
                errorText = tr("Invalid URL: “%1”").arg(url);
                ok = false;
            }
        }
        else // no option
        {
            addOptionToListToProcess(qMakePair(OpenBlankWindow, QString()));
        }
    }
    else
    {
        errorText = m_parser.errorText();
    }

    return ok;
}

void StarviewerApplicationCommandLine::runParsedArguments()
{
    emit newOptionsToRun();
}

bool StarviewerApplicationCommandLine::parseAndRun(QStringList arguments, QString &errorText)
{
    if (parse(arguments, errorText))
    {
        runParsedArguments();
        return true;
    }
    else
    {
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
