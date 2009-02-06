#ifndef UDGINFORMATIONTHEORY_CPP
#define UDGINFORMATIONTHEORY_CPP


#include "informationtheory.h"

#include <cmath>

#include "histogram.h"


namespace udg {


// H(X) = -sum[0,n)( p(x) log p(x) )
template <class T>
T InformationTheory<T>::entropy( const QVector<T> &probabilities )
{
    int size = probabilities.size();
    T entropy = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        T p = probabilities.at( i );
        if ( p > 0.0 ) entropy -= p * log( p );
    }

    entropy /= log( 2.0 );

    return entropy;
}


// H(X) = -sum[0,n)( p(x) log p(x) )
template <class T>
T InformationTheory<T>::entropy( const Histogram &histogram )
{
    int size = histogram.size();
    T count = histogram.count();
    T entropy = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        T p = histogram[i] / count;
        if ( p > 0.0 ) entropy -= p * log( p );
    }

    entropy /= log( 2.0 );

    return entropy;
}


// D_KL(P||Q) = sum[0,n)( P(i) log ( P(i) / Q(i) ) )
template <class T>
T InformationTheory<T>::kullbackLeiblerDivergence( const QVector<T> &probabilitiesP, const QVector<T> &probabilitiesQ )
{
    int size = probabilitiesP.size();
    T kullbackLeiblerDivergence = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        T p = probabilitiesP.at( i );

        if ( p > 0.0 )
        {
            T q = probabilitiesQ.at( i );
            kullbackLeiblerDivergence += p * log( p / q );
        }
    }

    kullbackLeiblerDivergence /= log( 2.0 );

    return kullbackLeiblerDivergence;
}


// JSD(pi1,pi2; P1,P2) = H(pi1*P1 + pi2*P2) - ( pi1 * H(P1) + pi2 * H(P2) )
template <class T>
T InformationTheory<T>::jensenShannonDivergence( T pi1, T pi2, const QVector<T> &probabilitiesP1, const QVector<T> &probabilitiesP2 )
{
    int size = probabilitiesP1.size();
    QVector<T> probabilitiesMix( size );

    for ( int i = 0; i < size; i++ )
        probabilitiesMix[i] = pi1 * probabilitiesP1.at( i ) + pi2 * probabilitiesP2.at( i );

    return entropy( probabilitiesMix ) - ( pi1 * entropy( probabilitiesP1 ) + pi2 * entropy( probabilitiesP2 ) );
}


template <class T>
InformationTheory<T>::InformationTheory()
{
}


template <class T>
InformationTheory<T>::~InformationTheory()
{
}


}


#endif
