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

VolumeSourceInformation::VolumeSourceInformation()
{
    m_windowLevel[0] = 0.0;
    m_windowLevel[1] = 0.0;
    m_dicomTagReader = new DICOMTagReader();
}

VolumeSourceInformation::~VolumeSourceInformation()
{
}

bool VolumeSourceInformation::loadDicomData( QString filename )
{
    if( m_dicomTagReader->setFile( filename ) )
    {
        QString value;
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

// void VolumeSourceInformation::setPatientPosition( QString patientPosition )
// {
//     m_patientPosition = patientPosition;
//     DEBUG_LOG( "Patient position:: " + m_patientPosition );
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
// }


};  // end namespace udg
