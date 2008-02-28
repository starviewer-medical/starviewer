/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "obscurancethread.h"

#include <QPair>
#include <QStack>
#include <vtkDirectionEncoder.h>
#include "logging.h"
#include "vector3.h"
#include <QLinkedList>

namespace udg {

ObscuranceThread::ObscuranceThread( unsigned char id, unsigned char numberOfThreads, const QVector<Vector3> & directions, const TransferFunction & transferFunction, QObject * parent )
    : QThread(parent),
      m_id( id ), m_numberOfThreads( numberOfThreads ),
      m_directions( directions ),
      m_transferFunction( transferFunction ),
      m_obscurance( 0 )
{
}


ObscuranceThread::~ObscuranceThread()
{
    delete [] m_obscurance;
}


void ObscuranceThread::setNormals( vtkDirectionEncoder * directionEncoder, const unsigned short * encodedNormals )
{
    m_directionEncoder = directionEncoder;
    m_encodedNormals = encodedNormals;
}


void ObscuranceThread::setData( const unsigned char * data, int dataSize, const int dimensions[3], const int increments[3] )
{
    m_data = data;
    m_dataSize = dataSize;
    m_dimensions = dimensions;
    m_increments = increments;
}


void ObscuranceThread::setObscuranceParameters( double obscuranceMaximumDistance, OptimalViewpointVolume::ObscuranceFunction obscuranceFunction, OptimalViewpointVolume::ObscuranceVariant obscuranceVariant )
{
    m_obscuranceMaximumDistance = obscuranceMaximumDistance;
    m_obscuranceFunction = obscuranceFunction;
    m_obscuranceVariant = obscuranceVariant;
}


double * ObscuranceThread::getObscurance() const
{
    return m_obscurance;
}


void ObscuranceThread::run()
{
    switch ( m_obscuranceVariant )
    {
        case OptimalViewpointVolume::Density: runDensity(); break;
        case OptimalViewpointVolume::DensitySmooth: runDensitySmooth(); break;
        case OptimalViewpointVolume::Opacity: runOpacity(); break;
        case OptimalViewpointVolume::OpacitySmooth: runOpacitySmooth(); break;
    }
}


void ObscuranceThread::runDensity()
{
    delete [] m_obscurance;
    m_obscurance = new double[m_dataSize];
    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] = 0.0;

    // preparem alguns objectes i variables que reutilitzarem per eficiència
    int nDirections = m_directions.size();
    QVector<Vector3> lineStarts;
    QStack< QPair<uchar,Vector3> > unresolvedVoxels;

    // iterem per les direccions
    for ( unsigned short i = m_id; i < nDirections; i += m_numberOfThreads )
    {
        const Vector3 & direction = m_directions.at( i );

        DEBUG_LOG( QString( "Direcció " ) + direction.toString() );

        // direcció dominant (0 = x, 1 = y, 2 = z)
        int dominant;
        Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
        if ( absDirection.x >= absDirection.y )
        {
            if ( absDirection.x >= absDirection.z ) dominant = 0;
            else dominant = 2;
        }
        else
        {
            if ( absDirection.y >= absDirection.z ) dominant = 1;
            else dominant = 2;
        }

        // vector per avançar
        Vector3 forward;
        switch ( dominant )
        {
            case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
            case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
            case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
        }
        forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );

        // dimensions i increments segons la direcció dominant
        int dimX = m_dimensions[dominant], dimY = m_dimensions[(dominant+1)%3], dimZ = m_dimensions[(dominant+2)%3];
        int incX = m_increments[dominant], incY = m_increments[(dominant+1)%3], incZ = m_increments[(dominant+2)%3];
        qptrdiff startDelta = 0;
        if ( forward.x < 0.0 )
        {
            startDelta += incX * ( dimX - 1 );
            incX = -incX;
            forward.x = -forward.x;
        }
        if ( forward.y < 0.0 )
        {
            startDelta += incY * ( dimY - 1 );
            incY = -incY;
            forward.y = -forward.y;
        }
        if ( forward.z < 0.0 )
        {
            startDelta += incZ * ( dimZ - 1 );
            incZ = -incZ;
            forward.z = -forward.z;
        }
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );
        // ara els 3 components són positius

        // llista dels vòxels que són començament de línia
        getLineStarts( lineStarts, dimX, dimY, dimZ, forward );

        const unsigned char * dataPtr = m_data + startDelta;
        int nLineStarts = lineStarts.size();

        // iterar per cada línia
        for ( int j = 0; j < nLineStarts; ++j )
        {
            Vector3 rv = lineStarts.at( j );
            Voxel v = { qRound( rv.x ), qRound( rv.y ), qRound( rv.z ) };
            Q_ASSERT( unresolvedVoxels.isEmpty() );

            // iterar per la línia
            while ( v.x < dimX && v.y < dimY && v.z < dimZ )
            {
                // tractar el vòxel
                unsigned char value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];

                while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value )
                {
                    Vector3 ru = unresolvedVoxels.pop().second;
                    Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                    int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    if ( uNormal * direction < 0.0 )
                    {
                        double distance = ( rv - ru ).length();
//                         Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                         double distance = ( dv - du ).length();
                        m_obscurance[uIndex] += obscurance( distance );
                    }
                }

                unresolvedVoxels.push( qMakePair( value, rv ) );

                // avançar el vòxel
                rv += forward;
                v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
            }

            while ( !unresolvedVoxels.isEmpty() )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }
        }
    }
}


