/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef VECTOR3_H
#define VECTOR3_H


class QString;


namespace udg {


/**
 * Vector de dimensió 3.
 *
 * Inclou tots els mètodes per treballar amb vectors 3D amb components reals.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Vector3 {

public:

    Vector3( double x = 0.0, double y = 0.0, double z = 0.0 );

    /// Retorna el mòdul del vector.
    double length() const;
    /// Normalitza el vector i el retorna per referència.
    Vector3 & normalize();

    /// Retorna una còpia del vector.
    Vector3 operator +() const;
    /// Retorna el vector negat.
    Vector3 operator -() const;
    /// Retorna la suma de vectors.
    Vector3 operator +( const Vector3 & v ) const;
    /// Suma els vectors, assigna el resultat sobre l'actual i el retorna per referència.
    Vector3 & operator +=( const Vector3 & v );
    /// Retorna la resta dels vectors.
    Vector3 operator -( const Vector3 & v ) const;
    /// Resta els vectors, assigna el resultat sobre l'actual i el retorna per referència.
    Vector3 & operator -=( const Vector3 & v );
    /// Retorna el producte escalar dels vectors.
    double operator *( const Vector3 & v ) const;   // producte escalar
    /// Retorna el producte vectorial dels vectors.
    Vector3 operator ^( const Vector3 & v ) const;  // producte vectorial

    /// Retorna una representació del vector en forma de text.
    QString toString() const;

    /// Producte d'un escalar per un vector.
    friend Vector3 operator *( double a, const Vector3 & v );
    /// Producte d'un vector per un escalar.
    friend Vector3 operator *( const Vector3 & v, double a );
    /// Producte d'un vector per un escalar assignat al propi vector.
    friend Vector3 & operator *=( Vector3 & v, double a );
    /// Divisió d'un vector per un escalar.
    friend Vector3 operator /( const Vector3 & v, double a );
    /// Divisió d'un vector per un escalar assignada al propi vector.
    friend Vector3 & operator /=( Vector3 & v, double a );

public:

    /// Components del vector.
    double x, y, z;

};


}


#endif
