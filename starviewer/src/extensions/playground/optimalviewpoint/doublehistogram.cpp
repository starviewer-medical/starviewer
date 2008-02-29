/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "doublehistogram.h"


namespace udg {


DoubleHistogram::DoubleHistogram()
{
    m_count = 0.0;
}


DoubleHistogram::DoubleHistogram( int size )
{
    m_histogram.resize( size );
    m_count = 0.0;
}


DoubleHistogram::~DoubleHistogram()
{
}


int DoubleHistogram::size() const
{
    return m_histogram.size();
}


void DoubleHistogram::setSize( int size )
{
    m_histogram.resize( size );
}


void DoubleHistogram::add( int value, double amount )
{
    m_histogram[value] += amount;
    m_count += amount;
}


QVectorIterator< double > * DoubleHistogram::getIterator() const
{
    return new QVectorIterator< double >( m_histogram );
}


double DoubleHistogram::count() const
{
    return m_count;
}


void DoubleHistogram::reset()
{
    m_histogram.fill( 0.0 );
    m_count = 0.0;
}


void DoubleHistogram::combineWith( const DoubleHistogram & histogram )
{
    if ( this->size() < histogram.size() )
        this->setSize( histogram.size() );

    for ( int i = 0; i < histogram.size(); i++ )
        m_histogram[i] += histogram.m_histogram[i];

    m_count += histogram.m_count;
}


}
