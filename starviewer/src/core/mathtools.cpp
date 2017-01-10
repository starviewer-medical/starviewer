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

#include "mathtools.h"
#include "logging.h"
#include "vector3.h"

#include <cmath>
#include <ctime>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <vtkLine.h>

#include <QVector2D>
#include <QVector3D>

namespace udg {

/// Definició de constants
// log_2 e
const double MathTools::NumberEBase2Logarithm = 1.4426950408889634074;
// 1/pi
const long double MathTools::ReversePiNumberLong = 0.3183098861837906715377675267450287L;
// pi
const double MathTools::PiNumber = 3.14159265358979323846;
// pi
const long double MathTools::PiNumberLong = 3.14159265358979323846;
// pi/2
const long double MathTools::PiNumberDivBy2Long = 1.5707963267948966192313216916397514L;
const double MathTools::Epsilon = 1E-8;
const double MathTools::DegreesToRadiansAsDouble = 0.017453292519943295;
const double MathTools::RadiansToDegreesAsDouble = 57.29577951308232;
// TODO Potser seria més conevnient fer servir std::numeric_limits<double>::max(). Caldria incloure <limits>
const double MathTools::DoubleMaximumValue = VTK_DOUBLE_MAX;

double MathTools::logTwo(const double x, const bool zero)
{
    if (x < 0)
    {
        WARN_LOG("MathTools::logTwo >> Log of negative number");
    }

    if (zero)
    {
        return (x == 0) ? 0 : double(log(double(x))) * NumberEBase2Logarithm;
    }
    else
    {
        if (x == 0)
        {
            WARN_LOG("MathTools::logTwo >> Log of zero");
        }

        return double(log(double(x))) * NumberEBase2Logarithm;
    }
}

double MathTools::angleInRadians(const QVector2D &vector)
{
    return atan2(vector.y(), vector.x());
}

double MathTools::angleInDegrees(const QVector2D &vector)
{
    return angleInRadians(vector) * MathTools::RadiansToDegreesAsDouble;
}

double MathTools::angleInRadians(const Vector3 &vec1, const Vector3 &vec2)
{
    return acos(Vector3::dot(vec1, vec2) / (vec1.length() * vec2.length()));
}

double MathTools::angleInDegrees(const Vector3 &vec1, const Vector3 &vec2)
{
    return angleInRadians(vec1, vec2) * MathTools::RadiansToDegreesAsDouble;
}

int MathTools::planeIntersection(double p[3], double n[3], double q[3], double m[3], double r[3], double t[3])
{
    if (angleInDegrees(Vector3(n[0], n[1], n[2]), Vector3(m[0], m[1], m[2])) == 0.0)
    {
        return 0;
    }
    // Solució extreta de http://vis.eng.uci.edu/courses/eecs104/current/GraphicsMath.pdf, pàg. 64
    // pla1 definit per (p,n); p: punt del pla, p.ex. origen; n: normal
    // pla2 definit per (q,m); q: punt del pla, p.ex. origen; m: normal
    // línia d'intersecció (r,t); r: punt de la recta que pertany a tots dos plans; t: vector director
    // u: vector perpendicular a n i t;
    // Cross: producte vectorial
    // Dot: producte escalar
    // * : multiplicació de vectors
    // + : suma de vectors
    //
    // ******* FORMULETA *************
    //
    // t = Cross(n,m)
    // u = Cross(n,t)
    // r = p + Dot(p-q)*u / Dot(u,m)

    double u[3];
    MathTools::crossProduct(n, m, t);
    MathTools::crossProduct(n, t, u);

    double pq[3], sum[3], pqDotm, dot_u_m;

    pq[0] = q[0] - p[0];
    pq[1] = q[1] - p[1];
    pq[2] = q[2] - p[2];

    pqDotm = MathTools::dotProduct(pq, m);
    dot_u_m = MathTools::dotProduct(u, m);
    sum[0] = (pqDotm * u[0]) / dot_u_m;
    sum[1] = (pqDotm * u[1]) / dot_u_m;
    sum[2] = (pqDotm * u[2]) / dot_u_m;

    r[0] = sum[0] + p[0];
    r[1] = sum[1] + p[1];
    r[2] = sum[2] + p[2];

    return 1;
}

Vector3 MathTools::directorVector(const Vector3 &point1, const Vector3 &point2)
{
    return point2 - point1;
}

double MathTools::modulus(double vector[3])
{
    return sqrt(pow(vector[0], 2) + pow(vector[1], 2) + pow(vector[2], 2));
}

double MathTools::dotProduct(double vector1[3], double vector2[3])
{
    return ((vector1[0] * vector2[0]) + (vector1[1] * vector2[1]) + (vector1[2] * vector2[2]));
}

void MathTools::crossProduct(double vectorDirector1[3], double vectorDirector2[3], double crossProductVector[3])
{
    crossProductVector[0] = vectorDirector1[1] * vectorDirector2[2] - vectorDirector1[2] * vectorDirector2[1];
    crossProductVector[1] = vectorDirector1[2] * vectorDirector2[0] - vectorDirector1[0] * vectorDirector2[2];
    crossProductVector[2] = vectorDirector1[0] * vectorDirector2[1] - vectorDirector1[1] * vectorDirector2[0];
}

double MathTools::normalize(double vector[3])
{
    return vtkMath::Normalize(vector);
}

bool MathTools::isOdd(int x)
{
    return (x % 2);
}

bool MathTools::isEven(int x)
{
    return !isOdd(x);
}

double MathTools::cubeRoot(double x)
{
    return std::pow(x, 1.0 / 3.0);
}

double MathTools::getDistance3D(const Vector3 &firstPoint, const Vector3 &secondPoint)
{
    return (firstPoint - secondPoint).length();
}

double MathTools::randomDouble(double minimum, double maximum)
{
    // Initializes random seed if necessary
    MathTools::initializeRandomSeed();
    // Generation of a number betweeen 0 and 1
    double r = (double)qrand() / RAND_MAX;
    // Returning a random number between the range indicated
    return minimum + r * (maximum - minimum);
}

int MathTools::randomInt(int minimum, int maximum)
{
    // Initializes random seed if necessary
    MathTools::initializeRandomSeed();
    
    // Random number between minimum and maximum
    return qrand() % ((maximum + 1) - minimum) + minimum;
}

double MathTools::getPointToClosestEdgeDistance(const Vector3 &point3D, const QList<Vector3> &pointsList, bool lastToFirstEdge, Vector3 &closestPoint,
                                                int &closestEdge)
{
    double minimumDistanceFound = MathTools::DoubleMaximumValue;

    if (!pointsList.isEmpty())
    {
        // Recorrem tots els punts del polígon calculant la distància a cadascun dels
        // segments que uneixen cada vèrtex
        double distance;
        Vector3 localClosestPoint;
        int i = 0;
        while (i < pointsList.count() - 1)
        {
            auto startPoint = pointsList.at(i);
            auto endPoint = pointsList.at(i + 1);
            distance = MathTools::getPointToFiniteLineDistance(point3D, startPoint, endPoint, localClosestPoint);
            if (distance < minimumDistanceFound)
            {
                minimumDistanceFound = distance;
                closestPoint = localClosestPoint;
                closestEdge = i;
            }

            ++i;
        }

        if (lastToFirstEdge)
        {
            // Calculem la distància del segment que va de l'últim al primer punt
            auto startPoint = pointsList.first();
            auto endPoint = pointsList.last();
            distance = MathTools::getPointToFiniteLineDistance(point3D, startPoint, endPoint, localClosestPoint);
            if (distance < minimumDistanceFound)
            {
                minimumDistanceFound = distance;
                closestPoint = localClosestPoint;
                closestEdge = i;
            }
        }
    }

    return minimumDistanceFound;
}

double MathTools::getPointToFiniteLineDistance(const Vector3 &point, const Vector3 &lineFirstPoint, const Vector3 &lineSecondPoint, Vector3 &closestPoint)
{
    double parametricCoordinate;

    // vtkLine::DistanceToLine() ens retorna la distància al quadrat, per això fem sqrt()
    return sqrt(vtkLine::DistanceToLine(point.toArray().data(), lineFirstPoint.toArray().data(), lineSecondPoint.toArray().data(), parametricCoordinate,
                                        closestPoint.data()));
}

Vector3 MathTools::infiniteLinesIntersection(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Vector3 &p4, int &state)
{
    //  Solution by Wolfram Mathematics
    //
    //   http://mathworld.wolfram.com/Line-LineIntersection.html
    //
    Vector3 intersection;

    // Line 1: x = x1 + (x2 - x1)s
    // Line 2: x = x3 + (x4 - x3)t
    double s;
    // Director vectors for each line
    Vector3 dv1, dv2, dv3;
    dv1 = p2 - p1;
    dv2 = p4 - p3;
    dv3 = p3 - p1;

    // Coplanarity test
    Vector3 cross = Vector3::cross(dv1, dv2);
    double dot = Vector3::dot(dv3, cross);

    // Coplanarity check
    if (MathTools::closeEnough(dot, 0.0))
    {
        Vector3 numerator1 = Vector3::cross(dv3, dv2);
        Vector3 numerator2 = Vector3::cross(dv1, dv2);

        double numerator = Vector3::dot(numerator1, numerator2);

        Vector3 denominator1 = Vector3::cross(dv1, dv2);

        double denominator = denominator1.lengthSquared();

        if (MathTools::closeEnough(denominator, 0.0))
        {
            state = ParallelLines;
            return intersection;
        }
        else
        {
            s = numerator / denominator;

            intersection = p1 + (s * dv1);

            state = LinesIntersect;
            return intersection;
        }
    }
    // Skew Lines
    else
    {
        state = SkewIntersection;
        return intersection;
    }

}

double MathTools::truncate(double x)
{
    return x > 0.0 ? std::floor(x) : std::ceil(x);
}

int MathTools::roundToNearestInteger(double x)
{
    return vtkMath::Round(x);
}

bool MathTools::closeEnough(double f1, double f2)
{
    return std::abs((f1 - f2) / ((f2 == 0.0) ? 1.0 : f2)) < Epsilon;
}

float MathTools::degreesToRadians(float degrees)
{
    return (degrees * PiNumber) / 180.0f;
}

float MathTools::radiansToDegrees(float radians)
{
    return (radians * 180.0f) / PiNumber;
}

bool MathTools::isNaN(double x)
{
    return x != x;
}

unsigned int MathTools::roundUpToPowerOf2(unsigned int v)
{
    // Font: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    // Si v == 0 el codi de sobre retorna 0. Ho solucionem amb aquesta suma.
    v += (v == 0);
    return v;
}

unsigned int MathTools::roundUpToMultipleOfNumber(unsigned int i, unsigned int multiple)
{
    if (i % multiple != 0)
    {
        i += multiple - i % multiple;
    }
    return i;
}

double MathTools::copySign(double x, double y)
{
    if ((x >= +0.0 && y >= +0.0) || (x <= -0.0 && y <= -0.0))
    {
        return x;
    }
    else
    {
        return -x;
    }
}

void MathTools::initializeRandomSeed()
{
    static bool seedInit = false;
    if (!seedInit)
    {
        // If random number generation has not been seeded, we initialize it
        qsrand((unsigned)std::time(NULL));
        seedInit = true;
    }
}

}; // End namespace udg