void ObscuranceThread::runDensitySmooth()
{
    delete [] m_obscurance;
    m_obscurance = new double[m_dataSize];
    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] = 0.0;

    // preparem alguns objectes i variables que reutilitzarem per eficiència
    int nDirections = m_directions.size();
    QVector<Vector3> lineStarts;
    QStack< QPair<uchar,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<uchar,Vector3> > postponedVoxels;

    // iterem per les direccions
    for ( unsigned short i = m_id; i < nDirections; i += m_numberOfThreads )
    {
        const Vector3 & direction = m_directions.at( i );

        DEBUG_LOG( QString( "Direcció " ) + direction.toString() );

        // direcció dominant (0 = x, 1 = y, 2 = z)
        int dominant;
        Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
        if ( absDirection.x >= absDirection.y )
        {
            if ( absDirection.x >= absDirection.z ) dominant = 0;
            else dominant = 2;
        }
        else
        {
            if ( absDirection.y >= absDirection.z ) dominant = 1;
            else dominant = 2;
        }

        // vector per avançar
        Vector3 forward;
        switch ( dominant )
        {
            case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
            case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
            case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
        }
        forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );

        // dimensions i increments segons la direcció dominant
        int dimX = m_dimensions[dominant], dimY = m_dimensions[(dominant+1)%3], dimZ = m_dimensions[(dominant+2)%3];
        int incX = m_increments[dominant], incY = m_increments[(dominant+1)%3], incZ = m_increments[(dominant+2)%3];
        qptrdiff startDelta = 0;
        if ( forward.x < 0.0 )
        {
            startDelta += incX * ( dimX - 1 );
            incX = -incX;
            forward.x = -forward.x;
        }
        if ( forward.y < 0.0 )
        {
            startDelta += incY * ( dimY - 1 );
            incY = -incY;
            forward.y = -forward.y;
        }
        if ( forward.z < 0.0 )
        {
            startDelta += incZ * ( dimZ - 1 );
            incZ = -incZ;
            forward.z = -forward.z;
        }
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );
        // ara els 3 components són positius

        // llista dels vòxels que són començament de línia
        getLineStarts( lineStarts, dimX, dimY, dimZ, forward );

        const unsigned char * dataPtr = m_data + startDelta;
        int nLineStarts = lineStarts.size();

        // iterar per cada línia
        for ( int j = 0; j < nLineStarts; ++j )
        {
            Vector3 rv = lineStarts.at( j );
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

                        int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                        float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                        Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                        double distance = ( rv - ru ).length();

                        if ( distance <= 3.0 )
                        {
                            // tangent plane at u
                            double a = uNormal.x, b = uNormal.y, c = uNormal.z, d = -uNormal * ru;
                            // distance from v to tangent plane at u
                            double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                            if ( D <= 1.5 ) // not blocking -> advance to the next
                            {
                                ++itPostponedVoxels;
                                continue;
                            }
                        }

                        // blocking
                        if ( uNormal * direction < 0.0 )
                        {
//                             Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                             double distance = ( dv - du ).length();
                            m_obscurance[uIndex] += obscurance( distance );
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

                    int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        double a = uNormal.x, b = uNormal.y, c = uNormal.z, d = -uNormal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // add u to postponed list
                        {
                            postponedVoxels.append( uPair );
                        }
                    }

                    if ( uNormal * direction < 0.0 )
                    {
//                         Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                         double distance = ( dv - du ).length();
                        m_obscurance[uIndex] += obscurance( distance );
                    }
                }

                unresolvedVoxels.push( qMakePair( value, rv ) );

                // avançar el vòxel
                rv += forward;
                v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
            }

            while ( !postponedVoxels.isEmpty() )
            {
                Vector3 ru = postponedVoxels.takeFirst().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }

            while ( !unresolvedVoxels.isEmpty() )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }
        }
    }
}


