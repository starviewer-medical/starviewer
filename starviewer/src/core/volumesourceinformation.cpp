/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumesourceinformation.h"

#include <vtkMath.h> // pel cross
#include "dicomtagreader.h"
#include "logging.h"

namespace udg {

VolumeSourceInformation::VolumeSourceInformation() : m_numberOfPhases(1), m_numberOfSlices(1)
{
    m_windowLevel[0] = 0.0;
    m_windowLevel[1] = 0.0;
    m_dicomTagReader = new DICOMTagReader();
}

VolumeSourceInformation::~VolumeSourceInformation()
{
}

QString VolumeSourceInformation::getRevertedPatientOrientationString()
{
    int i = 0;
    QString reversed;
    while( i < m_patientOrientationString.size() )
    {
        if( m_patientOrientationString.at( i ) == 'L' )
            reversed += "R";
        else if( m_patientOrientationString.at( i ) == 'R' )
            reversed += "L";
        else if( m_patientOrientationString.at( i ) == 'A' )
            reversed += "P";
        else if( m_patientOrientationString.at( i ) == 'P' )
            reversed += "A";
        else if( m_patientOrientationString.at( i ) == 'S' )
            reversed += "I";
        else if( m_patientOrientationString.at( i ) == 'I' )
            reversed += "S";
        else
            reversed += m_patientOrientationString.at( i );
        i++;
    }
    return reversed;
}

void VolumeSourceInformation::setDirectionCosines( double directionCosines[9] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<9; i++ )
        m_directionCosines[i] = directionCosines[i];
}

void VolumeSourceInformation::setDirectionCosines( double xCosines[3], double yCosines[3], double zCosines[3] )
{
    this->setXDirectionCosines( xCosines );
    this->setYDirectionCosines( yCosines );
    this->setZDirectionCosines( zCosines );
}

void VolumeSourceInformation::setXDirectionCosines( double xCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        m_directionCosines[i] = xCosines[i];
}

void VolumeSourceInformation::setXDirectionCosines( double x1Cosines, double x2Cosines, double x3Cosines )
{
    m_directionCosines[0] = x1Cosines;
    m_directionCosines[1] = x2Cosines;
    m_directionCosines[2] = x3Cosines;
}

void VolumeSourceInformation::setYDirectionCosines( double yCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        m_directionCosines[i+3] = yCosines[i];
}

void VolumeSourceInformation::setYDirectionCosines( double y1Cosines, double y2Cosines, double y3Cosines )
{
    m_directionCosines[3] = y1Cosines;
    m_directionCosines[4] = y2Cosines;
    m_directionCosines[5] = y3Cosines;
}

void VolumeSourceInformation::setZDirectionCosines( double zCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        m_directionCosines[i+6] = zCosines[i];
}

void VolumeSourceInformation::setZDirectionCosines( double z1Cosines, double z2Cosines, double z3Cosines )
{
    m_directionCosines[6] = z1Cosines;
    m_directionCosines[7] = z2Cosines;
    m_directionCosines[8] = z3Cosines;
}

void VolumeSourceInformation::getDirectionCosines( double directionCosines[9] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<9; i++ )
        directionCosines[i] = m_directionCosines[i];
}

void VolumeSourceInformation::getDirectionCosines( double xCosines[3], double yCosines[3], double zCosines[3] )
{
    this->getXDirectionCosines( xCosines );
    this->getYDirectionCosines( yCosines );
    this->getZDirectionCosines( zCosines );
}

void VolumeSourceInformation::getXDirectionCosines( double xCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        xCosines[i] = m_directionCosines[i];
}

void VolumeSourceInformation::getYDirectionCosines( double yCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        yCosines[i] = m_directionCosines[i+3];
}

void VolumeSourceInformation::getZDirectionCosines( double zCosines[3] )
{
    // \TODO és més eficient amb un memcpy?
    for( int i = 0; i<3; i++ )
        zCosines[i] = m_directionCosines[i+6];
}

