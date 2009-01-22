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
    /**
     * Retorna la divergència de Jensen-Shannon entre les distribucions de probabilitat P1 i P2, amb les probabilitats definides a \p probabilitiesP1 i \p probabilitiesP2 i
     * els pesos \p pi1 i \p pi2.
     * JSD(pi1,pi2; P1,P2) = H(pi1*P1 + pi2*P2) - ( pi1 * H(P1) + pi2 * H(P2) )
     */
    static double jensenShannonDivergence( double pi1, double pi2, const QVector<double> &probabilitiesP1, const QVector<double> &probabilitiesP2 );

private:

    InformationTheory();
    ~InformationTheory();

};


}


#endif
