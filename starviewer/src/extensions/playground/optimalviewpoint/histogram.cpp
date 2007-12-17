/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "histogram.h"


namespace udg {


Histogram::Histogram()
{
    m_count = 0;
}


Histogram::Histogram( int size )
{
    m_histogram.resize( size );
    m_count = 0;
}


Histogram::~Histogram()
{
}


int Histogram::size() const
{
    return m_histogram.size();
}


void Histogram::setSize( int size )
{
    m_histogram.resize( size );
}


void Histogram::add( int value )
{
    m_histogram[value]++;
    m_count++;
}


void Histogram::substract( int value )
{
    m_histogram[value]--;
    m_count--;
}


QVectorIterator<quint64 > * Histogram::getIterator() const
{
    return new QVectorIterator<quint64>( m_histogram );
}


quint64 Histogram::count() const
{
    return m_count;
}


void Histogram::reset()
{
    m_histogram.fill( 0 );
    m_count = 0;
}


void Histogram::combineWith( const Histogram & histogram )
{
    if ( this->size() < histogram.size() )
        this->setSize( histogram.size() );

    for ( int i = 0; i < histogram.size(); i++ )
        m_histogram[i] += histogram.m_histogram[i];

    m_count += histogram.m_count;
}


void Histogram::substract( const Histogram & histogram )
{
    int size = qMin( this->size(), histogram.size() );

    for ( int i = 0; i < size; i++ )
        m_histogram[i] -= histogram.m_histogram[i];

    m_count -= histogram.m_count;
}


}
