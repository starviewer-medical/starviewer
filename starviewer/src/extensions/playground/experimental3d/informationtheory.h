#ifndef UDGINFORMATIONTHEORY_H
#define UDGINFORMATIONTHEORY_H


#include <QVector>


namespace udg {


class Histogram;


/**
 * Aquesta serà una classe estàtica que agruparà mètodes de càlcul de mesures de teoria de la informació.
 */
class InformationTheory {

public:

    /**
     * Retorna l'entropia de Shannon d'una variable aleatòria amb les probabilitats \p probabilities.
     * H(X) = -sum[0,n)( p(x) log p(x) )
     */
    static double entropy( const QVector<double> &probabilities );
    /**
     * Retorna l'entropia de Shannon d'una variable aleatòria amb una distribució de probabilitat representada per \p histogram.
     * H(X) = -sum[0,n)( p(x) log p(x) )
     */
    static double entropy( const Histogram &histogram );
    /**
     * Retorna I(x,Y), on x és un valor de la variable aleatòria X, i Y és una altra variable aleatòria.
     * \p probabilities_yIx és el vector de probabilitats p(y|x).
     * \p probabilities_y és el vector de probabilitats p(y).
     * I(x,Y) = sum[0,ny)( p(y|x) log ( p(y|x) / p(y) ) )
     */
    static double xMutualInformation( const QVector<double> &probabilities_yIx, const QVector<double> &probabilities_y );

private:

    InformationTheory();
    ~InformationTheory();

};


}


#endif
