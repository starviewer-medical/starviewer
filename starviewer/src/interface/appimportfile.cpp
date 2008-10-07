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

void AppImportFile::openDirectory()
{
    QString directoryName = QFileDialog::getExistingDirectory( 0 , tr("Choose a directory") , m_workingDicomDirectory , QFileDialog::ShowDirsOnly );
    if ( !directoryName.isEmpty() )
    {
        emit selectedFiles( this->generateFilenames( directoryName ) );
        m_workingDicomDirectory = QFileInfo( directoryName ).dir().path();
        writeSettings();
        INFO_LOG( "S'obre el directori: " + directoryName );
    }
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
    m_lastExtension = settings.value( "defaultExtension", "MetaIO Image (*.mhd)" ).toString();
    settings.endGroup();
}

void AppImportFile::writeSettings()
{
    QSettings settings;
    settings.beginGroup("Starviewer-App-ImportFile");
    settings.setValue("workingDirectory", m_workingDirectory );
    settings.setValue("workingDicomDirectory", m_workingDicomDirectory );
    settings.setValue("defaultExtension", m_lastExtension );
    settings.endGroup();
}

};  // end namespace udg
