/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "transferfunction.h"
#include "logging.h"

// vtk
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

namespace udg {

TransferFunction::TransferFunction()
{
    m_changed = m_colorChanged = m_opacityChanged = true;
    m_colorTransferFunction = 0;
    m_opacityTransferFunction = 0;
}

TransferFunction::TransferFunction( const TransferFunction & transferFunction )
{
    m_color = transferFunction.m_color;
    m_opacity = transferFunction.m_opacity;

    m_changed = transferFunction.m_changed;
    if ( !m_changed )
        m_rgba = transferFunction.m_rgba;

    m_colorTransferFunction = 0;
    m_opacityTransferFunction = 0;
    m_colorChanged = m_opacityChanged = true;
}

TransferFunction::~TransferFunction()
{
    m_rgba.clear(); /// \todo en teoria no cal, però sense això peta (???!!!)
    if ( m_colorTransferFunction ) m_colorTransferFunction->Delete();
    if ( m_opacityTransferFunction ) m_opacityTransferFunction->Delete();
}

const QString & TransferFunction::name() const
{
    return m_name;
}

void TransferFunction::setName( const QString & name )
{
    m_name = name;
}

void TransferFunction::addPoint( double x, const QColor & rgba )
{
    m_color[x] = rgba;
    m_opacity[x] = rgba.alphaF();
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::addPoint( double x, const QColor & color, double opacity )
{
    m_color[x] = color;
    m_opacity[x] = opacity;
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::removePoint( double x )
{
    m_color.remove( x );
    m_opacity.remove( x );
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::addPointToColor( double x, const QColor & color )
{
    m_color[x] = color;
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToColorRGB( double x, int r, int g, int b )
{
    m_color[x] = QColor( r, g, b );
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToColorRGB( double x, double r, double g, double b )
{
    QColor color;
    color.setRgbF( r, g, b );
    m_color[x] = color;
    m_changed = m_colorChanged = true;
}

void TransferFunction::removePointFromColor( double x )
{
    m_color.remove( x );
    m_changed = m_colorChanged = true;
}

void TransferFunction::addPointToOpacity( double x, double opacity )
{
    m_opacity[x] = opacity;
    m_changed = m_opacityChanged = true;
}

void TransferFunction::removePointFromOpacity( double x )
{
    m_opacity.remove( x );
    m_changed = m_opacityChanged = true;
}

void TransferFunction::clear()
{
    m_color.clear();
    m_opacity.clear();
    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::clearColor()
{
    m_color.clear();
    m_changed = m_colorChanged = true;
}

void TransferFunction::clearOpacity()
{
    m_opacity.clear();
    m_changed = m_opacityChanged = true;
}

QList< double > TransferFunction::getPoints() const
{
    if ( m_changed )
    {
        m_rgba.clear();

        QList< double > ckeys = m_color.keys();
        foreach ( double x, ckeys )
            m_rgba[x] = get( x );

        QList< double > okeys = m_opacity.keys();
        foreach ( double x, okeys )
            if ( !m_rgba.contains( x ) ) m_rgba[x] = get( x );

        m_changed = false;
    }

    return m_rgba.keys();
}

QList< double > TransferFunction::getColorPoints() const
{
    return m_color.keys();
}

QMap< double, QColor > TransferFunction::getColorMap() const
{
    return m_color;
}

QList< double > TransferFunction::getOpacityPoints() const
{
    return m_opacity.keys();
}

QMap< double, double > TransferFunction::getOpacityMap() const
{
    return m_opacity;
}

vtkColorTransferFunction * TransferFunction::getColorTransferFunction() const
{
    if ( m_colorChanged )
    {
        if ( !m_colorTransferFunction )
            m_colorTransferFunction = vtkColorTransferFunction::New();

        m_colorTransferFunction->RemoveAllPoints();

        QMapIterator< double, QColor > it( m_color );

        while ( it.hasNext() )
        {
            it.next();
            m_colorTransferFunction->AddRGBPoint( it.key(), it.value().redF(), it.value().greenF(), it.value().blueF() );
        }

        m_colorChanged = false;
    }

    return m_colorTransferFunction;
}

vtkPiecewiseFunction * TransferFunction::getOpacityTransferFunction() const
{
    if ( m_opacityChanged )
    {
        if ( !m_opacityTransferFunction )
            m_opacityTransferFunction = vtkPiecewiseFunction::New();

        m_opacityTransferFunction->RemoveAllPoints();

        QMapIterator< double, double > it( m_opacity );

        while ( it.hasNext() )
        {
            it.next();
            m_opacityTransferFunction->AddPoint( it.key(), it.value() );
        }

        m_opacityChanged = false;
    }

    return m_opacityTransferFunction;
}

bool TransferFunction::setNewRange( double min, double max )
{
    // Primer afegim els extrems. Si ja existeixen es quedarà igual.
    this->addPoint( min, this->get( min ) );
    this->addPoint( max, this->get( max ) );

    // Després eliminem els punts fora del rang.

    QMutableMapIterator<double, QColor> itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        if ( itc.key() < min ) this->removePointFromColor( itc.key() );
        else break;
    }
    itc.toBack();
    while ( itc.hasPrevious() )
    {
        itc.previous();
        if ( itc.key() > max ) this->removePointFromColor( itc.key() );
        else break;
    }

    QMutableMapIterator<double, double> ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        if ( ito.key() < min ) this->removePointFromOpacity( ito.key() );
        else break;
    }
    ito.toBack();
    while ( ito.hasPrevious() )
    {
        ito.previous();
        if ( ito.key() > max ) this->removePointFromOpacity( ito.key() );
        else break;
    }

    m_changed = m_colorChanged = m_opacityChanged = true;
}

void TransferFunction::print() const
{
    DEBUG_LOG( "Color:" );
    QMapIterator< double, QColor > itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        DEBUG_LOG( qPrintable( QString( "    x = %1    rgb = (%2, %3, %4)" ).arg( itc.key() )
                  .arg( itc.value().redF() ).arg( itc.value().greenF() ).arg( itc.value().blueF() ) ) );
    }

    DEBUG_LOG( "Opacity:" );
    QMapIterator< double, double > ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        DEBUG_LOG( qPrintable( QString( "    x = %1    opacity = %2" ).arg( ito.key() ).arg( ito.value() ) ) );
    }
}

TransferFunction & TransferFunction::operator =( const TransferFunction & transferFunction )
{
    m_name = transferFunction.m_name;
    m_color = transferFunction.m_color;
    m_opacity = transferFunction.m_opacity;

    m_changed = transferFunction.m_changed;
    if ( !m_changed ) m_rgba = transferFunction.m_rgba;

    m_colorChanged = m_opacityChanged = true;

    return *this;
}

bool TransferFunction::operator ==( const TransferFunction & transferFunction ) const
{
    return m_name == transferFunction.m_name && m_color == transferFunction.m_color && m_opacity == transferFunction.m_opacity;
}

}
