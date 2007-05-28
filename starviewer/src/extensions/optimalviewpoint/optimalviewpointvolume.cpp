/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointvolume.h"

#include <iostream>

#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>

#include "vtkVolumeRayCastCompositeFunctionOptimalViewpoint.h"

#include <vtkPointData.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>


#include <vtkImageGaussianSmooth.h>
#include "vtk4DLinearRegressionGradientEstimator.h"


namespace udg {

OptimalViewpointVolume::OptimalViewpointVolume( vtkImageData * image )
{


    // POTSER CALDRIA AFEGIR UN Register A TOTS ELS OBJECTES QUE ES GUARDEN



    m_image = image; m_image->Register( 0 );




    m_data = reinterpret_cast<unsigned char *>( m_image->GetPointData()->GetScalars()->GetVoidPointer(0) );




    m_simplifiedImage = vtkImageData::New();
    m_simplifiedImage->DeepCopy( m_image );
    m_simplifiedImage->Register( 0 );
    // falta la comprovació del tipus
    m_simplifiedData = reinterpret_cast<unsigned char *>( m_simplifiedImage->GetPointData()->GetScalars()->GetVoidPointer(0) );

    m_segmentedImage = vtkImageData::New();
    m_segmentedImage->DeepCopy( m_image );
    m_segmentedImage->Register( 0 );
    // falta la comprovació del tipus
    m_segmentedData = reinterpret_cast<unsigned char *>( m_segmentedImage->GetPointData()->GetScalars()->GetVoidPointer(0) );

    m_dataSize = m_image->GetPointData()->GetScalars()->GetSize();





    m_mainMapper = vtkVolumeRayCastMapper::New(); m_mainMapper->Register( 0 );
    m_planeMapper = vtkVolumeRayCastMapper::New(); m_planeMapper->Register( 0 );





//     m_planeMapper->SetNumberOfThreads( 2 );






    m_mainVolumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New(); m_mainVolumeRayCastFunction->Register( 0 );
    m_planeVolumeRayCastFunction = vtkVolumeRayCastCompositeFunctionOptimalViewpoint::New(); m_planeVolumeRayCastFunction->Register( 0 );




    m_planeVolumeRayCastFunction->setOptimalViewpointVolume( this );
    m_planeVolumeRayCastFunction->setOpacityOn( false );
    m_planeVolumeRayCastFunction->setComputing( false );






//     m_mainVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
//     m_planeVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();





    m_mainMapper->SetVolumeRayCastFunction( m_mainVolumeRayCastFunction );
    m_mainMapper->SetInput( m_image );
    m_planeMapper->SetVolumeRayCastFunction( m_planeVolumeRayCastFunction );
//     m_planeMapper->SetVolumeRayCastFunction( m_mainVolumeRayCastFunction );





//     m_planeMapper->SetInput( m_segmentedImage );
    m_planeMapper->SetInput( m_image );




    m_planeMapper->AutoAdjustSampleDistancesOff();





    vtk4DLinearRegressionGradientEstimator * gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
    m_mainMapper->SetGradientEstimator( gradientEstimator );
    m_planeMapper->SetGradientEstimator( gradientEstimator );
    gradientEstimator->Delete();




    m_opacityTransferFunction = vtkPiecewiseFunction::New(); m_opacityTransferFunction->Register( 0 );
    m_opacityTransferFunction->AddPoint( 0.0, 0.0 );
    m_opacityTransferFunction->AddPoint( 255.0, 1.0 );
    m_colorTransferFunction = vtkColorTransferFunction::New(); m_colorTransferFunction->Register( 0 );
    m_colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
    m_colorTransferFunction->AddRGBPoint( 255.0, 1.0, 1.0, 1.0 );
    m_volumeProperty = vtkVolumeProperty::New(); m_volumeProperty->Register( 0 );
    m_volumeProperty->SetScalarOpacity( m_opacityTransferFunction );
//     m_volumeProperty->SetGradientOpacity( m_opacityTransferFunction );
    m_volumeProperty->SetColor( m_colorTransferFunction );



//     gradientOpacityTransferFunction = vtkPiecewiseFunction::New();
//     gradientOpacityTransferFunction->AddPoint( 0.0, 0.0 );
//     gradientOpacityTransferFunction->AddPoint( 255.0, 1.0 );
//     m_volumeProperty->SetGradientOpacity( gradientOpacityTransferFunction );




//     m_volumeProperty->SetInterpolationTypeToLinear();


//     m_volumeProperty->SetSpecular( 1.0 );
//     m_volumeProperty->SetSpecularPower( 64.0 );




    m_mainVolume = vtkVolume::New(); m_mainVolume->Register( 0 );
    m_mainVolume->SetMapper( m_mainMapper );
    m_mainVolume->SetProperty( m_volumeProperty );

    // centrem el volum a (0,0,0)
    double * center = m_mainVolume->GetCenter();
    m_mainVolume->AddPosition( -center[0], -center[1], -center[2] );

    m_planeVolume = vtkVolume::New(); m_planeVolume->Register( 0 );
    m_planeVolume->SetMapper( m_planeMapper );
    m_planeVolume->SetProperty( m_volumeProperty );
    m_planeVolume->AddPosition( -center[0], -center[1], -center[2] );
}

OptimalViewpointVolume::~OptimalViewpointVolume()
{
    m_opacityTransferFunction->Delete();
    m_colorTransferFunction->Delete();
    m_volumeProperty->Delete();
    m_mainVolumeRayCastFunction->Delete();
    m_planeVolumeRayCastFunction->Delete();
    m_mainMapper->Delete();
    m_planeMapper->Delete();
    m_mainVolume->Delete();
    m_planeVolume->Delete();
    m_image->Delete();
}

void OptimalViewpointVolume::setShade( bool on )
{
    on ? m_volumeProperty->ShadeOn() : m_volumeProperty->ShadeOff();
}

void OptimalViewpointVolume::setImageSampleDistance( double imageSampleDistance )
{
    m_mainMapper->SetImageSampleDistance( imageSampleDistance );
    m_planeMapper->SetImageSampleDistance( imageSampleDistance );
    m_imageSampleDistance = imageSampleDistance;
}

void OptimalViewpointVolume::setSampleDistance( double sampleDistance )
{
    m_mainMapper->SetSampleDistance( sampleDistance );
    m_planeMapper->SetSampleDistance( sampleDistance );
    m_sampleDistance = sampleDistance;
}

/// Retorna el vtkVolume corresponent a l'índex donat.
vtkVolume * OptimalViewpointVolume::getMainVolume() const
{
    return m_mainVolume;
}

/// Retorna el vtkVolume corresponent a l'índex donat.
vtkVolume * OptimalViewpointVolume::getPlaneVolume() const
{
    return m_planeVolume;
}

/**
 * Estableix la funció de transferència d'opacitat pel vtkVolume
 * corresponent a l'índex donat.
 */
void OptimalViewpointVolume::setOpacityTransferFunction( vtkPiecewiseFunction * opacityTransferFunction )
{
    m_opacityTransferFunction->Delete();
    m_opacityTransferFunction = opacityTransferFunction; m_opacityTransferFunction->Register( 0 );
    m_volumeProperty->SetScalarOpacity( opacityTransferFunction );
//     m_volumeProperty->SetGradientOpacity( opacityTransferFunction );
    opacityTransferFunction->Print( std::cout );
}

/**
 * Estableix la funció de transferència de color pel vtkVolume corresponent
 * a l'índex donat.
 */
void OptimalViewpointVolume::setColorTransferFunction( vtkColorTransferFunction * colorTransferFunction )
{
    m_colorTransferFunction->Delete();
    m_colorTransferFunction = colorTransferFunction; m_colorTransferFunction->Register( 0 );
    m_volumeProperty->SetColor( colorTransferFunction );
    colorTransferFunction->Print( std::cout );
}

/**
 * Sincronitza les tranformacions de tots els vtkVolumes. Concretament,
 * aplica la transformació del vtkVolume amb índex 0 a tots els altres
 * vtkVolumes.
 */
void OptimalViewpointVolume::synchronize()
{
    m_planeVolume->PokeMatrix( m_mainVolume->GetMatrix() );
    //m_planeVolume->ComputeMatrix();
}

void OptimalViewpointVolume::handle( int rayId, int offset )
{
    emit visited( rayId, *(m_simplifiedData + offset) );
}

void OptimalViewpointVolume::endRay( int rayId )
{
    emit rayEnd( rayId );
}

void OptimalViewpointVolume::setExcessEntropy( double excessEntropy )
{
    m_excessEntropy = excessEntropy;
}

double randomGenerator( long & seed )
{
    long a = 2147483647;
    long b = 16807;
    long c = ( seed * b ) % a;

    while ( c < 0 )
    {
        c += a;
    }

    seed = c;

    return static_cast<double>( c ) / static_cast<double>( a );
}

void OptimalViewpointVolume::segmentateVolume( unsigned short iterations, unsigned char numberOfClusters, double noise )
{
    unsigned short i, k1, k2;   // j, p
    unsigned char nLabels = numberOfClusters - 1;
    unsigned char maxLevel = 255;
    unsigned char value;
    double maxExcessEntropy = 0;
    std::vector<unsigned char> limits( nLabels );
    std::vector<unsigned char> sortLimits( nLabels );
    std::vector<unsigned char> maxLimits( nLabels );
    long seed = 1979;   // time( 0 );
    unsigned short maxIteration;
//    int prob[10][10];
    bool genetic = true;
    bool swapping = true;
    bool finest = true;
//     bool probok=false;
    int numbucleswap = 2;
//     std::ofstream fout("ResultatSegmetnacio.dat", std::ios::app);

//     for ( i = 0; i < 10; i++ )
//     {
//         for ( j = 0; j < 10; j++ )
//         {
//             prob[i][j] = 0;
//         }
//     }
    bool presegmented = false;

    if ( !m_segmentationFileName.isNull() )
    {
        std::cout << qPrintable( m_segmentationFileName ) << std::endl;
        QFile segFile( m_segmentationFileName );

        if ( segFile.open( QFile::ReadOnly | QFile::Text ) )
        {
            QTextStream in( &segFile );
            unsigned char nRead;

            for ( nRead = 0; nRead < nLabels && !in.atEnd(); nRead++ )
            {
                QString sValue;
                in >> sValue;
                bool ok;
                ushort usValue = sValue.toUShort( &ok );
                if ( ok ) maxLimits[nRead] = usValue;
                else break;
            }

            segFile.close();

            if ( nRead == nLabels ) presegmented = true;
        }
    }

    if ( !presegmented )
    {
        for ( i = 0; i < nLabels; i++ )
        {
            limits[i] = static_cast<int>( ( i + 1 ) * maxLevel / numberOfClusters );
        }

        // genetic algorithm
        if( genetic )
        {
            for ( k1 = 0; k1 < iterations; k1++ )
            {
                std::cout << "iteration " << k1 << std::flush;
                copy( limits.begin(), limits.end(), sortLimits.begin() );
                sort( sortLimits.begin(), sortLimits.end() );
                std::cout << "==> labels: ";

                for( i = 0; i < sortLimits.size(); i++ )
                    std::cout << (short) sortLimits[i] << " ";

                std::cout << "==> Maxlabels: [";

                for ( i = 0; i < maxLimits.size(); i++ )
                    std::cout << (short) maxLimits[i] << ", ";

                std::cout << "]" << std::endl;

                unsigned char * fixIt = m_data;
                unsigned char * segIt = m_simplifiedData;
                unsigned char * fixItEnd = m_data + m_dataSize;

                while ( fixIt < fixItEnd )
                {
                    i = 0;
                    value = *fixIt;

                    while ( value > sortLimits[i] && i < nLabels )
                    {
                        i++;
                    }

                    *segIt = i;
                    ++fixIt;
                    ++segIt;
                }

                std::cout << "Imatge labelitzada!!" << std::endl;



                // calcular excess entropy
                emit needsExcessEntropy();
                std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                if ( m_excessEntropy > maxExcessEntropy )
                {
                    maxExcessEntropy = m_excessEntropy;
                    maxIteration = k1;
                    copy( sortLimits.begin(), sortLimits.end(), maxLimits.begin() );
                    std::cout << "** is maximum **" << std::endl;
                }

                // Next limits

                for ( i = 0; i < nLabels; i++ )
                {
                    double randomValue = randomGenerator( seed );

    //                if ( maxLimits[i] < 0 )
    //                    maxLimits[i] = 0;

                    limits[i] = maxLimits[i] + static_cast<int>( noise * (2.0 * randomValue - 1.0 ) );
                }
            }

            std::cout << "MAX GENETIC ALGORITHM: Iteration " << (short) maxIteration << " ==> Max entropy = " << maxExcessEntropy << ", per " << std::flush;

            for ( i = 0; i < maxLimits.size(); i++ )
            {
                std::cout << (short) maxLimits[i] << " ";
            }

            std::cout << std::endl;
        }
        // fi genetic algorithm

        // swapping algorithm
        if ( swapping )
        {
            for ( int bucle = 0; bucle < numbucleswap; bucle++ )
            {
                for ( k1 = 0; k1 < nLabels; k1++ )
                {
                    copy( maxLimits.begin(), maxLimits.end(), limits.begin() );

                    for( k2 = 0; k2 < maxLevel; k2 += 8 )
                    {
                        std::cout << "iteration " << k1 << ": " << k2 << std::flush;
                        limits[k1] = k2;
                        copy( limits.begin(), limits.end(), sortLimits.begin() );
                        sort( sortLimits.begin(), sortLimits.end() );
                        std::cout << "==> labels: ";

                        for ( i = 0; i < sortLimits.size(); i++ )
                            std::cout << (short) sortLimits[i] << " ";

                        std::cout << "==> Maxlabels: [";

                        for ( i = 0; i < maxLimits.size(); i++ )
                            std::cout << (short) maxLimits[i] << ", ";

                        std::cout << "]" << std::endl;

                        unsigned char * fixIt = m_data;
                        unsigned char * segIt = m_simplifiedData;
                        unsigned char * fixItEnd = m_data + m_dataSize;

                        while ( fixIt < fixItEnd )
                        {
                            i = 0;
                            value = *fixIt;

                            while ( value > sortLimits[i] && i < nLabels )
                            {
                                i++;
                            }

                            *segIt = i;
                            ++fixIt;
                            ++segIt;
                        }

                        std::cout << "Imatge labelitzada!!" << std::endl;

                        // calcular excess entropy
                        emit needsExcessEntropy();
                        std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                        if ( m_excessEntropy > maxExcessEntropy )
                        {
                            maxExcessEntropy = m_excessEntropy;
                            maxIteration = k1;
                            copy( limits.begin(), limits.end(), maxLimits.begin() );
                            std::cout << "** is maximum **" << std::endl;
                        }
                    }
                }
            }

            std::cout << "MAX SWAPPING ALGORITHM  ==> Max entropy = " << maxExcessEntropy << ", per " << std::flush;

            for ( i = 0; i < maxLimits.size(); i++ )
            {
                std::cout << (short) maxLimits[i] << " ";
            }

            std::cout << std::endl;
        }
        // fi swapping algorithm

        // finest algorithm
        if ( finest )
        {
            bool canviat = true;

            while ( canviat )
            {
                canviat = false;

                for ( k1 = 0; k1 < nLabels; k1++ )
                {
                    copy( maxLimits.begin(), maxLimits.end(), limits.begin() );
                    limits[k1] = limits[k1] - 1;
                    std::cout << "iteration " << k1 << ": " << (short) limits[k1] << std::flush;
                    copy( limits.begin(), limits.end(), sortLimits.begin() );
                    sort( sortLimits.begin(), sortLimits.end() );
                    std::cout << "==> labels: ";

                    for ( i = 0; i < sortLimits.size(); i++ )
                        std::cout << (short) sortLimits[i] << " ";

                    std::cout << "==> Maxlabels: [";

                    for ( i = 0; i < maxLimits.size(); i++ )
                        std::cout << (short) maxLimits[i] << ", ";

                    std::cout << "]" << std::endl;

                    unsigned char * fixIt = m_data;
                    unsigned char * segIt = m_simplifiedData;
                    unsigned char * fixItEnd = m_data + m_dataSize;

                    while ( fixIt < fixItEnd )
                    {
                        i = 0;
                        value = *fixIt;

                        while ( value > sortLimits[i] && i < nLabels )
                        {
                            i++;
                        }

                        *segIt = i;
                        ++fixIt;
                        ++segIt;
                    }

                    std::cout << "Imatge labelitzada!!" << std::endl;

                    // calcular excess entropy
                    emit needsExcessEntropy();
                    std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                    if ( m_excessEntropy > maxExcessEntropy )
                    {
                        maxExcessEntropy = m_excessEntropy;
                        copy( limits.begin(), limits.end(), maxLimits.begin() );
                        canviat = true;
                        std::cout << "** is maximum **" << std::endl;
                    }

                    // cap a l'altra banda
                    limits[k1] = limits[k1] + 2;    // ara limits[k1] val "limits[k1] - 1"
                    std::cout << "iteration " << k1 << ": " << (short) limits[k1] << std::flush;
                    copy( limits.begin(), limits.end(), sortLimits.begin() );
                    sort( sortLimits.begin(), sortLimits.end() );
                    std::cout << "==> labels: ";

                    for ( i = 0; i < sortLimits.size(); i++ )
                        std::cout << (short) sortLimits[i] << " ";

                    std::cout << "==> Maxlabels: [";

                    for ( i = 0; i < maxLimits.size(); i++ )
                        std::cout << (short) maxLimits[i] << ", ";

                    std::cout << "]" << std::endl;

                    fixIt = m_data;
                    segIt = m_simplifiedData;

                    while ( fixIt < fixItEnd )
                    {
                        i = 0;
                        value = *fixIt;

                        while ( value > sortLimits[i] && i < nLabels )
                        {
                            i++;
                        }

                        *segIt = i;
                        ++fixIt;
                        ++segIt;
                    }

                    std::cout << "Imatge labelitzada!!" << std::endl;

                    // calcular excess entropy
                    emit needsExcessEntropy();
                    std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                    if ( m_excessEntropy > maxExcessEntropy )
                    {
                        maxExcessEntropy = m_excessEntropy;
                        copy( limits.begin(), limits.end(), maxLimits.begin() );
                        canviat = true;
                        std::cout << "** is maximum **" << std::endl;
                    }
                }
            }

            std::cout << "MAX FINEST ALGORITHM  ==> Max entropy = " << maxExcessEntropy << ", per " << std::flush;

            for ( i = 0; i < maxLimits.size(); i++ )
            {
                std::cout << (short) maxLimits[i] << " ";
            }

            std::cout << std::endl;
        }
        // fi finest algorithm
    }
    else QMessageBox::information( 0, "Presegmented", "Presegmented" );

    // desem la imatge labelitzada
    copy( maxLimits.begin(), maxLimits.end(), sortLimits.begin() );
    sort( sortLimits.begin(), sortLimits.end() );

    if ( !presegmented )
    {
        QFile segFile( m_segmentationFileName );

        if ( segFile.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
        {
            QTextStream out( &segFile );

            for ( unsigned char nWritten = 0; nWritten < nLabels; nWritten++ )
                out << static_cast< short >( sortLimits[nWritten] ) << " ";

            segFile.close();
        }
    }

    unsigned char * fixIt = m_data;
//    IteratorType grounIt(GrounImage, GrounImage->GetBufferedRegion());
    unsigned char * segIt = m_simplifiedData;
    unsigned char * seg2It = m_segmentedData;
    unsigned char * fixItEnd = m_data + m_dataSize;

    int cont = 0;

    while ( fixIt < fixItEnd )
    {
        i = 0;
        value = *fixIt;

        while ( value > sortLimits[i] && i < nLabels )
        {
            i++;
        }

        cont++;

//         if(probok)
//         {
//             prob[i][(int)grounIt.Get()]++;
//         }

        *segIt = i;

        if ( i == 0 )
        {
            *seg2It = sortLimits[i] / 2;
        }
        else if ( i == nLabels )
        {
            *seg2It = sortLimits[i-1] + (256 - sortLimits[i-1]) / 2;
        }
        else
        {
            *seg2It = sortLimits[i-1] + (sortLimits[i] + 1 - sortLimits[i-1]) / 2;
        }

        ++fixIt;
//        ++grounIt;
        ++segIt;
        ++seg2It;
    }

    std::cout << "Imatge Final labelitzada!!" << std::endl;
    std::cout << "Resultats Segmentació: " << std::endl;

//     if(probok)
//     {
//         for(i=0;i<nlabels+1;i++){
//             for(j=0;j<10;j++){
//                 std::cout<<prob[i][j]<<"\t";
//                 fout<<prob[i][j]<<"\t";
//             }
//             std::cout<<std::endl;
//             fout<<std::endl;
//         }
//         fout<<"Punts totals: "<<std::endl<<cont<<std::endl;
//     }


    // funció de transferència ajustada
    OptimalViewpoint::TransferFunction adjustedTransferFunction;
    int r, g, b, a;

    r = static_cast<int>( randomGenerator( seed ) * 255.0 );
    g = static_cast<int>( randomGenerator( seed ) * 255.0 );
    b = static_cast<int>( randomGenerator( seed ) * 255.0 );
    a = 4;//static_cast<int>( randomGenerator( seed ) * 255.0 );
    adjustedTransferFunction << QGradientStop( 0.0, QColor( r, g, b, a ) )
                             << QGradientStop( sortLimits[0] / 255.0, QColor( r, g, b, a ) );

    for ( i = 0; i < nLabels - 1; i++ )
    {
        r = static_cast<int>( randomGenerator( seed ) * 255.0 );
        g = static_cast<int>( randomGenerator( seed ) * 255.0 );
        b = static_cast<int>( randomGenerator( seed ) * 255.0 );
        a = static_cast<int>( randomGenerator( seed ) * 255.0 );
        adjustedTransferFunction << QGradientStop( ( sortLimits[i] + 1 ) / 255.0, QColor( r, g, b, a ) )
                                 << QGradientStop( sortLimits[i + 1] / 255.0, QColor( r, g, b, a ) );
    }   // i == nLabels - 1

    r = static_cast<int>( randomGenerator( seed ) * 255.0 );
    g = static_cast<int>( randomGenerator( seed ) * 255.0 );
    b = static_cast<int>( randomGenerator( seed ) * 255.0 );
    a = static_cast<int>( randomGenerator( seed ) * 255.0 );
    adjustedTransferFunction << QGradientStop( ( sortLimits[i] + 1 ) / 255.0, QColor( r, g, b, a ) )
                             << QGradientStop( 1.0, QColor( r, g, b, a ) );

    emit adjustedTransferFunctionDefined( adjustedTransferFunction );







    // suavitzat (pel brainweb)
//     vtkImageGaussianSmooth * smooth = vtkImageGaussianSmooth::New();
//     smooth->Print( std::cout );
//     smooth->SetInput( m_image );
// //     smooth->SetDimensionality( 3 );  // no cal
//     smooth->SetRadiusFactor( 1.0 );
//     smooth->SetStandardDeviation( 1.0 );
//     smooth->Update();
//     m_image->ShallowCopy( smooth->GetOutput() );
//     m_image->Update();
//     smooth->Print( std::cout );
//     smooth->Delete();
}



void OptimalViewpointVolume::setComputing( bool on )
{
    m_planeVolumeRayCastFunction->setComputing( on );
}



void OptimalViewpointVolume::setSegmentationFileName( QString name )
{
    m_segmentationFileName = name;
}



// nous paràmetres



void OptimalViewpointVolume::setOpacityForComputing( bool on )
{
    m_planeVolumeRayCastFunction->setOpacityOn( on );
}



void OptimalViewpointVolume::setInterpolation( int interpolation )
{
    switch ( interpolation )
    {
        case INTERPOLATION_NEAREST_NEIGHBOUR:
            m_volumeProperty->SetInterpolationTypeToNearest();
            break;
        case INTERPOLATION_LINEAR_INTERPOLATE_CLASSIFY:
            m_volumeProperty->SetInterpolationTypeToLinear();
            m_mainVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
            m_planeVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
            break;
        case INTERPOLATION_LINEAR_CLASSIFY_INTERPOLATE:
            m_volumeProperty->SetInterpolationTypeToLinear();
            m_mainVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            m_planeVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            break;
    }
}



void OptimalViewpointVolume::setSpecular( bool on )
{
    m_volumeProperty->SetSpecular( on ? 1.0 : 0.0 );
}



void OptimalViewpointVolume::setSpecularPower( double specularPower )
{
    m_volumeProperty->SetSpecularPower( specularPower );
}



}; // end namespace udg
