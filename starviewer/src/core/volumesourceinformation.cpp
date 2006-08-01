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

};  // end namespace udg 
