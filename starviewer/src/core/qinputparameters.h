/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGQINPUTPARAMETERS_H
#define UDGQINPUTPARAMETERS_H

#include <QWidget>

namespace udg {

/**
    Classe base per als widgets destinats a introduïr paràmetres.
    Els paràmetres es poden introduïr ja de forma simple i clàssica com pot ser amb 
    caixes de text i similar o de formes més complexes com amb histogrames o d'altres.
    
    Aquesta ofereix la interfície comuna i bàsica de signals o slots perquè les dades
    es mantinguin en sincronisme amb la resta de la interfície.

@author Grup de Gràfics de Girona  ( GGG )
*/

class QInputParameters : public QWidget{
Q_OBJECT
public:
    QInputParameters( QWidget *parent = 0 );

    virtual ~QInputParameters();

    /**
        Activa/desactiva l'emissió de la senyal "parameterChanged(int)"
    */
    void disableIndividualSincronization();
    void enableIndividualSincronization();
    bool isIndividualSincronizationEnabled() const { return m_individualSincronization; };
    
public slots:
    /**
        Slot que ens serveix per indicar que hem d'actualitzar el parametre que ens diguin mitjançant 
        un identificador (que, en realitat, serà un enum). Serveix per canviar els valors a partir 
        d'una classe Parameters
    */
    virtual void readParameter(int index) /*= 0*/{}; 
    /**
        Escriu tots els valors de paràmetres que té actualment al Parameters associat
    */
    virtual void writeAllParameters() /*= 0*/{};

private:
    /// indicarà si els paràmetres s'actualitzen 'en viu' o d'una tacada
    bool m_individualSincronization;
};

};  

#endif
