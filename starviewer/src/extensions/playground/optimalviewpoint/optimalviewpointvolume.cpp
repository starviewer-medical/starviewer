/***************************************************************************
 *   Copyright (C) 2006-2008 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "optimalviewpointvolume.h"

#include <ctime> //Per l'inicialització amb time() de srand()

#include <QFile>
#include <QTextStream>

#include <vtkColorTransferFunction.h>
#include <vtkImageCast.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageShiftScale.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>

#include "logging.h"
#include "transferfunction.h"
#include "vtk4DLinearRegressionGradientEstimator.h"

#include "povspherecloud.h"
#include "vector3.h"
#include <QLinkedList>
#include <QList>
#include <QMultiMap>
#include <vtkDirectionEncoder.h>
#include <QDir>
#include <QStack>
#include <QPair>
#include <QThread>
#include "obscurancethread.h"
#include "obscurancethread2.h"
#include <vtkMultiThreader.h>

#include "vtkVolumeRayCastCompositeFunctionObscurances.h"
#include <vtkDoubleArray.h>
#include "vtkVolumeRayCastCompositeFunctionViewpointSaliency.h"
#include <vtkVolumeRayCastFunction.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <QTime>
#include "vtkVolumeRayCastCompositeFunctionFx.h"
#include "vtkVolumeRayCastCompositeFxFunction.h"
#include "voxelshader.h"
#include "ambientvoxelshader.h"
#include "directilluminationvoxelshader.h"
#include <vtkEncodedGradientShader.h>
#include "contourvoxelshader.h"
#include "obscurancevoxelshader.h"
#include "saliencyvoxelshader.h"
#include "colorbleedingvoxelshader.h"


namespace udg {


OptimalViewpointVolume::OptimalViewpointVolume( vtkImageData *image, QObject *parent )
    : QObject( parent )
{
    Q_ASSERT( image != 0 );

    createImages( image );
    createVoxelShaders();
    createVolumeRayCastFunctions();
    createMapper();
    createProperty();
    createVolume();




    m_obscurance = 0;
    m_colorBleeding = 0;
    m_obscuranceDirections = 2;
    m_obscuranceMaximumDistance = 64.0;
    m_obscuranceFunction = Constant0;

    m_renderWithObscurances = false;





//     reduceToHalf();


    m_saliency = 0;

    DEBUG_LOG( "end constructor" );
}


OptimalViewpointVolume::~OptimalViewpointVolume()
{
    m_image->Delete();
    m_labeledImage->Delete();
    if ( m_clusterImage ) m_clusterImage->Delete();

    delete m_ambientVoxelShader;
    delete m_directIlluminationVoxelShader;
    delete m_contourVoxelShader;
    delete m_obscuranceVoxelShader;
    delete m_colorBleedingVoxelShader;
    delete m_saliencyVoxelShader;

    m_mainVolumeRayCastFunction->Delete();
    m_volumeRayCastFunctionObscurances->Delete();
    m_volumeRayCastFunctionFx->Delete();
    m_volumeRayCastFunctionFx2->Delete();
    m_volumeRayCastFunctionViewpointSaliency->Delete();

    m_mapper->Delete();

    m_property->Delete();

    m_volume->Delete();

    delete [] m_obscurance;
    delete [] m_colorBleeding;
    delete [] m_saliency;
}


void OptimalViewpointVolume::createImages( vtkImageData *image )
{
    double *range = image->GetScalarRange();
    double min = range[0], max = range[1];
    DEBUG_LOG( QString( "[OVV] min = %1, max = %2" ).arg( min ).arg( max ) );

    if ( min >= 0.0 && max <= 255.0 )   // si ja està dins del rang que volem només cal fer un cast
    {
        // cal fer el casting perquè ens arriba com a int
        vtkImageCast *caster = vtkImageCast::New();
        caster->SetInput( image );
        caster->SetOutputScalarTypeToUnsignedChar();
        caster->Update();

        m_image = caster->GetOutput(); m_image->Register( 0 );  // el register és necessari (comprovat)
        caster->Delete();

        m_rangeMin = static_cast<unsigned short>( qRound( min ) );
        m_rangeMax = static_cast<unsigned short>( qRound( max ) );
    }
    else
    {
        double shift = -min;
        double slope = 255.0 / ( max - min );

        vtkImageShiftScale *shifter = vtkImageShiftScale::New();
        shifter->SetInput( image );
        shifter->SetShift( shift );
        shifter->SetScale( slope );
        shifter->SetOutputScalarTypeToUnsignedChar();
        shifter->ClampOverflowOn();
        shifter->Update();

        m_image = shifter->GetOutput(); m_image->Register( 0 ); // el register és necessari (comprovat)
        shifter->Delete();

        m_rangeMin = 0; m_rangeMax = 255;

        double *newRange = m_image->GetScalarRange();
        DEBUG_LOG( QString( "[OVV] new min = %1, new max = %2" ).arg( newRange[0] ).arg( newRange[1] ) );
    }

    m_labeledImage = vtkImageData::New();   // no cal el register perquè hem fet un new
    m_labeledImage->DeepCopy( m_image );
    m_clusterImage = 0;

    m_data = reinterpret_cast<unsigned char*>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    m_labeledData = reinterpret_cast<unsigned char*>( m_labeledImage->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );

    m_dataSize = m_image->GetPointData()->GetScalars()->GetSize();
}


void OptimalViewpointVolume::createVoxelShaders()
{
    m_ambientVoxelShader = new AmbientVoxelShader();
    m_ambientVoxelShader->setData( m_data );
    m_directIlluminationVoxelShader = new DirectIlluminationVoxelShader();
    m_directIlluminationVoxelShader->setData( m_data );
    m_contourVoxelShader = new ContourVoxelShader();
    m_contourVoxelShader->setData( m_data );
    m_obscuranceVoxelShader = new ObscuranceVoxelShader();
    m_obscuranceVoxelShader->setData( m_data );
    m_colorBleedingVoxelShader = new ColorBleedingVoxelShader();
    m_colorBleedingVoxelShader->setData( m_data );
    m_saliencyVoxelShader = new SaliencyVoxelShader();
    m_saliencyVoxelShader->setData( m_data );
}


void OptimalViewpointVolume::createVolumeRayCastFunctions()
{
    m_mainVolumeRayCastFunction = vtkVolumeRayCastCompositeFunction::New();
    m_volumeRayCastFunctionObscurances = vtkVolumeRayCastCompositeFunctionObscurances::New();
    m_volumeRayCastFunctionFx = vtkVolumeRayCastCompositeFunctionFx::New();
    m_volumeRayCastFunctionFx2 = vtkVolumeRayCastCompositeFxFunction::New();
    m_volumeRayCastFunctionFx2->AddVoxelShader( m_ambientVoxelShader );
    m_volumeRayCastFunctionViewpointSaliency = vtkVolumeRayCastCompositeFunctionViewpointSaliency::New();
    m_volumeRayCastFunctionViewpointSaliency->SetVolume( this );
}


void OptimalViewpointVolume::createMapper()
{
    m_mapper = vtkVolumeRayCastMapper::New();
    m_mapper->SetInput( m_image );
    m_mapper->SetVolumeRayCastFunction( m_mainVolumeRayCastFunction );

    vtk4DLinearRegressionGradientEstimator *gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
    m_mapper->SetGradientEstimator( gradientEstimator );
    gradientEstimator->Delete();
}


void OptimalViewpointVolume::createProperty()
{
    m_property = vtkVolumeProperty::New();
    TransferFunction defaultTransferFunction;
    defaultTransferFunction.addPoint( m_rangeMin, QColor( 0, 0, 0, 0 ) );
    defaultTransferFunction.addPoint( m_rangeMax, QColor( 255, 255, 255, 255 ) );
    setTransferFunction( defaultTransferFunction );
}


void OptimalViewpointVolume::createVolume()
{
    m_volume = vtkVolume::New();
    m_volume->SetMapper( m_mapper );
    m_volume->SetProperty( m_property );

    // Centrem el volum a (0,0,0)
    double *center = m_volume->GetCenter();
    m_volume->AddPosition( -center[0], -center[1], -center[2] );
}


void OptimalViewpointVolume::setShade( bool on )
{
    on ? m_property->ShadeOn() : m_property->ShadeOff();
    if (on) {
        m_volumeRayCastFunctionFx2->RemoveVoxelShader( 0 );
        m_volumeRayCastFunctionFx2->InsertVoxelShader( 0, m_directIlluminationVoxelShader );
        vtkEncodedGradientEstimator *gradientEstimator = m_mapper->GetGradientEstimator();
        m_directIlluminationVoxelShader->setEncodedNormals( gradientEstimator->GetEncodedNormals() );
        vtkEncodedGradientShader *gradientShader = m_mapper->GetGradientShader();
        gradientShader->UpdateShadingTable( m_mainRenderer, m_volume, gradientEstimator );
        m_directIlluminationVoxelShader->setDiffuseShadingTables( gradientShader->GetRedDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetGreenDiffuseShadingTable( m_volume ),
                                                                  gradientShader->GetBlueDiffuseShadingTable( m_volume ) );
        m_directIlluminationVoxelShader->setSpecularShadingTables( gradientShader->GetRedSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetGreenSpecularShadingTable( m_volume ),
                                                                   gradientShader->GetBlueSpecularShadingTable( m_volume ) );
    }
    else {
        m_volumeRayCastFunctionFx2->RemoveVoxelShader( 0 );
        m_volumeRayCastFunctionFx2->InsertVoxelShader( 0, m_ambientVoxelShader );
    }
}

void OptimalViewpointVolume::setImageSampleDistance( double imageSampleDistance )
{
    m_mapper->SetImageSampleDistance( imageSampleDistance );
}

double OptimalViewpointVolume::getImageSampleDistance() const
{
    return m_mapper->GetImageSampleDistance();
}

void OptimalViewpointVolume::setSampleDistance( double sampleDistance )
{
    m_mapper->SetSampleDistance( sampleDistance );
}

double OptimalViewpointVolume::getSampleDistance() const
{
    return m_mapper->GetSampleDistance();
}

/// Retorna el vtkVolume corresponent a l'índex donat.
vtkVolume * OptimalViewpointVolume::getMainVolume() const
{
    return m_volume;
}

/// Retorna el vtkVolume corresponent a l'índex donat.
vtkVolume * OptimalViewpointVolume::getPlaneVolume() const
{
    return m_volume;
}


void OptimalViewpointVolume::setTransferFunction( const TransferFunction & transferFunction )
{
    m_property->SetScalarOpacity( transferFunction.getOpacityTransferFunction() );
    m_property->SetColor( transferFunction.getColorTransferFunction() );
    m_ambientVoxelShader->setTransferFunction( transferFunction );
    m_directIlluminationVoxelShader->setTransferFunction( transferFunction );

    m_transferFunction = transferFunction;
}


/**
 * Estableix la funció de transferència d'opacitat pel vtkVolume
 * corresponent a l'índex donat.
 */
