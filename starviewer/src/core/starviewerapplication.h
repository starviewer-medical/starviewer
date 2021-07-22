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

#ifndef UDG_STARVIEWER_APPLICATION
#define UDG_STARVIEWER_APPLICATION

#include <QDir>

namespace udg {

#ifdef STARVIEWER_CE
    #define CE_SUFFIX "-CE"
#else
    #define CE_SUFFIX ""
#endif

const QString StarviewerVersionString("1.2.0-devel" CE_SUFFIX);
const QString StarviewerBuildID("2021072200");

#undef CE_SUFFIX

#ifdef Q_OS_WIN
const QString StarviewerBuildPlatform("Windows");
#endif
#ifdef Q_OS_OSX
const QString StarviewerBuildPlatform("Mac");
#endif
#ifdef Q_OS_LINUX
const QString StarviewerBuildPlatform("Linux");
#endif

// Indica per aquesta versió d'starviewer quina és la revisió de bd necessària
const int StarviewerDatabaseRevisionRequired(9595);

const QString OrganizationNameString("GILab");
const QString OrganizationDomainString("starviewer.udg.edu");

#ifdef STARVIEWER_LITE
    const QString ApplicationNameString("Starviewer Lite");
#else
    const QString ApplicationNameString("Starviewer");
#endif

const QString OrganizationEmailString("support@starviewer.udg.edu");
const QString OrganizationWebURL("http://starviewer.udg.edu");

// Paths per defecte on es guarden configuracions i arxius d'usuari
/// Path base per a dades d'usuari
const QString UserDataRootPath(QDir::homePath() + "/.starviewer/");
/// Path del log
const QString UserLogsPath(UserDataRootPath + "log/");
/// Ruta absoluta del fitxer de log
const QString UserLogsFile(UserLogsPath + "starviewer.log");

// Note: the following two are methods because they depend on QApplication being initialized.

/// Returns the base directory where the application is installed.
QString installationPath();
/// Returns the root source directory (the one containing src).
QString sourcePath();

}

#endif
