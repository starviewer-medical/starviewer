/***************************************************************************
 *   Copyright (C) 2008 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html?langu=uk                           *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "obscurancethread.h"

#include <QLinkedList>
#include <QPair>
#include <QStack>

#include <vtkDirectionEncoder.h>
#include <vtkEncodedGradientEstimator.h>

#include "logging.h"
#include "mathtools.h"


namespace udg {


ObscuranceThread::ObscuranceThread( int id, int numberOfThreads, const TransferFunction & transferFunction, QObject * parent )
    : QThread(parent),
      m_id( id ), m_numberOfThreads( numberOfThreads ),
      m_transferFunction( transferFunction ),
      m_obscurance( 0 ), m_colorBleeding( 0 ), m_saliency( 0 )
{
}


ObscuranceThread::~ObscuranceThread()
{
}


void ObscuranceThread::setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator )
{
    m_directionEncoder = gradientEstimator->GetDirectionEncoder();
    m_encodedNormals = gradientEstimator->GetEncodedNormals();
}


void ObscuranceThread::setData( const uchar * data, int dataSize, const int dimensions[3], const int increments[3] )
{
    m_data = data;
    m_dataSize = dataSize;
    m_dimensions = dimensions;
    m_increments = increments;
}


void ObscuranceThread::setObscuranceParameters( double obscuranceMaximumDistance, OptimalViewpointVolume::ObscuranceFunction obscuranceFunction, OptimalViewpointVolume::ObscuranceVariant obscuranceVariant, double * obscurance, Vector3 * colorBleeding )
{
    m_obscuranceMaximumDistance = obscuranceMaximumDistance;
    m_obscuranceFunction = obscuranceFunction;
    m_obscuranceVariant = obscuranceVariant;
    m_obscurance = obscurance;
    m_colorBleeding = colorBleeding;
}


void ObscuranceThread::setSaliency( const double * saliency, double fxSaliencyA, double fxSaliencyB, double fxSaliencyLow, double fxSaliencyHigh )
{
    m_saliency = saliency;
    m_fxSaliencyA = fxSaliencyA;
    m_fxSaliencyB = fxSaliencyB;
    m_fxSaliencyLow = fxSaliencyLow;
    m_fxSaliencyHigh = fxSaliencyHigh;
}


void ObscuranceThread::setPerDirectionParameters( const Vector3 & direction, const Vector3 & forward, const int xyz[3], const int sXYZ[3], const QVector<Vector3> & lineStarts, qptrdiff startDelta )
{
    m_direction = direction;
    m_forward = forward;
    m_xyz = xyz;
    m_sXYZ = sXYZ;
    m_lineStarts = lineStarts;
    m_startDelta = startDelta;
}


void ObscuranceThread::run()
{
    DEBUG_LOG( QString( "%1: run()" ).arg( m_id ) );

    switch ( m_obscuranceVariant )
    {
        case OptimalViewpointVolume::Density: runDensity(); break;
        case OptimalViewpointVolume::DensitySmooth: runDensitySmooth(); break;
        case OptimalViewpointVolume::Opacity: runOpacity(); break;
        case OptimalViewpointVolume::OpacitySmooth: runOpacitySmooth(); break;
        case OptimalViewpointVolume::OpacitySaliency: runOpacitySaliency(); break;
        case OptimalViewpointVolume::OpacitySmoothSaliency: runOpacitySmoothSaliency(); break;
        case OptimalViewpointVolume::OpacityColorBleeding: runOpacityColorBleeding(); break;
        case OptimalViewpointVolume::OpacitySmoothColorBleeding: runOpacitySmoothColorBleeding(); break;
    }
}


void ObscuranceThread::runDensity() // optimitzat
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<uchar,Vector3> > unresolvedVoxels;

    const uchar * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            uchar value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );
                double cos = uNormal * m_direction;

                if ( cos < 0.0 )
                {
                    m_obscurance[uIndex] += -cos * obscurance( ( rv - ru ).length() );
                }
            }

            unresolvedVoxels.push( qMakePair( value, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runDensitySmooth()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<uchar,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<uchar,Vector3> > postponedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );
        Q_ASSERT( postponedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];

            QLinkedList< QPair<uchar,Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList< QPair<uchar,Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while ( itPostponedVoxels != itPostponedVoxelsEnd )
            {
                if ( itPostponedVoxels->first <= value )
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // not blocking -> advance to the next
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // blocking
                    double cos = uNormal * m_direction;
                    if ( cos < 0.0 )
                    {
                        m_obscurance[uIndex] += -cos * obscurance( distance );
                    }

                    itPostponedVoxels = postponedVoxels.erase( itPostponedVoxels );
                }
                else ++itPostponedVoxels;
            }

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value )
            {
                QPair<uchar,Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                double distance = ( rv - ru ).length();

                if ( distance <= 3.0 )
                {
                    // tangent plane at u
                    Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // distance from v to tangent plane at u
                    double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                    if ( D <= 1.5 ) // add u to postponed list
                    {
                        postponedVoxels.append( uPair );
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if ( cos < 0.0 )
                {
                    m_obscurance[uIndex] += -cos * obscurance( distance );
                }
            }

            unresolvedVoxels.push( qMakePair( value, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !postponedVoxels.isEmpty() )
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runOpacity()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<double,Vector3> > unresolvedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity( value );

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );
                double cos = uNormal * m_direction;

                if ( cos < 0.0 )
                {
                    double distance = ( rv - ru ).length();
                    m_obscurance[uIndex] += -cos * obscurance( distance );
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runOpacitySmooth()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<double,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<double,Vector3> > postponedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );
        Q_ASSERT( postponedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity( value );

            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while ( itPostponedVoxels != itPostponedVoxelsEnd )
            {
                if ( itPostponedVoxels->first <= opacity )
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // not blocking -> advance to the next
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // blocking
                    double cos = uNormal * m_direction;
                    if ( cos < 0.0 )
                    {
                        m_obscurance[uIndex] += -cos * obscurance( distance );
                    }

                    itPostponedVoxels = postponedVoxels.erase( itPostponedVoxels );
                }
                else ++itPostponedVoxels;
            }

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                QPair<double,Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                double distance = ( rv - ru ).length();

                if ( distance <= 3.0 )
                {
                    // tangent plane at u
                    Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // distance from v to tangent plane at u
                    double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                    if ( D <= 1.5 ) // add u to postponed list
                    {
                        postponedVoxels.append( uPair );
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if ( cos < 0.0 )
                {
                    m_obscurance[uIndex] += -cos * obscurance( distance );
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !postponedVoxels.isEmpty() )
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runOpacitySaliency()    // = runOpacity() (de moment)
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<double,Vector3> > unresolvedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity( value );

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );
                double cos = uNormal * m_direction;

                if ( cos < 0.0 )
                {
                    double distance = ( rv - ru ).length();
                    m_obscurance[uIndex] += -cos * obscurance( distance );
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runOpacitySmoothSaliency()
{
    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];
    double minFxSaliency = 1.0 - m_fxSaliencyA, maxFxSaliency = 1.0 + m_fxSaliencyB;

    QStack< QPair<double,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<double,Vector3> > postponedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );
        Q_ASSERT( postponedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            int vIndex = v.x * incX + v.y * incY + v.z * incZ;  // índex de v (sense el delta)
            unsigned char value = dataPtr[vIndex];
            double opacity = m_transferFunction.getOpacity( value );
            double fxSaliency = 1.0 + m_saliency[m_startDelta + vIndex] * ( m_fxSaliencyA + m_fxSaliencyB ) - m_fxSaliencyA;
            if ( fxSaliency < m_fxSaliencyLow ) fxSaliency = minFxSaliency;
            else if ( fxSaliency > m_fxSaliencyHigh ) fxSaliency = maxFxSaliency;
            opacity *= fxSaliency;

            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while ( itPostponedVoxels != itPostponedVoxelsEnd )
            {
                if ( itPostponedVoxels->first <= opacity )
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // not blocking -> advance to the next
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // blocking
                    double cos = uNormal * m_direction;
                    if ( cos < 0.0 )
                    {
                        m_obscurance[uIndex] += -cos * obscurance( distance );
                    }

                    itPostponedVoxels = postponedVoxels.erase( itPostponedVoxels );
                }
                else ++itPostponedVoxels;
            }

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                QPair<double,Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                double distance = ( rv - ru ).length();

                if ( distance <= 3.0 )
                {
                    // tangent plane at u
                    Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // distance from v to tangent plane at u
                    double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                    if ( D <= 1.5 ) // add u to postponed list
                    {
                        postponedVoxels.append( uPair );
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if ( cos < 0.0 )
                {
                    m_obscurance[uIndex] += -cos * obscurance( distance );
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !postponedVoxels.isEmpty() )
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_obscurance[uIndex] -= cos;
            }
        }
    }
}


void ObscuranceThread::runOpacityColorBleeding()    /// \todo encara és smooth
{
    const Vector3 AMBIENT_COLOR( 1.0, 1.0, 1.0 );

    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<double,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<double,Vector3> > postponedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );
        Q_ASSERT( postponedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity( value );
            QColor vColor = m_transferFunction.getColor( value );
            Vector3 vColorVector( vColor.redF(), vColor.greenF(), vColor.blueF() );

            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while ( itPostponedVoxels != itPostponedVoxelsEnd )
            {
                if ( itPostponedVoxels->first <= opacity )
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // not blocking -> advance to the next
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // blocking
                    double cos = uNormal * m_direction;
                    if ( cos < 0.0 )
                    {
                        m_colorBleeding[uIndex] += -cos * obscurance( distance ) * vColorVector;
                    }

                    itPostponedVoxels = postponedVoxels.erase( itPostponedVoxels );
                }
                else ++itPostponedVoxels;
            }

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                QPair<double,Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                double distance = ( rv - ru ).length();

                if ( distance <= 3.0 )
                {
                    // tangent plane at u
                    Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // distance from v to tangent plane at u
                    double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                    if ( D <= 1.5 ) // add u to postponed list
                    {
                        postponedVoxels.append( uPair );
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if ( cos < 0.0 )
                {
                    m_colorBleeding[uIndex] += -cos * obscurance( distance ) * vColorVector;
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !postponedVoxels.isEmpty() )
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_colorBleeding[uIndex] += -cos * AMBIENT_COLOR;
            }
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_colorBleeding[uIndex] += -cos * AMBIENT_COLOR;
            }
        }
    }
}


void ObscuranceThread::runOpacitySmoothColorBleeding()
{
    const Vector3 AMBIENT_COLOR( 1.0, 1.0, 1.0 );

    int x = m_xyz[0], y = m_xyz[1], z = m_xyz[2];
    int sX = m_sXYZ[0], sY = m_sXYZ[1], sZ = m_sXYZ[2];
    int dimX = m_dimensions[x], dimY = m_dimensions[y], dimZ = m_dimensions[z];
    int incX = sX * m_increments[x], incY = sY * m_increments[y], incZ = sZ * m_increments[z];

    QStack< QPair<double,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<double,Vector3> > postponedVoxels;

    const unsigned char * dataPtr = m_data + m_startDelta;
    int nLineStarts = m_lineStarts.size();

    // iterar per cada línia
    for ( int j = m_id; j < nLineStarts; j += m_numberOfThreads )
    {
        Vector3 rv = m_lineStarts.at( j );
        Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
        Q_ASSERT( unresolvedVoxels.isEmpty() );
        Q_ASSERT( postponedVoxels.isEmpty() );

        // iterar per la línia
        while ( v.x < dimX && v.y < dimY && v.z < dimZ )
        {
            // tractar el vòxel
            unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
            double opacity = m_transferFunction.getOpacity( value );
            QColor vColor = m_transferFunction.getColor( value );
            Vector3 vColorVector( vColor.redF(), vColor.greenF(), vColor.blueF() );

            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxels = postponedVoxels.begin();
            QLinkedList< QPair<double,Vector3> >::iterator itPostponedVoxelsEnd = postponedVoxels.end();

            while ( itPostponedVoxels != itPostponedVoxelsEnd )
            {
                if ( itPostponedVoxels->first <= opacity )
                {
                    Vector3 ru = itPostponedVoxels->second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                        double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // not blocking -> advance to the next
                        {
                            ++itPostponedVoxels;
                            continue;
                        }
                    }

                    // blocking
                    double cos = uNormal * m_direction;
                    if ( cos < 0.0 )
                    {
                        m_colorBleeding[uIndex] += -cos * obscurance( distance ) * vColorVector;
                    }

                    itPostponedVoxels = postponedVoxels.erase( itPostponedVoxels );
                }
                else ++itPostponedVoxels;
            }

            while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= opacity )
            {
                QPair<double,Vector3> uPair = unresolvedVoxels.pop();
                Vector3 ru = uPair.second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                double distance = ( rv - ru ).length();

                if ( distance <= 3.0 )
                {
                    // tangent plane at u
                    Vector3 uNormalLocal( sX * uGradient[x], sY * uGradient[y], sZ * uGradient[z] ); // normal en espai local (transformat)
                    double a = uNormalLocal.x, b = uNormalLocal.y, c = uNormalLocal.z, d = -uNormalLocal * ru;
                    // distance from v to tangent plane at u
                    double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                    if ( D <= 1.5 ) // add u to postponed list
                    {
                        postponedVoxels.append( uPair );
                        continue;
                    }
                }

                double cos = uNormal * m_direction;
                if ( cos < 0.0 )
                {
                    m_colorBleeding[uIndex] += -cos * obscurance( distance ) * vColorVector;
                }
            }

            unresolvedVoxels.push( qMakePair( opacity, rv ) );

            // avançar el vòxel
            rv += m_forward;
            v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
        }

        while ( !postponedVoxels.isEmpty() )
        {
            Vector3 ru = postponedVoxels.takeFirst().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_colorBleeding[uIndex] += -cos * AMBIENT_COLOR;
            }
        }

        while ( !unresolvedVoxels.isEmpty() )
        {
            Vector3 ru = unresolvedVoxels.pop().second;
            Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

            int uIndex = m_startDelta + u.x * incX + u.y * incY + u.z * incZ;
            float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
            Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

            double cos = uNormal * m_direction;
            if ( cos < 0.0 )
            {
                m_colorBleeding[uIndex] += -cos * AMBIENT_COLOR;
            }
        }
    }
}


inline double ObscuranceThread::obscurance( double distance ) const
{
    const double EXP_NORM = 1.0 - exp( -1.0 );

    if ( distance > m_obscuranceMaximumDistance ) return 1.0;

    switch ( m_obscuranceFunction )
    {
        case OptimalViewpointVolume::Constant0: return 0.0;
        case OptimalViewpointVolume::Distance: return distance / m_obscuranceMaximumDistance;
        case OptimalViewpointVolume::SquareRoot: return sqrt( distance / m_obscuranceMaximumDistance );
        case OptimalViewpointVolume::Exponential: return 1.0 - exp( -distance / m_obscuranceMaximumDistance );
        case OptimalViewpointVolume::ExponentialNorm: return ( 1.0 - exp( -distance / m_obscuranceMaximumDistance ) ) / EXP_NORM;
        case OptimalViewpointVolume::CubeRoot: return MathTools::cbrt( distance / m_obscuranceMaximumDistance );
    }
}


}
