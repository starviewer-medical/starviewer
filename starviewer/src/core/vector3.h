#ifndef VECTOR3_H
#define VECTOR3_H


#include <cmath>

#include <QString>


namespace udg {


/**
 * Vector de dimensió 3.
 *
 * Inclou tots els mètodes per treballar amb vectors 3D amb components reals.
 */
template <class T>
class TVector3 {

public:

    TVector3( T x = 0.0, T y = 0.0, T z = 0.0 );
    template <class C>
    TVector3( const TVector3<C> &v );

    /// Retorna el mòdul del vector.
    T length() const;
    /// Normalitza el vector i el retorna per referència.
    TVector3<T>& normalize();

    /// Operador d'assignació.
    template <class C>
    TVector3<T>& operator =( const TVector3<C> &v );

    /// Retorna una còpia del vector.
    TVector3<T> operator +() const;
    /// Retorna el vector negat.
    TVector3<T> operator -() const;
    /// Retorna la suma de vectors.
    TVector3<T> operator +( const TVector3<T> &v ) const;
    /// Suma els vectors, assigna el resultat sobre l'actual i el retorna per referència.
    TVector3<T>& operator +=( const TVector3<T> &v );
    /// Retorna la resta dels vectors.
    TVector3<T> operator -( const TVector3<T> &v ) const;
    /// Resta els vectors, assigna el resultat sobre l'actual i el retorna per referència.
    TVector3<T>& operator -=( const TVector3<T> &v );
    /// Retorna el producte escalar dels vectors.
    T operator *( const TVector3<T> &v ) const; // producte escalar
    /// Retorna el producte vectorial dels vectors.
    TVector3<T> operator ^( const TVector3<T> &v ) const;   // producte vectorial

    /// Retorna una representació del vector en forma de text.
    QString toString() const;

    /// Producte d'un escalar per un vector.
    template <class C>
    friend TVector3<C> operator *( double a, const TVector3<C> &v );
    /// Producte d'un vector per un escalar.
    template <class C>
    friend TVector3<C> operator *( const TVector3<C> &v, double a );
    /// Producte d'un vector per un escalar assignat al propi vector.
    template <class C>
    friend TVector3<C>& operator *=( TVector3<C> &v, double a );
    /// Divisió d'un vector per un escalar.
    template <class C>
    friend TVector3<C> operator /( const TVector3<C> &v, double a );
    /// Divisió d'un vector per un escalar assignada al propi vector.
    template <class C>
    friend TVector3<C>& operator /=( TVector3<C> &v, double a );

public:

    /// Components del vector.
    T x, y, z;

};


template <class T>
inline TVector3<T>::TVector3( T aX, T aY, T aZ )
 : x( aX ), y( aY ), z( aZ )
{
}


template <class T>
template <class C>
inline TVector3<T>::TVector3( const TVector3<C> &v )
 : x( v.x ), y( v.y ), z( v.z )
{
}


template <class T>
inline T TVector3<T>::length() const
{
    return sqrt( x * x + y * y + z * z );
}


template <class T>
inline TVector3<T>& TVector3<T>::normalize()
{
    T length = this->length();
    x /= length; y /= length; z /= length;
    return *this;
}


template <class T>
template <class C>
inline TVector3<T>& TVector3<T>::operator =( const TVector3<C> &v )
{
    x = v.x; y = v.y; z = v.z;
    return *this;
}


template <class T>
inline TVector3<T> TVector3<T>::operator +() const
{
    return *this;
}


template <class T>
inline TVector3<T> TVector3<T>::operator -() const
{
    return TVector3<T>( -x, -y, -z );
}


template <class T>
inline TVector3<T> TVector3<T>::operator +( const TVector3<T> &v ) const
{
    return TVector3<T>( x + v.x, y + v.y, z + v.z );
}


template <class T>
inline TVector3<T>& TVector3<T>::operator +=( const TVector3<T> &v )
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}


template <class T>
inline TVector3<T> TVector3<T>::operator -( const TVector3<T> &v ) const
{
    return TVector3<T>( x - v.x, y - v.y, z - v.z );
}


template <class T>
inline TVector3<T>& TVector3<T>::operator -=( const TVector3<T> &v )
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}


template <class T>
inline T TVector3<T>::operator *( const TVector3<T> &v ) const  // producte escalar
{
    return x * v.x + y * v.y + z * v.z;
}


template <class T>
inline TVector3<T> TVector3<T>::operator ^( const TVector3<T> &v ) const    // producte vectorial
{
    return TVector3<T>( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
}


template <class T>
inline QString TVector3<T>::toString() const
{
    return QString( "(%1, %2, %3)" ).arg( x ).arg( y ).arg( z );
}


template <class T>
inline TVector3<T> operator *( double a, const TVector3<T> &v )
{
    return TVector3<T>( a * v.x, a * v.y, a * v.z );
}


template <class T>
inline TVector3<T> operator *( const TVector3<T> &v, double a )
{
    return TVector3<T>( v.x * a, v.y * a, v.z * a );
}


template <class T>
inline TVector3<T>& operator *=( TVector3<T> &v, double a )
{
    v.x *= a; v.y *= a; v.z *= a;
    return v;
}


template <class T>
inline TVector3<T> operator /( const TVector3<T> &v, double a )
{
    return Vector3(v.x / a, v.y / a, v.z / a);
}


template <class T>
inline TVector3<T>& operator /=( TVector3<T> &v, double a )
{
    v.x /= a; v.y /= a; v.z /= a;
    return v;
}


typedef TVector3<float> Vector3Float;
typedef TVector3<double> Vector3Double;
typedef TVector3<double> Vector3;


}


#endif
