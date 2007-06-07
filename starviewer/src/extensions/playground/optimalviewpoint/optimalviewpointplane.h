/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGOPTIMALVIEWPOINTPLANE_H
#define UDGOPTIMALVIEWPOINTPLANE_H



#include <QObject>


#include <deque>

#include <QHash>
#include <QMutex>



class vtkCamera;
class vtkImageActor;
class vtkMatrix4x4;
class vtkRenderer;

class QVTKWidget;



namespace udg {



class Histogram;
class OptimalViewpointPlaneHelper;


/**
 * \brief Encapsula tot el tractament d'un pla de l'extensió Optimal Viewpoint.
 * Cada pla correspon a un punt de vista.
 *
 * El pla té un identificador numèric que permet diferenciar-lo d'altres plans.
 *
 * Té un conjunt de mètodes per controlar-ne la posició i la mida. La posició es
 * pot definir amb coordenades geogràfiques.
 *
 * L'altre conjunt de mètodes és per fer càlculs referents al punt de vista
 * corresponent al pla. Aquests càlculs són les mesures de l'entropy rate i
 * l'excess entropy. Es poden fer treballant amb més d'un thread.
 *
 * \note També es compta el nombre de canvis de material màxim d'un raig des
 * d'aquest punt de vista i quants cops es travessa cada material, però aquestes
 * dades s'escriuen directament a la sortida estàndard i de moment no es poden
 * consultar de cap altra manera.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
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



    void setNumberOfThreads( unsigned char numberOfThreads );



    vtkMatrix4x4 * getTransformMatrix();



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




    unsigned char m_numberOfThreads;



signals:

    void excessEntropyComputed( double excessEntropy );
    void goingToRecompute();

}; // end class OptimalViewpointPlane



}



#endif
