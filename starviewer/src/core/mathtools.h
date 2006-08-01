/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGMATHTOOLS_H
#define UDGMATHTOOLS_H
 
/**
    Aquest arxiu recull tot tipus de funcions matemàtiques útils
*/
#include <cmath>
 
namespace udg{

class MathTools{ 

public:

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
};

} // end namespace udg

 
#endif


