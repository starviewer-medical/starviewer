/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

 
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <qobject.h>

/**
    Classe base Parameters. Aquesta classe s'encarrega d'encapsular els paràmetres d'un mètode
    en concret. Aquesta proporciona la interfície comuna per a totes les que heredin d'aquesta.
    
    Proporciona un signal changed(int) el qual indicarà quin paràmetre s'ha actualitzat. El paràmetre 
    d'aquest signal serà un índex que identifica quin paràmetre ha canviat. Les sub-classes tindran
    un tipus enumerat que farà d'índex per cada paràmetre
*/

namespace udg{

class Parameters : public QObject{
Q_OBJECT
public:
    Parameters(QObject *parent = 0, const char *name = 0);

    virtual ~Parameters();

signals:
    /// indica a les interfícies que un dels seus paràmetres s'ha actualitzat
    /// El paràmetre és un enter que identifica el paràmetre
    void changed( int ); 

};

}; // end namespace udg
#endif
// fi de #endif