bool VolumeSourceInformation::loadDicomData( QString filename )
{
    if( m_dicomTagReader->setFile( filename ) )
    {
        // primer llegim els valors dels window level
        QString windowWidth = m_dicomTagReader->getAttributeByName( DCM_WindowWidth );
        QStringList windowWidthList = windowWidth.split("\\");
        QString windowLevel = m_dicomTagReader->getAttributeByName( DCM_WindowCenter );
        QStringList windowLevelList = windowLevel.split("\\");

        double *wl;
        for( int i = 0; i < windowWidthList.size(); i++ )
        {
            wl = new double[2];
            wl[0] = windowWidthList.at(i).toDouble();
            wl[1] = windowLevelList.at(i).toDouble();
            m_windowLevelList.push_back( wl );
        }
        // i després les respectives descripcions si n'hi ha
        QString descriptions = m_dicomTagReader->getAttributeByName( DCM_WindowCenterWidthExplanation );
        QStringList descriptionsList = descriptions.split("\\");
        foreach( QString desc , descriptionsList )
        {
            m_windowLevelDescriptions << desc;
        }

        this->setSeriesDescription( m_dicomTagReader->getAttributeByName( DCM_SeriesDescription ) );
        this->setSeriesInstanceUID( m_dicomTagReader->getAttributeByName( DCM_SeriesInstanceUID ) );

        //obtenim l'string amb la posició del pacient relativa a la màquina(series level). Obligatori per MR i CT. No ha d'estar present si Patient Orientation Code Sequence (0054,0410) hi és, altrament és camp obligatori.
        this->setPatientPosition( m_dicomTagReader->getAttributeByTag( 0x0018, 0x5100 ).trimmed() );

        //obtenim la orientació de la imatge(image level). Això ens designarà la direcció anatòmica (R,L,P,A,H,F) dels pixels que van d'esquerra-dreta/dalt-abaix. És un camp requerit si l'imatge no requereix Image Orientation(0020,0037) i Image Position(0020,0032)
        QString value = m_dicomTagReader->getAttributeByTag( 0x0020, 0x0020 );
        if( !value.isEmpty() )
        {
            value.replace( QString( "\\" ) , QString( "," ) );
            this->setPatientOrientationString( value );
        }
        else
        {
            // si no tenim la informació directament l'haurem de deduir a partir dels direction cosines
            // l'Image Orientation
            value = m_dicomTagReader->getAttributeByTag( 0x0020,0x0037 );
            if( !value.isEmpty() )
            {
                // passem de l'string als valors double
                double dirCosinesValuesX[3] , dirCosinesValuesY[3] , dirCosinesValuesZ[3];
                QStringList list = value.split( "\\" );
                if( list.size() == 6 )
                {
                    for ( int i = 0; i < 3; i++ )
                    {
                        dirCosinesValuesX[ i ] = list.at( i ).toDouble();
                        dirCosinesValuesY[ i ] = list.at( i+3 ).toDouble();
                    }

                    vtkMath::Cross( dirCosinesValuesX , dirCosinesValuesY , dirCosinesValuesZ );
                    // I ara ens disposem a crear l'string amb l'orientació del pacient
                    QString patientOrientationString;

                    patientOrientationString = this->getOrientation( dirCosinesValuesX );
                    patientOrientationString += ",";
                    patientOrientationString += this->getOrientation( dirCosinesValuesY );
                    patientOrientationString += ",";
                    patientOrientationString += this->getOrientation( dirCosinesValuesZ );
                    this->setPatientOrientationString( patientOrientationString );
                    this->setXDirectionCosines( dirCosinesValuesX );
                    this->setYDirectionCosines( dirCosinesValuesY );
                    this->setZDirectionCosines( dirCosinesValuesZ );

                    DEBUG_LOG("Patient orientation string: " + patientOrientationString );
                }
                else
                {
                    // hi ha algun error en l'string ja que han de ser 2 parells de 3 valors
                    DEBUG_LOG( "No s'ha pogut determinar l'orientació del pacient (Tags 0020|0020 , 0020|0037) : " + value );
                }
            }
            else
            {
                // no podem obtenir l'string d'orientació del pacient
            }
        }
        // nom de la institució on s'ha fet l'estudi
        value = m_dicomTagReader->getAttributeByTag( 0x0008, 0x0080 );
        if( !value.isEmpty() )
            this->setInstitutionName( value );
        else
        {
            // no tenim aquesta informació \TODO cal posar res?
            this->setInstitutionName( QObject::tr( "Unknown" ) );
        }

        // nom del pacient
        value = m_dicomTagReader->getAttributeByTag( 0x0010,0x0010 );
        if( !value.isEmpty() )
        {
            // pre-tractament per treure caràcters estranys com ^ que en alguns casos fan de separadors en comptes dels espais
            while( value.indexOf("^") >= 0 )
                value.replace( value.indexOf("^") , 1 , QString(" ") );
            this->setPatientName( value );
        }
        // ID del pacient
        this->setPatientID( m_dicomTagReader->getAttributeByTag( 0x0010,0x0020 ) );

        // data de l'estudi, la data està en format YYYYMMDD
        this->setStudyDate( m_dicomTagReader->getAttributeByTag( 0x0008,0x0020 ) );

        // hora de l'estudi, format HHMMSS
        this->setStudyTime( m_dicomTagReader->getAttributeByTag( 0x0008,0x0030 ) );

        // accession number
        this->setAccessionNumber( m_dicomTagReader->getAttributeByTag( 0x0008,0x0050 ) );

        // Protocol name
        this->setProtocolName( m_dicomTagReader->getAttributeByTag( 0x0018,0x1030 ) );

        //Number of Phases, TAG PRIVAT PHILIPS
        this->setNumberOfPhases( m_dicomTagReader->getAttributeByTag( 0x2001,0x1017 ).toInt() );

        //Number of Slices Per Phase, nombre de llesques per cada fase, TAG PRIVAT PHILIPS
        this->setNumberOfSlices( m_dicomTagReader->getAttributeByTag( 0x2001,0x1018 ).toInt() );
    }
    else
        DEBUG_LOG("El DICOMTagReader no ha pogut llegir l'arxiu " + filename );
}

