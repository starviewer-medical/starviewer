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
#include <QProgressDialog>
#include <QMessageBox>
// recursos
#include "volumerepository.h"
#include "input.h"
#include "logging.h"

namespace udg {

AppImportFile::AppImportFile(QObject *parent, QString name)
 : QObject( parent )
{
    this->setObjectName( name );

    m_volumeRepository = udg::VolumeRepository::getRepository();
    m_inputReader = new udg::Input;

    m_progressDialog = new QProgressDialog;
    m_progressDialog->setRange( 0 , 100 );
    m_progressDialog->setMinimumDuration( 0 );
    m_progressDialog->setWindowTitle( tr("Serie loading") );
    // atenció: el missatge triga una miqueta a aparèixer...
    m_progressDialog->setLabelText( tr("Loading, please wait...") );
    m_progressDialog->setCancelButton( 0 );
    connect( m_inputReader , SIGNAL( progress(int) ) , m_progressDialog , SLOT( setValue(int) ) );

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
            if( loadFile( fileName ) )
            {
                // cal informar a l'aplicació de l'id del volum
                // la utilitat
                m_lastOpenedFilename = fileName;
                openedImage = true;
            }
            else
            {
                QMessageBox::critical( 0, tr("Error"), tr("Cannot read this File or File Format is wrong") );
            }
        }
        else if (selectedFilter == KeyImageNoteFilter)
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
        if( loadDirectory( directoryName ) )
        {
            m_workingDicomDirectory = QFileInfo( directoryName ).dir().path();
            writeSettings();
            INFO_LOG( "S'obre el directori: " + directoryName );
        }
        else
            ok = false;
    }
    else
        ok = false;

    return ok;
}

int AppImportFile::loadFile( QString fileName )
{
    int errorCode;
    errorCode = m_inputReader->openFile( fileName.toLatin1() );
    switch( errorCode )
    {
    case Input::NoError:
        m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );
        break;
    case Input::InvalidFileName:
        break;
    case Input::SizeMismatch:
        break;
    }
    return errorCode;
}

int AppImportFile::loadDirectory( QString directoryName )
{
    int errorCode;
    errorCode = m_inputReader->readSeries( directoryName.toLatin1() );
    switch( errorCode )
    {
    case Input::NoError:
        m_volumeID = m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );
        break;
    case Input::InvalidFileName:
        break;
    case Input::SizeMismatch:
        break;
    }
    return errorCode;
}

QString AppImportFile::getLastOpenedFilename()
{
    return m_lastOpenedFilename;
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
