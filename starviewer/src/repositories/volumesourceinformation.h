/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGVOLUMESOURCEINFORMATION_H
#define UDGVOLUMESOURCEINFORMATION_H

#include <qstring.h>

namespace udg {

/**
Conté informació relacionada amb el volum, com per exemple dades del pacient.

@author Grup de Gràfics de Girona  ( GGG )
*/
class VolumeSourceInformation 
{
public:
    VolumeSourceInformation();

    ~VolumeSourceInformation();

    /// Assignar/Obtenir el nom del pacient
    void setPatientName( const char *name );
    const char *getPatienName(){ return m_patientName.latin1(); };
    
protected:
    QString m_patientName;
};

};  //  end  namespace udg 

#endif