// void OptimalViewpointVolume::setOpacityTransferFunction( vtkPiecewiseFunction * opacityTransferFunction )
// {
//     m_opacityTransferFunction->Delete();
//     m_opacityTransferFunction = opacityTransferFunction; m_opacityTransferFunction->Register( 0 );
//     m_volumeProperty->SetScalarOpacity( opacityTransferFunction );
// //     m_volumeProperty->SetGradientOpacity( opacityTransferFunction );
//     opacityTransferFunction->Print( std::cout );
// }

/**
 * Estableix la funció de transferència de color pel vtkVolume corresponent
 * a l'índex donat.
 */
// void OptimalViewpointVolume::setColorTransferFunction( vtkColorTransferFunction * colorTransferFunction )
// {
//     m_colorTransferFunction->Delete();
//     m_colorTransferFunction = colorTransferFunction; m_colorTransferFunction->Register( 0 );
//     m_volumeProperty->SetColor( colorTransferFunction );
//     colorTransferFunction->Print( std::cout );
// }

/**
 * Sincronitza les tranformacions de tots els vtkVolumes. Concretament,
 * aplica la transformació del vtkVolume amb índex 0 a tots els altres
 * vtkVolumes.
 */
void OptimalViewpointVolume::synchronize()
{
    //m_planeVolume->PokeMatrix( m_mainVolume->GetMatrix() );
    //m_planeVolume->ComputeMatrix();
}

