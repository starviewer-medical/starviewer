#include "histogram.h"


namespace udg {


Histogram::Histogram()
 : m_count( 0 )
{
}


Histogram::Histogram( int size )
 : m_count( 0 )
{
    m_histogram.resize( size );
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


unsigned int Histogram::count() const
{
    return m_count;
}


void Histogram::reset()
{
    m_histogram.fill( 0 );
    m_count = 0;
}


unsigned int Histogram::operator []( int value ) const
{
    return m_histogram.at( value );
}


}