void VolumeSourceInformation::setFilenames( QStringList filenames )
{
    m_filenamesList = filenames;
    if( !m_filenamesList.isEmpty() )
        this->loadDicomData( m_filenamesList.at(0) ); //\TODO ara es fa així, però podria ser que tinguèssim un tracte més "refinat"
    else
        WARN_LOG("La llista de fitxers és buida");
}

void VolumeSourceInformation::setFilenames( QString filename )
{
    this->setFilenames( QStringList( filename ) );
}

unsigned VolumeSourceInformation::getPhotometricInterpretation()
{
    QString photoString = this->getPhotometricInterpretationAsString();

    QString qPhotoString( photoString );
    if( qPhotoString == "MONOCHROME1" )
        return Monochrome1;
    else if( qPhotoString == "MONOCHROME2" )
        return Monochrome2;
    else if( qPhotoString == "PALETTE COLOR" )
        return PaletteColor;
    else if( qPhotoString == "RGB" )
        return RGB;
    else if( qPhotoString == "YBR_FULL" )
        return YBRFull;
    else if( qPhotoString == "YBR_FULL_422" )
        return YBRFull422;
    else if( qPhotoString == "YBR_PARTIAL_422" )
        return YBRPartial422;
    else if( qPhotoString == "YBR_PARTIAL_420" )
        return YBRPartial420;
    else if( qPhotoString == "YBR_ICT" )
        return YBRICT;
    else if( qPhotoString == "YBR_RCT" )
        return YBRRCT;
    else
        return Unknown;
}

QString VolumeSourceInformation::getPhotometricInterpretationAsString()
{
    // \TODO es podria afegir una mica és de control a nivell de debug per si no es llegeix aquesta dada, perquè per exemple no existeix
    return m_dicomTagReader->getAttributeByName( DCM_PhotometricInterpretation );
}

bool VolumeSourceInformation::isMonochrome1()
{
    return this->getPhotometricInterpretation() == Monochrome1;
}

unsigned VolumeSourceInformation::getBitsStored()
{
    return m_dicomTagReader->getAttributeByName( DCM_BitsStored ).toUInt();
}

unsigned VolumeSourceInformation::getBitsAllocated()
{
    return m_dicomTagReader->getAttributeByName( DCM_BitsAllocated ).toUInt();
}

unsigned VolumeSourceInformation::getPixelRepresentation()
{
    return m_dicomTagReader->getAttributeByName( DCM_PixelRepresentation ).toUInt();
}

bool VolumeSourceInformation::hasModalityRescale()
{
    return m_dicomTagReader->tagExists( DCM_RescaleIntercept );
}

double VolumeSourceInformation::getRescaleSlope()
{
    //comprovar abans si el valor és empty o no. Si és empty, lo correcte és tornar 1 (valor neutre), el valor corresponent altrament
    QString value = m_dicomTagReader->getAttributeByName( DCM_RescaleSlope );
    if( value.isEmpty() )
        return 1.;
    else
    return value.toDouble();
}

double VolumeSourceInformation::getRescaleIntercept()
{
    //comprovar abans si el valor és empty o no. Si és empty, lo correcte és tornar 0 (valor neutre), el valor corresponent altrament
    QString value = m_dicomTagReader->getAttributeByName( DCM_RescaleIntercept );
    if( value.isEmpty() )
        return 0.0;
    else
    return value.toDouble();
}

