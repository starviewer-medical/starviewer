/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTPLANE_H
#define UDGOPTIMALVIEWPOINTPLANE_H

#include <QObject>

#include <QHash>
#include <QMutex>
#include <deque>
#include <vector>

class vtkCamera;
class vtkImageActor;
class vtkRenderer;
class QVTKWidget;

namespace udg {

class Histogram;
class OptimalViewpointPlaneHelper;

class OptimalViewpointPlane : public QObject {

    Q_OBJECT

public:

    OptimalViewpointPlane( unsigned short id, unsigned short size );
    ~OptimalViewpointPlane();

    vtkImageActor * getPlane() const;
    vtkRenderer * getRenderer() const;
    void setDistance( double distance );
    void setLatitude( double latitude );
    void setLongitude( double longitude );
    void update();
    void hide();

    void setEntropyL( unsigned char L );
    void setEntropyN( unsigned char N );
    void startEntropyComputing();
    void endEntropyComputing();
    void setToRecompute();
    double getEntropyRate() const;
    double getExcessEntropy() const;

public slots:

    void updateAndRecompute();
    void compute( int threadId, unsigned char value );
    void endLBlock( int threadId );

private:

    unsigned short m_id;
    unsigned short m_size;
    vtkRenderer * m_renderer;
    vtkCamera * m_camera;
    vtkImageActor * m_plane;
    QVTKWidget * m_window;
    double m_latitude;
    double m_longitude;

    OptimalViewpointPlaneHelper * m_helper;

    unsigned char m_L;
    unsigned char m_N;
    bool m_compute;
    bool m_computing;
    unsigned short m_histogramL_1Size;
//     std::vector<unsigned long> * m_histogramL_1;
//     unsigned long m_countL_1;
    unsigned short m_histogramLSize;
//     std::vector<unsigned long> * m_histogramL;
//     unsigned long m_countL;
//     QHash< int, std::deque< unsigned char > > * m_lastLValuesPerRay;
//     std::deque<unsigned char> * m_lastLValues;
    QMutex m_mutex;
    QHash< int, Histogram > m_histogramL_1PerThread;
    QHash< int, Histogram > m_histogramLPerThread;
    QHash< int, std::deque< unsigned char > > m_lastLValuesPerThread;

    double m_hx;
    double m_E;


    // per comptar els canvis
    QHash< int, unsigned char > m_lastValuePerThread;
    QHash< int, unsigned short > m_currentNumberOfMaterialsPerThread;
    QHash< int, QList< unsigned short > > m_currentMaterialsPerThread;
    QHash< int, unsigned short > m_maxNumberOfMaterialsPerThread;
    QHash< int, QList< unsigned short > > m_maxMaterialsPerThread;

    unsigned short m_maxNumberOfMaterials;
    QList< unsigned short > m_maxMaterials;


signals:

    void excessEntropyComputed( double excessEntropy );
    void goingToRecompute();

}; // end class OptimalViewpointPlane

}; // end namespace udg

#endif
