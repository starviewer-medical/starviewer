/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
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
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ObscuranceThread : public QThread
{

    Q_OBJECT

public:

    ObscuranceThread( unsigned char id, unsigned char numberOfThreads, const QVector<Vector3> & directions, const TransferFunction & transferFunction, QObject * parent = 0 );
    virtual ~ObscuranceThread();

    void setNormals( vtkDirectionEncoder * directionEncoder, const unsigned short * encodedNormals );
    void setData( const unsigned char * data, int dataSize, const int dimensions[3], const int increments[3] );
    void setObscuranceParameters( double obscuranceMaximumDistance, OptimalViewpointVolume::ObscuranceFunction obscuranceFunction, OptimalViewpointVolume::ObscuranceVariant obscuranceVariant );

    double * getObscurance() const;

protected:

    virtual void run();

private:

    struct Voxel { unsigned short x, y, z; };

    void runDensity();
    void runDensitySmooth();
    void runOpacity();
    void runOpacitySmooth();
    void getLineStarts( QVector<Vector3> & lineStarts, int dimX, int dimY, int dimZ, const Vector3 & forward ) const;
    double obscurance( double distance ) const;

    unsigned char m_id, m_numberOfThreads;
    const QVector<Vector3> & m_directions;
    const TransferFunction & m_transferFunction;
    vtkDirectionEncoder * m_directionEncoder;
    const unsigned short * m_encodedNormals;
    const unsigned char * m_data;
    int m_dataSize;
    const int * m_dimensions;
    const int * m_increments;
    double m_obscuranceMaximumDistance;
    OptimalViewpointVolume::ObscuranceFunction m_obscuranceFunction;
    OptimalViewpointVolume::ObscuranceVariant m_obscuranceVariant;
    double * m_obscurance;

};

}

#endif
