/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "optimalviewpointvolume.h"

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
#include "vtkVolumeRayCastCompositeFunctionOptimalViewpoint.h"

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
#include "vtkMultiThreader.h"

namespace udg {


OptimalViewpointVolume::OptimalViewpointVolume( vtkImageData * image, QObject * parent )
    : QObject( parent )
{
    Q_CHECK_PTR( image );

    double range[2];
    image->GetScalarRange( range );
    double min = range[0], max = range[1];
    DEBUG_LOG( QString( "[OVV] min = %1, max = %2" ).arg( min ).arg( max ) );

    if ( min >= 0.0 && max <= 255.0 )   // si ja està dins del rang que volem només cal fer un cast
    {
        // cal fer el casting perquè ens arriba com a int
        vtkImageCast * caster = vtkImageCast::New();
        caster->SetInput( image );
        caster->SetOutputScalarTypeToUnsignedChar();
        caster->Update();

        m_image = caster->GetOutput(); m_image->Register( 0 );
        caster->Delete();

        m_rangeMin = static_cast<unsigned short>( qRound( min ) );
        m_rangeMax = static_cast<unsigned short>( qRound( max ) );
    }
    else
    {
        double shift = -min;
        double slope = 255.0 / ( max - min );

        vtkImageShiftScale * shifter = vtkImageShiftScale::New();
        shifter->SetInput( image );
        shifter->SetShift( shift );
        shifter->SetScale( slope );
        shifter->SetOutputScalarTypeToUnsignedChar();
        shifter->ClampOverflowOn();
        shifter->Update();

        m_image = shifter->GetOutput(); m_image->Register( 0 );
        shifter->Delete();

        m_rangeMin = 0; m_rangeMax = 255;

        double newRange[2];
        m_image->GetScalarRange( newRange );
        DEBUG_LOG( QString( "[OVV] new min = %1, new max = %2" ).arg( newRange[0] ).arg( newRange[1] ) );
    }

    m_data = reinterpret_cast<unsigned char *>( m_image->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );

    m_labeledImage = vtkImageData::New();
    m_labeledImage->DeepCopy( m_image );    //m_labeledImage->Register( 0 );    // no cal el register perquè hem fet un new
    m_labeledData = reinterpret_cast<unsigned char *>( m_labeledImage->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );

    // ja no cal
//     m_segmentedImage = vtkImageData::New();
//     m_segmentedImage->DeepCopy( m_image );  //m_segmentedImage->Register( 0 );  // no cal el register perquè hem fet un new
//     m_segmentedData = reinterpret_cast< unsigned char *>( m_segmentedImage->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );

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





//     m_planeMapper->SetInput( m_segmentedImage ); // ara s'hauria de fer amb labeled
    m_planeMapper->SetInput( m_image );




    m_planeMapper->AutoAdjustSampleDistancesOff();





    vtk4DLinearRegressionGradientEstimator * gradientEstimator = vtk4DLinearRegressionGradientEstimator::New();
    m_mainMapper->SetGradientEstimator( gradientEstimator );
    m_planeMapper->SetGradientEstimator( gradientEstimator );
    gradientEstimator->Delete();




//     m_opacityTransferFunction = vtkPiecewiseFunction::New(); m_opacityTransferFunction->Register( 0 );
//     m_opacityTransferFunction->AddPoint( 0.0, 0.0 );
//     m_opacityTransferFunction->AddPoint( 255.0, 1.0 );
    vtkPiecewiseFunction * opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( m_rangeMin, 0.0 );
    opacityTransferFunction->AddPoint( m_rangeMax, 1.0 );
//     m_colorTransferFunction = vtkColorTransferFunction::New(); m_colorTransferFunction->Register( 0 );
//     m_colorTransferFunction->AddRGBPoint( 0.0, 0.0, 0.0, 0.0 );
//     m_colorTransferFunction->AddRGBPoint( 255.0, 1.0, 1.0, 1.0 );
    vtkColorTransferFunction * colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint( m_rangeMin, 0.0, 0.0, 0.0 );
    colorTransferFunction->AddRGBPoint( m_rangeMax, 1.0, 1.0, 1.0 );
    m_volumeProperty = vtkVolumeProperty::New(); m_volumeProperty->Register( 0 );
//     m_volumeProperty->SetScalarOpacity( m_opacityTransferFunction );
    m_volumeProperty->SetScalarOpacity( opacityTransferFunction );
//     m_volumeProperty->SetGradientOpacity( m_opacityTransferFunction );
//     m_volumeProperty->SetColor( m_colorTransferFunction );
    m_volumeProperty->SetColor( colorTransferFunction );



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





    m_renderCluster = false;
    m_clusterImage = 0;
    m_clusterVolume = 0;
    m_clusterMapper = 0;


    m_obscurance = 0;
    m_obscuranceDirections = 2;
    m_obscuranceMaximumDistance = 64.0;
    m_obscuranceFunction = Constant0;


    DEBUG_LOG( "end constructor" );
}

OptimalViewpointVolume::~OptimalViewpointVolume()
{
//     m_opacityTransferFunction->Delete();
//     m_colorTransferFunction->Delete();
    m_volumeProperty->Delete();
    m_mainVolumeRayCastFunction->Delete();
    m_planeVolumeRayCastFunction->Delete();
    m_mainMapper->Delete();
    m_planeMapper->Delete();
    m_mainVolume->Delete();
    m_planeVolume->Delete();
    m_image->Delete();
    m_labeledImage->Delete();
//     m_segmentedImage->Delete();  // ja no cal

    if ( m_clusterImage ) m_clusterImage->Delete();

    delete [] m_obscurance;
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


void OptimalViewpointVolume::setTransferFunction( const TransferFunction & transferFunction )
{
    m_volumeProperty->SetScalarOpacity( transferFunction.getOpacityTransferFunction() );
    m_volumeProperty->SetColor( transferFunction.getColorTransferFunction() );
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
    m_planeVolume->PokeMatrix( m_mainVolume->GetMatrix() );
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
    std::cout << "OVV::ldSegFromFile(): " << qPrintable( m_segmentationFileName ) << std::endl;
    QFile segFile( segmentationFileName );

    if ( !segFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        std::cerr << "OVV::ldSegFromFile(): cannot open file " << qPrintable( m_segmentationFileName ) << std::endl;
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
    if ( m_renderCluster != renderCluster )
    {
        if ( renderCluster )
            m_mainMapper->SetInput( m_clusterImage );
        else
            m_mainMapper->SetInput( m_image );
    }

    m_renderCluster = renderCluster;
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
        /// \todo Posar un booleà per fer això
        m_renderCluster = false;    // perquè després es cridarà setRenderCluster i s'actualitzarà bé
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
//     emit visited( rayId, *(m_labeledData + offset) );
}


void OptimalViewpointVolume::endRayObscurances( int rayId )
{
//     emit rayEnd( rayId );
}


// versió amb threads
void OptimalViewpointVolume::computeObscurances()
{
    synchronize();

    vtkDirectionEncoder * directionEncoder = m_mainMapper->GetGradientEstimator()->GetDirectionEncoder();
    unsigned short * encodedNormals = m_mainMapper->GetGradientEstimator()->GetEncodedNormals();

    // càlcul de direccions
    POVSphereCloud cloud( 1.0, m_obscuranceDirections );    // 0 -> 12 dir, 1 -> 42 dir, 2 -> 162 dir
    cloud.createPOVCloud();
    const QVector<Vector3> & directions = cloud.getVertices();

    // variables necessàries
    int dimensions[3];
    m_image->GetDimensions( dimensions );
    int increments[3];
    m_image->GetIncrements( increments );

    unsigned char numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();
    ObscuranceThread * threads[numberOfThreads];

    for ( unsigned char i = 0; i < numberOfThreads; ++i )
    {
        ObscuranceThread * thread = new ObscuranceThread( i, numberOfThreads, directions, this );
        thread->setNormals( directionEncoder, encodedNormals );
        thread->setData( m_data, m_dataSize, dimensions, increments );
        thread->setObscuranceParameters( m_obscuranceMaximumDistance, m_obscuranceFunction );
        thread->start();
        threads[i] = thread;
    }

    delete [] m_obscurance;
    m_obscurance = new double[m_dataSize];
    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] = 0.0;

    double maximumObscurance = 0.0;

    for ( unsigned char i = 0; i < numberOfThreads; ++i )
    {
        ObscuranceThread * thread = threads[i];
        thread->wait();

        double * threadObscurance = thread->getObscurance();
        for ( int j = 0; j < m_dataSize; ++j )
        {
            m_obscurance[j] += threadObscurance[j];
            if ( m_obscurance[j] > maximumObscurance ) maximumObscurance = m_obscurance[j];
        }

        delete thread;
    }

    for ( int i = 0; i < m_dataSize; ++i ) m_obscurance[i] /= maximumObscurance;

    {
        // obscurances to file
        QFile outFile( QDir::tempPath().append( QString( "/obscurance.raw" ) ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QDataStream out( &outFile );
            for ( int i = 0; i < m_dataSize; ++i )
            {
                uchar value = m_data[i] > 0 ? static_cast<uchar>( qRound( m_obscurance[i] * 255.0 ) ) : 0;
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
}


// versió sense threads
// void OptimalViewpointVolume::computeObscurances()
// {
//     synchronize();
// 
//     vtkDirectionEncoder * directionEncoder = m_mainMapper->GetGradientEstimator()->GetDirectionEncoder();
//     unsigned short * encodedNormals = m_mainMapper->GetGradientEstimator()->GetEncodedNormals();
//     unsigned char * gradientMagnitudes = m_mainMapper->GetGradientEstimator()->GetGradientMagnitudes();
// 
//     // càlcul de direccions
//     POVSphereCloud cloud( 1.0, m_obscuranceDirections );    // 0 -> 12 dir, 1 -> 42 dir, 2 -> 162 dir
//     cloud.createPOVCloud();
//     const QVector<Vector3> & vertices = cloud.getVertices();
// 
//     // variables necessàries
//     int dimensions[3];
//     m_image->GetDimensions( dimensions );
//     int increments[3];
//     m_image->GetIncrements( increments );
// 
//     delete [] m_obscurance;
//     m_obscurance = new double[m_dataSize];
//     for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] = 0.0;
// 
//     unsigned int progress = 0, total = vertices.count();
// 
//     int selection = static_cast<int>( round( m_obscuranceMaximumDistance * 100.0 ) ) % 100;
//     double maximumObscurance = 0.0;
// 
//     // iterem per les direccions
//     foreach ( Vector3 direction, vertices )
//     {
// //         if ( progress != selection )
// //         {
// //             progress++;
// //             continue;
// //         }
// 
//         DEBUG_LOG( QString( "Direcció " ) + direction.toString() );
// 
//         // direcció dominant (0 = x, 1 = y, 2 = z)
//         int dominant;
//         Vector3 absDirection( qAbs( direction.x ), qAbs( direction.y ), qAbs( direction.z ) );
//         if ( absDirection.x >= absDirection.y )
//         {
//             if ( absDirection.x >= absDirection.z ) dominant = 0;
//             else dominant = 2;
//         }
//         else
//         {
//             if ( absDirection.y >= absDirection.z ) dominant = 1;
//             else dominant = 2;
//         }
// 
//         // vector per avançar
//         Vector3 forward;
//         switch ( dominant )
//         {
//             case 0: forward = Vector3( direction.x, direction.y, direction.z ); break;
//             case 1: forward = Vector3( direction.y, direction.z, direction.x ); break;
//             case 2: forward = Vector3( direction.z, direction.x, direction.y ); break;
//         }
//         forward /= qAbs( forward.x );   // la direcció x passa a ser 1 o -1
//         DEBUG_LOG( QString( "forward = " ) + forward.toString() );
// 
//         // dimensions i increments segons la direcció dominant
//         int dimX = dimensions[dominant], dimY = dimensions[(dominant+1)%3], dimZ = dimensions[(dominant+2)%3];
//         int incX = increments[dominant], incY = increments[(dominant+1)%3], incZ = increments[(dominant+2)%3];
//         qptrdiff startDelta = 0;
//         if ( forward.x < 0.0 )
//         {
//             startDelta += incX * ( dimX - 1 );
//             incX = -incX;
//             forward.x = -forward.x;
//         }
//         if ( forward.y < 0.0 )
//         {
//             startDelta += incY * ( dimY - 1 );
//             incY = -incY;
//             forward.y = -forward.y;
//         }
//         if ( forward.z < 0.0 )
//         {
//             startDelta += incZ * ( dimZ - 1 );
//             incZ = -incZ;
//             forward.z = -forward.z;
//         }
//         DEBUG_LOG( QString( "forward = " ) + forward.toString() );
//         // ara els 3 components són positius
// 
//         // llista dels vòxels que són començament de línia
//         QList<Vector3> lineStarts = getLineStarts( dimX, dimY, dimZ, forward );
// 
// //         uint i = 0;
// //         uint c = 0;
// 
// //         // prova
// //         int length = dimX * dimY * dimZ;
// //         unsigned char array[length];
// //         for (int k = 0; k < length; k++) array[k] = 0;
// //         unsigned char * ptr = array;
// //         ptr += startDelta;
// 
//         unsigned char * dataPtr = m_data + startDelta;
// 
//         // iterar per cada línia
//         while ( !lineStarts.isEmpty() )
//         {
// //             i++;
//             Vector3 rv = lineStarts.takeFirst();
//             Voxel v = { round( rv.x ), round( rv.y ), round( rv.z ) };
//             Voxel pv = v;
//             QStack< QPair<uchar,Vector3> > unresolvedVoxels;
// 
//             // iterar per la línia
//             while ( v.x < dimX && v.y < dimY && v.z < dimZ )
//             {
// //                 // prova
// //                 ptr[v.x * incX + v.y * incY + v.z * incZ]++;
// 
//                 // tractar el vòxel
//                 uchar value = dataPtr[v.x * incX + v.y * incY + v.z * incZ];
// 
//                 while ( !unresolvedVoxels.isEmpty() && unresolvedVoxels.top().first <= value )
//                 {
//                     Vector3 ru = unresolvedVoxels.pop().second;
//                     Voxel u = { round( ru.x ), round( ru.y ), round( ru.z ) };
// 
//                     int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
//                     float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[uIndex] );
//                     Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );
// //                     DEBUG_LOG( "-------------" );
// //                     DEBUG_LOG( QString( "voxel: " ) + ru.toString() );
// //                     DEBUG_LOG( QString( "value: %1" ).arg( it.key() ) );
// //                     DEBUG_LOG( QString( "gradient: %1" ).arg( gradientMagnitudes[uIndex] ) );
// //                     DEBUG_LOG( QString( "normal: " ) + uNormal.toString() );
// //                     DEBUG_LOG( QString( "direction: " ) + direction.toString() );
// //                     DEBUG_LOG( QString( "normal · direction = %1" ).arg( uNormal * direction ) );
// //                     DEBUG_LOG( "-------------" );
//                     // sembla que ja venen normalitzades
// //                     if ( uNormal.length() != 0.0 && uNormal.length() != 1.0 )
// //                     {
// //                         DEBUG_LOG( QString( "normal: " ) + uNormal.toString() + QString( " | length = %1" ).arg( uNormal.length() ) );
// //                         uNormal.normalize();
// //                     }
// 
//                     if ( uNormal * direction < 0.0 )
//                     {
// //                         DEBUG_LOG( QString( "entro: " ) + ru.toString() );
//                         double distance = ( rv - ru ).length();
// //                         Vector3 du( u.x, u.y, u.z), dv( v.x, v.y, v.z );
// //                         double distance = ( dv - du ).length();
//                         m_obscurance[uIndex] += obscurance( distance );
//                         if ( m_obscurance[uIndex] > maximumObscurance )
//                             maximumObscurance = m_obscurance[uIndex];
//                     }
//                 }
// 
//                 unresolvedVoxels.push( qMakePair( value, rv ) );
// 
//                 // avançar el vòxel
//                 rv += forward;
//                 pv = v;
//                 v.x = round( rv.x ); v.y = round( rv.y ); v.z = round( rv.z );
// //                 c++;
//             }
// 
//             while ( !unresolvedVoxels.isEmpty() )
//             {
//                 Vector3 ru = unresolvedVoxels.pop().second;
//                 Voxel u = { round( ru.x ), round( ru.y ), round( ru.z ) };
//                 int uIndex = startDelta + u.x * incX + u.y * incY + u.z * incZ;
//                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[uIndex] );
//                 Vector3 uNormal( uGradient[0], uGradient[1], uGradient[2] );
// 
// //                 DEBUG_LOG( "-------------" );
// //                 DEBUG_LOG( QString( "voxel: " ) + ru.toString() );
// //                 DEBUG_LOG( QString( "value: %1" ).arg( it.key() ) );
// //                 DEBUG_LOG( QString( "gradient: %1" ).arg( gradientMagnitudes[uIndex] ) );
// //                 DEBUG_LOG( QString( "normal: " ) + uNormal.toString() );
// //                 DEBUG_LOG( QString( "direction: " ) + direction.toString() );
// //                 DEBUG_LOG( QString( "normal · direction = %1" ).arg( uNormal * direction ) );
// //                 DEBUG_LOG( "-------------" );
// 
//                 if ( uNormal * direction < 0.0 )
//                 {
//                     m_obscurance[uIndex]++;
//                     if ( m_obscurance[uIndex] > maximumObscurance )
//                             maximumObscurance = m_obscurance[uIndex];
//                 }
//             }
//         }
// 
// //         DEBUG_LOG( QString( "i = %1" ).arg( i ) );
// //         DEBUG_LOG( QString( "c = %1" ).arg( c ) );
// //         for ( int k = 0; k < length; k++ )
// //         {
// //             if ( array[k] != 1 )
// //                 DEBUG_LOG( QString( "malament!!!! a[%1] = %2" ).arg( k ).arg( array[k] ) );
// //         }
// 
//         DEBUG_LOG( QString( "progress: %1/%2" ).arg( ++progress ).arg( total ) );
//     }
// 
//     unsigned int count = vertices.count();
//     for ( int i = 0; i < m_dataSize; i++ ) m_obscurance[i] /= maximumObscurance;
// 
//     for ( int i = 0; i < m_dataSize; i++ )
//     {
//         if ( m_obscurance[i] > 1.0 )
//             DEBUG_LOG( QString( "bad obscurance: o[%1] = %2" ).arg( i ).arg( m_obscurance[i] ) );
//     }
// 
//     {
//         // obscurances to file
//         QFile outFile( QDir::tempPath().append( QString( "/obscurance.raw" ) ) );
//         if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
//         {
//             QDataStream out( &outFile );
//             for ( int i = 0; i < m_dataSize; i++ )
//             {
//                 uchar value = m_data[i] > 0 ? static_cast<uchar>( round( m_obscurance[i] * 255.0 ) ) : 0;
//                 out << value;
// //                 out << gradientMagnitudes[i];
// //                 float * uGradient = directionEncoder->GetDecodedGradient( encodedNormals[i] );
// //                 out << static_cast<uchar>( round( ( uGradient[selection] + 1.0 ) * 127.5 ) );
//             }
//             outFile.close();
//         }
//         QFile outFileMhd( QDir::tempPath().append( QString( "/obscurance.mhd" ) ) );
//         if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
//         {
//             QTextStream out( &outFileMhd );
//             out << "NDims = 3\n";
//             out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
//             double spacing[3];
//             m_image->GetSpacing( spacing );
//             out << "ElementSpacing = " << spacing[0] << " " << spacing[1] << " " << spacing[2] << "\n";
//             out << "ElementType = MET_UCHAR\n";
//             out << "ElementDataFile = obscurance.raw";
//             outFileMhd.close();
//         }
//     }
// }


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


}
