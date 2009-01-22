#include "informationtheory.h"

#include <cmath>

#include "histogram.h"


namespace udg {


// H(X) = -sum[0,n)( p(x) log p(x) )
double InformationTheory::entropy( const QVector<double> &probabilities )
{
    int size = probabilities.size();
    double entropy = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        double p = probabilities.at( i );
        if ( p > 0.0 ) entropy -= p * log( p );
    }

    entropy /= log( 2.0 );

    return entropy;
}


// H(X) = -sum[0,n)( p(x) log p(x) )
double InformationTheory::entropy( const Histogram &histogram )
{
    int size = histogram.size();
    double count = histogram.count();
    double entropy = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        double p = histogram[i] / count;
        if ( p > 0.0 ) entropy -= p * log( p );
    }

    entropy /= log( 2.0 );

    return entropy;
}


// D_KL(P||Q) = sum[0,n)( P(i) log ( P(i) / Q(i) ) )
double InformationTheory::kullbackLeiblerDivergence( const QVector<double> &probabilitiesP, const QVector<double> &probabilitiesQ )
{
    int size = probabilitiesP.size();
    double kullbackLeiblerDivergence = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        double p = probabilitiesP.at( i );

        if ( p > 0.0 )
        {
            double q = probabilitiesQ.at( i );
            kullbackLeiblerDivergence += p * log( p / q );
        }
    }

    kullbackLeiblerDivergence /= log( 2.0 );

    return kullbackLeiblerDivergence;
}


InformationTheory::InformationTheory()
{
}


InformationTheory::~InformationTheory()
{
}


}
