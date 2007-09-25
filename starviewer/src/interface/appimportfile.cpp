/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "appimportfile.h"
// qt
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
// itk
#include <itkGDCMSeriesFileNames.h> // per generar els noms dels arxius DICOM d'un directori
// recursos
#include "logging.h"

namespace udg {

AppImportFile::AppImportFile(QObject *parent, QString name)
 : QObject( parent )
{
    this->setObjectName( name );
    readSettings();
}

AppImportFile::~AppImportFile()
{
}

bool AppImportFile::open()
{
    bool openedImage = false;

    const QString PresentationStateFilter("Presentation State (*)"), KeyImageNoteFilter("Key Image Note (*)");
    const QString ResultFileFilter("Result (*.res)");
    const QString MetaIOImageFilter("MetaIO Image (*.mhd)"), DICOMImageFilter("DICOM Images (*.dcm)"), AllFilesFilter("All Files (*)");
    QStringList imagesFilter;
    imagesFilter << MetaIOImageFilter << DICOMImageFilter << AllFilesFilter;

    QString fileFilter = MetaIOImageFilter + ";;" + DICOMImageFilter + ";;";
    fileFilter += PresentationStateFilter + ";;" + KeyImageNoteFilter + ";;" + ResultFileFilter + ";;" + AllFilesFilter;

    QString selectedFilter, fileName;
    fileName = QFileDialog::getOpenFileName( NULL, tr("Choose a file to open..."), m_workingDirectory, fileFilter , &selectedFilter );

    if ( !fileName.isEmpty() )
    {
        INFO_LOG( "S'obre el fitxer: " + fileName + " amb el filtre " + selectedFilter );
        if (imagesFilter.contains(selectedFilter))
        {
            QStringList files;
            files << fileName;
            emit selectedFiles( files );
            openedImage = true;
        }
        else if (selectedFilter == KeyImageNoteFilter) // TODO aquest dos els mantenim temporalment,però el que cal és implementar el KINFillerStep i el PresentationStateFiller
        {
            emit openKeyImageNote(fileName);
        }
        else if (selectedFilter == PresentationStateFilter)
        {
            emit openPresentationState(fileName);
        }
        else
        {
            ERROR_LOG("Cas no tractat al obrir un fitxer");
        }
        m_workingDirectory = QFileInfo( fileName ).dir().path();
        writeSettings();
    }

    return openedImage;
}

bool AppImportFile::openDirectory()
{
    bool ok = true;

    QString directoryName = QFileDialog::getExistingDirectory( 0 , tr("Choose a directory") , m_workingDicomDirectory , QFileDialog::ShowDirsOnly );
    if ( !directoryName.isEmpty() )
    {
        emit selectedFiles( this->generateFilenames( directoryName ) );
        m_workingDicomDirectory = QFileInfo( directoryName ).dir().path();
        writeSettings();
        INFO_LOG( "S'obre el directori: " + directoryName );
    }
    else
        ok = false;

    return ok;
}

QStringList AppImportFile::generateFilenames( QString dirPath )
{
    //generador dels noms dels fitxers DICOM d'un directori
    itk::GDCMSeriesFileNames::Pointer namesGenerator = itk::GDCMSeriesFileNames::New();
    namesGenerator->SetInputDirectory( qPrintable(dirPath) );
    const std::vector< std::string > &filenames = namesGenerator->GetInputFileNames();
    // convertim el vector en QStringList
    QStringList list;
    for( unsigned int i = 0; i < filenames.size(); i++ )
    {
        list += filenames[i].c_str();
    }
    return list;
}

void AppImportFile::readSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-ImportFile");
    m_workingDirectory = settings.value("workingDirectory", ".").toString();
    m_workingDicomDirectory = settings.value("workingDicomDirectory", ".").toString();
    settings.endGroup();
}

void AppImportFile::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-ImportFile");
    settings.setValue("workingDirectory", m_workingDirectory );
    settings.setValue("workingDicomDirectory", m_workingDicomDirectory );
    settings.endGroup();
}

};  // end namespace udg
