/***************************************************************************
 *   Copyright (C) 2007-2008 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGDOUBLEHISTOGRAM_H
#define UDGDOUBLEHISTOGRAM_H


#include <QVector>


namespace udg {


/**
 * Classe que encapsula un histograma fraccionari d'enters.
 *
 * Guarda el nombre d'ocurrències de cada enter i també el nombre total
 * d'ocurrències. El nombre d'ocurrències és real.
 *
 * \todo Potser quedaria bé amb templates.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class DoubleHistogram {

public:

    /// Constructor. Crea un histograma buit i de mida 0.
    DoubleHistogram();
    /// Constructor. Crea un histograma buit i de mida \p size.
    DoubleHistogram( int size );
    ~DoubleHistogram();

    /// Retorna la mida de l'histograma.
    int size() const;
    /**
     * Posa la mida de l'histograma a \p size. Si la nova mida és més gran que
     * l'actual s'afegeixen noves entrades a 0 i si és més petita s'esborren les
     * entrades sobrants. No varia el comptador total.
     */
    void setSize( int size );
    /**
     * Incrementa en \p amount unitats el comptador d'ocurrències de \p value i
     * el total. \p value ha de ser un valor en l'interval [0, size()) (no es
     * comprova).
     */
    void add( int value, double amount = 1.0 );
    /// Crea un iterador sobre els valors de l'histograma i el retorna.
    QVectorIterator< double > * getIterator() const;
    /// Retorna el recompte total d'ocurrències.
    double count() const;
    /// Posa tots els comptadors (individuals i total) a 0.
    void reset();

    /**
     * Combina aquest histograma amb \p histogram. S'agafa la mida del més gros
     * i es sumen tots els comptadors dels dos histogrames.
     */
    void combineWith( const DoubleHistogram & histogram );

private:

    /// Vector amb els comptadors d'ocurrències individuals.
    QVector< double > m_histogram;
    /// Comptador del nombre total d'ocurrències.
    double m_count;

};


inline void DoubleHistogram::add( int value, double amount )
{
    m_histogram[value] += amount;
    m_count += amount;
}


}


#endif
