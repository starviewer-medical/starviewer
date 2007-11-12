/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISTANCETOOLDATA_H
#define UDGDISTANCETOOLDATA_H

#include "oldtooldata.h"

//Forward declarations
class QString;

namespace udg {
/**
Classe que implementa una especialització de la classe ToolData, per a contenir dades necessàries per a la Tool de creació de distàncies.

@author Grup de Gràfics de Girona  ( GGG )
*/

class DistanceToolData : public OldToolData {
Q_OBJECT
public:
    ///constructor amb paràmetres
    DistanceToolData( double p1[3], double p2[3] );

    ~DistanceToolData();

    ///assignem el primer punt
    void setFirstPoint( double p1[3] );

    ///assignem el segon punt
    void setSecondPoint( double p2[3] );

    ///retornem el primer punt
    double* getFirstPoint()
    { return( m_firstPoint ); }

    ///retornem el segon punt
    double* getSecondPoint()
    { return( m_secondPoint ); }

    ///retornem el segon punt
    double* getTextPosition()
    { return( m_textPosition ); }

    ///retornem el text
    QString getDistanceText()
    { return( m_text ); }

    ///Mètode per calcular el la distància entre els dos punts, per tant el text que contindrà la distància i la posició
    void calculateDistance();

signals:
    ///s'emet quan canvia el primer punt de la distància
    void firstPointChanged();

    ///s'emet quan canvia el segon punt de la distància
    void secondPointChanged();

    ///s'emet quan canvia el text de la distància
    void distanceTextChanged();

private:

    ///atributs corresponents als punts de la distància
    double m_firstPoint[3];
    double m_secondPoint[3];

    ///atribut que conté el text de la distància
    QString m_text;

    ///atribut corresponent a la posició del text de la distància
    double m_textPosition[3];
};

};  //  end  namespace udg

#endif
