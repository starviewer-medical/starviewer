/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "optimalviewpointparameters.h"


namespace udg {


OptimalViewpointParameters::OptimalViewpointParameters( QObject * parent )
    : Parameters( parent )
{
}


OptimalViewpointParameters::~OptimalViewpointParameters()
{
}


void OptimalViewpointParameters::init()
{
}


bool OptimalViewpointParameters::getShade() const
{
    return m_shade;
}

void OptimalViewpointParameters::setShade( bool shade )
{
    if ( m_shade != shade )
    {
        m_shade = shade; emit changed( Shade );
    }
}

double OptimalViewpointParameters::getVisualizationImageSampleDistance() const
{
    return m_visualizationImageSampleDistance;
}

void OptimalViewpointParameters::setVisualizationImageSampleDistance( double visualizationImageSampleDistance )
{
    if ( m_visualizationImageSampleDistance != visualizationImageSampleDistance )
    {
        m_visualizationImageSampleDistance = visualizationImageSampleDistance; emit changed( VisualizationImageSampleDistance );
    }
}

double OptimalViewpointParameters::getVisualizationSampleDistance() const
{
    return m_visualizationSampleDistance;
}

void OptimalViewpointParameters::setVisualizationSampleDistance( double visualizationSampleDistance )
{
    if ( m_visualizationSampleDistance != visualizationSampleDistance )
    {
        m_visualizationSampleDistance = visualizationSampleDistance; emit changed( VisualizationSampleDistance );
    }
}



void OptimalViewpointParameters::setNumberOfPlanes( unsigned char numberOfPlanes )
{
    m_numberOfPlanes = numberOfPlanes; emit changed( NumberOfPlanes );
}

void OptimalViewpointParameters::setTransferFunctionObject( const TransferFunction & transferFunctionObject )
{
    m_transferFunctionObject = transferFunctionObject; emit changed( TransferFunctionObject );
}



unsigned char OptimalViewpointParameters::getNumberOfPlanes() const
{
    return m_numberOfPlanes;
}

const TransferFunction & OptimalViewpointParameters::getTransferFunctionObject() const
{
    return m_transferFunctionObject;
}

unsigned char OptimalViewpointParameters::getVisualizationBlockLength() const
{
    return m_visualizationBlockLength;
}

void OptimalViewpointParameters::setVisualizationBlockLength( unsigned char visualizationBlockLength )
{
    if ( m_visualizationBlockLength != visualizationBlockLength )
    {
        m_visualizationBlockLength = visualizationBlockLength; emit changed( VisualizationBlockLength );
    }
}

void OptimalViewpointParameters::setAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction )
{
    emit signalAdjustedTransferFunction( adjustedTransferFunction );
}



// nous paràmetres



bool OptimalViewpointParameters::getComputeWithOpacity() const
{
    return m_computeWithOpacity;
}

void OptimalViewpointParameters::setComputeWithOpacity( bool computeWithOpacity )
{
    if ( m_computeWithOpacity != computeWithOpacity )
    {
        m_computeWithOpacity = computeWithOpacity; emit changed( ComputeWithOpacity );
    }
}



int OptimalViewpointParameters::getInterpolation() const
{
    return m_interpolation;
}

void OptimalViewpointParameters::setInterpolation( int interpolation )
{
    if ( m_interpolation != interpolation )
    {
        m_interpolation = interpolation; emit changed( Interpolation );
    }
}



bool OptimalViewpointParameters::getSpecular() const
{
    return m_specular;
}

void OptimalViewpointParameters::setSpecular( bool specular )
{
    if ( m_specular != specular )
    {
        m_specular = specular; emit changed( Specular );
    }
}



double OptimalViewpointParameters::getSpecularPower() const
{
    return m_specularPower;
}

void OptimalViewpointParameters::setSpecularPower( double specularPower )
{
    if ( m_specularPower != specularPower )
    {
        m_specularPower = specularPower; emit changed( SpecularPower );
    }
}



short OptimalViewpointParameters::getUpdatePlane() const
{
    return m_updatePlane;
}

void OptimalViewpointParameters::setUpdatePlane( short updatePlane )
{
    if ( m_updatePlane != updatePlane )
    {
        m_updatePlane = updatePlane; emit changed( UpdatePlane );
    }
}


void OptimalViewpointParameters::setSimilarityThreshold( double similarityThreshold )
{
    if ( m_similarityThreshold != similarityThreshold )
    {
        m_similarityThreshold = similarityThreshold; emit changed( SimilarityThreshold );
    }
}


double OptimalViewpointParameters::getSimilarityThreshold() const
{
    return m_similarityThreshold;
}


void OptimalViewpointParameters::setCluster( bool cluster )
{
    if ( m_cluster != cluster )
    {
        m_cluster = cluster; emit changed( Cluster );
    }
}


bool OptimalViewpointParameters::getCluster() const
{
    return m_cluster;
}


