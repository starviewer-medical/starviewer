/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGMATHTOOLS_H
#define UDGMATHTOOLS_H

#include <cmath>

/**
    Aquest arxiu recull tot tipus de funcions matemàtiques útils
*/

namespace udg{

class MathTools{

public:

    enum Sign{ POSITIVE, NEGATIVE };

/// Declaració de constants
static const double LOGARITHM_BASE2_E;
static const long double REVERS_PI_LONG;
static const double PI;
static const long double PI_LONG;
static const long double PI_DIV_2_LONG;


/// Epsilon, nombre extremadament petit
static double Epsilon() { return 1E-9; };

/// Logaritme en base 2
static double logTwo(const double x, const bool zero = true);

/// Càlcul de l'entropia binària
static double binaryEntropy(const double p);

/// Divisió reals
static double divReal( const double dividend, const double divisor, const bool zero = true, const double value = 0);

/// Definim un rang pel que un nombre pot considerar-se 0
inline static double zero(const double x)
{
  return (- MathTools::Epsilon() < x && x < + MathTools::Epsilon() ) ? 0 : x;
}

///retorna el signe del número passat per paràmetre
static int getSign( double number );

///ens diu si els dos nombres passats per paràmetre tenen el mateix signe.
static bool haveSameSign( double numberA, double numberB );

/// Calcula l'angle entre dos vectors. Retorna el valor en radians
static double angleInRadians( double vec1[3] , double vec2[3] );
/// Calcula l'angle entre dos vectors. Retorna el valor en graus
static double angleInDegrees( double vec1[3] , double vec2[3] );

/// Calcula la intersecció de dos plans definits per un punt i una normal (p,n) i (q,m) respectivament
/// Retorna 0 si els plans no intersecten perquè són paral·lels , altrament >0
static int planeIntersection( double p[3] , double n[3], double q[3] , double m[3], double r[3] , double t[3] );

/// Calcula la intersecció de tres plans definits per un punt i una normal (p,n) , (q,m) , (r,t) respectivament
/// Retorna 0 si ... -1 si ... perquè són paral·lels , altrament >0
static int planeIntersection( double p[3] , double n[3], double q[3] , double m[3], double r[3] , double t[3] , double intersectionPoint[3] );

///a partir del segment determinat pels dos punts passats com a paràmetres, calcula un dels possibles vectors directors
static double* directorVector( const double point1[3], const double point2[3] );

///producte vectorial dels dos vectors directors passats per paràmetre.
static double* vectorialProduct( double vectorDirector1[3], double vectorDirector2[3] );

///ens retorna el mínim dels dos valors
static double minimum( double a, double b);

///ens retorna el màxim dels dos valors
static double maximum( double a, double b);

/// Arrel cúbica.
inline static double cbrt( double x )
{
    return std::pow( x, 1.0 / 3.0 );
}

/// distància entre punts 2D i 3D
static double getDistance2D( const double firstPoint[2], const double secondPoint[2] );
static double getDistance3D( const double firstPoint[3], const double secondPoint[3] );

};

} // end namespace udg


#endif