void OptimalViewpointVolume::handle( int rayId, int offset )
{
    emit visited( rayId, *(m_labeledData + offset) );
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



signed char OptimalViewpointVolume::loadSegmentationFromFile( const QString & segmentationFileName )
{
    std::cout << "OVV::ldSegFromFile(): " << qPrintable( segmentationFileName ) << std::endl;
    QFile segFile( segmentationFileName );

    if ( !segFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        std::cerr << "OVV::ldSegFromFile(): cannot open file " << qPrintable( segmentationFileName ) << std::endl;
        return -1;
    }

    QVector< unsigned char > limits;
    QTextStream in( &segFile );

    while ( !in.atEnd() && limits.size() < 5 )  /// \warning Es llegirà un màxim de 5 límits.
    {
        QString sValue;
        in >> sValue;
        bool ok;
        ushort usValue = sValue.toUShort( &ok );
        if ( ok && usValue <= m_rangeMax ) limits.push_back( usValue );
        else
        {
            WARN_LOG( "OVV::ldSegFromFile(): S'ha aturat la lectura del fitxer de segmentació perquè s'ha trobat un valor no vàlid" );
            break;
        }
    }

    segFile.close();

    if ( limits.size() == 0 )
    {
        std::cerr << "OVV::ldSegFromFile(): no limits read" << std::endl;
        return -1;
    }

    labelize( limits );
    generateAdjustedTransferFunction( limits );

    return limits.size() + 1;
}



unsigned char OptimalViewpointVolume::segmentateVolume( unsigned short iterations, unsigned char numberOfClusters, double noise )
{
    unsigned short i, k1, k2;   // j, p
    unsigned char nLabels = numberOfClusters - 1;
    unsigned char minLevel = m_rangeMin;
    unsigned char maxLevel = m_rangeMax;
    double maxExcessEntropy = 0;
    QVector<unsigned char> limits( nLabels );
    QVector<unsigned char> sortLimits( nLabels );
    QVector<unsigned char> maxLimits( nLabels );
    long seed = 1979;   // time( 0 );
    unsigned short maxIteration;
    bool genetic = true;
    bool swapping = true;
    bool finest = true;
    int numbucleswap = 2;


    for ( i = 0; i < nLabels; i++ )
    {
        limits[i] = minLevel + ( i + 1 ) * maxLevel / numberOfClusters;
    }

    // genetic algorithm
    if( genetic )
    {
        for ( k1 = 0; k1 < iterations; k1++ )
        {
            std::cout << "iteration " << k1 << std::flush;
//             copy( limits.begin(), limits.end(), sortLimits.begin() );
            sortLimits = limits;
            qSort( sortLimits.begin(), sortLimits.end() );
            std::cout << "==> labels: ";

            for( i = 0; i < sortLimits.size(); i++ )
                std::cout << (short) sortLimits[i] << " ";

            std::cout << "==> Maxlabels: [";

            for ( i = 0; i < maxLimits.size(); i++ )
                std::cout << (short) maxLimits[i] << ", ";

            std::cout << "]" << std::endl;

            labelize( sortLimits ); /// \TODO es podria millorar perquè ara fa dos etiquetatges
            std::cout << "Imatge labelitzada!!" << std::endl;



            // calcular excess entropy
            emit needsExcessEntropy();
            std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

            if ( m_excessEntropy > maxExcessEntropy )
            {
                maxExcessEntropy = m_excessEntropy;
                maxIteration = k1;
//                 copy( sortLimits.begin(), sortLimits.end(), maxLimits.begin() );
                maxLimits = sortLimits;
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

        std::cout << "MAX GENETIC ALGORITHM: Iteration " << maxIteration << " ==> Max entropy = " << maxExcessEntropy << ", per " << std::flush;

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
//                 copy( maxLimits.begin(), maxLimits.end(), limits.begin() );
                limits = maxLimits;

                unsigned char pas = ( maxLevel - minLevel ) / 32;
                if ( pas == 0 ) pas = 1;

                for( k2 = minLevel; k2 < maxLevel; k2 += pas )
                {
                    std::cout << "iteration " << k1 << ": " << k2 << std::flush;
                    limits[k1] = k2;
//                     copy( limits.begin(), limits.end(), sortLimits.begin() );
                    sortLimits = limits;
                    qSort( sortLimits.begin(), sortLimits.end() );
                    std::cout << "==> labels: ";

                    for ( i = 0; i < sortLimits.size(); i++ )
                        std::cout << (short) sortLimits[i] << " ";

                    std::cout << "==> Maxlabels: [";

                    for ( i = 0; i < maxLimits.size(); i++ )
                        std::cout << (short) maxLimits[i] << ", ";

                    std::cout << "]" << std::endl;

                    labelize( sortLimits ); /// \TODO es podria millorar perquè ara fa dos etiquetatges
                    std::cout << "Imatge labelitzada!!" << std::endl;

                    // calcular excess entropy
                    emit needsExcessEntropy();
                    std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                    if ( m_excessEntropy > maxExcessEntropy )
                    {
                        maxExcessEntropy = m_excessEntropy;
                        maxIteration = k1;
//                         copy( limits.begin(), limits.end(), maxLimits.begin() );
                        maxLimits = limits;
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
//                 copy( maxLimits.begin(), maxLimits.end(), limits.begin() );
                limits = maxLimits;
                limits[k1] = limits[k1] - 1;
                std::cout << "iteration " << k1 << ": " << (short) limits[k1] << std::flush;
//                 copy( limits.begin(), limits.end(), sortLimits.begin() );
                sortLimits = limits;
                qSort( sortLimits.begin(), sortLimits.end() );
                std::cout << "==> labels: ";

                for ( i = 0; i < sortLimits.size(); i++ )
                    std::cout << (short) sortLimits[i] << " ";

                std::cout << "==> Maxlabels: [";

                for ( i = 0; i < maxLimits.size(); i++ )
                    std::cout << (short) maxLimits[i] << ", ";

                std::cout << "]" << std::endl;

                labelize( sortLimits ); /// \TODO es podria millorar perquè ara fa dos etiquetatges
                std::cout << "Imatge labelitzada!!" << std::endl;

                // calcular excess entropy
                emit needsExcessEntropy();
                std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                if ( m_excessEntropy > maxExcessEntropy )
                {
                    maxExcessEntropy = m_excessEntropy;
//                     copy( limits.begin(), limits.end(), maxLimits.begin() );
                    maxLimits = limits;
                    canviat = true;
                    std::cout << "** is maximum **" << std::endl;
                }

                // cap a l'altra banda
                limits[k1] = limits[k1] + 2;    // ara limits[k1] val "limits[k1] - 1"
                std::cout << "iteration " << k1 << ": " << (short) limits[k1] << std::flush;
//                 copy( limits.begin(), limits.end(), sortLimits.begin() );
                sortLimits = limits;
                qSort( sortLimits.begin(), sortLimits.end() );
                std::cout << "==> labels: ";

                for ( i = 0; i < sortLimits.size(); i++ )
                    std::cout << (short) sortLimits[i] << " ";

                std::cout << "==> Maxlabels: [";

                for ( i = 0; i < maxLimits.size(); i++ )
                    std::cout << (short) maxLimits[i] << ", ";

                std::cout << "]" << std::endl;

                labelize( sortLimits ); /// \TODO es podria millorar perquè ara fa dos etiquetatges
                std::cout << "Imatge labelitzada!!" << std::endl;

                // calcular excess entropy
                emit needsExcessEntropy();
                std::cout << "Exces entropy (" << k1 << ") => " << m_excessEntropy << std::endl;

                if ( m_excessEntropy > maxExcessEntropy )
                {
                    maxExcessEntropy = m_excessEntropy;
//                     copy( limits.begin(), limits.end(), maxLimits.begin() );
                    maxLimits = limits;
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

    // desem la imatge labelitzada
//     copy( maxLimits.begin(), maxLimits.end(), sortLimits.begin() );
    sortLimits = maxLimits;
    qSort( sortLimits.begin(), sortLimits.end() );


    labelize( sortLimits );
    std::cout << "Imatge Final labelitzada!!" << std::endl;

    generateAdjustedTransferFunction( sortLimits );

    return nLabels + 1; // retornem el nombre de regions trobades
}



void OptimalViewpointVolume::labelize( const QVector< unsigned char > & limits )
{
    unsigned char * fixIt = m_data;
//    IteratorType grounIt(GrounImage, GrounImage->GetBufferedRegion());
    unsigned char * segIt = m_labeledData;
//     unsigned char * seg2It = m_segmentedData;    // ja no cal
    unsigned char * fixItEnd = m_data + m_dataSize;

//     int cont = 0;

    while ( fixIt < fixItEnd )
    {
        unsigned char i = 0;
        unsigned char value = *fixIt;

        while ( i < limits.size() && value > limits[i] )
        {
            i++;
        }

//         cont++;

        *segIt = i;

//         if ( i == 0 )
//         {
//             *seg2It = limits[i] / 2;
//         }
//         else if ( i == limits.size() )
//         {
//             *seg2It = limits[i-1] + (256 - limits[i-1]) / 2;
//         }
//         else
//         {
//             *seg2It = limits[i-1] + (limits[i] + 1 - limits[i-1]) / 2;
//         }


        // ja no cal
//         unsigned char a = ( i == 0 ) ? m_rangeMin : limits[i-1];
//         unsigned short b = 1 + ( ( i == limits.size() ) ? m_rangeMax : limits[i] );
//         *seg2It = a + ( b - a ) / 2;




        ++fixIt;
//        ++grounIt;
        ++segIt;
//         ++seg2It;    // ja no cal
    }
}



void OptimalViewpointVolume::generateAdjustedTransferFunction( const QVector< unsigned char> & limits )
{
    srand( time( 0 ) );

    TransferFunction adjustedTransferFunction;
    int r, g, b, a;

    r = static_cast<int>( rand() % 256 );
    g = static_cast<int>( rand() % 256 );
    b = static_cast<int>( rand() % 256 );
    a = 1;
//     adjustedTransferFunction << QGradientStop( 0.0, QColor( r, g, b, a ) )
//                              << QGradientStop( limits[0] / 255.0, QColor( r, g, b, a ) );
    adjustedTransferFunction.addPoint( m_rangeMin, QColor( r, g, b, a ) );
    adjustedTransferFunction.addPoint( limits[0], QColor( r, g, b, a ) );

    for ( unsigned char i = 0; i < limits.size() - 1; i++ )
    {
        r = static_cast<int>( rand() % 256 );
        g = static_cast<int>( rand() % 256 );
        b = static_cast<int>( rand() % 256 );
        a = static_cast<int>( rand() % 256 );
//         adjustedTransferFunction << QGradientStop( ( limits[i] + 1 ) / 255.0, QColor( r, g, b, a ) )
//                                  << QGradientStop( limits[i + 1] / 255.0, QColor( r, g, b, a ) );
        adjustedTransferFunction.addPoint( limits[i] + 1, QColor( r, g, b, a ) );
        adjustedTransferFunction.addPoint( limits[i+1], QColor( r, g, b, a ) );
    }   // i == nLabels - 1

    r = static_cast<int>( rand() % 256 );
    g = static_cast<int>( rand() % 256 );
    b = static_cast<int>( rand() % 256 );
    a = static_cast<int>( rand() % 256 );
//     adjustedTransferFunction << QGradientStop( ( limits[limits.size()-1] + 1 ) / 255.0, QColor( r, g, b, a ) )
//                              << QGradientStop( 1.0, QColor( r, g, b, a ) );
    adjustedTransferFunction.addPoint( limits[limits.size()-1] + 1, QColor( r, g, b, a ) );
    adjustedTransferFunction.addPoint( m_rangeMax, QColor( r, g, b, a ) );

    std::cout << "OVV: atf:" << std::endl;
    adjustedTransferFunction.print();
    std::cout << "-------------------" << std::endl;

//     TransferFunction * atf = TransferFunctionIO::fromFile( "/scratch/tf0.tf" );

    emit adjustedTransferFunctionDefined( adjustedTransferFunction );
//     emit adjustedTransferFunctionDefined( *atf );

//     delete atf;
}



void OptimalViewpointVolume::setComputing( bool on )
{
//    m_planeVolumeRayCastFunction->setComputing( on );
}



void OptimalViewpointVolume::setSegmentationFileName( QString name )
{
    //m_segmentationFileName = name;
}



// nous paràmetres



void OptimalViewpointVolume::setOpacityForComputing( bool on )
{
//    m_planeVolumeRayCastFunction->setOpacityOn( on );
}



void OptimalViewpointVolume::setInterpolation( int interpolation )
{
    switch ( interpolation )
    {
        case INTERPOLATION_NEAREST_NEIGHBOUR:
            m_property->SetInterpolationTypeToNearest();
            break;
        case INTERPOLATION_LINEAR_INTERPOLATE_CLASSIFY:
            m_property->SetInterpolationTypeToLinear();
            m_mainVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
//            m_planeVolumeRayCastFunction->SetCompositeMethodToInterpolateFirst();
            m_volumeRayCastFunctionObscurances->SetCompositeMethodToInterpolateFirst();
            m_volumeRayCastFunctionViewpointSaliency->SetCompositeMethodToInterpolateFirst();
            m_volumeRayCastFunctionFx->SetCompositeMethodToInterpolateFirst();
            m_volumeRayCastFunctionFx2->SetCompositeMethodToInterpolateFirst();
            break;
        case INTERPOLATION_LINEAR_CLASSIFY_INTERPOLATE:
            m_property->SetInterpolationTypeToLinear();
            m_mainVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            //m_planeVolumeRayCastFunction->SetCompositeMethodToClassifyFirst();
            m_volumeRayCastFunctionObscurances->SetCompositeMethodToClassifyFirst();
            m_volumeRayCastFunctionViewpointSaliency->SetCompositeMethodToClassifyFirst();
            m_volumeRayCastFunctionFx->SetCompositeMethodToClassifyFirst();
            m_volumeRayCastFunctionFx2->SetCompositeMethodToClassifyFirst();
            break;
    }
}



void OptimalViewpointVolume::setSpecular( bool on )
{
    m_property->SetSpecular( on ? 1.0 : 0.0 );
}



void OptimalViewpointVolume::setSpecularPower( double specularPower )
{
    m_property->SetSpecularPower( specularPower );
}


unsigned char OptimalViewpointVolume::getRangeMin() const
{
    return m_rangeMin;
}


unsigned char OptimalViewpointVolume::getRangeMax() const
{
    return m_rangeMax;
}


void OptimalViewpointVolume::setRenderCluster( bool renderCluster )
{
    if ( renderCluster ) m_mapper->SetInput( m_clusterImage );
    else m_mapper->SetInput( m_image );
}


void OptimalViewpointVolume::setClusterLimits( unsigned short first, unsigned short last )
{
    if ( m_clusterFirst != first || m_clusterLast != last )
    {
        vtkImageClip * clip = vtkImageClip::New();
        clip->SetInput( m_image );
        int dims[3];
        m_image->GetDimensions( dims );
        clip->SetOutputWholeExtent( 0, dims[0] - 1, 0, dims[1] - 1, first, last );
        clip->ClipDataOn();
        clip->Update();
        if ( m_clusterImage ) m_clusterImage->Delete();
        m_clusterImage = clip->GetOutput(); m_clusterImage->Register( 0 );
        clip->Delete();
    }

    m_clusterFirst = first; m_clusterLast = last;
}


signed char OptimalViewpointVolume::rescale( int bins )
{
    QVector< unsigned char > limits;

    int nValues = m_rangeMax - m_rangeMin + 1;

    for ( int i = 1; i < bins; i++ )
    {
        limits.push_back( i * nValues / bins - 1 );
    }

    labelize( limits );
    generateAdjustedTransferFunction( limits );

    return limits.size() + 1;
}


// synchronized? potser no, si els threads es reparteixen el model sense interseccions
void OptimalViewpointVolume::handleObscurances( int rayId, int offset )
{
    Q_UNUSED(rayId);
    Q_UNUSED(offset);
//     emit visited( rayId, *(m_labeledData + offset) );
}

void OptimalViewpointVolume::endRayObscurances( int rayId )
{
    Q_UNUSED(rayId);
//     emit rayEnd( rayId );
}

// versió amb threads (deprecated)
void OptimalViewpointVolume::computeObscurances()
{
    synchronize();

    // Alliberem memòria, per començar
    delete [] m_obscurance; m_obscurance = 0;
    delete [] m_colorBleeding; m_colorBleeding = 0;

    vtkDirectionEncoder * directionEncoder = m_mapper->GetGradientEstimator()->GetDirectionEncoder();
    unsigned short * encodedNormals = m_mapper->GetGradientEstimator()->GetEncodedNormals();

    // càlcul de direccions
    POVSphereCloud cloud( 1.0, m_obscuranceDirections );    // 0 -> 12 dir, 1 -> 42 dir, 2 -> 162 dir
    cloud.createPOVCloud();
    const QVector<Vector3> & directions = cloud.getVertices();

    // variables necessàries
    int dimensions[3];
    m_image->GetDimensions( dimensions );
    int increments[3];
    m_image->GetIncrements( increments );

    int numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();
    QVector<ObscuranceThread *> threads(numberOfThreads);

    for ( int i = 0; i < numberOfThreads; i++ )
    {
        ObscuranceThread * thread = new ObscuranceThread( i, numberOfThreads, directions, m_transferFunction, this );
        thread->setNormals( directionEncoder, encodedNormals );
        thread->setData( m_data, m_dataSize, dimensions, increments );
        thread->setObscuranceParameters( m_obscuranceMaximumDistance, m_obscuranceFunction, m_obscuranceVariant );
        thread->start();
        threads[i] = thread;
    }

    if ( m_obscuranceVariant <= OpacitySmooth ) // obscurances
    {
        double maximumObscurance = 0.0;

        for ( int i = 0; i < numberOfThreads; i++ )
        {
            ObscuranceThread * thread = threads[i];
            thread->wait();

            double * threadObscurance = thread->getObscurance();

            if ( i == 0 )
            {
                m_obscurance = threadObscurance;
            }
            else
            {
                for ( int j = 0; j < m_dataSize; j++ )
                {
                    m_obscurance[j] += threadObscurance[j];
                    if ( m_obscurance[j] > maximumObscurance ) maximumObscurance = m_obscurance[j];
                }
            }

            delete thread;
        }

        for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] /= maximumObscurance;

        {
            bool density = m_obscuranceVariant <= DensitySmooth;
            // obscurances to file
            QFile outFile( QDir::tempPath().append( QString( "/obscurance.raw" ) ) );
            if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &outFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
                    uchar value = m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) ? static_cast<uchar>( qRound( m_obscurance[i] * 255.0 ) ) : 0;
                    out << value;
    //                 out << gradientMagnitudes[i];
    //                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[i] );
    //                 out << static_cast<uchar>( round( ( uGradient[selection] + 1.0 ) * 127.5 ) );
                }
                outFile.close();
            }
            QFile outFileMhd( QDir::tempPath().append( QString( "/obscurance.mhd" ) ) );
            if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream out( &outFileMhd );
                out << "NDims = 3\n";
                out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
                double spacing[3];
                m_image->GetSpacing( spacing );
                out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
                out << "ElementType = MET_UCHAR\n";
                out << "ElementDataFile = obscurance.raw";
                outFileMhd.close();
            }
        }

    //     for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] *= 1.272;    // raó àuria

        m_volumeRayCastFunctionObscurances->SetObscurance( m_obscurance );
        m_volumeRayCastFunctionObscurances->SetColor( false );
    }
    else    // color bleeding
    {
        double maximumObscurance = 0.0;

        for ( int i = 0; i < numberOfThreads; i++ )
        {
            ObscuranceThread * thread = threads[i];
            thread->wait();

            Vector3 * threadColorBleeding = thread->getColorBleeding();

            if ( i == 0 )
            {
                m_colorBleeding = threadColorBleeding;
            }
            else
            {
                for ( int j = 0; j < m_dataSize; j++ )
                {
                    m_colorBleeding[j] += threadColorBleeding[j];
                    if ( m_colorBleeding[j].x > maximumObscurance ) maximumObscurance = m_colorBleeding[j].x;
                    if ( m_colorBleeding[j].y > maximumObscurance ) maximumObscurance = m_colorBleeding[j].y;
                    if ( m_colorBleeding[j].z > maximumObscurance ) maximumObscurance = m_colorBleeding[j].z;
                }
            }

            delete thread;
        }

        for ( int i = 0; i < m_dataSize; i++ ) m_colorBleeding[i] /= maximumObscurance;

        {
            bool density = m_obscuranceVariant <= DensitySmooth/*ColorBleeding*/;
            // obscurances to file
            QFile outFile( QDir::tempPath().append( QString( "/colorbleeding.raw" ) ) );
            if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &outFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
//                     uchar value0 = 0, valueR = 0, valueG = 0, valueB = 0;
//                     if ( m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) )
//                     {
//                         Vector3 color = 255.0 * m_colorBleeding[i];
//                         valueR = static_cast<uchar>( qRound( color.x ) );
//                         valueG = static_cast<uchar>( qRound( color.y ) );
//                         valueB = static_cast<uchar>( qRound( color.z ) );
//                     }
//                     out << value0 << valueR << valueG << valueB;

                    uchar value = 0;
                    if ( m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) )
                    {
                        Vector3 color = 3.0 * m_colorBleeding[i];
                        value += static_cast<uchar>( qRound( color.x ) ) << 4;
                        value += static_cast<uchar>( qRound( color.y ) ) << 2;
                        value += static_cast<uchar>( qRound( color.z ) );
                    }   // value = 00RRGGBB (base 2)
                    out << value;

    //                 out << gradientMagnitudes[i];
    //                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[i] );
    //                 out << static_cast<uchar>( round( ( uGradient[selection] + 1.0 ) * 127.5 ) );
                }
                outFile.close();
            }
            QFile outFileMhd( QDir::tempPath().append( QString( "/colorbleeding.mhd" ) ) );
            if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream out( &outFileMhd );
                out << "NDims = 3\n";
                out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
                double spacing[3];
                m_image->GetSpacing( spacing );
                out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
//                 out << "ElementType = MET_UINT\n";
                out << "ElementType = MET_UCHAR\n";
                out << "ElementByteOrderMSB = True\n";
                out << "ElementDataFile = colorbleeding.raw";
                outFileMhd.close();
            }
        }

    //     for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] *= 1.272;    // raó àuria

        m_volumeRayCastFunctionObscurances->SetColorBleeding( m_colorBleeding );
        m_volumeRayCastFunctionObscurances->SetColor( true );
    }
}


