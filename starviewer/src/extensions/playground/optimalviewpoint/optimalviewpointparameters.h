/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTPARAMETERS_H
#define UDGOPTIMALVIEWPOINTPARAMETERS_H

#include "parameters.h"

#include <list> // per std::list<int>
#include <QGradient>

#include "optimalviewpoint.h"   // per les funcions de transferència

class QColor;

namespace udg {

class Volume;

/**
    Subclasse de Parameters. En aquest cas encapsula els paràmetres d'un mètode
    de visualització anomenat Magic Mirrors.
    
    Tindrem un mètode set/get per tots i cadascun dels paràmetres del mètode.
    Els mètodes set emetran el signal changed amb l'id de paràmetre que hem
    creat amb el tipus enumerat.
*/

class OptimalViewpointParameters : public Parameters {
    
    Q_OBJECT
    
public:



    //typedef QPair<qreal, QColor> QGradientStop;       // a <QGradient>
    //typedef QVector<QGradientStop> QGradientStops;    // a <QGradient>
    //typedef QGradientStops TransferFunction;                                // funció de transferència per un volum d'un mirall
    //typedef QVector<TransferFunction> OptimalViewpointTransferFunction;               // funció de transferència per tots els volums d'un mirall
    
    
    /**
        Aquest tipus enumerat ens serveix per crear els identificadors de cada
        paràmetre.
    */
    enum OptimalViewpointParametersNames
    {
        VolumeObject,
        SegmentationFileName,
        SegmentationIterations, SegmentationBlockLength, SegmentationNumberOfClusters,
            SegmentationNoise, SegmentationImageSampleDistance, SegmentationSampleDistance,
        NumberOfPlanes, Shade, VisualizationBlockLength,
            VisualizationImageSampleDistance, VisualizationSampleDistance, TransferFunction,

        // nous paràmetres
        ComputeWithOpacity, Interpolation, Specular, SpecularPower, UpdatePlane,
        Compute, NumberOfClusters
    };

    OptimalViewpointParameters( QObject * parent = 0 );
    ~OptimalViewpointParameters();


    unsigned short getSegmentationIterations() const;
    void setSegmentationIterations( unsigned short segmentationIterations );

    unsigned char getSegmentationBlockLength() const;
    void setSegmentationBlockLength( unsigned char segmentationBlockLenth );

    unsigned char getSegmentationNumberOfClusters() const;
    void setSegmentationNumberOfClusters( unsigned char segmentationNumberOfClusters );

    double getSegmentationNoise() const;
    void setSegmentationNoise( double segmentationNoise );

    double getSegmentationImageSampleDistance() const;
    void setSegmentationImageSampleDistance( double segmentationImageSampleDistance );

    double getSegmentationSampleDistance() const;
    void setSegmentationSampleDistance( double segmentationSampleDistance );

    const QString & getSegmentationFileName() const;
    void setSegmentationFileName( const QString & segmentationFileName );




    

    bool getShade() const;
    
    void setShade( bool shade );

    double getVisualizationImageSampleDistance() const;

    void setVisualizationImageSampleDistance( double visualizationImageSampleDistance );

    double getVisualizationSampleDistance() const;

    void setVisualizationSampleDistance( double visualizationSampleDistance );
    
    void setVolumeObject( Volume * volumeObject );
    void setNumberOfPlanes( unsigned char numberOfPlanes );
    void setTransferFunction( const OptimalViewpoint::TransferFunction & transferFunction );
    
    Volume * getVolumeObject() const;
    unsigned char getNumberOfPlanes() const;
    const OptimalViewpoint::TransferFunction & getTransferFunction() const;

    unsigned char getVisualizationBlockLength() const;
    void setVisualizationBlockLength( unsigned char visualizationBlockLength );

    void setAdjustedTransferFunction( const OptimalViewpoint::TransferFunction & adjustedTransferFunction );




    // nous paràmetres
    bool getComputeWithOpacity() const;
    void setComputeWithOpacity( bool computeWithOpacity );
    int getInterpolation() const;
    void setInterpolation( int interpolation );
    bool getSpecular() const;
    void setSpecular( bool specular );
    double getSpecularPower() const;
    void setSpecularPower( double specularPower );
    signed char getUpdatePlane() const;
    void setUpdatePlane( signed char updatePlane );
    bool getCompute() const;
    void setCompute( bool compute );
    unsigned char getNumberOfClusters() const;
    void setNumberOfClusters( unsigned char numberOfClusters );



private:
    
    Volume * m_volumeObject;

    QString m_segmentationFileName;

    unsigned short m_segmentationIterations;
    unsigned char m_segmentationBlockLength;
    unsigned char m_segmentationNumberOfClusters;
    double m_segmentationNoise;
    double m_segmentationImageSampleDistance;
    double m_segmentationSampleDistance;



    
    unsigned char m_numberOfPlanes;
    bool m_shade;
    double m_visualizationImageSampleDistance;
    double m_visualizationSampleDistance;
    unsigned char m_visualizationBlockLength;
    OptimalViewpoint::TransferFunction m_transferFunction;



    // nous paràmetres
    bool m_computeWithOpacity;
    int m_interpolation;
    bool m_specular;
    double m_specularPower;
    signed char m_updatePlane;
    bool m_compute;
    unsigned char m_numberOfClusters;



signals:

    void signalAdjustedTransferFunction( const OptimalViewpoint::TransferFunction & adjustedTransferFunction );

}; // end class OptimalViewpointParameters

}; // end namespace udg

#endif // UDGOPTIMALVIEWPOINTPARAMETERS_H
