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
        Segmentation,
        SegmentationFileName,
        SegmentationNumberOfIterations, SegmentationBlockLength,
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
//         NumberOfSlices,

        ReadExtentFromFile
    };

    /// Tipus de segmentació disponibles.
    enum SegmentationType { LoadSegmentation, AutomaticSegmentation };

    OptimalViewpointParameters( QObject * parent = 0 );
    ~OptimalViewpointParameters();

    /// Inicialitza els valors dels paràmetres.
    void init();

    //@{
    /// Obtenir/assignar el volum.
    Volume * getVolumeObject() const;
    void setVolumeObject( Volume * volumeObject );
    //@}

    //@{
    /// Obtenir/assignar el tipus de segmentació.
    SegmentationType getSegmentation() const;
    void setSegmentation( SegmentationType segmentation );
    //@}

    //@{
    /// Obtenir/assignar el nom del fitxer de segmentació.
    const QString & getSegmentationFileName() const;
    void setSegmentationFileName( const QString & segmentationFileName );
    //@}

    //@{
    /// Obtenir/assignar el nombre d'iteracions per a la segmentació.
    unsigned short getSegmentationNumberOfIterations() const;
    void setSegmentationNumberOfIterations( unsigned short segmentationNumberOfIterations );
    //@}
    //@{
    /// Obtenir/assignar la longitud de bloc per a la segmentació.
    unsigned char getSegmentationBlockLength() const;
    void setSegmentationBlockLength( unsigned char segmentationBlockLenth );
    //@}
    //@{
    /// Obtenir/assignar el nombre de clústers per a la segmentació.
    unsigned char getSegmentationNumberOfClusters() const;
    void setSegmentationNumberOfClusters( unsigned char segmentationNumberOfClusters );
    //@}
    //@{
    /// Obtenir/assignar el soroll per a la segmentació.
    double getSegmentationNoise() const;
    void setSegmentationNoise( double segmentationNoise );
    //@}
    //@{
    /// Obtenir/assignar la distància entre raigs per a la segmentació.
    double getSegmentationImageSampleDistance() const;
    void setSegmentationImageSampleDistance( double segmentationImageSampleDistance );
    //@}
    //@{
    /// Obtenir/assignar la distància entre mostres per a la segmentació.
    double getSegmentationSampleDistance() const;
    void setSegmentationSampleDistance( double segmentationSampleDistance );
    //@}

    //@{
    /// Obtenir/assignar el nombre de clústers després de la segmentació.
    unsigned char getNumberOfClusters() const;
    void setNumberOfClusters( unsigned char numberOfClusters );
    //@}



    

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


    bool getReadExtentFromFile() const;
    void setReadExtentFromFile( bool readExtentFromFile );




signals:

    void signalAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction );

private:

    /// Volum.
    Volume * m_volumeObject;

    /// Tipus de segmentació.
    SegmentationType m_segmentation;

    /// Nom del fitxer de segmentació.
    QString m_segmentationFileName;

    /// Nombre d'iteracions per a la segmentació.
    unsigned short m_segmentationNumberOfIterations;
    /// Longitud de bloc per a la segmentació.
    unsigned char m_segmentationBlockLength;
    /// Nombre de clústers per a la segmentació.
    unsigned char m_segmentationNumberOfClusters;
    /// Soroll per a la segmentació.
    double m_segmentationNoise;
    /// Distància entre raigs per a la segmentació.
    double m_segmentationImageSampleDistance;
    /// Distància entre mostres per a la segmentació.
    double m_segmentationSampleDistance;

    /// Nombre de clústers després de la segmentació.
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

    bool m_readExtentFromFile;


};


}


#endif