// QList<Vector3> OptimalViewpointVolume::getLineStarts( int dimX, int dimY, int dimZ, const Vector3 & forward ) const
// {
//     // llista dels vòxels que són començament de línia
//     QList<Vector3> lineStarts;
//
//     // tots els (0,y,z) són començament de línia
//     Vector3 lineStart( 0, 0, 0 );
//     for ( int iy = 0; iy < dimY; iy++ )
//     {
//         lineStart.y = iy;
//         for ( int iz = 0; iz < dimZ; iz++ )
//         {
//             lineStart.z = iz;
//             lineStarts << lineStart;
// //             DEBUG_LOG( QString( "line start: (%1,%2,%3)" ).arg( lineStart.x ).arg( lineStart.y ).arg( lineStart.z ) );
//         }
//     }
//     DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.count() ) );
//
//     // més començaments de línia
//     Vector3 rv;
//     Voxel v = { 0, 0, 0 }, pv = v;
//
//     // iterar per la línia que comença a (0,0,0)
//     while ( v.x < dimX )
//     {
//         if ( v.y != pv.y )
//         {
//             lineStart.x = rv.x; lineStart.y = rv.y - v.y;   // y = 0
//             for ( double iz = rv.z - v.z; iz < dimZ; iz++ )
//             {
//                 lineStart.z = iz;
//                 lineStarts << lineStart;
//             }
//         }
//         if ( v.z != pv.z )
//         {
//             lineStart.x = rv.x; lineStart.z = rv.z - v.z;   // z = 0
//             for ( double iy = rv.y - v.y; iy < dimY; iy++ )
//             {
//                 lineStart.y = iy;
//                 lineStarts << lineStart;
//             }
//         }
//
//         // avançar el vòxel
//         rv += forward;
//         pv = v;
//         v.x = round( rv.x ); v.y = round( rv.y ); v.z = round( rv.z );
//     }
//     DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.count() ) );
//
//     return lineStarts;
// }


