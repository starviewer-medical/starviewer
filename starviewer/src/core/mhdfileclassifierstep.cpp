/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "mhdfileclassifierstep.h"

#include "logging.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "image.h"
#include "volume.h"

#include <QDateTime>
#include <QFileInfo>

#include <metaCommand.h> // fer servir la llibreria metaIO de les itk
#include <vtkMetaImageReader.h> // fer servir el reader de vtk

namespace udg {

MHDFileClassifierStep::MHDFileClassifierStep()
 : PatientFillerStep()
{
}

MHDFileClassifierStep::~MHDFileClassifierStep()
{
}

bool MHDFileClassifierStep::fill()
{
    bool ok = false;
    // processarem cadascun dels arxius de l'input i els anirem col·locant a Patient
    if( m_input )
    {
        ok = true;
        QStringList fileList = m_input->getFilesList();
        foreach( QString file, fileList )
        {
            if( !classifyFile( file ) )
            {
                ok = false;
                break;
            }
        }
        if( ok )
            m_input->addLabel("MHDFileClassifierStep");
    }
    else
        DEBUG_LOG("No tenim input!");

    return ok;
}

bool MHDFileClassifierStep::classifyFile( QString file )
{
    // primer comprovem si l'arxiu es pot processar
    bool ok = false;
    vtkMetaImageReader *mhdReader = vtkMetaImageReader::New();
    switch( mhdReader->CanReadFile( qPrintable(file) ) )
    {
        case 0: // no és un arxiu mhd :(
            ok = false;
            DEBUG_LOG( file + " no es pot llegir com arxiu mhd vàlid amb vtkMetaImageReader");
            break;

        case 1: // I think I can read the file but I cannot prove it
            DEBUG_LOG( "vtkMetaImageReader creu que pot llegir l'arxiu " + file + " però no pot provar-ho de totes totes" );
            ok = true;
            break;

        case 2: // I definitely can read the file
            DEBUG_LOG( "vtkMetaImageReader assegura que pot llegir l'arxiu " + file );
            ok = true;
            break;

        case 3: // I can read the file and I have validated that I am the correct reader for this file
            DEBUG_LOG( "vtkMetaImageReader assegura que pot llegir l'arxiu " + file + " i que a més ha validat ser el reader adequat per llegir l'arxiu" );
            ok = true;
            break;
    }
    if( ok )
    {
        // abans de res comprovar que l'arxiu no estigui ja classificat
        // comprovem primer que l'arxiu no estigui ja dins de l'estructura, el qual vol dir que ja l'han classificat
        bool found = false;
        int i = 0;
        while( i < m_input->getNumberOfPatients() && !found )
        {
            found = m_input->getPatient( i )->hasFile( file );
            i++;
        }
        if( found )
            return true;


        //TODO faltaria comprovar si tenim algun pacient igual o no? i assignar al mateix estudi o algo semblant? En certa manera si sempre assignem la mateixa informació, posteriorment els pacients que anem creant, ja es fusionaran ells mateixos. Els id's que han de ser diferents haurien de ser els de les series

        // creem el pacient
        Patient *patient = new Patient;
        QFileInfo fileInfo(file);
        patient->setFullName( "MHD File " + fileInfo.fileName() );
        patient->setID( "MHDPatient" + fileInfo.fileName() );
        m_input->addPatient( patient );

        // creem l'estudi
        Study *study = new Study;
        study->setInstanceUID( "MHDStudy-#123456#" );
        study->setDate( QDate::currentDate() );
        study->setTime( QTime::currentTime() );
        study->setID( "MHDStudy-#123456#" );
        study->setDescription( "MHD Study" );
        patient->addStudy( study );

        // creem la serie
        Series *series = new Series;
        static int seriesUID = 123456;
        static int seriesNumber = 0;
        series->setInstanceUID( QString("MHDSeries-#%1#").arg( seriesUID++ ) );
        series->setModality( "OT" ); // TODO de moment li diem OTHER, però si l'mhd té aquesta info, li donarem de l'mhd
        series->setSeriesNumber( QString::number( seriesNumber+= 101 ) );
        series->setDate( QDate::currentDate() );
        series->setTime( QTime::currentTime() );
//         series->setPatientPosition(); -> si podem l'hauríem d'obtenir de l'mhd
        series->setDescription( "MHD Series" );
        series->addFilePath( file );
        study->addSeries( series );

        // aquí fem el pas de l'image filler stepf
        // creem la imatge
        Image *image = new Image;
        image->setPath( file );
        static int mhdSOPIntanceUID = 0;
        image->setSOPInstanceUID( QString("MHDImage-#%1#").arg(mhdSOPIntanceUID++) );
        // TODO obtenir l'spacing i thickness de l'mhd, aquests valors són temporals i per defecte únicament
        image->setPixelSpacing( 1., 1. );
        image->setSliceThickness( 1. );
        image->setSamplesPerPixel( 1 );
        // TODO obtenir aquests valor de l'mhd si és possible
//         image->setImageOrientationPatient( orientation );
//         image->setPatientOrientation( value );
        // si no tenim aquest valor, calcular partir dels direction cosines
//         image->setPatientOrientation( patientOrientationString );
//         image->setImagePositionPatient( position );
//         image->setPhotometricInterpretation( ? );
//         image->setRows( ? );
//         image->setColumns( ? );
//         image->setBitsAllocated( ? );
//         image->setBitsStored( ? );
        series->addImage( image );

        // aquí fem el pasdel volume generator step
        Volume *volume = new Volume;
        volume->setImages( series->getImages() );
        series->addVolume(volume);
        DEBUG_LOG("Generem un volum MHD!");
    }

    return ok;
}

}