void ObscuranceThread::runOpacity()
{
    delete [] m_obscurance;
    m_obscurance = new double[m_dataSize];
    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] = 0.0;

    // preparem alguns objectes i variables que reutilitzarem per eficiència
    int nDirections = m_directions.size();
    QVector<Vector3> lineStarts;
    QStack< QPair<double,Vector3> > unresolvedVoxels;

    // iterem per les direccions
    for ( unsigned short i = m_id; i < nDirections; i += m_numberOfThreads )
    {
        const Vector3 & direction = m_directions.at( i );

        DEBUG_LOG( QString( "Direcció " ) + direction.toString() );

        // direcció dominant (0 = x, 1 = y, 2 = z)
        int dominant;
        Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
        if ( absDirection.x >= absDirection.y )
        {
            if ( absDirection.x >= absDirection.z ) dominant = 0;
            else dominant = 2;
        }
        else
        {
            if ( absDirection.y >= absDirection.z ) dominant = 1;
            else dominant = 2;
        }

        // vector per avançar
        Vector3 forward;
        switch ( dominant )
        {
            case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
            case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
            case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
        }
        forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );

        // dimensions i increments segons la direcció dominant
        int dimX = m_dimensions[dominant], dimY = m_dimensions[(dominant+1)%3], dimZ = m_dimensions[(dominant+2)%3];
        int incX = m_increments[dominant], incY = m_increments[(dominant+1)%3], incZ = m_increments[(dominant+2)%3];
        qptrdiff startDelta = 0;
        if ( forward.x < 0.0 )
        {
            startDelta += incX * ( dimX - 1 );
            incX = -incX;
            forward.x = -forward.x;
        }
        if ( forward.y < 0.0 )
        {
            startDelta += incY * ( dimY - 1 );
            incY = -incY;
            forward.y = -forward.y;
        }
        if ( forward.z < 0.0 )
        {
            startDelta += incZ * ( dimZ - 1 );
            incZ = -incZ;
            forward.z = -forward.z;
        }
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );
        // ara els 3 components són positius

        // llista dels vòxels que són començament de línia
        getLineStarts( lineStarts, dimX, dimY, dimZ, forward );

        const unsigned char * dataPtr = m_data + startDelta;
        int nLineStarts = lineStarts.size();

        // iterar per cada línia
        for ( int j = 0; j < nLineStarts; ++j )
        {
            Vector3 rv = lineStarts.at( j );
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

                    int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    if ( uNormal * direction < 0.0 )
                    {
                        double distance = ( rv - ru ).length();
//                         Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                         double distance = ( dv - du ).length();
                        m_obscurance[uIndex] += obscurance( distance );
                    }
                }

                unresolvedVoxels.push( qMakePair( opacity, rv ) );

                // avançar el vòxel
                rv += forward;
                v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
            }

            while ( !unresolvedVoxels.isEmpty() )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }
        }
    }
}


