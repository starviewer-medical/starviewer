/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTRANSFERFUNCTION_H
#define UDGTRANSFERFUNCTION_H

#include <QColor>
#include <QMap>

class vtkColorTransferFunction;
class vtkPiecewiseFunction;

namespace udg {

/**
 * Encapsula les dades i la funcionalitat d'una funció de transferència.
 *
 * Permet definir funcions de transferència mitjançant una sèrie de punts que
 * estableixen una correspondència entre els valors de propietat i els colors i
 * opacitats. Internament, els valors de propietat es representen com a reals
 * positius, els colors com a QColors i les opacitats com a reals a l'interval
 * [0,1].
 *
 * \warning En cap dels mètodes d'aquesta classe no es fan comprovacions de
 * rang.
 *
 * Hi ha mètodes per treballar amb la part de color i la d'opacitat per separat
 * i mètodes per treballar-hi alhora. Es poden usar indistintament els dos modes
 * de treball amb el mateix objecte TransferFunction.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class TransferFunction
{
public:
    /// Construeix una funció de transferència buida (sense punts).
    TransferFunction();
    TransferFunction( const TransferFunction & transferFunction );
    ~TransferFunction();

    /// Retorna el nom de la funció de transferència.
    const QString & name() const;
    /// Assigna un nom a la funció de transferència.
    void setName( const QString & name );

    /// Retorna el color i l'opacitat corresponents a \a x en un QColor RGBA.
    QColor get( double x ) const;

    /// Retorna el color corresponent a \a x.
    QColor getColor( double x ) const;

    /// Retorna l'opacitat corresponent a \a x.
    double getOpacity( double x ) const;

    /// Afegeix o modifica un punt de color i d'opacitat mitjançant un QColor RGBA.
    void addPoint( double x, const QColor & rgba );

    /// Afegeix o modifica un punt de color i d'opacitat.
    void addPoint( double x, const QColor & color, double opacity );

    /// Esborra un punt de color i d'opacitat (si existeix).
    void removePoint( double x );

    /// Afegeix o modifica un punt de color.
    void addPointToColor( double x, const QColor & color );

    /// Afegeix o modifica un punt de color, en format RGB enter.
    void addPointToColorRGB( double x, int r, int g, int b );

    /// Afegeix o modifica un punt de color, en format RGB real.
    void addPointToColorRGB( double x, double r, double g, double b );

    /// Esborra un punt de color (si existeix).
    void removePointFromColor( double x );

    /// Afegeix o modifica un punt d'opacitat.
    void addPointToOpacity( double x, double opacity );

    /// Esborra un punt d'opacitat (si existeix).
    void removePointFromOpacity( double x );

    /// Esborra tots els punts de color i d'opacitat.
    void clear();

    /// Esborra tots els punts de color.
    void clearColor();

    /// Esborra tots els punts d'opacitat.
    void clearOpacity();

    /// Retorna els punts x de color i d'opacitat. Per accedir al valor d'un punt cal cridar get( x ).
    QList< double > getPoints() const;

    /// Retorna els punts x de color. Per accedir al valor d'un punt cal cridar getColor( x ).
    QList< double > getColorPoints() const;

    /// Retorna els punts x d'opacitat. Per accedir al valor d'un punt cal cridar getOpacity( x ).
    QList< double > getOpacityPoints() const;

    /// Retorna la funció de transferència de color en format VTK.
    vtkColorTransferFunction * getColorTransferFunction() const;

    /// Retorna la funció de transferència d'opacitat en format VTK.
    vtkPiecewiseFunction * getOpacityTransferFunction() const;

    /// Escriu la funció de transferència a la sortida estàndard (per a debug).
    void print() const;

    TransferFunction & operator =( const TransferFunction & transferFunction );
    bool operator ==( const TransferFunction & transferFunction ) const;

private:
    /// Nom de la funció de transferència.
    QString m_name;

    /// Punts de color RGB.
    QMap< double, QColor > m_color;

    /// Punts d'opacitat A.
    QMap< double, double > m_opacity;

    /// Punts RGBA. S'actualitza només quan es necessita.
    mutable QMap< double, QColor > m_rgba;

    /// Indica si hi ha hagut canvis a la funció des de l'últim cop que s'ha actualitzat m_rgba.
    mutable bool m_changed;

    /// Funció de transferència de color. S'actualitza només quan es necessita.
    mutable vtkColorTransferFunction * m_colorTransferFunction;

    /// Indica si hi ha hagut canvis de color des de l'últim cop que s'ha actualitzat m_colorTransferFunction.
    mutable bool m_colorChanged;

    /// Funció de transferència d'opacitat. S'actualitza només quan es necessita.
    mutable vtkPiecewiseFunction * m_opacityTransferFunction;

    /// Indica si hi ha hagut canvis d'opacitat des de l'últim cop que s'ha actualitzat m_opacityTransferFunction.
    mutable bool m_opacityChanged;

};


inline QColor TransferFunction::get( double x ) const
{
    QColor rgba = getColor( x );
    rgba.setAlphaF( getOpacity( x ) );
    return rgba;
}

inline QColor TransferFunction::getColor( double x ) const
{
    if ( m_color.isEmpty() )
        return Qt::black;

    QMap<double, QColor>::const_iterator lowerBound = m_color.lowerBound( x );

    if ( lowerBound == m_color.end() )  // > últim
        return ( --lowerBound ).value();

    if ( lowerBound.key() == x || lowerBound == m_color.begin() )   // exacte o < primer
        return lowerBound.value();

    QMap<double, QColor>::const_iterator a = lowerBound - 1, b = lowerBound;
    double alpha = ( x - a.key() ) / ( b.key() - a.key() );
    QColor aValue = a.value(), bValue = b.value();
    QColor color;
    color.setRedF( aValue.redF() + alpha * ( bValue.redF() - aValue.redF() ) );
    color.setGreenF( aValue.greenF() + alpha * ( bValue.greenF() - aValue.greenF() ) );
    color.setBlueF( aValue.blueF() + alpha * ( bValue.blueF() - aValue.blueF() ) );

    return color;
}

inline double TransferFunction::getOpacity( double x ) const
{
    if ( m_opacity.isEmpty() )
        return 0.0;

    QMap<double, double>::const_iterator lowerBound = m_opacity.lowerBound( x );

    if ( lowerBound == m_opacity.end() )    // > últim
        return ( --lowerBound ).value();

    if ( lowerBound.key() == x || lowerBound == m_opacity.begin() ) // exacte o < primer
        return lowerBound.value();

    QMap<double, double>::const_iterator a = lowerBound - 1, b = lowerBound;
    double alpha = ( x - a.key() ) / ( b.key() - a.key() );

    return a.value() + alpha * ( b.value() - a.value() );
}


}


#endif