void OptimalViewpointVolume::setObscuranceDirections( int obscuranceDirections )
{
    m_obscuranceDirections = obscuranceDirections;
}


void OptimalViewpointVolume::setObscuranceMaximumDistance( double obscuranceMaximumDistance )
{
    m_obscuranceMaximumDistance = obscuranceMaximumDistance;
}


void OptimalViewpointVolume::setObscuranceFunction( ObscuranceFunction obscuranceFunction )
{
    m_obscuranceFunction = obscuranceFunction;
}


void OptimalViewpointVolume::setObscuranceVariant( ObscuranceVariant obscuranceVariant )
{
    m_obscuranceVariant = obscuranceVariant;
}


// inline double OptimalViewpointVolume::obscurance( double distance ) const
// {
//     if ( distance > m_obscuranceMaximumDistance ) return 1.0;
//
//     switch ( m_obscuranceFunction )
//     {
//         case Constant0: return 0.0;
//         case SquareRoot: return sqrt( distance / m_obscuranceMaximumDistance );
//         case Exponential: return 1.0 - exp( distance / m_obscuranceMaximumDistance );
//     }
// }


void OptimalViewpointVolume::setRenderWithObscurances( bool renderWithObscurances )
{
    DEBUG_LOG( QString( "srwo:b (%1,%2)" ).arg( m_renderWithObscurances ).arg( renderWithObscurances ) );
    //if ( m_renderWithObscurances == renderWithObscurances ) return; // això no passarà mai mentre fem servir OVParameters

    m_renderWithObscurances = renderWithObscurances;

    m_volumeRayCastFunctionFx->SetFxObscurance( renderWithObscurances );

    // no podem fer-ho a l'if de més avall perquè surt amb l'if m_fx
    if ( renderWithObscurances )
    {
        if ( m_obscurance )
        {
            if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
                m_volumeRayCastFunctionFx2->AddVoxelShader( m_obscuranceVoxelShader );

            m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
        }
        else
        {
            m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );

            if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_colorBleedingVoxelShader ) < 0 )
                m_volumeRayCastFunctionFx2->AddVoxelShader( m_colorBleedingVoxelShader );
        }
    }
    else
    {
        m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );
        m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
    }
    m_volumeRayCastFunctionFx2->Print( std::cout );

    if ( m_fx ) return;

    if ( m_renderWithObscurances )
    {
        m_mapper->SetVolumeRayCastFunction( m_volumeRayCastFunctionObscurances );
    }
    else
    {
        m_mapper->SetVolumeRayCastFunction( m_mainVolumeRayCastFunction );
    }
    DEBUG_LOG( "srwo:e" );
}


void OptimalViewpointVolume::setObscurancesFactor( double obscurancesFactor )
{
    m_volumeRayCastFunctionObscurances->SetObscuranceFactor( obscurancesFactor );
    m_volumeRayCastFunctionFx->SetObscuranceFactor( obscurancesFactor );
    m_obscuranceVoxelShader->setFactor( obscurancesFactor );
    m_colorBleedingVoxelShader->setFactor( obscurancesFactor );
}


void OptimalViewpointVolume::setObscurancesFilters( double obscurancesFilterLow, double obscurancesFilterHigh )
{
    m_volumeRayCastFunctionObscurances->SetObscuranceFilterLow( obscurancesFilterLow );
    m_volumeRayCastFunctionObscurances->SetObscuranceFilterHigh( obscurancesFilterHigh );
    m_volumeRayCastFunctionFx->SetObscuranceFilterLow( obscurancesFilterLow );
    m_volumeRayCastFunctionFx->SetObscuranceFilterHigh( obscurancesFilterHigh );
    m_obscuranceVoxelShader->setFilters( obscurancesFilterLow, obscurancesFilterHigh );
}


void OptimalViewpointVolume::computeSaliency()
{
    if ( !m_obscurance ) return;

    vtkDoubleArray * obscuranceArray = vtkDoubleArray::New();
    obscuranceArray->SetArray( m_obscurance, m_dataSize, 1 );

    vtkImageData * obscuranceData = vtkImageData::New();
    obscuranceData->CopyStructure( m_image );
    obscuranceData->SetScalarTypeToDouble();

    vtkPointData * obscurancePointData = obscuranceData->GetPointData();
    obscurancePointData->SetScalars( obscuranceArray );

    vtk4DLinearRegressionGradientEstimator * gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
    gradientEstimator->SetInput( obscuranceData );

    unsigned char * gradientMagnitudes = gradientEstimator->GetGradientMagnitudes();
    delete [] m_saliency;
    m_saliency = new double[m_dataSize];
    double maxSaliency = 0.0;
    for (int i = 0; i < m_dataSize; i++) {
        if (gradientMagnitudes[i] > maxSaliency) maxSaliency = gradientMagnitudes[i];
    }

    for (int i = 0; i < m_dataSize; i++) m_saliency[i] = gradientMagnitudes[i] / maxSaliency;

    m_volumeRayCastFunctionFx->SetSaliency( m_saliency );
    m_saliencyVoxelShader->setSaliency( m_saliency );

    {
        bool density = m_obscuranceVariant <= DensitySmooth;
        // saliency to file
        QFile outFile( QDir::tempPath().append( QString( "/saliency.raw" ) ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QDataStream out( &outFile );
            for ( int i = 0; i < m_dataSize; ++i )
            {
                uchar value = m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) ? static_cast<uchar>( qRound( 255.0 * m_saliency[i] ) ) : 0;
                out << value;
            }
            outFile.close();
        }
        QFile outFileMhd( QDir::tempPath().append( QString( "/saliency.mhd" ) ) );
        if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFileMhd );
            out << "NDims = 3\n";
            int dimensions[3];
            m_image->GetDimensions( dimensions );
            out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
            double spacing[3];
            m_image->GetSpacing( spacing );
            out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
            out << "ElementType = MET_UCHAR\n";
            out << "ElementDataFile = saliency.raw";
            outFileMhd.close();
        }
    }



