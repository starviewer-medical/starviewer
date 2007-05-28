#ifndef VECTOR3_H
#define VECTOR3_H



#include <iostream>



class Vector3 {


public:

    double x, y, z;

    Vector3( double x = 0.0, double y = 0.0, double z = 0.0 );

    Vector3 & normalise();
    Vector3 & normalize();
    double length() const;

    Vector3 operator -() const;
    Vector3 operator +( const Vector3 & v ) const;
    Vector3 & operator +=( const Vector3 & v );
    Vector3 operator -( const Vector3 & v ) const;
    double operator *( const Vector3 & v ) const;   // producte escalar
    Vector3 operator ^( const Vector3 & v ) const;  // producte vectorial
//     double operator []( unsigned short i ) const;

    friend Vector3 operator *( double a, const Vector3 & v );
    friend Vector3 operator *( const Vector3 & v, double a );
    friend Vector3 & operator *=( Vector3 & v, double a );
    friend Vector3 operator /( const Vector3 & v, double a );
    friend Vector3 & operator /=( Vector3 & v, double a );

    friend std::ostream & operator <<( std::ostream & o, const Vector3 & v );


};



#endif
