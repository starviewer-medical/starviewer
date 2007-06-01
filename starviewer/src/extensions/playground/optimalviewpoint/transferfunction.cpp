/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#include "transferfunction.h"

#include <QColor>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>



namespace udg {



/**
 * Construeix una funció de transferència buida (sense punts).
 */
TransferFunction::TransferFunction()
{
}



TransferFunction::~TransferFunction()
{
}



void TransferFunction::addPoint( double x, const QColor & rgba )
{
    m_color[x] = rgba;
    m_opacity[x] = rgba.alphaF();
}



void TransferFunction::addPoint( double x, const QColor & color, double opacity )
{
    m_color[x] = color;
    m_opacity[x] = opacity;
}



void TransferFunction::removePoint( double x )
{
    m_color.remove( x );
    m_opacity.remove( x );
}



/**
 * Afegeix un punt a la part de color.
 *
 * Si \a x ja té un color definit, aquest es modifica.
 *
 * \param x Valor de propietat. Real positiu.
 * \param color Color que correspon a \a x.
 */
void TransferFunction::addPointToColor( double x, const QColor & color )
{
    m_color[x] = color;
}



/**
 * Afegeix un punt a la part de color, en format RGB enter.
 *
 * Si \a x ja té un color definit, aquest es modifica.
 *
 * \param x Valor de propietat. Real positiu.
 * \param r,g,b Valors RGB del color que correspon a \a x. Enters a l'interval [0,255].
 */
void TransferFunction::addPointToColorRGB( double x, int r, int g, int b )
{
    m_color[x] = QColor( r, g, b );
}



/**
 * Afegeix un punt a la part de color, en format RGB real.
 *
 * Si \a x ja té un color definit, aquest es modifica.
 *
 * \param x Valor de propietat. Real positiu.
 * \param r,g,b Valors RGB del color que correspon a \a x. Reals a l'interval [0,1].
 */
void TransferFunction::addPointToColorRGB( double x, double r, double g, double b )
{
    QColor color;
    color.setRgbF( r, g, b );
    m_color[x] = color;
}



/**
 * Esborra un punt de la part de color.
 *
 * Si \a x no té cap color definit, el mètode no té efecte.
 *
 * \param x Valor de propietat. Real positiu.
 */
void TransferFunction::removePointFromColor( double x )
{
    m_color.remove( x );
}



/**
 * Afegeix un punt a la part d'opacitat.
 *
 * Si \a x ja té una opacitat definida, aquesta es modifica.
 *
 * \param x Valor de propietat. Real positiu.
 * \param opacity Opacitat que correspon a \a x. Real a l'interval [0,1].
 */
void TransferFunction::addPointToOpacity( double x, double opacity )
{
    m_opacity[x] = opacity;
}



/**
 * Esborra un punt de la part d'opacitat.
 *
 * Si \a x no té cap opacitat definida, el mètode no té efecte.
 *
 * \param x Valor de propietat. Real positiu.
 */
void TransferFunction::removePointFromOpacity( double x )
{
    m_opacity.remove( x );
}



/**
 * Esborra tots els punts de color i d'opacitat.
 */
void TransferFunction::clear()
{
    m_color.clear();
    m_opacity.clear();
}



/**
 * Esborra tots els punts de color.
 */
void TransferFunction::clearColor()
{
    m_color.clear();
}



/**
 * Esborra tots els punts d'opacitat.
 */
void TransferFunction::clearOpacity()
{
    m_opacity.clear();
}



/**
 * Accés als punts de color mitjançant un iterador.
 *
 * L'iterador pot ser destruït després d'utilitzar-lo.
 *
 * \return Iterador sobre els punts de color.
 */
QMapIterator< double, QColor > * TransferFunction::getColorPoints() const
{
    return new QMapIterator< double, QColor >( m_color );
}



/**
 * Accés als punts d'opacitat mitjançant un iterador.
 *
 * L'iterador pot ser destruït després d'utilitzar-lo.
 *
 * \return Iterador sobre els punts d'opacitat.
 */
QMapIterator< double, double > * TransferFunction::getOpacityPoints() const
{
    return new QMapIterator< double, double >( m_opacity );
}



/**
 * Retorna la funció de transferència de color en format VTK.
 *
 * No es guarda cap referència de la funció en format VTK.
 *
 * \return Funció de transferència de color en format VTK.
 */
vtkColorTransferFunction * TransferFunction::getColorTransferFunction() const
{
    vtkColorTransferFunction * colorTransferFunction = vtkColorTransferFunction::New();

    QMapIterator< double, QColor > it( m_color );

    while ( it.hasNext() )
    {
        it.next();
        colorTransferFunction->AddRGBPoint( it.key(), it.value().redF(), it.value().greenF(), it.value().blueF() );
    }

    return colorTransferFunction;
}



/**
 * Retorna la funció de transferència d'opacitat en format VTK.
 *
 * No es guarda cap referència de la funció en format VTK.
 *
 * \return Funció de transferència d'opacitat en format VTK.
 */
vtkPiecewiseFunction * TransferFunction::getOpacityTransferFunction() const
{
    vtkPiecewiseFunction * opacityTransferFunction = vtkPiecewiseFunction::New();

    QMapIterator< double, double > it( m_opacity );

    while ( it.hasNext() )
    {
        it.next();
        opacityTransferFunction->AddPoint( it.key(), it.value() );
    }

    return opacityTransferFunction;
}



/**
 * Escriu la funció de transferència a la sortida estàndard (per a debug).
 */
void TransferFunction::print() const
{
    std::cout << "Color:" << std::endl;
    QMapIterator< double, QColor > itc( m_color );
    while ( itc.hasNext() )
    {
        itc.next();
        std::cout << "\tx = " << itc.key()
                << "\trgb = " << itc.value().redF() << " " << itc.value().greenF() << " " << itc.value().blueF() << std::endl;
    }

    std::cout << "Opacity:" << std::endl;
    QMapIterator< double, double > ito( m_opacity );
    while ( ito.hasNext() )
    {
        ito.next();
        std::cout << "\tx = " << ito.key() << "\topacity = " << ito.value() << std::endl;
    }
}


/// \warning No està fet general!!!!!
QGradientStops TransferFunction::getGradientStops() const
{
    QGradientStops gradientStops;

    QMapIterator< double, QColor > it( m_color );

    while ( it.hasNext() )
    {
        it.next();
        gradientStops << QGradientStop( it.key() / 255.0, it.value() );
    }

    return gradientStops;
}



bool TransferFunction::operator ==( const TransferFunction & transferFunction ) const
{
    return m_color == transferFunction.m_color && m_opacity == transferFunction.m_opacity;
}



}