void ObscuranceThread::runOpacitySmooth()
{
    delete [] m_obscurance;
    m_obscurance = new double[m_dataSize];
    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] = 0.0;

    // preparem alguns objectes i variables que reutilitzarem per eficiència
    int nDirections = m_directions.size();
    QVector<Vector3> lineStarts;
    QStack< QPair<double,Vector3> > unresolvedVoxels;
    QLinkedList< QPair<double,Vector3> > postponedVoxels;

    // iterem per les direccions
    for ( unsigned short i = m_id; i < nDirections; i += m_numberOfThreads )
    {
        const Vector3 & direction = m_directions.at( i );

        DEBUG_LOG( QString( "Direcció " ) + direction.toString() );

        // direcció dominant (0 = x, 1 = y, 2 = z)
        int dominant;
        Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
        if ( absDirection.x >= absDirection.y )
        {
            if ( absDirection.x >= absDirection.z ) dominant = 0;
            else dominant = 2;
        }
        else
        {
            if ( absDirection.y >= absDirection.z ) dominant = 1;
            else dominant = 2;
        }

        // vector per avançar
        Vector3 forward;
        switch ( dominant )
        {
            case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
            case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
            case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
        }
        forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );

        // dimensions i increments segons la direcció dominant
        int dimX = m_dimensions[dominant], dimY = m_dimensions[(dominant+1)%3], dimZ = m_dimensions[(dominant+2)%3];
        int incX = m_increments[dominant], incY = m_increments[(dominant+1)%3], incZ = m_increments[(dominant+2)%3];
        qptrdiff startDelta = 0;
        if ( forward.x < 0.0 )
        {
            startDelta += incX * ( dimX - 1 );
            incX = -incX;
            forward.x = -forward.x;
        }
        if ( forward.y < 0.0 )
        {
            startDelta += incY * ( dimY - 1 );
            incY = -incY;
            forward.y = -forward.y;
        }
        if ( forward.z < 0.0 )
        {
            startDelta += incZ * ( dimZ - 1 );
            incZ = -incZ;
            forward.z = -forward.z;
        }
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );
        // ara els 3 components són positius

        // llista dels vòxels que són començament de línia
        getLineStarts( lineStarts, dimX, dimY, dimZ, forward );

        const unsigned char * dataPtr = m_data + startDelta;
        int nLineStarts = lineStarts.size();

        // iterar per cada línia
        for ( int j = 0; j < nLineStarts; ++j )
        {
            Vector3 rv = lineStarts.at( j );
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

                        int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                        float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                        Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                        double distance = ( rv - ru ).length();

                        if ( distance <= 3.0 )
                        {
                            // tangent plane at u
                            double a = uNormal.x, b = uNormal.y, c = uNormal.z, d = -uNormal * ru;
                            // distance from v to tangent plane at u
                            double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                            if ( D <= 1.5 ) // not blocking -> advance to the next
                            {
                                ++itPostponedVoxels;
                                continue;
                            }
                        }

                        // blocking
                        if ( uNormal * direction < 0.0 )
                        {
//                             Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                             double distance = ( dv - du ).length();
                            m_obscurance[uIndex] += obscurance( distance );
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

                    int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                    float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                    Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                    double distance = ( rv - ru ).length();

                    if ( distance <= 3.0 )
                    {
                        // tangent plane at u
                        double a = uNormal.x, b = uNormal.y, c = uNormal.z, d = -uNormal * ru;
                        // distance from v to tangent plane at u
                        double D = qAbs( a * rv.x + b * rv.y + c * rv.z + d );

                        if ( D <= 1.5 ) // add u to postponed list
                        {
                            postponedVoxels.append( uPair );
                        }
                    }

                    if ( uNormal * direction < 0.0 )
                    {
//                         Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
//                         double distance = ( dv - du ).length();
                        m_obscurance[uIndex] += obscurance( distance );
                    }
                }

                unresolvedVoxels.push( qMakePair( opacity, rv ) );

                // avançar el vòxel
                rv += forward;
                v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
            }

            while ( !postponedVoxels.isEmpty() )
            {
                Vector3 ru = postponedVoxels.takeFirst().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }

            while ( !unresolvedVoxels.isEmpty() )
            {
                Vector3 ru = unresolvedVoxels.pop().second;
                Voxel u = { qRound( ru.x ), qRound( ru.y ), qRound( ru.z ) };

                int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
                float * uGradient = m_directionEncoder->GetDecodedGradient( m_encodedNormals[uIndex] );
                Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );

                if ( uNormal * direction < 0.0 )
                {
                    m_obscurance[uIndex]++;
                }
            }
        }
    }
}


void ObscuranceThread::getLineStarts( QVector<Vector3> & lineStarts, int dimX, int dimY, int dimZ, const Vector3 & forward ) const
{
    lineStarts.resize( 0 );

    // tots els (0,y,z) són començament de línia
    Vector3 lineStart;  // (0,0,0)
    for ( int iy = 0; iy < dimY; ++iy )
    {
        lineStart.y = iy;
        for ( int iz = 0; iz < dimZ; ++iz )
        {
            lineStart.z = iz;
            lineStarts << lineStart;
//             DEBUG_LOG( QString( "line start: (%1,%2,%3)" ).arg( lineStart.x ).arg( lineStart.y ).arg( lineStart.z ) );
        }
    }
    DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.size() ) );

    // més començaments de línia
    Vector3 rv; // (0,0,0)
    Voxel v = { 0, 0, 0 }, pv = v;

    // iterar per la línia que comença a (0,0,0)
    while ( v.x < dimX )
    {
        if ( v.y != pv.y )
        {
            lineStart.x = rv.x; lineStart.y = rv.y - v.y;   // [y] = 0
            for ( double iz = rv.z - v.z; iz < dimZ; iz++ )
            {
                lineStart.z = iz;
                lineStarts << lineStart;
            }
        }
        if ( v.z != pv.z )
        {
            lineStart.x = rv.x; lineStart.z = rv.z - v.z;   // [z] = 0
            for ( double iy = rv.y - v.y; iy < dimY; iy++ )
            {
                lineStart.y = iy;
                lineStarts << lineStart;
            }
        }

        // avançar el vòxel
        rv += forward;
        pv = v;
        v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
    }
    DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.size() ) );
}


inline double ObscuranceThread::obscurance( double distance ) const
{
    if ( distance > m_obscuranceMaximumDistance ) return 1.0;

    switch ( m_obscuranceFunction )
    {
        case OptimalViewpointVolume::Constant0: return 0.0;
        case OptimalViewpointVolume::SquareRoot: return sqrt( distance / m_obscuranceMaximumDistance );
        case OptimalViewpointVolume::Exponential: return 1.0 - exp( distance / m_obscuranceMaximumDistance );
    }
}


}
