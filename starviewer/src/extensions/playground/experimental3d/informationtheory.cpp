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


// I(x,Y) = sum[0,ny)( p(y|x) log ( p(y|x) / p(y) ) )
double InformationTheory::xMutualInformation( const QVector<double> &probabilities_yIx, const QVector<double> &probabilities_y )
{
    int size = probabilities_y.size();
    double xMutualInformation = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        double p_yIx = probabilities_yIx.at( i );

        if ( p_yIx > 0.0 )
        {
            double p_y = probabilities_y.at( i );
            xMutualInformation += p_yIx * log( p_yIx / p_y );
        }
    }

    xMutualInformation /= log( 2.0 );

    return xMutualInformation;
}


InformationTheory::InformationTheory()
{
}


InformationTheory::~InformationTheory()
{
}


}
