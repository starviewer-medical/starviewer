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

#ifndef UDGSTARVIEWERAPPLICATIONCOMMANDLINE_H
#define UDGSTARVIEWERAPPLICATIONCOMMANDLINE_H

#include <QObject>

#include <QCommandLineParser>
#include <QMutex>

class QStringList;

namespace udg {

class ApplicationCommandLineOptions;

/**
 * @brief The StarviewerApplicationCommandLine class processes received command line options using QCommandLineParser.
 *
 * Current accepted options are:
 *
 * -studyinstanceuid <studyInstanceUid>:    Queries default PACS servers with the given Study Instance UID and downloads the study if found.
 * -accessionnumber <accessionNumberValue>: Queries default PACS servers with the given Accession Number and downloads the study if found.
 * -url <url>:                              Alternative way to specify a Study Instance UID or Accession Number to query. Accepted URL formats:
 *                                          - starviewer://studyinstanceuid/1.3.449.28373623
 *                                          - starviewer://accessionnumber/30563404640574
 * <blank>:                                 Opens a new Starviewer window.
 */
class StarviewerApplicationCommandLine : public QObject {

    Q_OBJECT

public:
    /// Possible option effects.
    enum StarviewerCommandLineOption { OpenBlankWindow, RetrieveStudyByUid, RetrieveStudyByAccessionNumber };

    /// Initializes the parser with valid options.
    StarviewerApplicationCommandLine(QObject *parent = 0);

    /// Returns a QCommandLineParser with valid options set.
    const QCommandLineParser& getStarviewerApplicationCommandLineParser() const;

    /// Parses the given argument list and returns true if they are valid and false otherwise.
    /// If there is a parse error an explanation is returned in \a errorText.
    bool parse(QStringList arguments, QString &errorText);

    /// Runs previously parsed arguments.
    void runParsedArguments();

    /// Parses the given argument list and returns true if they are valid and false otherwise.
    /// If there is a parse error an explanation is returned in \a errorText.
    /// If all arguments are valid emits the newOptionsToRun() signal to indicate that there are new queries to attend.
    bool parseAndRun(QStringList arguments, QString &errorText);

    /// Parses the given argument list given as a QString separated by ';' and returns true if they are valid and false otherwise.
    /// If there is a parse error an explanation is returned in \a errorText.
    /// If all arguments are valid emits the newOptionsToRun() signal to indicate that there are new queries to attend.
    bool parseAndRun(const QString &arguments, QString &errorText);

    /// If there are options pending to process returns the next one in \a optionValue and returns true. Otherwise returns false.
    bool takeOptionToRun(QPair<StarviewerCommandLineOption, QString> &optionAndValue);

public slots:
    /// Parses the given argument list given as a QString separated by ';' and returns true if they are valid and false otherwise.
    /// If all arguments are valid emits the newOptionsToRun() signal to indicate that there are new queries to attend.
    bool parseAndRun(const QString &arguments);

signals:
    /// Emitted to indicate that there are new queries pending to attend.
    void newOptionsToRun();

private:
    /// Adds the given option and value to the list of options to process.
    void addOptionToListToProcess(QPair<StarviewerCommandLineOption, QString> optionAndValue);

private:
    /// QCommandLineParser instance used to parse the options.
    QCommandLineParser m_parser;

    /// List of command line options pending to process together with their values.
    QList<QPair<StarviewerCommandLineOption, QString> > m_commandLineOptionListToProcess;

    /// Used to access the list of options pending to process because StarviewerApplicationCommandLine is used as a singleton.
    QMutex m_mutexCommandLineOptionListToProcess;

};

}

#endif
