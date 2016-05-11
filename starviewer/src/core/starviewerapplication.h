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

const QString StarviewerVersionString("0.14.0-alpha1");
const QString StarviewerBuildID("2016051100");

// Indica per aquesta versió d'starviewer quina és la revisió de bd necessària
const int StarviewerDatabaseRevisionRequired(9593);

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

// TODO const QString LogConfigurationFileLocation();

}

#endif
