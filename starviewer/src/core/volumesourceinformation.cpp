/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumesourceinformation.h"
#define HAVE_CONFIG_H 1
// #include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcvrds.h" // DcmDecimalString
#include "dcmtk/dcmdata/dcvrlo.h" // DcmLongString
#include "logging.h"

namespace udg {

VolumeSourceInformation::VolumeSourceInformation()
{
    m_windowLevel[0] = 0.0;
    m_windowLevel[1] = 0.0;
    m_dicomData = 0;
}

VolumeSourceInformation::~VolumeSourceInformation()
{
}

QString VolumeSourceInformation::getRevertedPatientOrientationString()
{
    int i = 0;
    QString reverted;
    while( i < m_patientOrientationString.size() )
    {
        if( m_patientOrientationString.at( i ) == 'L' )
            reverted += "R";
        else if( m_patientOrientationString.at( i ) == 'R' )
            reverted += "L";
        else if( m_patientOrientationString.at( i ) == 'A' )
            reverted += "P";
        else if( m_patientOrientationString.at( i ) == 'P' )
            reverted += "A";
        else if( m_patientOrientationString.at( i ) == 'S' )
            reverted += "I";
        else if( m_patientOrientationString.at( i ) == 'I' )
            reverted += "S";
        else
            reverted += m_patientOrientationString.at( i );
        i++;
    }
    return reverted;
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

bool VolumeSourceInformation::loadDicomDataset( const char *filename )
{
    if( !m_dicomData )
        m_dicomData = new DcmDataset;

    DcmFileFormat dicomFile;
    OFCondition status = dicomFile.loadFile( filename );
    if( status.good() )
    {
        this->setDicomDataset( dicomFile.getAndRemoveDataset() );
        return true;
    }
    else
    {
        std::cerr << "algo falla::: " << status.text() << std::endl << "ARXIU: "<< filename << std::endl;
        return false;
    }
}

void VolumeSourceInformation::setFilenames( std::vector< std::string > filenames )
{
    m_filenamesArray = filenames;
    if( !m_filenamesArray.empty() )
        this->loadDicomDataset( m_filenamesArray[0].c_str() ); //\TODO ara es fa així, però podria ser que tinguèssim un tracte més "refinat"
    else
        WARN_LOG("La llista de fitxers és buida");
}

void VolumeSourceInformation::setFilenames( std::string filenames )
{
    m_filenamesArray.clear();
    m_filenamesArray.push_back( filenames );
    this->loadDicomDataset( m_filenamesArray[0].c_str() );
}

void VolumeSourceInformation::setDicomDataset( DcmDataset *data )
{
    m_dicomData = data;
    readWindowLevelData();
}

DcmDataset *VolumeSourceInformation::getDicomDataset()
{
    return m_dicomData;
}

unsigned VolumeSourceInformation::getPhotometricInterpretation()
{
    const char *photoString = this->getPhotometricInterpretationAsString();

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

const char *VolumeSourceInformation::getPhotometricInterpretationAsString()
{
    // \TODO es podria afegir una mica és de control a nivell de debug per si no es llegeix aquesta dada, perquè per exemple no existeix
    const char *photoString = NULL;
    if( m_dicomData )
        m_dicomData->findAndGetString( DCM_PhotometricInterpretation , photoString );
    else
        DEBUG_LOG( "No hi ha m_dicomData creat" );

    return photoString;
}

bool VolumeSourceInformation::isMonochrome1()
{
    return this->getPhotometricInterpretation() == Monochrome1;
}

unsigned VolumeSourceInformation::getBitsStored()
{
    if( m_dicomData )
    {
        Uint16 stored;
        m_dicomData->findAndGetUint16( DCM_BitsStored , stored );
        return stored;
    }
    else
    {
        DEBUG_LOG( "No hi ha m_dicomData creat" );
        return 0;
    }
}

unsigned VolumeSourceInformation::getBitsAllocated()
{
    if( m_dicomData )
    {
        Uint16 allocated;
        m_dicomData->findAndGetUint16( DCM_BitsAllocated , allocated );
        return allocated;
    }
    else
    {
        DEBUG_LOG( "No hi ha m_dicomData creat" );
        return 0;
    }
}

unsigned VolumeSourceInformation::getPixelRepresentation()
{
    if( m_dicomData )
    {
        Uint16 representation;
        m_dicomData->findAndGetUint16( DCM_PixelRepresentation , representation );
        switch( representation )
        {
        case 0:
            return UnsignedPixelRepresentation;
        break;
        case 1:
            return SignedPixelRepresentation;
        break;
        }
    }
    else
    {
        DEBUG_LOG( "No hi ha m_dicomData creat" );
        return 0;
    }
}

bool VolumeSourceInformation::hasModalityRescale()
{
    if( m_dicomData )
    {
        return m_dicomData->tagExists( DCM_RescaleIntercept );
    }
    else
        return false;

}

double VolumeSourceInformation::getRescaleSlope()
{
    if( m_dicomData )
    {
        Float64 value;
        m_dicomData->findAndGetFloat64( DCM_RescaleSlope , value );
        return value;
    }
    else
    {
        DEBUG_LOG( "No hi ha m_dicomData creat" );
        return 1; // element neutre
    }
}

double VolumeSourceInformation::getRescaleIntercept()
{
    if( m_dicomData )
    {
        Float64 value;
        m_dicomData->findAndGetFloat64( DCM_RescaleIntercept , value );
        return value;
    }
    else
    {
        DEBUG_LOG( "No hi ha m_dicomData creat" );
        return 0; // element neutre
    }
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

const char *VolumeSourceInformation::getWindowLevelDescription( int position )
{
    if( position > m_windowLevelDescriptions.size() - 1 || position < 0 )
        return NULL;
    else
        return qPrintable( m_windowLevelDescriptions.at( position ) );
}

void VolumeSourceInformation::readWindowLevelData()
{
    if( m_dicomData )
    {
        DcmDecimalString windowLevelString(DCM_WindowCenter);
        DcmDecimalString windowWidthString(DCM_WindowWidth);
        DcmStack stack;
        stack.clear();
        if( EC_Normal == m_dicomData->search( DCM_WindowCenter, stack, ESM_fromHere, OFFalse) )
        {
            windowLevelString = *((DcmDecimalString *)(stack.top()));
            m_dicomData->search( DCM_WindowWidth, stack, ESM_fromHere, OFFalse );
            windowWidthString = *((DcmDecimalString *)(stack.top()));
        }
        double *wl;
        for( int i = 0; i < windowLevelString.getVM(); i++ )
        {
            wl = new double[2];
            windowWidthString.getFloat64( wl[0], i );
            windowLevelString.getFloat64( wl[1], i );
            m_windowLevelList.push_back( wl );
        }

        // i les descripcions
        DcmLongString wlDescriptionString( DCM_WindowCenterWidthExplanation );
        stack.clear();
        if( EC_Normal == m_dicomData->search( DCM_WindowCenterWidthExplanation, stack, ESM_fromHere, OFFalse) )
        {
            wlDescriptionString = *((DcmLongString *)(stack.top()));
            OFString value;
            for( int i = 0; i < wlDescriptionString.getVM(); i++ )
            {
                wlDescriptionString.getOFString( value , i );
                m_windowLevelDescriptions << value.c_str();
            }
        }
    }
    else
    {
        DEBUG_LOG("No s'ha pogut llegir la informació de window level. No hi ha un dicom dataset vàlid.");
    }
}

};  // end namespace udg
