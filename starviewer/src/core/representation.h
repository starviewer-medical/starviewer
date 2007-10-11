/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDREPRESENTATION_H
#define UDGDREPRESENTATION_H

#include <QObject>

//Forward declarations
class QString;

namespace udg {
//Forward declarations
    
/**
Classe base per a totes les representacions

@author Grup de Gràfics de Girona  ( GGG )
*/

class Representation : public QObject{
    Q_OBJECT
public:
    
    Representation( QObject *parent = 0 );
    ~Representation(){}
    
    ///Fem visible la primitiva
    void visibilityOn()
    { visibility( true ); }
    
    ///Fem invisible la primitiva
    void visibilityOff()
    { visibility( true ); }
    
    ///assignem la visibilitat amb un paràmetre
    void visibility( bool visibility )
    { m_visible = visibility; } 
    
    ///ens diu si la primitiva és visible o no
    bool isVisible()
    { return( m_visible ); }
    
    ///ens diu el tipus de representació que és. (reimplementat en les sub-classes)
    virtual QString getRepresentationType() = 0;
protected:
   
    ///visibilitat de la primitiva
    bool m_visible;
};

};  
#endif

