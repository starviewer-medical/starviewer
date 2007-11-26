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

    for ( int i = 1; i < bins; i++ )
    {
        limits.push_back( i * 256 / bins - 1 );
    }

    labelize( limits );
    generateAdjustedTransferFunction( limits );

    return limits.size() + 1;
}


}
