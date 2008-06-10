/***************************************************************************
 *   Copyright (C) 2008 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html?langu=uk                           *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGOBSCURANCETHREAD_H
#define UDGOBSCURANCETHREAD_H


#include <QThread>

#include <QVector>

#include "optimalviewpointvolume.h"
#include "transferfunction.h"


class vtkDirectionEncoder;


namespace udg {


class Vector3;


/**
 * Thread que implementa els mètodes de càlcul d'obscurances.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
*/
class ObscuranceThread : public QThread {

    Q_OBJECT

public:

    ObscuranceThread( int id, int numberOfThreads, const QVector<Vector3> & directions, const TransferFunction & transferFunction, QObject * parent = 0 );
    virtual ~ObscuranceThread();

    void setNormals( vtkDirectionEncoder * directionEncoder, const ushort * encodedNormals );
    void setData( const uchar * data, int dataSize, const int dimensions[3], const int increments[3] );
    void setObscuranceParameters( double obscuranceMaximumDistance, OptimalViewpointVolume::ObscuranceFunction obscuranceFunction, OptimalViewpointVolume::ObscuranceVariant obscuranceVariant );

    double * getObscurance() const;
    Vector3 * getColorBleeding() const;

protected:

    virtual void run();

private:

    struct Voxel { int x, y, z; };

    void runDensity();
    void runDensitySmooth();
    void runOpacity();
    void runOpacitySmooth();
    void runOpacityColorBleeding();
    void runOpacitySmoothColorBleeding();
    void getLineStarts( QVector<Vector3> & lineStarts, int dimX, int dimY, int dimZ, const Vector3 & forward ) const;
    double obscurance( double distance ) const;

    int m_id, m_numberOfThreads;
    const QVector<Vector3> & m_directions;
    const TransferFunction & m_transferFunction;
    vtkDirectionEncoder * m_directionEncoder;
    const ushort * m_encodedNormals;
    const uchar * m_data;
    int m_dataSize;
    const int * m_dimensions;
    const int * m_increments;
    double m_obscuranceMaximumDistance;
    OptimalViewpointVolume::ObscuranceFunction m_obscuranceFunction;
    OptimalViewpointVolume::ObscuranceVariant m_obscuranceVariant;
    double * m_obscurance;
    Vector3 * m_colorBleeding;

};


}


#endif
