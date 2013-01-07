#ifndef UDG_STARVIEWER_APPLICATION
#define UDG_STARVIEWER_APPLICATION

#include <QDir>

namespace udg {

const QString StarviewerVersionString("0.12.0-devel");
const QString StarviewerBuildID("2012111500");

// Indica per aquesta versió d'starviewer quina és la revisió de bd necessària
const int StarviewerDatabaseRevisionRequired(7794);

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

}; // End namespace udg

#endif
