/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGMATHTOOLS_H
#define UDGMATHTOOLS_H

#include "vector3.h"

#include <limits>

#include <QList>
#include <QVector>

class QVector2D;
class QVector3D;

namespace udg {

/**
    Aquesta classe és un recull de funcions matemàtiques de caràcter general.
    Inclou la declaració de constants (Pi, Nombre E, ...), operacions aritmètiques,
    operacions amb vectors, etc.
  */
class MathTools {
public:
    enum IntersectionResults { ParallelLines, SkewIntersection, LinesIntersect };

/**
 *  Constant declarations.
 *  \todo Use constants from <numbers> header when upgrading to C++20.
 */
static constexpr double PiNumber = 3.14159265358979323846;                                  // pi
static constexpr long double PiNumberDivBy2Long = 1.5707963267948966192313216916397514L;    // pi/2
static constexpr double RadiansToDegreesAsDouble = 57.29577951308232;
/// Epsilon, nombre extremadament petit
static constexpr double Epsilon = 1e-9;
/// Valor màxim d'un tipus double
static constexpr double DoubleMaximumValue = std::numeric_limits<double>::max();

///
/// Operacions aritmètiques
///

/// Returns the bounded value between minimum and maximum corresponding to the given value.
/// If loop is true, when value is out of bounds, the oppposite of the surpassed bound (min or max) is returned
/// NOTE: This method has been implemented here to avoid the include of the cpp file at the end of the file because of the templated method
template<typename T>
static T getBoundedValue(T value, T minimum, T maximum, bool loop)
{
    T boundedValue = value;
    
    if (loop)
    {
        if (boundedValue < minimum)
        {
            boundedValue = maximum;
        }
        else if (value > maximum)
        {
            boundedValue = minimum;
        }
    }
    else
    {
        boundedValue = qBound(minimum, value, maximum);
    }

    return boundedValue;
}

/// Returns true if value is between min and mix (both inclusive), false otherwise
/// NOTE: This method has been implemented here to avoid the include of the cpp file at the end of the file because of the templated method
template<typename T>
static bool isInsideRange(T value, T min, T max)
{
    return value >= min && value <= max;
}

/// Tells if the number is odd/even
static bool isOdd(int x);
static bool isEven(int x);

/// Arrodoneix un nombre real a l'enter més proper
static int roundToNearestInteger(double x);

/// Transforma el valor passat per paràmetre de graus a radians i vice-versa
static constexpr float degreesToRadians(float degrees);
static constexpr float radiansToDegrees(float radians);

/// Returns log2 of x. Special case: if x is 0 returns 0.
static double logTwo(double x);

/// Determina si podem considerar pràcticament iguals els dos
/// valors passats per paràmetre si la seva diferència és menor al valor d'Epsilon
static bool closeEnough(float f1, float f2);

/// Returns true if difference between \a x and \a y is smaller than or equal to \a absoluteEpsilon, or smaller than or equal to the maximum of them multiplied
/// by \a relativeEpsilon, and false otherwise.
template <class T>
static bool almostEqual(T x, T y, T absoluteEpsilon = std::numeric_limits<T>::epsilon(), T relativeEpsilon = Epsilon)
{
    // Source: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    T difference = std::abs(x - y);

    if (difference <= absoluteEpsilon)
    {
        return true;
    }
    else
    {
        return difference <= std::max(std::abs(x), std::abs(y)) * relativeEpsilon;
    }
}

/// Returns true if almostEqual returns true for each component of \a v1 and \a v2, and false otherwise.
static bool almostEqual(const Vector3 &v1, const Vector3 &v2, double absoluteEpsilon = std::numeric_limits<double>::epsilon(),
                        double relativeEpsilon = Epsilon);

/// Distància entre punts 3D
static double getDistance3D(const Vector3 &firstPoint, const Vector3 &secondPoint);

/// Random number generation helpers

/// Generates a random number in the specified range, double and int precision
static double randomDouble(double minimum, double maximum);
static int randomInt(int minimum, int maximum);

/// Retorna a distància entre un punt i l'aresta més propera.
/// LastToFirstEdge significa si volem que es comprovi l'aresta que forma l'últim i el primer punt de la llista.
/// El paràmetre de sortida closestPoint, ens indicarà quin és el punt de la línia que queda més a prop del punt indicat i ClosestEdge ens indicarà l'aresta.
static double getPointToClosestEdgeDistance(double point3D[3], const QList<QVector<double> > &pointsList, bool lastToFirstEdge, double closestPoint[3], int &closestEdge);

/// Distància entre un punt i un segment finit definit per lineFirstPoint i lineSecondPoint
/// El paràmetre de sortida closestPoint, ens indicarà quin és el punt de la línia que queda més a prop del punt indicat
static double getPointToFiniteLineDistance(double point[3], double lineFirstPoint[3], double lineSecondPoint[3], double closestPoint[3]);

///
/// Càlculs d'interseccions
///

/// Calcula la intersecció de dos plans definits per un punt i una normal (p,n) i (q,m) respectivament
/// Retorna 0 si els plans no intersecten perquè són paral·lels, altrament >0
static int planeIntersection(double p[3], double n[3], double q[3], double m[3], double r[3], double t[3]);

/// Retorna el punt d'intersecció de dues línies
/// infinites definides per dos segments
/// @param p1 Primer punt de la primera recta
/// @param p2 Segon punt de la primera recta
/// @param p3 Primer punt de la segona recta
/// @param p4 Segon punt de la segona recta
/// @param state Contindrà el resultat de la intersecció: ParallelLines, LinesIntersect, SkewLines (no intersecten però es creuen, estan a plans paral·lels)
static double* infiniteLinesIntersection(double *p1, double *p2, double *p3, double *p4, int &state);

///
/// Càlculs amb vectors
///

/// A partir del segment determinat pels dos punts passats com a paràmetres, calcula un dels possibles vectors directors
static QVector3D directorVector(const QVector3D &point1, const QVector3D &point2);

/// Calcula el mòdul d'un vector
static double modulus(double vector[3]);

/// Producte escalar dels dos vectors passats per paràmetre
static double dotProduct(double vector1[3], double vector2[3]);

/// Calcula el producte vectorial dels dos vectors directors passats per paràmetre i ens deixa el resultat en el tercer paràmetre
static void crossProduct(double vectorDirector1[3], double vectorDirector2[3], double crossProductVector[3]);

/// Normalitza el vector passat per paràmetre
static double normalize(double vector[3]);

/// Retorna l'angle en radians d'un vector 2D amb l'eix horitzontal. Retorna valors en el rang [-pi, pi].
static double angleInRadians(const QVector2D &vector);
/// Retorna l'angle en graus d'un vector 2D amb l'eix horitzontal. Retorna valors en el rang [-180, 180].
static double angleInDegrees(const QVector2D &vector);

/// Calcula l'angle entre dos vectors. Retorna el valor en radians o graus
static double angleInRadians(const Vector3 &vec1, const Vector3 &vec2);
static double angleInDegrees(const Vector3 &vec1, const Vector3 &vec2);

/// Retorna cert si \a x és NaN.
static bool isNaN(double x);

/// Retorna la potència de 2 superior més propera a v.
static unsigned int roundUpToPowerOf2(unsigned int v);

/// Retorna el primer múltiple de multiple més gran o igual que i. Només funciona si múltiple > 0, però no es comproven els paràmetres.
static unsigned int roundUpToMultipleOfNumber(unsigned int i, unsigned int multiple);

private:
    /// Initializes random seed if needed
    static void initializeRandomSeed();
};

inline bool MathTools::isOdd(int x)
{
    return (x % 2);
}

inline bool MathTools::isEven(int x)
{
    return !isOdd(x);
}

inline int MathTools::roundToNearestInteger(double x)
{
    return static_cast<int>(std::lround(x));
}

inline constexpr float MathTools::degreesToRadians(float degrees)
{
    return static_cast<float>((static_cast<double>(degrees) * PiNumber) / 180.0);
}

inline constexpr float MathTools::radiansToDegrees(float radians)
{
    return static_cast<float>((static_cast<double>(radians) * 180.0) / PiNumber);
}

inline bool MathTools::almostEqual(const Vector3 &v1, const Vector3 &v2, double absoluteEpsilon, double relativeEpsilon)
{
    return almostEqual(v1.x, v2.x, absoluteEpsilon, relativeEpsilon) &&
            almostEqual(v1.y, v2.y, absoluteEpsilon, relativeEpsilon) &&
            almostEqual(v1.z, v2.z, absoluteEpsilon, relativeEpsilon);
}

} // End namespace udg

#endif
