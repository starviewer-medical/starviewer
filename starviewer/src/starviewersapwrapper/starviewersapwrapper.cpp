#include "logging.h"
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

#include <QDir>
#include <QApplication>

// Definicions globals d'aplicació
#include "starviewerapplication.h"
#include <QProcess>


/// Imprimim l'ajuda del programa
void printHelp()
{
    printf("Invalid arguments: you must specify one parameter, the accession number of the study to retrieve.\n\n");
    printf("\t Example calling 'starviewer_sapwrapper 123456' will retrieve the study with accession number 123456.\n\n");
}

QString getStarviewerExecutableFilePath()
{
    #ifdef _WIN32
        // En windows per poder executar l'starviewer hem de tenir en compte que si està en algun directori que conte espais
        // com el directori C:\Program Files\Starviewer\starviewer.exe, hem de posar el path entre cometes
        // per a que no ho interpreti com a paràmetres, per exemple "C:\Program Files\Starviewer\starviewer.exe"

        // Afegim les cometes per si algun dels directori conté espai
        return "\"" + QCoreApplication::applicationDirPath() + "/starviewer.exe" + "\"";
    #else
        return QCoreApplication::applicationDirPath() + "/starviewer";
    #endif
}

/// Engega un starviewer passant-li per comandes de línia el accessionNumber del estudi a descarragar
void retrieveStudy(QString accessionNumber)
{
    QProcess process;
    QString starviewerCommandLine = " -accessionnumber " + accessionNumber;

    // Executem una instància del Starviewer utiltizant la opció de línia de comandes -accessionnumber "valor del accessio number"

    INFO_LOG("Starviewer_sapwrapper::S'iniciara nova instancia del Starviewer per demanar descarrega de l'estudi amb accession number" + accessionNumber);
    process.startDetached(getStarviewerExecutableFilePath() + starviewerCommandLine);
}

int main(int argc, char *argv[])
{
    int returnValue = 0;
    QApplication application(argc, argv);
    udg::beginLogging();
    INFO_LOG("==================================================== BEGIN STARVIEWER SAP WRAPPER ====================================================");
    INFO_LOG(QString("%1 Version %2 BuildID %3").arg(udg::ApplicationNameString).arg(udg::StarviewerVersionString).arg(udg::StarviewerBuildID));

    QStringList parametersList = application.arguments();

    application.setOrganizationName(udg::OrganizationNameString);
    application.setOrganizationDomain(udg::OrganizationDomainString);
    application.setApplicationName(udg::ApplicationNameString);


    if (parametersList.count() == 2)
    {
        // Hem d'agafar el segon paràmetre perquè el primer és el nom del programa
        retrieveStudy(parametersList.at(1));
    }
    else
    {
        INFO_LOG(QString("StarviewerSAPWrapper::Número de parametres incorrecte, s'han passat %1 parametres").arg(QString().setNum(argc - 1)));
        printHelp();
    }

    INFO_LOG(QString("%1 Version %2 BuildID %3, returnValue %4").arg(udg::ApplicationNameString).arg(udg::StarviewerVersionString)
             .arg(udg::StarviewerBuildID).arg(returnValue));
    INFO_LOG("===================================================== END STARVIEWER SAP WRAPPER =====================================================");

    return returnValue;
}
