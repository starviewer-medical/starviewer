/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imagefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "dicomdictionary.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomsequenceattribute.h"
#include "dicomsequenceitem.h"
#include "dicomvalueattribute.h"

#include <cmath> // pel fabs
#include <QApplication> //Per el process events, TODO Treure i fer amb threads.

namespace udg {

ImageFillerStep::ImageFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

ImageFillerStep::~ImageFillerStep()
{
}

bool ImageFillerStep::fill()
{
    bool ok = false;
    // processarem cadascun dels pacients que hi hagi en l'input i per cadascun totes les sèries que siguin de tipus imatge
    if( m_input )
    {
        unsigned int i = 0;
        while( i < m_input->getNumberOfPatients() )
        {
            Patient *patient = m_input->getPatient( i );
            this->processPatient( patient );
            i++;
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

bool ImageFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    bool ok = false;

    DICOMTagReader * dicomReader = m_input->getDICOMFile();

    Image *image = new Image;
    image->setPath( dicomReader->getFileName() );

    if( processImage( image , dicomReader ) )
    {
        ok = true;
        m_input->getCurrentSeries()->addImage( image );
        m_input->setCurrentImage( image );
        m_input->addLabelToSeries("ImageFillerStep", m_input->getCurrentSeries() );
    }

    return ok;
}

void ImageFillerStep::processPatient( Patient *patient )
{
    QList<Study *> studyList = patient->getStudies();
    foreach( Study *study, studyList )
    {
        QList<Series *> seriesList = study->getSeries();
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
    }
}

void ImageFillerStep::processSeries( Series *series )
{
    // Podrem tenir o bé Images, o bé KINs o bé PresentationStates
    if( isImageSeries(series) )
    {
        bool ok = false;
        foreach (QString file, series->getFilesPathList())
        {
            Image *image = new Image;
            image->setPath( file );

            if( processImage( image ) )
            {
                ok = true;
                series->addImage( image );
            }
            else
                DEBUG_LOG( "L'arxiu [" + file + "] no es pot afegir com a imatge a la sèrie amb UID [" + series->getInstanceUID() + "]" );

            qApp->processEvents();
        }
        if( ok )
            m_input->addLabelToSeries("ImageFillerStep", series );
        else
            DEBUG_LOG( "La sèrie amb UID [" + series->getInstanceUID() + "] no té cap arxiu que sigui una imatge. No s'etiquetarà amb l'ImageFillerStep." );
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és una sèrie d'Imatges. És de modalitat: " + series->getModality() );
    }
}

bool ImageFillerStep::processImage( Image *image )
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( image->getPath() );
    if( !ok )
    {
        DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + image->getPath() );
        return false;
    }

    return processImage( image , &dicomReader );
}

bool ImageFillerStep::processImage( Image *image , DICOMTagReader * dicomReader )
{

    // comprovem si l'arxiu és una imatge, per això caldrà que existeixi el tag PixelData
    bool ok = dicomReader->tagExists( DICOMPixelData );
    if( ok )
    {
        image->setSOPInstanceUID( dicomReader->getAttributeByName( DICOMSOPInstanceUID ) );
        image->setInstanceNumber( dicomReader->getAttributeByName( DICOMInstanceNumber ) );
        // Calculem el pixel spacing
        computePixelSpacing(image,dicomReader);

        QString value;
        value = dicomReader->getAttributeByName( DICOMSliceThickness );
        if( !value.isEmpty() )
            image->setSliceThickness( value.toDouble() );

        QStringList list;
        value = dicomReader->getAttributeByName( DICOMImageOrientationPatient );
        list = value.split( "\\" );
        if( list.size() == 6 )
        {
            double orientation[6];
            for( int i = 0; i < 6; i++ )
            {
                orientation[ i ] = list.at( i ).toDouble();
            }
            image->setImageOrientationPatient( orientation );

            // cerquem l'string amb la orientació del pacient
            value = dicomReader->getAttributeByName( DICOMPatientOrientation );
            if( !value.isEmpty() )
                image->setPatientOrientation( value );
            else  // si no tenim aquest valor, el calculem a partir dels direction cosines
            {
                // I ara ens disposem a crear l'string amb l'orientació del pacient
                double *orientation = (double *)image->getImageOrientationPatient();
                double dirCosinesX[3], dirCosinesY[3], dirCosinesZ[3];
                for( int i = 0; i < 3; i++ )
                {
                    dirCosinesX[i] = orientation[i];
                    dirCosinesY[i] = orientation[3+i];
                    dirCosinesZ[i] = orientation[6+i];
                }
                QString patientOrientationString;
                // \TODO potser el delimitador hauria de ser '\' en comptes de ','
                patientOrientationString = this->mapDirectionCosinesToOrientationString( dirCosinesX );
                patientOrientationString += "\\";
                patientOrientationString += this->mapDirectionCosinesToOrientationString( dirCosinesY );
                patientOrientationString += "\\";
                patientOrientationString += this->mapDirectionCosinesToOrientationString( dirCosinesZ );
                image->setPatientOrientation( patientOrientationString );
            }
        }
        else
        {
            /* Si la modalitat no requereix el image plane module ( CR per exemple ) no disposem de ImageOrientationPatient.
             * Això fa que el tag PatientOrientation no s'ompli.El PatientOrientation es necessari en cas que no hi hagi
             * ImageOrientationPatient i ImagePositionPatient.
             */
            // \TODO Part afegida per sortir del pas. S'hauria de refer aquesta part tenint mes en compte la dependencia de tags
            value = dicomReader->getAttributeByName( DICOMPatientOrientation );
            if( !value.isEmpty() )
                image->setPatientOrientation( value );
            else
                DEBUG_LOG("No s'ha pogut trobar informació d'orientació del pacient, ni ImageOrientationPatient ni PatientOrientation. Modalitat de la imatge: [" + dicomReader->getAttributeByName(DICOMModality) + "]");
        }
        value = dicomReader->getAttributeByName( DICOMImagePositionPatient );
        if( !value.isEmpty() )
        {
            list = value.split("\\");
            if( list.size() == 3 )
            {
                double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
                image->setImagePositionPatient( position );
            }
        }
        else
        {
            DEBUG_LOG("La imatge no conté informació de l'origen. Modalitat: [" + dicomReader->getAttributeByName(DICOMModality) + "]");
        }

        image->setSamplesPerPixel( dicomReader->getAttributeByName( DICOMSamplesPerPixel ).toInt() );
        image->setPhotometricInterpretation( dicomReader->getAttributeByName( DICOMPhotometricInterpretation ) );
        image->setRows( dicomReader->getAttributeByName( DICOMRows ).toInt() );
        image->setColumns( dicomReader->getAttributeByName( DICOMColumns ).toInt() );
        image->setBitsAllocated( dicomReader->getAttributeByName( DICOMBitsAllocated ).toInt() );
        image->setBitsStored( dicomReader->getAttributeByName( DICOMBitsStored ).toInt() );
        image->setPixelRepresentation( dicomReader->getAttributeByName( DICOMPixelRepresentation ).toInt() );

        value = dicomReader->getAttributeByName( DICOMRescaleSlope );
        if( value.toDouble() == 0 )
            image->setRescaleSlope( 1. );
        else
            image->setRescaleSlope( value.toDouble() );

        image->setRescaleIntercept( dicomReader->getAttributeByName( DICOMRescaleIntercept ).toDouble() );
        // llegim els window levels
        QStringList windowWidthList = dicomReader->getAttributeByName( DICOMWindowWidth ).split("\\");
        QStringList windowLevelList = dicomReader->getAttributeByName( DICOMWindowCenter ).split("\\");
        for( int i = 0; i < windowWidthList.size(); i++ )
            image->addWindowLevel( windowWidthList.at(i).toDouble(), windowLevelList.at(i).toDouble() );
        // i després les respectives descripcions si n'hi ha
        image->setWindowLevelExplanations( dicomReader->getAttributeByName( DICOMWindowCenterWidthExplanation ).split("\\") );

        int frames = dicomReader->getAttributeByName( DICOMNumberOfFrames ).toInt();
        image->setNumberOfFrames( frames ? frames : 1 );

        if (dicomReader->tagExists( DICOMSliceLocation ))
        {
            image->setSliceLocation( dicomReader->getAttributeByName( DICOMSliceLocation ) );
        }

        image->setImageType( dicomReader->getAttributeByName( DICOMImageType ) );
        value = dicomReader->getAttributeByName( DICOMImageLaterality );
        if( !value.isEmpty() )
            image->setImageLaterality( value.at(0) );
        // De moment només ho aprofitarem per mammografia, però pot ser vàlid per altres modalitats
        DICOMSequenceAttribute *viewCodeSequence = dicomReader->getSequenceAttribute(DICOMViewCodeSequence);
        if( viewCodeSequence )
        {
            QList<DICOMSequenceItem *> items = viewCodeSequence->getItems();
            // Per definició, només hauríem de tenir un ítem
            switch( items.count() )
            {
            case 0:
                DEBUG_LOG("ViewCodeSequence no té cap ítem o no existeix");
                break;
            
            case 1:
                image->setViewCodeMeaning( items.at(0)->getValueAttribute(DICOMCodeMeaning)->getValueAsQString() );
                break;
            
            default:
                DEBUG_LOG("ViewCodeSequence té més d'un ítem!");
                break;
            }
        }
        
        // Només pel cas que sigui DX tindrem aquest atribut a nivell d'imatge
        image->setViewPosition( dicomReader->getAttributeByName( DICOMViewPosition ) );
    }
    else
    {
        DEBUG_LOG( "L'arxiu [" + image->getPath() + "] no conté el tag PixelData" );
    }

    return ok;
}

void ImageFillerStep::computePixelSpacing( Image *image, DICOMTagReader *dicomReader )
{
    Q_ASSERT(image);
    Q_ASSERT(dicomReader);
    
    QString value;
    // \TODO Txapussa per sortir del pas. Serveix per calcular correctament el PixelSpacing
    QString modality = dicomReader->getAttributeByName( DICOMModality );
    if ( modality == "CT" || modality == "MR")
    {
        value = dicomReader->getAttributeByName( DICOMPixelSpacing );
    }
    else if ( modality == "US" )
    {
        DICOMSequenceAttribute *ultraSoundsRegionsSequence = dicomReader->getSequenceAttribute(DICOMSequenceOfUltrasoundRegions);
        if( ultraSoundsRegionsSequence ) // Ho hem de comprovar perquè és opcional.
        {
            // Aquesta seqüència pot tenir més d'un ítem. TODO Nosaltres només tractem el primer, però ho hauríem de fer per tots, 
            // ja que defineix més d'una regió i podríem estar obtenint informació equivocada
            QList<DICOMSequenceItem *> items = ultraSoundsRegionsSequence->getItems();
            if( !items.isEmpty() )
            {
                int physicalUnitsX = items.at(0)->getValueAttribute(DICOMPhysicalUnitsXDirection)->getValueAsInt();
                int physicalUnitsY = items.at(0)->getValueAttribute(DICOMPhysicalUnitsYDirection)->getValueAsInt();

                if ( physicalUnitsX == 3 && physicalUnitsY == 3) // 3 significa que les unitats son cm
                {
                    double physicalDeltaX = items.at(0)->getValueAttribute(DICOMPhysicalDeltaX)->getValueAsDouble();
                    double physicalDeltaY = items.at(0)->getValueAttribute(DICOMPhysicalDeltaY)->getValueAsDouble();

                    physicalDeltaX = std::abs( physicalDeltaX )* 10.;
                    physicalDeltaY = std::abs( physicalDeltaY ) * 10.;

                    value = QString("%1").arg(physicalDeltaX);
                    value += "\\";
                    value += QString("%1").arg(physicalDeltaY);
                    DEBUG_LOG( QString("Pixel Spacing Ultrasound: %1").arg(value) );
                }
            }
        }
    }
    else // Per altres modalitats li assignarem a partir d'aquest tag
    {
        value = dicomReader->getAttributeByName( DICOMImagerPixelSpacing );
    }
    
    QStringList list;
    if ( !value.isEmpty() )
    {
        list = value.split( "\\" );
        if( list.size() == 2 )
            image->setPixelSpacing( list.at(0).toDouble(), list.at(1).toDouble() );
        else
            DEBUG_LOG("No s'ha trobat cap valor de pixel spacing definit de forma estàndar esperada. Modalitat de la imatge: [" + modality + "]" );
    }
}

QString ImageFillerStep::mapDirectionCosinesToOrientationString( double vector[3] )
{
    char *orientation = new char[4];
    char *optr = orientation;
    *optr='\0';

    char orientationX = vector[0] < 0 ? 'R' : 'L';
    char orientationY = vector[1] < 0 ? 'A' : 'P';
    char orientationZ = vector[2] < 0 ? 'F' : 'H';

    double absX = fabs( vector[0] );
    double absY = fabs( vector[1] );
    double absZ = fabs( vector[2] );

    int i;
    for ( i = 0; i < 3; ++i )
    {
        if ( absX > .0001 && absX > absY && absX > absZ )
        {
            *optr++= orientationX;
            absX = 0;
        }
        else if ( absY > .0001 && absY > absX && absY > absZ )
        {
            *optr++= orientationY;
            absY = 0;
        }
        else if ( absZ > .0001 && absZ > absX && absZ > absY )
        {
            *optr++= orientationZ;
            absZ = 0;
        }
        else break;
        *optr='\0';
    }
    return QString( orientation );
}

}
