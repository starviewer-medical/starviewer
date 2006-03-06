/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "appimportfile.h"
// qt
#include <qfiledialog.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <qfiledialog.h>

// recursos
#include "volumerepository.h"
#include "input.h"
#include "output.h"
#include <iostream>

namespace udg {

AppImportFile::AppImportFile(QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_openFileFilters = tr("MetaIO Images (*.mhd);;DICOM Images (*.dcm);;All Files (*)");
    
    m_volumeRepository = udg::VolumeRepository::getRepository();
    m_inputReader = new udg::Input;
    
    readSettings();
}

AppImportFile::~AppImportFile()
{
}

void AppImportFile::open()
{
    QString fileName = QFileDialog::getOpenFileName( m_workingDirectory, m_openFileFilters, 0 , tr("Open file dialog"),tr("Chose an image filename") );
            
    if ( !fileName.isEmpty() )
    {
        ///\TODO: on decidim si s'obre una nova finestra o no? és responsabilitat del handler o de la mainwindow
        if( loadFile( fileName ) )  
        {
            // cal informar a l'aplicació de l'id del volum
            // la utilitat
            m_workingDirectory = QFileInfo(fileName).dirPath();
        }
    }
}

bool AppImportFile::loadFile( QString fileName )
{
    bool ok = true; 

    // indiquem que ens obri el fitxer
    if( QFileInfo( fileName ).extension() == "dcm") // petita prova per provar lectura de DICOM's
    {
        if( m_inputReader->readSeries( QFileInfo(fileName).dirPath( TRUE ).latin1() ) ) 
        { 
            // afegim el nou volum al repositori
            m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );            
        }
        else
        {
            ok = false;
        }
    }
    else
    {
        if( m_inputReader->openFile( fileName.latin1() ) ) 
        { 
            // afegim el nou volum al repositori
            m_volumeID = m_volumeRepository->addVolume( m_inputReader->getData() );            
        }
        else
        {
            // no s'ha pogut obrir l'arxiu per algun motiu
            ok = false;
        }
    }

    return ok;
}

void AppImportFile::finish()
{
    writeSettings();
}

void AppImportFile::readSettings()
{
    QSettings settings;
    settings.setPath("GGG", "StarViewer-App-ImportFile");
    settings.beginGroup("/StarViewer-App-ImportFile");
    m_workingDirectory = settings.readEntry("/workingDirectory", ".");
    settings.endGroup();
}

void AppImportFile::writeSettings()
{
    QSettings settings;
    settings.setPath("GGG", "StarViewer-App-ImportFile");
    settings.beginGroup("/StarViewer-App-ImportFile");
    settings.writeEntry("/workingDirectory", m_workingDirectory );
    settings.endGroup();
}

};  // end namespace udg 
