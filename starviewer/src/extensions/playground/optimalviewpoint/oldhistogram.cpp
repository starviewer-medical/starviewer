/***************************************************************************
 *   Copyright (C) 2007-2008 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "oldhistogram.h"


namespace udg {


OldHistogram::OldHistogram()
{
    m_count = 0;
}


OldHistogram::OldHistogram( int size )
{
    m_histogram.resize( size );
    m_count = 0;
}


OldHistogram::~OldHistogram()
{
}


int OldHistogram::size() const
{
    return m_histogram.size();
}


void OldHistogram::setSize( int size )
{
    m_histogram.resize( size );
}


QVectorIterator<quint64 > * OldHistogram::getIterator() const
{
    return new QVectorIterator<quint64>( m_histogram );
}


quint64 OldHistogram::count() const
{
    return m_count;
}


void OldHistogram::reset()
{
    m_histogram.fill( 0 );
    m_count = 0;
}


void OldHistogram::combineWith( const OldHistogram & histogram )
{
    int otherSize = histogram.size();

    if ( this->size() < otherSize )
        this->setSize( otherSize );

    for ( int i = 0; i < otherSize; i++ )
        m_histogram[i] += histogram.m_histogram.at(i);

    m_count += histogram.m_count;
}


void OldHistogram::substract( const OldHistogram & histogram )
{
    int size = qMin( this->size(), histogram.size() );

    for ( int i = 0; i < size; i++ )
        m_histogram[i] -= histogram.m_histogram.at(i);

    m_count -= histogram.m_count;
}


}
