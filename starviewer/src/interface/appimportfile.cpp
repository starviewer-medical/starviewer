/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "appimportfile.h"

#include "interfacesettings.h"
#include "starviewerapplication.h"
// qt
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
// itk
#include <itkGDCMSeriesFileNames.h> // per generar els noms dels arxius DICOM d'un directori
// recursos
#include "logging.h"

namespace udg {

AppImportFile::AppImportFile(QObject *parent, QString name)
 : QObject( parent )
{
    //TODO: De moment es desactiven els warnings en release perquè no apareixi la finestra vtkOutputWindow 
 	//      però la solució bona passa per evitar els warnings o bé redirigir-los a un fitxer. 
#ifdef QT_NO_DEBUG 
    itk::Object::GlobalWarningDisplayOff(); 
#endif 

    this->setObjectName( name );
    readSettings();
}

AppImportFile::~AppImportFile()
{
}

void AppImportFile::open()
{
    const QString MetaIOImageFilter("MetaIO Image (*.mhd)"), DICOMImageFilter("DICOM Images (*.dcm)"), AllFilesFilter("All Files (*)");
    QStringList imagesFilter;
    imagesFilter << MetaIOImageFilter << DICOMImageFilter << AllFilesFilter;

    QFileDialog *openDialog = new QFileDialog(0);
    openDialog->setWindowTitle( tr("Choose a file to open...") );
    openDialog->setDirectory( m_workingDirectory );
    openDialog->setFilters( imagesFilter );
    openDialog->selectFilter ( m_lastExtension );
    openDialog->setFileMode( QFileDialog::ExistingFiles );
    openDialog->setAcceptMode( QFileDialog::AcceptOpen );

    if( openDialog->exec() == QDialog::Accepted )
    {
        QStringList fileNames = openDialog->selectedFiles();
        
        emit selectedFiles( fileNames );
        
        m_workingDirectory = QFileInfo( fileNames.first() ).dir().path();
        m_lastExtension = openDialog->selectedFilter();
        
        writeSettings();
    }
    delete openDialog;
}

void AppImportFile::openDirectory( bool recursively )
{
    QString directoryName = QFileDialog::getExistingDirectory( 0 , tr("Choose a directory to scan") , m_workingDicomDirectory , QFileDialog::ShowDirsOnly );
    if ( !directoryName.isEmpty() )
    {
        INFO_LOG( "S'escaneja el directori: " + directoryName + " per obrir els estudis que hi contingui" );
        m_workingDicomDirectory = QFileInfo( directoryName ).dir().path();
        writeSettings();

        // llista on guardarem tots els arxius compatibles que hi ha als directoris
        QStringList filenames;
        if( recursively )
        {
            // explorem recursivament tots els directoris
            QStringList dirList;
            scanDirectories( directoryName, dirList );   
            // per cada directori, obtenim els arxius que podem tractar
            foreach( QString dirName, dirList )
            {
                filenames << generateFilenames( dirName );
            }
        }
        else // tindrem en compte únicament els arxius que hi hagi en el directori arrel sense explorar recursivament
            filenames << generateFilenames( directoryName );
        
        if( !filenames.isEmpty() )
            emit selectedFiles( filenames );
        else
            QMessageBox::warning(0, ApplicationNameString, tr("No supported input files found") );
    }
}

QStringList AppImportFile::generateFilenames( const QString &dirPath )
{
    QStringList list;
    // Comprovem que el directori tingui arxius
    QDir dir(dirPath);
    if( !dir.entryList( QDir::Files ).isEmpty() )
    {
        // Generador dels noms dels fitxers DICOM d'un directori
        itk::GDCMSeriesFileNames::Pointer namesGenerator = itk::GDCMSeriesFileNames::New();
        namesGenerator->SetInputDirectory( qPrintable(dirPath) );
        const std::vector< std::string > &filenames = namesGenerator->GetInputFileNames();
        // convertim el vector en QStringList
        for( unsigned int i = 0; i < filenames.size(); i++ )
        {
            list += filenames[i].c_str();
        }
    }

    return list;
}

void AppImportFile::scanDirectories( const QString &rootPath, QStringList &dirsList )
{
    QDir rootDir( rootPath );
    if( rootDir.exists() )
    {
        // afegim el directori actual a la llista
        dirsList << rootPath;
        // busquem si tenim més directoris
        QStringList subdirs = rootDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot );
        if( !subdirs.isEmpty() )
        {
            // per cada subdirectori escanejem recursivament
            foreach( QString subDir, subdirs )
            {
                scanDirectories( rootPath + "/" + subDir, dirsList );
            }
        }
    }
}

void AppImportFile::readSettings()
{
    Settings settings;

    m_workingDirectory = settings.getValue( InterfaceSettings::openFileLastPathKey).toString();
    m_workingDicomDirectory = settings.getValue( InterfaceSettings::openDirectoryLastPathKey).toString();
    m_lastExtension = settings.getValue( InterfaceSettings::openFileLastFileExtensionKey).toString();
}

void AppImportFile::writeSettings()
{
    Settings settings;

    settings.setValue( InterfaceSettings::openFileLastPathKey, m_workingDirectory );
    settings.setValue( InterfaceSettings::openDirectoryLastPathKey, m_workingDicomDirectory );
    settings.setValue( InterfaceSettings::openFileLastFileExtensionKey, m_lastExtension );
}

};  // end namespace udg
