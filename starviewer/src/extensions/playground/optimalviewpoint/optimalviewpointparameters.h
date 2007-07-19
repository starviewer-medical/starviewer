/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/



#ifndef UDGOPTIMALVIEWPOINTPARAMETERS_H
#define UDGOPTIMALVIEWPOINTPARAMETERS_H



#include "parameters.h"


#include "transferfunction.h"



namespace udg {



class Volume;



/**
 * Subclasse de Parameters que encapsula els paràmetres de l'extensió
 * OptimalViewpoint.
 *
 * Tindrem un mètode set/get per tots i cadascun dels paràmetres del mètode. Els
 * mètodes set emetran el signal changed amb l'id de paràmetre que hem creat amb
 * el tipus enumerat.
 */
class OptimalViewpointParameters : public Parameters {


    Q_OBJECT


public:

    /// Identificadors dels diversos paràmetres.
    enum OptimalViewpointParametersNames
    {
        VolumeObject,
        SegmentationFileName,
        SegmentationIterations, SegmentationBlockLength,
            SegmentationNumberOfClusters, SegmentationNoise,
            SegmentationImageSampleDistance, SegmentationSampleDistance,
        NumberOfClusters,

        NumberOfPlanes, Shade, VisualizationBlockLength,
            VisualizationImageSampleDistance, VisualizationSampleDistance, TransferFunctionObject,

        // nous paràmetres
        ComputeWithOpacity, Interpolation, Specular, SpecularPower, UpdatePlane,
        Compute,
        SimilarityThreshold,
        Cluster, ClusterFirst, ClusterLast,
//         NumberOfSlices
    };

    OptimalViewpointParameters( QObject * parent = 0 );
    ~OptimalViewpointParameters();

    void setVolumeObject( Volume * volumeObject );
    Volume * getVolumeObject() const;

    void setSegmentationFileName( const QString & segmentationFileName );
    const QString & getSegmentationFileName() const;

    void setSegmentationIterations( unsigned short segmentationIterations );
    unsigned short getSegmentationIterations() const;
    void setSegmentationBlockLength( unsigned char segmentationBlockLenth );
    unsigned char getSegmentationBlockLength() const;
    void setSegmentationNumberOfClusters( unsigned char segmentationNumberOfClusters );
    unsigned char getSegmentationNumberOfClusters() const;
    void setSegmentationNoise( double segmentationNoise );
    double getSegmentationNoise() const;
    void setSegmentationImageSampleDistance( double segmentationImageSampleDistance );
    double getSegmentationImageSampleDistance() const;
    void setSegmentationSampleDistance( double segmentationSampleDistance );
    double getSegmentationSampleDistance() const;

    void setNumberOfClusters( unsigned char numberOfClusters );
    unsigned char getNumberOfClusters() const;


    

    bool getShade() const;
    
    void setShade( bool shade );

    double getVisualizationImageSampleDistance() const;

    void setVisualizationImageSampleDistance( double visualizationImageSampleDistance );

    double getVisualizationSampleDistance() const;

    void setVisualizationSampleDistance( double visualizationSampleDistance );
    
    
    void setNumberOfPlanes( unsigned char numberOfPlanes );
    void setTransferFunctionObject( const TransferFunction & transferFunctionObject );
    
    
    unsigned char getNumberOfPlanes() const;
    const TransferFunction & getTransferFunctionObject() const;

    unsigned char getVisualizationBlockLength() const;
    void setVisualizationBlockLength( unsigned char visualizationBlockLength );

    void setAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction );




    // nous paràmetres
    bool getComputeWithOpacity() const;
    void setComputeWithOpacity( bool computeWithOpacity );
    int getInterpolation() const;
    void setInterpolation( int interpolation );
    bool getSpecular() const;
    void setSpecular( bool specular );
    double getSpecularPower() const;
    void setSpecularPower( double specularPower );
    short getUpdatePlane() const;
    void setUpdatePlane( short updatePlane );
    bool getCompute() const;
    void setCompute( bool compute );

    void setSimilarityThreshold( double similarityThreshold );
    double getSimilarityThreshold() const;

    void setCluster( bool cluster );
    bool getCluster() const;
    void setClusterFirst( unsigned short clusterFirst );
    unsigned short getClusterFirst() const;
    void setClusterLast( unsigned short clusterLast );
    unsigned short getClusterLast() const;
//     void setNumberOfSlices( unsigned short numberOfSlices );
//     unsigned short getNumberOfSlices() const;



private:

    Volume * m_volumeObject;

    QString m_segmentationFileName;

    unsigned short m_segmentationIterations;
    unsigned char m_segmentationBlockLength;
    unsigned char m_segmentationNumberOfClusters;
    double m_segmentationNoise;
    double m_segmentationImageSampleDistance;
    double m_segmentationSampleDistance;

    unsigned char m_numberOfClusters;

    
    unsigned char m_numberOfPlanes;
    bool m_shade;
    double m_visualizationImageSampleDistance;
    double m_visualizationSampleDistance;
    unsigned char m_visualizationBlockLength;
    TransferFunction m_transferFunctionObject;



    // nous paràmetres
    bool m_computeWithOpacity;
    int m_interpolation;
    bool m_specular;
    double m_specularPower;
    short m_updatePlane;
    bool m_compute;

    double m_similarityThreshold;

    bool m_cluster;
    unsigned short m_clusterFirst;
    unsigned short m_clusterLast;
//     unsigned short m_numberOfSlices;



signals:

    void signalAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction );


};



}



#endif
