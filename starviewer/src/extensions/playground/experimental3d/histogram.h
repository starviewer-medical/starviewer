#ifndef UDGHISTOGRAM_H
#define UDGHISTOGRAM_H


#include <QVector>


namespace udg {


/**
 * Histograma d'enters.
 *
 * Guarda el nombre d'ocurrències de cada enter i també el nombre total d'ocurrències.
 */
class Histogram {

public:

    /// Crea un histograma buit i de mida 0.
    Histogram();
    /// Crea un histograma buit i de mida \p size.
    Histogram( int size );
    ~Histogram();

    /// Retorna la mida de l'histograma.
    int size() const;
    /**
     * Posa la mida de l'histograma a \p size. Si la nova mida és més gran que l'actual s'afegeixen noves entrades a 0 i si és més petita s'esborren les entrades sobrants.
     * No varia el comptador total.
     */
    void setSize( int size );
    /// Incrementa el comptador d'ocurrències de \p value i el total. \p value ha de ser un valor en l'interval [0, size()) (no es comprova).
    void add( int value );
    /**
     * Decrementa el comptador d'ocurrències de \p value i el total. \p value ha de ser un valor en l'interval [0, size()) i el seu comptador d'ocurrències ha de ser > 0
     * (no es comprova).
     */
    void substract( int value );
    /// Retorna el recompte total d'ocurrències.
    unsigned int count() const;
    /// Posa tots els comptadors (individuals i total) a 0. No canvia la mida de l'histograma.
    void reset();

    /// Retorna el nombre d'ocurrències de \p value. \p value ha de ser un valor en l'interval [0, size()) (no es comprova).
    unsigned int operator []( int value ) const;

private:

    /// Vector amb els comptadors d'ocurrències individuals.
    QVector<unsigned int> m_histogram;
    /// Comptador del nombre total d'ocurrències.
    unsigned int m_count;

};


}


#endif
