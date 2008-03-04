/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "vector3.h"

#include <QString>


namespace udg {


Vector3 & Vector3::normalize()
{
    double length = this->length();
    x /= length; y /= length; z /= length;
    return *this;
}


Vector3 Vector3::operator +() const
{
    return *this;
}


Vector3 Vector3::operator +( const Vector3 & v ) const
{
    return Vector3( x + v.x, y + v.y, z + v.z );
}


Vector3 & Vector3::operator +=( const Vector3 & v )
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}


Vector3 & Vector3::operator -=( const Vector3 & v )
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}


Vector3 Vector3::operator ^( const Vector3 & v ) const  // producte vectorial
{
    return Vector3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}


QString Vector3::toString() const
{
    return QString( "(%1, %2, %3)" ).arg( x ).arg( y ).arg( z );
}


Vector3 operator *( const Vector3 & v, double a )
{
    return Vector3( v.x * a, v.y * a, v.z * a );
}


Vector3 & operator *=( Vector3 & v, double a )
{
    v.x *= a; v.y *= a; v.z *= a;
    return v;
}


Vector3 operator /( const Vector3 & v, double a )
{
    return Vector3(v.x / a, v.y / a, v.z / a);
}


Vector3 & operator /=( Vector3 & v, double a )
{
    v.x /= a; v.y /= a; v.z /= a;
    return v;
}


}