int VolumeSourceInformation::getNumberOfWindowLevels()
{
    return m_windowLevelList.size();
}

bool VolumeSourceInformation::hasWindowLevel()
{
    if( this->getNumberOfWindowLevels() )
        return true;
    else
        return false;
}

double VolumeSourceInformation::getWindow( int position )
{
    if( this->hasWindowLevel() )
    {
        int numberOfWindowLevels = this->getNumberOfWindowLevels();
        if( position > numberOfWindowLevels || position < 0 )
            position = 0;
        return m_windowLevelList[position][0];
    }
    else
        return 0.0;
}

double VolumeSourceInformation::getLevel( int position )
{
    if( this->hasWindowLevel() )
    {
        int numberOfWindowLevels = this->getNumberOfWindowLevels();
        if( position > numberOfWindowLevels || position < 0 )
            position = 0;
        return m_windowLevelList[position][1];
    }
    else
        return 0.0;
}

double *VolumeSourceInformation::getWindowLevel( int position )
{
    if( this->hasWindowLevel() )
    {
        int numberOfWindowLevels = this->getNumberOfWindowLevels();
        if( position > numberOfWindowLevels || position < 0 )
            position = 0;
        return m_windowLevelList[position];
    }
    else
        return NULL;
}

void VolumeSourceInformation::getWindowLevel( double &window , double &level, int position )
{
    window = this->getWindow( position );
    level = this->getLevel( position );
}

void VolumeSourceInformation::getWindowLevel( double windowLevel[2], int position )
{
    windowLevel[0] = this->getWindow( position );
    windowLevel[1] = this->getLevel( position );
}

QString VolumeSourceInformation::getWindowLevelDescription( int position )
{
    if( position > m_windowLevelDescriptions.size() - 1 || position < 0 )
    {
        DEBUG_LOG("Posició incorrecte, o bé negativa o major que el nombre de descripcions disponibles");
        return QString();
    }
    else
        return m_windowLevelDescriptions.at( position );
}

bool VolumeSourceInformation::isMultiFrame()
{
    return m_dicomTagReader->tagExists( DCM_RescaleIntercept );
}

int VolumeSourceInformation::getNumberOfFrames()
{
    return m_dicomTagReader->getAttributeByName( DCM_NumberOfFrames ).toInt();
}

QString VolumeSourceInformation::getImageSOPInstanceUID( int index )
{
    return m_dicomTagReader->getAttributeByName( DCM_SOPInstanceUID );
}

void VolumeSourceInformation::setPatientPosition( QString patientPosition )
{
    m_patientPosition = patientPosition;
    DEBUG_LOG( "Patient position:: " + m_patientPosition );
//     if( m_patientPosition == "HFP" ) // Head First-Prone: Entra de cap i mira cap avall
//         setPatientOrientationString("LRPASI");
//     else if( m_patientPosition == "HFDR" ) // Head First-Decubitus Right: Entra de cap i recolzat sobre el braç dret
//         setPatientOrientationString("APLRSI");
//     else if( m_patientPosition == "FFDR" ) // Feet First-Decubitus Right: Entra de peus i recolzat sobre el braç dret
//         setPatientOrientationString("PALRIS");
//     else if( m_patientPosition == "FFP" ) // Feet First-Prone: Entra de peus i mira cap avall
//         setPatientOrientationString("RLPAIS");
//     else if( m_patientPosition == "HFS" ) // Head First-Supine: Entra de cap i mira cap amunt
//         setPatientOrientationString("RLAPSI");
//     else if( m_patientPosition == "HFDL" ) // Head First-Decubitus Left: Entra de cap i recolzat sobre el braç esquerre
//         setPatientOrientationString("PARLSI");
//     else if( m_patientPosition == "FFDL" ) // Feet First-Decubitus Left: Entra de peus i recolzat sobre el braç esquerre
//         setPatientOrientationString("APRLIS");
//     else if( m_patientPosition == "FFS" ) // Feet First-Supine: Entra de peus i mira cap amunt
//         setPatientOrientationString("LRAPIS");
//     else // No tenim l'atribut esperat
//         setPatientOrientationString("??????");
}

QString VolumeSourceInformation::getOrientation( double vector[3] )
{
    char *orientation = new char[4];
    char *optr = orientation;
    *optr='\0';

    char orientationX = vector[0] < 0 ? 'R' : 'L';
    char orientationY = vector[1] < 0 ? 'A' : 'P';
    char orientationZ = vector[2] < 0 ? 'I' : 'S';

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

};  // end namespace udg