//     // phong amb saliency
//     ushort * gradientNormals = m_mainMapper->GetGradientEstimator()->GetEncodedNormals();
//     ushort * saliencyNormals = gradientEstimator->GetEncodedNormals();
//     for (int i = 0; i < m_dataSize; i++) gradientNormals[i] = saliencyNormals[i];



    obscuranceArray->Delete();
    obscuranceData->Delete();
    gradientEstimator->Delete();
}


void OptimalViewpointVolume::reduceToHalf()
{
    int dimensions[3];
    m_image->GetDimensions( dimensions );
    int increments[3];
    m_image->GetIncrements( increments );

    int dimX = dimensions[0], dimY = dimensions[1], dimZ = dimensions[2];
    int incX = increments[0], incY = increments[1], incZ = increments[2];
    int newDimX = dimX / 2 + dimX % 2, newDimY = dimY / 2 + dimY % 2, newDimZ = dimZ / 2 + dimZ % 2;
    int newIncX = 1, newIncY = newDimX, newIncZ = newDimX * newDimY;
    int newDataSize = newDimX * newDimY * newDimZ;

    unsigned char * newData = new unsigned char[newDataSize];

    for (int ix = 0, nx = 0; ix < dimX; ix += 2, nx++)
    {
        for (int iy = 0, ny = 0; iy < dimY; iy += 2, ny++)
        {
            for (int iz = 0, nz = 0; iz < dimZ; iz += 2, nz++)
            {
                float value = 0.0;
                unsigned char n = 0;

                for (int i = 0, x = ix; i < 2 && x < dimX; i++, x++)
                {
                    for (int j = 0, y = iy; j < 2 && y < dimY; j++, y++)
                    {
                        for (int k = 0, z = iz; k < 2 && z < dimZ; k++, z++)
                        {
                            int offset = x * incX + y * incY + z * incZ;
                            Q_ASSERT( offset < m_dataSize );
                            value += m_data[offset];
                            n++;
                        }
                    }
                }

                unsigned char newValue = qRound( value / n );
                int newOffset = nx * newIncX + ny * newIncY + nz * newIncZ;
                Q_ASSERT( newOffset < newDataSize );
                newData[newOffset] = newValue;
            }
        }
    }

    // new data to file
    QFile outFile( QDir::tempPath().append( QString( "/newdata.raw" ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QDataStream out( &outFile );
        for ( int i = 0; i < newDataSize; i++ )
        {
            out << newData[i];
        }
        outFile.close();
    }
    QFile outFileMhd( QDir::tempPath().append( QString( "/newdata.mhd" ) ) );
    if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFileMhd );
        out << "NDims = 3\n";
        out << "DimSize = " << newDimX << " " << newDimY << " " << newDimZ << "\n";
        double spacing[3];
        m_image->GetSpacing( spacing );
        out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
        out << "ElementType = MET_UCHAR\n";
        out << "ElementDataFile = newdata.raw";
        outFileMhd.close();
    }
}


// nova versió amb threads
void OptimalViewpointVolume::computeObscurances2()
{
    synchronize();

    // Guardem en un booleà si treballem amb color o sense
    bool color = m_obscuranceVariant >= OpacityColorBleeding;

    // Preparem el vector que farem servir, reaprofitant memòria si podem
    if ( !color )   // obscurances
    {
        delete m_colorBleeding; m_colorBleeding = 0;
        if ( !m_obscurance ) m_obscurance = new double[m_dataSize];
        for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] = 0.0;
    }
    else    // color bleeding
    {
        delete m_obscurance; m_obscurance = 0;
        if ( !m_colorBleeding ) m_colorBleeding = new Vector3[m_dataSize];
        else for ( int i = 0; i < m_dataSize; i++ ) m_colorBleeding[i] = Vector3();
    }

    vtkDirectionEncoder * directionEncoder = m_mapper->GetGradientEstimator()->GetDirectionEncoder();
    unsigned short * encodedNormals = m_mapper->GetGradientEstimator()->GetEncodedNormals();

    // càlcul de direccions
    POVSphereCloud cloud( 1.0, m_obscuranceDirections );    // 0 -> 12 dir, 1 -> 42 dir, 2 -> 162 dir
    cloud.createPOVCloud();
    const QVector<Vector3> & directions = cloud.getVertices();
    int nDirections = directions.size();

    // variables necessàries
    int dimensions[3];
    m_image->GetDimensions( dimensions );
    int increments[3];
    m_image->GetIncrements( increments );

    // Creem els threads
    int numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();
    QVector<ObscuranceThread2 *> threads(numberOfThreads);

    for ( int i = 0; i < numberOfThreads; i++ )
    {
        ObscuranceThread2 * thread = new ObscuranceThread2( i, numberOfThreads, m_transferFunction, this );
        thread->setNormals( directionEncoder, encodedNormals );
        thread->setData( m_data, m_dataSize, dimensions, increments );
        thread->setObscuranceParameters( m_obscuranceMaximumDistance, m_obscuranceFunction, m_obscuranceVariant, m_obscurance, m_colorBleeding );
        thread->setSaliency( m_saliency, m_fxSaliencyA, m_fxSaliencyB, m_fxSaliencyLow, m_fxSaliencyHigh );
        threads[i] = thread;
    }

    // estructures de dades reaprofitables
    QVector<Vector3> lineStarts;

    // iterem per les direccions
    for ( int i = 0; i < nDirections; i++ )
    {
        const Vector3 & direction = directions.at( i );

        DEBUG_LOG( QString( "Direcció %1: %2" ).arg( i ).arg( direction.toString() ) );

        // direcció dominant (0 = x, 1 = y, 2 = z)
        int dominant;
        Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
        if ( absDirection.x >= absDirection.y )
        {
            if ( absDirection.x >= absDirection.z ) dominant = 0;
            else dominant = 2;
        }
        else
        {
            if ( absDirection.y >= absDirection.z ) dominant = 1;
            else dominant = 2;
        }

        // vector per avançar
        Vector3 forward;
        switch ( dominant )
        {
            case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
            case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
            case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
        }
        forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );

        // dimensions i increments segons la direcció dominant
        int x = dominant, y = ( dominant + 1 ) % 3, z = ( dominant + 2 ) % 3;
        int dimX = dimensions[x], dimY = dimensions[y], dimZ = dimensions[z];
        int incX = increments[x], incY = increments[y], incZ = increments[z];
        int sX = 1, sY = 1, sZ = 1;
        qptrdiff startDelta = 0;
        if ( forward.x < 0.0 )
        {
            startDelta += incX * ( dimX - 1 );
//             incX = -incX;
            forward.x = -forward.x;
            sX = -1;
        }
        if ( forward.y < 0.0 )
        {
            startDelta += incY * ( dimY - 1 );
//             incY = -incY;
            forward.y = -forward.y;
            sY = -1;
        }
        if ( forward.z < 0.0 )
        {
            startDelta += incZ * ( dimZ - 1 );
//             incZ = -incZ;
            forward.z = -forward.z;
            sZ = -1;
        }
        DEBUG_LOG( QString( "forward = " ) + forward.toString() );
        // ara els 3 components són positius

        // llista dels vòxels que són començament de línia
        getLineStarts( lineStarts, dimX, dimY, dimZ, forward );

