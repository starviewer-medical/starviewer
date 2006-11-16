/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumesourceinformation.h"

namespace udg {

VolumeSourceInformation::VolumeSourceInformation()
{
    m_windowLevel[0] = 0.0;
    m_windowLevel[1] = 0.0;
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

};  // end namespace udg 
