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
     * Retorna la divergència de Kullback-Leibler entre les distribucions de probabilitat P i Q, amb les probabilitats definides a \p probabilitiesP i \p probabilitiesQ.
     * D_KL(P||Q) = sum[0,n)( P(i) log ( P(i) / Q(i) ) )
     */
    static double kullbackLeiblerDivergence( const QVector<double> &probabilitiesP, const QVector<double> &probabilitiesQ );

private:

    InformationTheory();
    ~InformationTheory();

};


}


#endif