//         int incXYZ[3] = { incX, incY, incZ };
        int xyz[3] = { x, y, z };
        int sXYZ[3] = { sX, sY, sZ };

        // iniciem els threads
        for ( int j = 0; j < numberOfThreads; j++ )
        {
            ObscuranceThread2 * thread = threads[j];
            thread->setPerDirectionParameters( direction, forward, xyz, sXYZ, lineStarts, startDelta );
            thread->start();
        }

        // esperem que acabin els threads
        for ( int j = 0; j < numberOfThreads; j++ )
        {
            threads[j]->wait();
        }
    }

    // destruïm els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        delete threads[j];
    }

    if ( !color ) // obscurances
    {
        double maximumObscurance = 0.0;

        for ( int i = 0; i < m_dataSize; i++ )
        {
            if ( m_obscurance[i] > maximumObscurance ) maximumObscurance = m_obscurance[i];
        }

        for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] /= maximumObscurance;

        {
            bool density = m_obscuranceVariant <= DensitySmooth;
            // obscurances to file
            QFile outFile( QDir::tempPath().append( QString( "/obscurance.raw" ) ) );
            if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &outFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
                    uchar value = m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) ? static_cast<uchar>( qRound( m_obscurance[i] * 255.0 ) ) : 0;
                    out << value;
    //                 out << gradientMagnitudes[i];
    //                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[i] );
    //                 out << static_cast<uchar>( round( ( uGradient[selection] + 1.0 ) * 127.5 ) );
                }
                outFile.close();
            }
            QFile outFileMhd( QDir::tempPath().append( QString( "/obscurance.mhd" ) ) );
            if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream out( &outFileMhd );
                out << "NDims = 3\n";
                out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
                double spacing[3];
                m_image->GetSpacing( spacing );
                out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
                out << "ElementType = MET_UCHAR\n";
                out << "ElementDataFile = obscurance.raw";
                outFileMhd.close();
            }

            QFile obscurancesFile( QDir::tempPath().append( QString( "/obscurance.dat" ) ) );
            if ( obscurancesFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &obscurancesFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
                    out << m_obscurance[i];
                }
                obscurancesFile.close();
            }
        }

    //     for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] *= 1.272;    // raó àuria

        m_volumeRayCastFunctionObscurances->SetObscurance( m_obscurance );
        m_volumeRayCastFunctionObscurances->SetColor( false );

        m_volumeRayCastFunctionFx->SetObscurance( m_obscurance );
        m_volumeRayCastFunctionFx->SetColor( false );

        m_obscuranceVoxelShader->setObscurance( m_obscurance );

        if ( m_renderWithObscurances )
        {
            if ( m_obscurance )
            {
                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_obscuranceVoxelShader );

                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
            }
            else
            {
                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );

                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_colorBleedingVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_colorBleedingVoxelShader );
            }
        }
    }
    else    // color bleeding
    {
        double maximumObscurance = 0.0;

        for ( int i = 0; i < m_dataSize; i++ )
        {
            if ( m_colorBleeding[i].x > maximumObscurance ) maximumObscurance = m_colorBleeding[i].x;
            if ( m_colorBleeding[i].y > maximumObscurance ) maximumObscurance = m_colorBleeding[i].y;
            if ( m_colorBleeding[i].z > maximumObscurance ) maximumObscurance = m_colorBleeding[i].z;
        }

        for ( int i = 0; i < m_dataSize; i++ ) m_colorBleeding[i] /= maximumObscurance;

        {
            bool density = m_obscuranceVariant <= DensitySmooth/*ColorBleeding*/;
            // obscurances to file
            QFile outFile( QDir::tempPath().append( QString( "/colorbleeding.raw" ) ) );
            if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &outFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
//                     uchar value0 = 0, valueR = 0, valueG = 0, valueB = 0;
//                     if ( m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) )
//                     {
//                         Vector3 color = 255.0 * m_colorBleeding[i];
//                         valueR = static_cast<uchar>( qRound( color.x ) );
//                         valueG = static_cast<uchar>( qRound( color.y ) );
//                         valueB = static_cast<uchar>( qRound( color.z ) );
//                     }
//                     out << value0 << valueR << valueG << valueB;

                    uchar value = 0;
                    if ( m_data[i] > 0 && ( density || m_transferFunction.getOpacity( m_data[i] ) > 0 ) )
                    {
                        Vector3 color = 3.0 * m_colorBleeding[i];
                        value += static_cast<uchar>( qRound( color.x ) ) << 4;
                        value += static_cast<uchar>( qRound( color.y ) ) << 2;
                        value += static_cast<uchar>( qRound( color.z ) );
                    }   // value = 00RRGGBB (base 2)
                    out << value;

    //                 out << gradientMagnitudes[i];
    //                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[i] );
    //                 out << static_cast<uchar>( round( ( uGradient[selection] + 1.0 ) * 127.5 ) );
                }
                outFile.close();
            }
            QFile outFileMhd( QDir::tempPath().append( QString( "/colorbleeding.mhd" ) ) );
            if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QTextStream out( &outFileMhd );
                out << "NDims = 3\n";
                out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
                double spacing[3];
                m_image->GetSpacing( spacing );
                out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
//                 out << "ElementType = MET_UINT\n";
                out << "ElementType = MET_UCHAR\n";
                out << "ElementByteOrderMSB = True\n";
                out << "ElementDataFile = colorbleeding.raw";
                outFileMhd.close();
            }

            QFile colorBleedingFile( QDir::tempPath().append( QString( "/colorbleeding.dat" ) ) );
            if ( colorBleedingFile.open( QFile::WriteOnly | QFile::Truncate ) )
            {
                QDataStream out( &colorBleedingFile );
                for ( int i = 0; i < m_dataSize; ++i )
                {
                    out << m_colorBleeding[i].x << m_colorBleeding[i].y << m_colorBleeding[i].z;
                }
                colorBleedingFile.close();
            }
        }

    //     for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] *= 1.272;    // raó àuria

        m_volumeRayCastFunctionObscurances->SetColorBleeding( m_colorBleeding );
        m_volumeRayCastFunctionObscurances->SetColor( true );

        m_volumeRayCastFunctionFx->SetColorBleeding( m_colorBleeding );
        m_volumeRayCastFunctionFx->SetColor( true );

        m_colorBleedingVoxelShader->setColorBleeding( m_colorBleeding );

        if ( m_renderWithObscurances )
        {
            if ( m_obscurance )
            {
                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_obscuranceVoxelShader );

                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
            }
            else
            {
                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );

                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_colorBleedingVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_colorBleedingVoxelShader );
            }
        }
    }
}


bool OptimalViewpointVolume::loadObscurances( const QString & obscurancesFileName, bool color )
{
    QFile obscurancesFile( obscurancesFileName );

    if ( !obscurancesFile.open( QFile::ReadOnly ) ) return false;

    QDataStream in( &obscurancesFile );

    if ( !color )   // obscurances
    {
        delete m_colorBleeding; m_colorBleeding = 0;
        if ( !m_obscurance ) m_obscurance = new double[m_dataSize];
        for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] = 0.0;

        for ( int i = 0; i < m_dataSize; i++ )
        {
            if ( in.atEnd() ) {
                ERROR_LOG( QString( "Not enough data in file " ).append( obscurancesFileName ) );
                return false;
            }
            else in >> m_obscurance[i];
        }

        m_volumeRayCastFunctionObscurances->SetObscurance( m_obscurance );
        m_volumeRayCastFunctionObscurances->SetColor( false );

        m_volumeRayCastFunctionFx->SetObscurance( m_obscurance );
        m_volumeRayCastFunctionFx->SetColor( false );

        m_obscuranceVoxelShader->setObscurance( m_obscurance );

        if ( m_renderWithObscurances )
        {
            if ( m_obscurance )
            {
                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_obscuranceVoxelShader );

                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
            }
            else
            {
                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );

                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_colorBleedingVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_colorBleedingVoxelShader );
            }
        }
    }
    else    // color bleeding
    {
        delete m_obscurance; m_obscurance = 0;
        if ( !m_colorBleeding ) m_colorBleeding = new Vector3[m_dataSize];
        else for ( int i = 0; i < m_dataSize; i++ ) m_colorBleeding[i] = Vector3();

        for ( int i = 0; i < m_dataSize; i++ )
        {
            Vector3 colorBleeding;
            if ( in.atEnd() ) {
                ERROR_LOG( QString( "Not enough data in file " ).append( obscurancesFileName ) );
                return false;
            }
            else in >> colorBleeding.x;
            if ( in.atEnd() ) {
                ERROR_LOG( QString( "Not enough data in file " ).append( obscurancesFileName ) );
                return false;
            }
            else in >> colorBleeding.y;
            if ( in.atEnd() ) {
                ERROR_LOG( QString( "Not enough data in file " ).append( obscurancesFileName ) );
                return false;
            }
            else in >> colorBleeding.z;
            m_colorBleeding[i] = colorBleeding;
        }

        m_volumeRayCastFunctionObscurances->SetColorBleeding( m_colorBleeding );
        m_volumeRayCastFunctionObscurances->SetColor( true );

        m_volumeRayCastFunctionFx->SetColorBleeding( m_colorBleeding );
        m_volumeRayCastFunctionFx->SetColor( true );

        m_colorBleedingVoxelShader->setColorBleeding( m_colorBleeding );

        if ( m_renderWithObscurances )
        {
            if ( m_obscurance )
            {
                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_obscuranceVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_obscuranceVoxelShader );

                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_colorBleedingVoxelShader );
            }
            else
            {
                m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_obscuranceVoxelShader );

                if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_colorBleedingVoxelShader ) < 0 )
                    m_volumeRayCastFunctionFx2->AddVoxelShader( m_colorBleedingVoxelShader );
            }
        }
    }

    obscurancesFile.close();

    return true;
}


