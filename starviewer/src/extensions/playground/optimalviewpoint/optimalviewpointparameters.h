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
 *
 * \todo Es podria fer amb propietats.
 */
class OptimalViewpointParameters : public Parameters {

    Q_OBJECT

public:

    /// Identificadors dels diversos paràmetres.
    enum OptimalViewpointParametersNames
    {
        NumberOfPlanes, Shade, VisualizationBlockLength,
            VisualizationImageSampleDistance, VisualizationSampleDistance, TransferFunctionObject,

        // nous paràmetres
        ComputeWithOpacity, Interpolation, Specular, SpecularPower, UpdatePlane,
        SimilarityThreshold,
        Cluster, ClusterFirst, ClusterLast,
//         NumberOfSlices,

        ReadExtentFromFile,

        GroupingMethod,

        Obscurances, ObscurancesFactor, ObscurancesFilterLow, ObscurancesFilterHigh,

        Fx, FxContour, FxSaliency
    };

    OptimalViewpointParameters( QObject * parent = 0 );
    ~OptimalViewpointParameters();

    /// Inicialitza els valors dels paràmetres.
    void init();




    

    bool getShade() const;

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
    bool getSpecular() const;
    void setSpecular( bool specular );
    double getSpecularPower() const;
    void setSpecularPower( double specularPower );
    short getUpdatePlane() const;
    void setUpdatePlane( short updatePlane );

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

    int getGroupingMethod() const;
    void setGroupingMethod( int groupingMethod );

    bool getObscurances() const;
    void setObscurances( bool obscurances );

    double getObscurancesFactor() const;
    void setObscurancesFactor( double obscurancesFactor );

    double getObscurancesFilterLow() const;
    void setObscurancesFilterLow( double obscurancesFilterLow );

    double getObscurancesFilterHigh() const;
    void setObscurancesFilterHigh( double obscurancesFilterHigh );

    bool getFx() const;
    void setFx( bool fx );

    double getFxContour() const;
    void setFxContour( double fxContour );

    bool getFxSaliency() const;
    void setFxSaliency( bool fxSaliency );

public slots:

    void setInterpolation( int interpolation );
    void setShade( bool shade );



signals:

    void signalAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction );

private:


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

    double m_similarityThreshold;

    bool m_cluster;
    unsigned short m_clusterFirst;
    unsigned short m_clusterLast;
//     unsigned short m_numberOfSlices;

    bool m_readExtentFromFile;

    int m_groupingMethod;

    bool m_obscurances;
    double m_obscurancesFactor;
    double m_obscurancesFilterLow;
    double m_obscurancesFilterHigh;

    bool m_fx;
    double m_fxContour;
    bool m_fxSaliency;


};


}


#endif
