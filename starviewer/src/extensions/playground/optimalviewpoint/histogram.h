/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGHISTOGRAM_H
#define UDGHISTOGRAM_H


#include <QVector>


namespace udg {


/**
 * Classe que encapsula un histograma d'enters.
 *
 * Guarda el nombre d'ocurrències de cada enter i també el nombre total
 * d'ocurrències.
 *
 * \todo Potser quedaria bé amb templates.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Histogram {

public:

    /// Constructor. Crea un histograma buit i de mida 0.
    Histogram();
    /// Constructor. Crea un histograma buit i de mida \p size.
    Histogram( int size );
    ~Histogram();

    /// Retorna la mida de l'histograma.
    int size() const;
    /**
     * Posa la mida de l'histograma a \p size. Si la nova mida és més gran que
     * l'actual s'afegeixen noves entrades a 0 i si és més petita s'esborren les
     * entrades sobrants. No varia el comptador total.
     */
    void setSize( int size );
    /**
     * Incrementa el comptador d'ocurrències de \p value i el total. \p value ha
     * de ser un valor en l'interval [0, size()) (no es comprova).
     */
    void add( int value );
    /**
     * Decrementa el comptador d'ocurrències de \p value i el total. \p value ha
     * de ser un valor en l'interval [0, size()) i el seu comptador
     * d'ocurrències ha de ser > 0 (no es comprova).
     */
    void substract( int value );
    /// Crea un iterador sobre els valors de l'histograma i el retorna.
    QVectorIterator<quint64> * getIterator() const;
    /// Retorna el recompte total d'ocurrències.
    quint64 count() const;
    /// Posa tots els comptadors (individuals i total) a 0.
    void reset();

    /**
     * Combina aquest histograma amb \p histogram. S'agafa la mida del més gros
     * i es sumen tots els comptadors dels dos histogrames.
     */
    void combineWith( const Histogram & histogram );
    /// Resta \p histogram d'aquest histograma. Es resten tots els comptadors.
    void substract( const Histogram & histogram );

private:

    /// Vector amb els comptadors d'ocurrències individuals.
    QVector<quint64> m_histogram;
    /// Comptador del nombre total d'ocurrències.
    quint64 m_count;

};


inline void Histogram::add( int value )
{
    m_histogram[value]++;
    m_count++;
}


inline void Histogram::substract( int value )
{
    m_histogram[value]--;
    m_count--;
}


}


#endif