void OptimalViewpointParameters::setClusterFirst( unsigned short clusterFirst )
{
    if ( m_clusterFirst != clusterFirst )
    {
        m_clusterFirst = clusterFirst; emit changed( ClusterFirst );
    }
}


unsigned short OptimalViewpointParameters::getClusterFirst() const
{
    return m_clusterFirst;
}


void OptimalViewpointParameters::setClusterLast( unsigned short clusterLast )
{
    if ( m_clusterLast != clusterLast )
    {
        m_clusterLast = clusterLast; emit changed( ClusterLast );
    }
}


unsigned short OptimalViewpointParameters::getClusterLast() const
{
    return m_clusterLast;
}


// void OptimalViewpointParameters::setNumberOfSlices( unsigned short numberOfSlices )
// {
//     if ( m_numberOfSlices != numberOfSlices )
//     {
//         m_numberOfSlices = numberOfSlices; emit changed( NumberOfSlices );
//     }
// }
// 
// 
// unsigned short OptimalViewpointParameters::getNumberOfSlices() const
// {
//     return m_numberOfSlices;
// }


bool OptimalViewpointParameters::getReadExtentFromFile() const
{
    return m_readExtentFromFile;
}


void OptimalViewpointParameters::setReadExtentFromFile( bool readExtentFromFile )
{
    if ( m_readExtentFromFile != readExtentFromFile )
    {
        m_readExtentFromFile = readExtentFromFile; emit changed( ReadExtentFromFile );
    }
}


int OptimalViewpointParameters::getGroupingMethod() const
{
    return m_groupingMethod;
}


void OptimalViewpointParameters::setGroupingMethod( int groupingMethod )
{
    if ( m_groupingMethod != groupingMethod )
    {
        m_groupingMethod = groupingMethod; emit changed( GroupingMethod );
    }
}


bool OptimalViewpointParameters::getObscurances() const
{
    return m_obscurances;
}


void OptimalViewpointParameters::setObscurances( bool obscurances )
{
    if ( m_obscurances != obscurances )
    {
        m_obscurances = obscurances; emit changed( Obscurances );
    }
}


double OptimalViewpointParameters::getObscurancesFactor() const
{
    return m_obscurancesFactor;
}


void OptimalViewpointParameters::setObscurancesFactor( double obscurancesFactor )
{
    if ( m_obscurancesFactor != obscurancesFactor )
    {
        m_obscurancesFactor = obscurancesFactor; emit changed( ObscurancesFactor );
    }
}


double OptimalViewpointParameters::getObscurancesFilterLow() const
{
    return m_obscurancesFilterLow;
}


void OptimalViewpointParameters::setObscurancesFilterLow( double obscurancesFilterLow )
{
    if ( m_obscurancesFilterLow != obscurancesFilterLow )
    {
        m_obscurancesFilterLow = obscurancesFilterLow; emit changed( ObscurancesFilterLow );
    }
}


double OptimalViewpointParameters::getObscurancesFilterHigh() const
{
    return m_obscurancesFilterHigh;
}


void OptimalViewpointParameters::setObscurancesFilterHigh( double obscurancesFilterHigh )
{
    if ( m_obscurancesFilterHigh != obscurancesFilterHigh )
    {
        m_obscurancesFilterHigh = obscurancesFilterHigh; emit changed( ObscurancesFilterHigh );
    }
}


bool OptimalViewpointParameters::getFx() const
{
    return m_fx;
}


void OptimalViewpointParameters::setFx( bool fx )
{
    if ( m_fx != fx )
    {
        m_fx = fx; emit changed( Fx );
    }
}


double OptimalViewpointParameters::getFxContour() const
{
    return m_fxContour;
}


void OptimalViewpointParameters::setFxContour( double fxContour )
{
    if ( m_fxContour != fxContour )
    {
        m_fxContour = fxContour; emit changed( FxContour );
    }
}


bool OptimalViewpointParameters::getFxSaliency() const
{
    return m_fxSaliency;
}


void OptimalViewpointParameters::setFxSaliency( bool fxSaliency )
{
    if ( m_fxSaliency != fxSaliency )
    {
        m_fxSaliency = fxSaliency; emit changed( FxSaliency );
    }
}


double OptimalViewpointParameters::getFxSaliencyA() const
{
    return m_fxSaliencyA;
}


void OptimalViewpointParameters::setFxSaliencyA( double fxSaliencyA )
{
    if ( m_fxSaliencyA != fxSaliencyA )
    {
        m_fxSaliencyA = fxSaliencyA; emit changed( FxSaliencyA );
    }
}


double OptimalViewpointParameters::getFxSaliencyB() const
{
    return m_fxSaliencyB;
}


void OptimalViewpointParameters::setFxSaliencyB( double fxSaliencyB )
{
    if ( m_fxSaliencyB != fxSaliencyB )
    {
        m_fxSaliencyB = fxSaliencyB; emit changed( FxSaliencyB );
    }
}


}
