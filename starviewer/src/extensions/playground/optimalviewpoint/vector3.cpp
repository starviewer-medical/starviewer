#include "vector3.h"

#include <math.h>

Vector3::Vector3( double x, double y, double z )
{
    this->x = x; this->y = y; this->z = z;
}

Vector3 & Vector3::normalise()
{
    double mod = this->length();
    x /= mod; y /= mod; z /= mod;
    return *this;
}

Vector3 & Vector3::normalize()
{
    double mod = this->length();
    x /= mod; y /= mod; z /= mod;
    return *this;
}

double Vector3::length() const
{
    return sqrt( x * x + y * y + z * z );
}

Vector3 Vector3::operator -() const
{
    return Vector3( -x, -y, -z );
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

Vector3 Vector3::operator -( const Vector3 & v ) const
{
    return Vector3( x - v.x, y - v.y, z - v.z );
}

double Vector3::operator *( const Vector3 & v ) const  // producte escalar
{
    return x * v.x + y * v.y + z * v.z;
}

Vector3 Vector3::operator ^( const Vector3 & v ) const // producte vectorial
{
    return Vector3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}

// double Vector3::operator []( unsigned short i ) const
// {
//     return (&x)[i];
// }

Vector3 operator *( double a, const Vector3 & v )
{
    return Vector3( a * v.x, a * v.y, a * v.z );
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

std::ostream & operator <<( std::ostream & o, const Vector3 & v )
{
    o << "(" << v.x << "," << v.y << "," << v.z << ")";

    return o;
}
