#ifndef UDGHDRCOLOR_H
#define UDGHDRCOLOR_H


#include <QColor>


namespace udg {


/**
 * Color amb les components representades en coma flotant.
 */
class HdrColor {

public:

    /// Crea un color (0, 0, 0, 0).
    HdrColor();
    /// Crea un color amb els paràmetres desitjats.
    HdrColor( float r, float g, float b, float a = 1.0 );
    /// Crea un color a partir d'un QColor.
    HdrColor( const QColor &color );
    /// Destructor.
    ~HdrColor();

    /// Multiplica les components de color per \a f i retorna el color.
    HdrColor& multiplyColorBy( float f );
    /// Posa a 1 les components més grans que 1.
    HdrColor& clamp();

    /// Suma component a component.
    HdrColor operator +( const HdrColor &c ) const;
    /// Suma component a component.
    HdrColor& operator +=( const HdrColor &c );
    /// Producte component a component.
    HdrColor operator *( const HdrColor &c ) const;
    /// Producte component a component.
    HdrColor& operator *=( const HdrColor &c );
    /// Producte de totes les components per un real.
    HdrColor operator *( float f ) const;
    /// Producte de totes les components per un real.
    HdrColor& operator *=( float f );

    /// Producte de totes les components per un real.
    friend HdrColor operator *( float f, const HdrColor &c );

    /// Retorna una string representativa del color.
    QString toString() const;

    float red, green, blue, alpha;

};


inline HdrColor::HdrColor()
{
    red = green = blue = alpha = 0.0;
}


inline HdrColor::HdrColor( float r, float g, float b, float a )
{
    red = r;
    green = g;
    blue = b;
    alpha = a;
}


inline HdrColor::HdrColor( const QColor &color )
{
    red = color.redF();
    green = color.greenF();
    blue = color.blueF();
    alpha = color.alphaF();
}


inline HdrColor::~HdrColor()
{
}


inline HdrColor& HdrColor::multiplyColorBy( float f )
{
    red *= f;
    green *= f;
    blue *= f;
    return *this;
}


inline HdrColor& HdrColor::clamp()
{
    if ( red > 1.0 ) red = 1.0;
    if ( green > 1.0 ) green = 1.0;
    if ( blue > 1.0 ) blue = 1.0;
    if ( alpha > 1.0 ) alpha = 1.0;
    return *this;
}


inline HdrColor HdrColor::operator +( const HdrColor &c ) const
{
    return HdrColor( red + c.red, green + c.green, blue + c.blue, alpha + c.alpha );
}


inline HdrColor& HdrColor::operator +=( const HdrColor &c )
{
    red += c.red;
    green += c.green;
    blue += c.blue;
    alpha += c.alpha;
    return *this;
}


inline HdrColor HdrColor::operator *( const HdrColor &c ) const
{
    return HdrColor( red * c.red, green * c.green, blue * c.blue, alpha * c.alpha );
}


inline HdrColor& HdrColor::operator *=( const HdrColor &c )
{
    red *= c.red;
    green *= c.green;
    blue *= c.blue;
    alpha *= c.alpha;
    return *this;
}


inline HdrColor HdrColor::operator *( float f ) const
{
    return HdrColor( red * f, green * f, blue * f, alpha * f );
}


inline HdrColor& HdrColor::operator *=( float f )
{
    red *= f;
    green *= f;
    blue *= f;
    alpha *= f;
    return *this;
}


inline HdrColor operator *( float f, const HdrColor &c )
{
    return c * f;
}


inline QString HdrColor::toString() const
{
    return QString( "(%1, %2, %3, %4)" ).arg( red ).arg( green ).arg( blue ).arg( alpha );
}


}


#endif
