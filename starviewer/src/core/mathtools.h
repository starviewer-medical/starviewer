/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGMATHTOOLS_H
#define UDGMATHTOOLS_H

namespace udg{

/**
 *
 *  Aquesta classe és un recull de funcions matemàtiques de caràcter general.
 *  Inclou la declaració de constants (Pi, Nombre E, ...), operacions aritmètiques,
 *  operacions amb vectors, etc.
 *
 */
class MathTools {
public:
    enum IntersectionResults{ ParallelLines, SkewIntersection, LinesIntersect };

///
/// Declaració de constants
///
static const double NumberEBase2Logarithm;
static const long double ReversePiNumberLong;
static const double PiNumber;
static const long double PiNumberLong;
static const long double PiNumberDivBy2Long;
static const double DegreesToRadiansAsDouble;
static const double RadiansToDegreesAsDouble;
/// Epsilon, nombre extremadament petit
static const double Epsilon;

///
/// Operacions aritmètiques 
///

/// Ens retorna el mínim dels dos valors
static double minimum( double a, double b);

/// Ens retorna el màxim dels dos valors
static double maximum( double a, double b);

/// Ens retorna el valor truncat
static double truncate( double x);

/// Arrodoneix un nombre real a l'enter més proper
static int roundToNearestInteger( double x );

/// Transforma el valor passat per paràmetre de graus a radians i vice-versa
static float degreesToRadians(float degrees);
static float radiansToDegrees(float radians);

/// Calcula el logaritme en base 2
/// @param x Valor del que es calcula el logaritme
/// @param zero Si cert, en cas que x sigui 0, ens retornarà 0, altrament
///             farà el càlcul igualment del logaritme sense fer aquesta comprovació
static double logTwo(const double x, const bool zero = true);

/// Arrel cúbica
static double cubeRoot( double x );

/// Determina si podem considerar pràcticament iguals els dos 
/// valors passats per paràmetre si la seva diferència és menor al valor d'Epsilon 
static bool closeEnough(float f1, float f2);

/// Distància entre punts 2D i 3D
static double getDistance2D( const double firstPoint[2], const double secondPoint[2] );
static double getDistance3D( const double firstPoint[3], const double secondPoint[3] );

///
/// Càlculs d'interseccions
///

/// Calcula la intersecció de dos plans definits per un punt i una normal (p,n) i (q,m) respectivament
/// Retorna 0 si els plans no intersecten perquè són paral·lels , altrament >0
static int planeIntersection( double p[3] , double n[3], double q[3] , double m[3], double r[3] , double t[3] );

/// Calcula la intersecció de tres plans definits per un punt i una normal (p,n) , (q,m) , (r,t) respectivament
/// Retorna 0 si ... -1 si ... perquè són paral·lels , altrament >0
static int planeIntersection( double p[3] , double n[3], double q[3] , double m[3], double r[3] , double t[3] , double intersectionPoint[3] );

/**
* Retorna el punt d'intersecció de dues línies 
* infinites definides per dos segments
* @param p1 Primer punt de la primera recta
* @param p2 Segon punt de la primera recta
* @param p3 Primer punt de la segona recta
* @param p4 Segon punt de la segona recta
* @param state Contindrà el resultat de la intersecció: ParallelLines, LinesIntersect, SkewLines (no intersecten però es creuen, estan a plans paral·lels)
*/
static double *infiniteLinesIntersection(double *p1, double *p2, double *p3, double *p4, int &state);

///
/// Càlculs amb vectors
///

/// A partir del segment determinat pels dos punts passats com a paràmetres, calcula un dels possibles vectors directors
static double* directorVector( const double point1[3], const double point2[3] );

/// Calcula el mòdul d'un vector
static double modulus( double vector[3] );

/// Producte escalar dels dos vectors passats per paràmetre
static double dotProduct( double vector1[3], double vector2[3] );

/// Calcula el producte vectorial dels dos vectors directors passats per paràmetre i ens deixa el resultat en el tercer paràmetre
static void crossProduct( double vectorDirector1[3], double vectorDirector2[3], double crossProductVector[3] );

/// Calcula l'angle entre dos vectors. Retorna el valor en radians o graus
static double angleInRadians( double vec1[3] , double vec2[3] );
static double angleInDegrees( double vec1[3] , double vec2[3] );

};

} // end namespace udg


#endif


