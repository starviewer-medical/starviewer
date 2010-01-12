/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include <QDir>
#include <QApplication>

#include "../core/starviewerapplication.h"// definicions globals d'aplicació
#include "../core/logging.h"
#include "../inputoutput/inputoutputsettings.h"
#include <QProcess>

/// configurem el logging
// \TODO Còpia exacta del main.cpp de l'starviewer. Caldria refactoritzar-ho.
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

QString getStarviewerExecutableFilePath()
{
    #ifdef _WIN32
        /*En windows per poder executar l'starviewer hem de tenir en compte que si està en algun directori que conte espais
         *com el directori C:\Program Files\Starviewer\starviewer.exe, hem de posar el path entre cometes 
         * per a que no ho interpreti com a paràmetres, per exemple "C:\Program Files\Starviewer\starviewer.exe" */

        return "\"" + QCoreApplication::applicationDirPath() + "/starviewer.exe" + "\""; //afegim les cometes per si algun dels directori conté espai
    #else 
        return QCoreApplication::applicationDirPath() + "/starviewer";
    #endif
}

///Engega un starviewer passant-li per comandes de línia el accessionNumber del estudi a descarragar
void retrieveStudy(QString accessionNumber)
{
    QProcess process;
    QString starviewerCommandLine = " -accessionnumber " + accessionNumber; 

    /*Com que tenim implementat el QtSingleApplication a Starviewer.exe el que hem de fer simplement és executar una altra instància del Starviewer, aquesta
      ja s'encarregarà de fer arribar la instància principal la petició de descàrrega d'un estudi*/
    process.startDetached(getStarviewerExecutableFilePath() + starviewerCommandLine);
    INFO_LOG("S'ha iniciat una nova instància de Starviewer");
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QStringList parametersList = application.arguments();

    application.setOrganizationName(udg::OrganizationNameString);
    application.setOrganizationDomain(udg::OrganizationDomainString);
    application.setApplicationName(udg::ApplicationNameString);

    configureLogging();

    udg::InputOutputSettings inputOutputSettings;
    inputOutputSettings.init();

    if (parametersList.count() == 2)
    {
        udg::Settings settings;

        if (settings.getValue(udg::InputOutputSettings::ListenToRISRequests).toBool()) //comprovem si el servei d'escolta del RIS està activat
        {
            //Hem d'agafar el segon paràmetre perquè el primer és el nom del programa
            retrieveStudy(parametersList.at(1));
        }
        else 
        {
            QString message = QString("No es pot connectar amb %1 perquè no té activada l'opció d'escoltar les peticions del RIS").arg(udg::ApplicationNameString);

            printf(qPrintable(message));
            printf("Per activar l'opció, executeu l'Starviewer aneu al menú Eines->Configuració, allà escolliu el diàleg 'Escoltar peticions del RIS' i marqueu la opció 'Escoltar peticions del RIS pel port'");

            INFO_LOG(message);
        }
    }
    else
    {
        INFO_LOG(QString("StarviewerSAPWrapper::Número de parametres incorrecte, s'han passat %1 parametres").arg(QString().setNum(argc -1)));
        printHelp();
    }
}