void OptimalViewpointVolume::getLineStarts( QVector<Vector3> & lineStarts, int dimX, int dimY, int dimZ, const Vector3 & forward )
{
    lineStarts.resize( 0 );

    // tots els (0,y,z) són començament de línia
    Vector3 lineStart;  // (0,0,0)
    for ( int iy = 0; iy < dimY; ++iy )
    {
        lineStart.y = iy;
        for ( int iz = 0; iz < dimZ; ++iz )
        {
            lineStart.z = iz;
            lineStarts << lineStart;
//             DEBUG_LOG( QString( "line start: (%1,%2,%3)" ).arg( lineStart.x ).arg( lineStart.y ).arg( lineStart.z ) );
        }
    }
    DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.size() ) );

    // més començaments de línia
    Vector3 rv; // (0,0,0)
    Voxel v = { 0, 0, 0 }, pv = v;

    // iterar per la línia que comença a (0,0,0)
    while ( v.x < dimX )
    {
        if ( v.y != pv.y )
        {
            lineStart.x = rv.x; lineStart.y = rv.y - v.y;   // [y] = 0
            for ( double iz = rv.z - v.z; iz < dimZ; iz++ )
            {
                lineStart.z = iz;
                lineStarts << lineStart;
            }
        }
        if ( v.z != pv.z )
        {
            lineStart.x = rv.x; lineStart.z = rv.z - v.z;   // [z] = 0
            for ( double iy = rv.y - v.y; iy < dimY; iy++ )
            {
                lineStart.y = iy;
                lineStarts << lineStart;
            }
        }

        // avançar el vòxel
        rv += forward;
        pv = v;
        v.x = qRound( rv.x ); v.y = qRound( rv.y ); v.z = qRound( rv.z );
    }
    DEBUG_LOG( QString( "line starts: %1" ).arg( lineStarts.size() ) );
}


void OptimalViewpointVolume::computeViewpointSaliency( int numberOfDirections, vtkRenderer * renderer, bool divArea )
{
    if ( !m_saliency ) return;

    synchronize();

    m_volumeRayCastFunctionViewpointSaliency->SetSaliency( m_saliency );

    vtkVolumeRayCastFunction * current = m_mapper->GetVolumeRayCastFunction();
    m_mapper->SetVolumeRayCastFunction( m_volumeRayCastFunctionViewpointSaliency );

    vtkCamera * camera = renderer->GetActiveCamera();

    // càlcul de direccions
    POVSphereCloud cloud( 2.0 * m_volume->GetLength(), numberOfDirections );    // 0 -> 12 dir, 1 -> 42 dir, 2 -> 162 dir
    cloud.createPOVCloud();
    const QVector<Vector3> & directions = cloud.getVertices();
    int nDirections = directions.size();
    camera->SetFocalPoint( 0.0, 0.0, 0.0 );
    camera->SetViewUp( 0.0, 1.0, 0.0 );

    double * viewpointSaliency = new double[nDirections];

    for (int i = 0; i < nDirections; i++ )
    {
        Vector3 direction = directions[i];
        camera->SetPosition( direction.x, direction.y, direction.z );
        camera->OrthogonalizeViewUp();
        renderer->ResetCameraClippingRange();

        // inicialitzar viewpoint saliency
        m_accumulatedViewpointSaliencyPerThread.clear();
        m_pixelsPerThread.clear();

        QTime t;
        t.start();
        renderer->Render();
        int elapsed = t.elapsed();
        DEBUG_LOG( QString( "Time elapsed: %1 s" ).arg( elapsed / 1000.0 ) );

        // finalitzar viewpoint saliency
        viewpointSaliency[i] = 0.0;
        QHashIterator<int, double> it( m_accumulatedViewpointSaliencyPerThread );
        QHashIterator<int, uint> it2( m_pixelsPerThread );
        uint area = 0;
        while ( it.hasNext() )
        {
            viewpointSaliency[i] += it.next().value();
            area += it2.next().value();
        }
        if ( divArea ) viewpointSaliency[i] /= area;
    }

    int maxView = 0;
    double maxSaliency = 0.0;
    for (int i = 0; i < nDirections; i++) {
        DEBUG_LOG( QString( "view %1 %2: saliency = %3" ).arg( i+1 ).arg( directions[i].toString() ).arg( viewpointSaliency[i] ) );
        if ( viewpointSaliency[i] > maxSaliency ) {
            maxView = i;
            maxSaliency = viewpointSaliency[i];
        }
    }

    m_mapper->SetVolumeRayCastFunction( current );

    DEBUG_LOG( QString( "most salient view: %1 (%2)" ).arg( maxView+1 ).arg( maxSaliency ) );
    camera->SetPosition( directions[maxView].x, directions[maxView].y, directions[maxView].z );
    camera->OrthogonalizeViewUp();
    renderer->ResetCameraClippingRange();
    renderer->Render();

    {
        // Guardar resultats en un fitxer
        QFile outFile( QDir::tempPath().append( "/viewpointsaliency.txt" ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFile );
            for ( int i = 0; i < nDirections; i++ )
            {
                out << i+1 << "\t" << directions[i].toString() << "\t" << viewpointSaliency[i] << "\n";
            }
            outFile.close();
        }
    }

    delete [] viewpointSaliency;
}


void OptimalViewpointVolume::accumulateViewpointSaliency( int threadId, double saliency )
{
    if ( !m_accumulatedViewpointSaliencyPerThread.contains( threadId ) )
    {
        m_mutex.lock();
        m_accumulatedViewpointSaliencyPerThread[threadId] = 0.0;  // crear l'entrada
        m_pixelsPerThread[threadId] = 0;  // crear l'entrada
        m_mutex.unlock();
    }

    m_accumulatedViewpointSaliencyPerThread[threadId] += saliency;
    m_pixelsPerThread[threadId]++;
}


void OptimalViewpointVolume::setFx( bool fx )
{
    DEBUG_LOG( QString( "sfx:b (%1,%2)" ).arg( m_fx ).arg( fx ) );
    //if ( m_fx == fx ) return; // això no passarà mai mentre fem servir OVParameters

    m_fx = fx;

    if ( m_fx )
    {
        m_mapper->SetVolumeRayCastFunction( m_volumeRayCastFunctionFx2 );
    }
    else
    {
        // truc lleig perquè agafi la correcta
        m_renderWithObscurances = !m_renderWithObscurances;
        setRenderWithObscurances( !m_renderWithObscurances );
    }
    DEBUG_LOG( "sfx:e" );
}


void OptimalViewpointVolume::setFxContour( double fxContour )
{
    m_volumeRayCastFunctionFx->SetFxContour( fxContour );
    if ( fxContour > 0.0 ) {
        m_contourVoxelShader->setGradientEstimator( m_mapper->GetGradientEstimator() );
        m_contourVoxelShader->setThreshold( fxContour );
        if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_contourVoxelShader ) < 0 )
            m_volumeRayCastFunctionFx2->AddVoxelShader( m_contourVoxelShader );
    }
    else m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_contourVoxelShader );
}


void OptimalViewpointVolume::setFxSaliency( bool fxSaliency )
{
    m_volumeRayCastFunctionFx->SetFxSaliency( fxSaliency );
    if ( fxSaliency ) {
        if ( m_volumeRayCastFunctionFx2->IndexOfVoxelShader( m_saliencyVoxelShader ) < 0 )
            m_volumeRayCastFunctionFx2->AddVoxelShader( m_saliencyVoxelShader );
    }
    else m_volumeRayCastFunctionFx2->RemoveVoxelShader( m_saliencyVoxelShader );
    m_volumeRayCastFunctionFx2->Print( std::cout );
}


void OptimalViewpointVolume::setFxSaliencyA( double fxSaliencyA )
{
    m_volumeRayCastFunctionFx->SetFxSaliencyA( fxSaliencyA );
    m_fxSaliencyA = fxSaliencyA;
    m_saliencyVoxelShader->setScale( m_fxSaliencyA, m_fxSaliencyB );
}


void OptimalViewpointVolume::setFxSaliencyB( double fxSaliencyB )
{
    m_volumeRayCastFunctionFx->SetFxSaliencyB( fxSaliencyB );
    m_fxSaliencyB = fxSaliencyB;
    m_saliencyVoxelShader->setScale( m_fxSaliencyA, m_fxSaliencyB );
}


void OptimalViewpointVolume::setFxSaliencyLow( double fxSaliencyLow )
{
    m_volumeRayCastFunctionFx->SetFxSaliencyLow( fxSaliencyLow );
    m_fxSaliencyLow = fxSaliencyLow;
    m_saliencyVoxelShader->setFilters( m_fxSaliencyLow, m_fxSaliencyHigh );
}


void OptimalViewpointVolume::setFxSaliencyHigh( double fxSaliencyHigh )
{
    m_volumeRayCastFunctionFx->SetFxSaliencyHigh( fxSaliencyHigh );
    m_fxSaliencyHigh = fxSaliencyHigh;
    m_saliencyVoxelShader->setFilters( m_fxSaliencyLow, m_fxSaliencyHigh );
}


}
