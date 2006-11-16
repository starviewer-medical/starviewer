/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
 
#ifndef UDGIDENTIFIER_H
#define UDGIDENTIFIER_H

#include <iostream>

namespace udg {

/**
    Encapsula un identificador numèric.
    
@author Grup de Gràfics de Girona  ( GGG )
*/

class Identifier
{
public:
    /// Constructor
    Identifier();
    Identifier( int id );
    
    /// Constructor de còpia
    Identifier( const Identifier &id );

    ~Identifier();

    /// Li assigna un nou valor a l'identificador
    void setValue( int newId );
    /// Ens indica si l'dentificador té un valor vàlid o no
    bool isNull();
    
    int getValue() const { return m_id; }
    
    bool operator != ( const Identifier &identifier );
    bool operator==( const Identifier &id );
    
    bool operator <=( const Identifier &id );
    bool operator >( const Identifier &id );
    bool operator >=( const Identifier &id );
    bool operator <( const Identifier &id ) const;
    Identifier & operator =( const Identifier &id );
    Identifier& operator=( const int id );
    Identifier operator+( const Identifier & id );
    Identifier operator+( const int id );
    Identifier operator+=( const Identifier & id );
    Identifier operator+=( const int id );
    Identifier operator-( const Identifier & id );
    Identifier operator-( const int id );
    Identifier operator-=( const Identifier & id );
    Identifier operator-=( const int id );
    
    friend std::ostream & operator << ( std::ostream &out, const Identifier &id );
private:
    int m_id;
};

};  

#endif
