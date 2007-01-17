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
// recursos
#include "volumerepository.h"
#include "input.h"
#include "logging.h"

namespace udg {

AppImportFile::AppImportFile(QObject *parent, const char *name)
 : QObject( parent )
{
    this->setObjectName( name );
    m_openFileFilters = tr("MetaIO Images (*.mhd);;DICOM Images (*.dcm);;All Files (*)");

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
    bool ok = true;

    QString fileName = QFileDialog::getOpenFileName( 0 , tr("Chose an image filename") ,  m_workingDirectory, m_openFileFilters );

    if ( !fileName.isEmpty() )
    {
        if( loadFile( fileName ) )
        {
            // cal informar a l'aplicació de l'id del volum
            // la utilitat
            m_workingDirectory = QFileInfo( fileName ).dir().path();
            writeSettings();
            INFO_LOG( qPrintable( "S'obre el fitxer: " + fileName ) );
        }
        else
            ok = false;
    }
    else
        ok = false;

    return ok;
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
            INFO_LOG( qPrintable( "S'obre el directori: " + directoryName ) );
        }
        else
            ok = false;
    }
    else
        ok = false;

    return ok;
}

bool AppImportFile::loadFile( QString fileName )
{
    bool ok = true;

    if( m_inputReader->openFile( fileName.toLatin1() ) )
    {
        // afegim el nou volum al repositori
        m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );
    }
    else
    {
        // no s'ha pogut obrir l'arxiu per algun motiu
        ERROR_LOG( qPrintable( "No s'ha pogut obrir el fitxer: " + fileName ) );
        ok = false;
    }

    return ok;
}

bool AppImportFile::loadDirectory( QString directoryName )
{
    bool ok = true;

    if( m_inputReader->readSeries( directoryName.toLatin1() ) )
    {
        // afegim el nou volum al repositori
        m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );
    }
    else
    {
        ERROR_LOG( qPrintable( "No s'ha pogut obrir el directori: " + directoryName ) );
        ok = false;
    }

    return ok;
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
