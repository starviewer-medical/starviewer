#ifndef UDGVOMITHREAD_H
#define UDGVOMITHREAD_H


#include <QThread>

#include <QVector>

#include "vector3.h"


namespace udg {


class VomiThread : public QThread {

public:

    VomiThread( const QVector<float> &viewProbabilities, const QVector<float> &objectProbabilities, const QVector<Vector3Float> &viewpointColors, QVector<Vector3Float> &colorVomi );
    void setPOV( float **pOV );
    void setDimensions( int dimX, int dimY, int dimZ );
    void setYStartAndStep( int yStart, int yStep );
    void setMeasuresToCompute( bool computeColorVomi );
    void setZ( int z );
    virtual void run();
    float maximumColorVomi() const;

private:

    const QVector<float> &m_viewProbabilities;
    const QVector<float> &m_objectProbabilities;
    const QVector<Vector3Float> &m_viewpointColors;
    QVector<Vector3Float> &m_colorVomi;
    float **m_pOV;
    int m_dimX, m_dimY, m_dimZ;
    int m_yStart, m_yStep;
    bool m_computeColorVomi;
    int m_z;
    float m_maximumColorVomi;

};


} // namespace udg


#endif // UDGVOMITHREAD_H
