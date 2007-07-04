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
    m_numberOfPhases = 1;
    m_numberOfSlices = 1;
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

bool VolumeSourceInformation::loadDicomDataset( QString filename )
{
    if( !m_dicomData )
        m_dicomData = new DcmDataset;

    DcmFileFormat dicomFile;
    OFCondition status = dicomFile.loadFile( qPrintable(filename) );
    if( status.good() )
    {
        this->setDicomDataset( dicomFile.getAndRemoveDataset() );
        // omplim la informació necessària
        this->collectSerieInformation();
        return true;
    }
    else
    {
        DEBUG_LOG( QString( "algo falla::: %1\nARXIU: %2 ").arg( status.text() ).arg( filename ) );
        return false;
    }
}

void VolumeSourceInformation::collectSerieInformation()
{
    if( m_dicomData )
    {
        // TODO ara només agafem aquesta informació, a la llarga hem de recolectar tota la informació que es recolecta a la classe Input quan fem un read.
        const char *value = NULL;
        if( m_dicomData->findAndGetString( DCM_SeriesDescription , value ).good() )
        {
            this->setSeriesDescription( value );
        }
        if( m_dicomData->findAndGetString( DCM_SeriesInstanceUID , value ).good() )
        {
            this->setSeriesInstanceUID( value );
        }
    }
    else
    {
        DEBUG_LOG("No tenim un dicom dataset vàlid!");
    }
}

void VolumeSourceInformation::setFilenames( QStringList filenames )
{
    m_filenamesList = filenames;
    if( !m_filenamesList.isEmpty() )
        this->loadDicomDataset( m_filenamesList.at(0) ); //\TODO ara es fa així, però podria ser que tinguèssim un tracte més "refinat"
    else
        WARN_LOG("La llista de fitxers és buida");
}

void VolumeSourceInformation::setFilenames( QString filename )
{
    this->setFilenames( QStringList( filename ) );
}

void VolumeSourceInformation::setDicomDataset( DcmDataset *data )
{
    m_dicomData = data;
    readWindowLevelData();
}

DcmDataset *VolumeSourceInformation::getDicomDataset( int index )
{
    if( index == 0 )
        return m_dicomData;
    else if( index > 0 && index < m_filenamesList.size() )
    {
        DcmFileFormat dicomFile;
        OFCondition status = dicomFile.loadFile( qPrintable( m_filenamesList.at(index) ) );
        if( status.good() )
        {
            DcmDataset *dataset = NULL;
            dataset = dicomFile.getAndRemoveDataset();
            return dataset;
        }
        else
        {
            ERROR_LOG( QString("No s'ha pogut carregar arxiu dicom [%1]. Missatge d'error:[%2] ").arg( status.text() ).arg(m_filenamesList.at(index)) );
            return false;
        }
    }
    else
    {
        ERROR_LOG("S'ha demanat una imatge fora de rang");
        return 0;
    }
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
    const char *photoString = NULL;
    if( m_dicomData )
        m_dicomData->findAndGetString( DCM_PhotometricInterpretation , photoString );
    else
        DEBUG_LOG( "No hi ha m_dicomData creat" );

    return QString( photoString );
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
        for( unsigned int i = 0; i < windowLevelString.getVM(); i++ )
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
            for( unsigned int i = 0; i < wlDescriptionString.getVM(); i++ )
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

bool VolumeSourceInformation::isMultiFrame()
{
    bool result = false;
    if( m_dicomData )
    {
        if( m_dicomData->tagExists( DCM_NumberOfFrames ) )
            result = true;
    }
    else
    {
        DEBUG_LOG("No hi ha un dicom dataset vàlid");
    }
    return result;
}

int VolumeSourceInformation::getNumberOfFrames()
{
    Sint32 frames = 0;
    if( m_dicomData )
    {
        OFCondition status = m_dicomData->findAndGetSint32( DCM_NumberOfFrames, frames );
        if( status.bad() )
            DEBUG_LOG( QString("No s'han pogut llegir els frames: ") + status.text() );
    }
    else
    {
        DEBUG_LOG("No hi ha un dicom dataset vàlid");
    }
    return frames;
}

QString VolumeSourceInformation::getImageSOPInstanceUID( int index )
{
    QString result;
    DcmDataset *dataset = this->getDicomDataset( index );
    if( dataset )
    {
        const char *value;
        dataset->findAndGetString( DCM_SOPInstanceUID, value );
        result = QString( value );
        DEBUG_LOG( QString("El valor del sop instance és %1 :: %2").arg(value).arg(result) );
    }
    else
    {
        ERROR_LOG("El dataset retornat és nul");
    }
    return result;
}

void VolumeSourceInformation::setPatientPosition( QString patientPosition )
{
    m_patientPosition = patientPosition;
    DEBUG_LOG( "Patient position:; " + m_patientPosition );
    if( m_patientPosition == "HFP" ) // Head First-Prone: Entra de cap i mira cap avall
        setPatientOrientationString("LRPASI");
    else if( m_patientPosition == "HFDR" ) // Head First-Decubitus Right: Entra de cap i recolzat sobre el braç dret
        setPatientOrientationString("APLRSI");
    else if( m_patientPosition == "FFDR" ) // Feet First-Decubitus Right: Entra de peus i recolzat sobre el braç dret
        setPatientOrientationString("PALRIS");
    else if( m_patientPosition == "FFP" ) // Feet First-Prone: Entra de peus i mira cap avall
        setPatientOrientationString("RLPAIS");
    else if( m_patientPosition == "HFS" ) // Head First-Supine: Entra de cap i mira cap amunt
        setPatientOrientationString("RLAPSI");
    else if( m_patientPosition == "HFDL" ) // Head First-Decubitus Left: Entra de cap i recolzat sobre el braç esquerre
        setPatientOrientationString("PARLSI");
    else if( m_patientPosition == "FFDL" ) // Feet First-Decubitus Left: Entra de peus i recolzat sobre el braç esquerre
        setPatientOrientationString("APRLIS");
    else if( m_patientPosition == "FFS" ) // Feet First-Supine: Entra de peus i mira cap amunt
        setPatientOrientationString("LRAPIS");
    else // No tenim l'atribut esperat
        setPatientOrientationString("??????");
}

};  // end namespace udg
