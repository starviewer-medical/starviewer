/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGTRANSFERFUNCTION_H
#define UDGTRANSFERFUNCTION_H



#include <QGradient>
#include <QMap>



class QColor;

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
 * \warning En cap dels mètodes d'aquesta classe no es fan comprovacions de rang.
 *
 * @author Grup de Gràfics de Girona ( GGG ) <vismed@ima.udg.edu>
 */
class TransferFunction
{

public:

    /// Construeix una funció de transferència buida (sense punts).
    TransferFunction();
    /// Destructor.
    ~TransferFunction();

    /// Retorna el color i l'opacitat corresponents a x.
    QColor get( double x ) const;
    /// Retorna el color corresponent a x.
    QColor getColor( double x ) const;
    /// Retorna l'opacitat corresponent a x.
    double getOpacity( double x ) const;

    /// Afegeix un punt de color i d'opacitat mitjançant un QColor RGBA.
    void addPoint( double x, const QColor & rgba );
    /// Afegeix un punt de color i d'opacitat.
    void addPoint( double x, const QColor & color, double opacity );
    /// Esborra un punt de color i d'opacitat.
    void removePoint( double x );

    /// Afegeix un punt a la part de color.
    void addPointToColor( double x, const QColor & color );
    /// Afegeix un punt a la part de color, en format RGB enter.
    void addPointToColorRGB( double x, int r, int g, int b );
    /// Afegeix un punt a la part de color, en format RGB real.
    void addPointToColorRGB( double x, double r, double g, double b );
    /// Esborra un punt de la part de color.
    void removePointFromColor( double x );

    /// Afegeix un punt a la part d'opacitat.
    void addPointToOpacity( double x, double opacity );
    /// Esborra un punt de la part d'opacitat.
    void removePointFromOpacity( double x );

    /// Esborra tots els punts de color i d'opacitat.
    void clear();
    /// Esborra tots els punts de color.
    void clearColor();
    /// Esborra tots els punts d'opacitat.
    void clearOpacity();

    /// Accés als punts de color i d'opacitat mitjançant un iterador.
    QMapIterator< double, QColor > * getPoints() const;
    /// Accés als punts de color mitjançant un iterador.
    QMapIterator< double, QColor > * getColorPoints() const;
    /// Accés als punts d'opacitat mitjançant un iterador.
    QMapIterator< double, double > * getOpacityPoints() const;

    /// Retorna la funció de transferència de color en format VTK.
    vtkColorTransferFunction * getColorTransferFunction() const;
    /// Retorna la funció de transferència d'opacitat en format VTK.
    vtkPiecewiseFunction * getOpacityTransferFunction() const;

    /// Escriu la funció de transferència a la sortida estàndard (per a debug).
    void print() const;

    /// Retorna la funció de transferència en forma de QGradientStops.
    QGradientStops getGradientStops() const;

    bool operator ==( const TransferFunction & transferFunction ) const;

private:

    QMap< double, QColor > m_color;
    QMap< double, double > m_opacity;

    mutable QMap< double, QColor > m_rgba;
    mutable bool m_changed;

};



}



#endif
