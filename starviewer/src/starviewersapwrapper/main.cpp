/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QString>
#include <QDir>
#include <QApplication>
#include <QCoreApplication>

#include "starviewerapplication.h"// definicions globals d'aplicació
#include "starviewersapwrapper.h"
#include "logging.h"

///configurem el logging
void configureLogging()
{
    // primer comprovem que existeixi el direcotori ~/.starviewer/log/ on guradarem els logs
    QDir logDir = udg::UserLogsPath;
    if( !logDir.exists() )
    {
        // creem el directori
        logDir.mkpath( udg::UserLogsPath );
    }
    // TODO donem per fet que l'arxiu es diu així i es troba a la localització que indiquem. S'hauria de fer una mica més flexible o genèric;
    // està així perquè de moment volem anar per feina i no entretenir-nos però s'ha de fer bé.
    QString configurationFile = "/etc/starviewer/log.conf";
    if( ! QFile::exists(configurationFile) )
    {
        configurationFile = QCoreApplication::applicationDirPath() + "/log.conf";
    }
    LOGGER_INIT( configurationFile.toStdString() );
    DEBUG_LOG("Arxiu de configuració del log: " + configurationFile );
}

///Imprimim l'ajuda del programa
void printHelp()
{
    printf("Invalid arguments: you must specify one parameter, the accession number of the study to retrieve.\n\n");
    printf("\t Example calling 'starviewer_sapwrapper 123456' will retrieve the study with accession number 123456.\n\n");
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QStringList parametersList = application.arguments();
    udg::StarviewerSapWrapper starviewerSapWrapper;

    configureLogging();

    STAT_LOG("Starviewer_sapwrapper::Iniciat");

    if (parametersList.count() == 2)
    {
        //Hem d'agafar el segon paràmetre perquè el primer és el nom del programa
        starviewerSapWrapper.sendRequestToLocalStarviewer(parametersList.at(1));
    }
    else
    {
        INFO_LOG(QString("Starviewer_sapwrapper::Nombre de parametres incorrecte, s'han passat %1 parametres").arg(QString().setNum(argc -1)));
        printHelp();
    }

    STAT_LOG("Starviewer_sapwrapper::Finalitzat");
}
