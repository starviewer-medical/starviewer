#include "volumereslicer.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>

#include "histogram.h"
#include "informationtheory.h"
#include "logging.h"


namespace udg {


VolumeReslicer::VolumeReslicer( unsigned short id )
 : m_id( id ), m_input( 0 ), m_resliceAxes( 0 ), m_xSpacing( 1.0 ), m_ySpacing( 1.0 ), m_zSpacing( 1.0 ), m_reslicedImage( 0 )
{
}


VolumeReslicer::~VolumeReslicer()
{
    if ( m_resliceAxes ) m_resliceAxes->Delete();
    if ( m_reslicedImage ) m_reslicedImage->Delete();
}


void VolumeReslicer::setInput( vtkImageData *input )
{
    m_input = input;
}


void VolumeReslicer::setViewpoint( const Vector3 &viewpoint, const Vector3 &up )
{
    if ( m_resliceAxes ) m_resliceAxes->Delete();
    m_resliceAxes = vtkMatrix4x4::New();

    vtkCamera *camera = vtkCamera::New();
    camera->ParallelProjectionOn();
    camera->SetPosition( -viewpoint.x, -viewpoint.y, -viewpoint.z );
    camera->SetFocalPoint( 0.0, 0.0, 0.0 );
    camera->SetViewUp( up.x, up.y, up.z );
    m_resliceAxes->DeepCopy( camera->GetViewTransformMatrix() );
    camera->Delete();

    m_resliceAxes->SetElement( 3, 0, 0.0 );
    m_resliceAxes->SetElement( 3, 1, 0.0 );
    m_resliceAxes->SetElement( 3, 2, 0.0 );
    m_resliceAxes->SetElement( 3, 3, 1.0 );
    m_resliceAxes->SetElement( 0, 3, 0.0 );
    m_resliceAxes->SetElement( 1, 3, 0.0 );
    m_resliceAxes->SetElement( 2, 3, 0.0 );

    m_resliceAxes->Invert();
}


void VolumeReslicer::setSpacing( double xSpacing, double ySpacing, double zSpacing )
{
    m_xSpacing = xSpacing; m_ySpacing = ySpacing; m_zSpacing = zSpacing;
}


void VolumeReslicer::reslice( bool saveMhd, bool doClip, int maxRange )
{
    DEBUG_LOG( "reslice" );

    Q_ASSERT( m_input );
    Q_ASSERT( m_resliceAxes );

    m_input->UpdateInformation();

    // Reslice the image in the desired orientation
    vtkImageReslice *reslice = vtkImageReslice::New();
    reslice->SetInput( m_input );
    reslice->SetOutputDimensionality( 3 );
    reslice->SetResliceAxes( m_resliceAxes );
    reslice->AutoCropOutputOn();
    reslice->SetBackgroundLevel( 0.0 );
    reslice->SetOutputSpacing( m_xSpacing, m_ySpacing, m_zSpacing );
    reslice->Update();

    // Find minimum extent
    vtkImageData *resliced = reslice->GetOutput();
    unsigned short *data = reinterpret_cast<unsigned short*>( resliced->GetScalarPointer() );
    vtkIdType increments[3];
    resliced->GetIncrements( increments );
    int dimensions[3];
    resliced->GetDimensions( dimensions );
    int minX, maxX, minY, maxY, minZ, maxZ;

    findExtent( data, dimensions[0], dimensions[1], dimensions[2], increments[0], increments[1], increments[2], minX, maxX );
    findExtent( data, dimensions[1], dimensions[0], dimensions[2], increments[1], increments[0], increments[2], minY, maxY );
    findExtent( data, dimensions[2], dimensions[0], dimensions[1], increments[2], increments[0], increments[1], minZ, maxZ );

    DEBUG_LOG( QString( "minX = %1, maxX = %2" ).arg( minX ).arg( maxX ) );
    DEBUG_LOG( QString( "minY = %1, maxY = %2" ).arg( minY ).arg( maxY ) );
    DEBUG_LOG( QString( "minZ = %1, maxZ = %2" ).arg( minZ ).arg( maxZ ) );

    // Clip image to minimum extent
    vtkImageClip *clip = vtkImageClip::New();
    clip->SetInput( resliced );
    clip->SetOutputWholeExtent( minX, maxX, minY, maxY, minZ, maxZ );
    if ( doClip ) clip->ClipDataOn();
    clip->Update();

    // Get some data about resliced image
    m_reslicedImage = clip->GetOutput(); m_reslicedImage->Register( 0 );
    m_reslicedData = reinterpret_cast<unsigned short*>( m_reslicedImage->GetScalarPointer() );
    m_reslicedDataSize = m_reslicedImage->GetNumberOfPoints();
    m_reslicedImage->GetDimensions( dimensions );
    m_sliceSize = dimensions[0] * dimensions[1];
    m_sliceCount = dimensions[2];
    double *range = m_reslicedImage->GetScalarRange();
    unsigned short min = static_cast<unsigned short>( qRound( range[0] ) ); // sempre serà 0 amb la implementació actual
    unsigned short max = static_cast<unsigned short>( qRound( range[1] ) );

    // si limitem el rang (maxRange > 0) llavors maxRange ha de ser >= max
    if ( maxRange > 0 && max >= maxRange )
    {
        int max1 = max + 1;

        for ( int i = 0; i < m_reslicedDataSize; i++ ) m_reslicedData[i] = m_reslicedData[i] * maxRange / max1;

        max = maxRange - 1;
        m_reslicedImage->GetPointData()->GetScalars()->Modified();  // perquè s'actualitzi l'scalar range
    }

    m_nLabels = max - min + 1;

    // Destroy created objects
    reslice->Delete();
    clip->Delete();

    if ( saveMhd )
    {
        // Save resliced image to file
        QFile outFile( QDir::tempPath().append( QString( "/resliced%1.raw" ).arg( m_id ) ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QDataStream out( &outFile );
            for ( int i = 0; i < m_reslicedDataSize; i++ ) out << m_reslicedData[i];
            outFile.close();
        }
        QFile outFileMhd( QDir::tempPath().append( QString( "/resliced%1.mhd" ).arg( m_id ) ) );
        if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFileMhd );
            out << "NDims = 3\n";
            out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
            out << "ElementSpacing = " << m_xSpacing << " " << m_ySpacing << " " << m_zSpacing << "\n";
            out << "ElementType = MET_USHORT\n";
            out << "ElementByteOrderMSB = True\n";
            out << "ElementDataFile = resliced" << m_id << ".raw";
            outFileMhd.close();
        }
    }

    DEBUG_LOG( "end reslice" );
}


void VolumeReslicer::noReslice()
{
    DEBUG_LOG( "noReslice" );

    Q_ASSERT( m_input );

    // Get some data about resliced image
    m_reslicedImage = m_input; m_reslicedImage->Register( 0 );
    m_reslicedData = reinterpret_cast<unsigned short*>( m_reslicedImage->GetScalarPointer() );
    m_reslicedDataSize = m_reslicedImage->GetNumberOfPoints();
    int *dimensions = m_reslicedImage->GetDimensions();
    m_sliceSize = dimensions[0] * dimensions[1];
    m_sliceCount = dimensions[2];
    double *spacing = m_reslicedImage->GetSpacing();
    m_xSpacing = spacing[0]; m_ySpacing = spacing[1]; m_zSpacing = spacing[2];
    double *range = m_reslicedImage->GetScalarRange();
    unsigned short min = static_cast<unsigned short>( qRound( range[0] ) ); // sempre serà 0 amb la implementació actual
    unsigned short max = static_cast<unsigned short>( qRound( range[1] ) );
    m_nLabels = max - min + 1;

    DEBUG_LOG( "end noReslice" );
}


void VolumeReslicer::computeSmi()   /// \todo Fer-ho més eficient!!!
{
    if ( m_slices.isEmpty() ) createSlices();

    DEBUG_LOG( "SMI: primer pas" );
    // Primer una passada per tenir un histograma independent de les llesques
    Histogram oneHistogram( m_nLabels );    // to rule them all
    for ( int i = 0; i < m_reslicedDataSize; i++ )
    {
        unsigned short value = m_reslicedData[i];
        if ( value > 0 )    // no comptem cap background
            oneHistogram.add( value );
    }
    QVector<double> valueProbabilities( m_nLabels );    // vector de probabilitats p(o)
    double oneCount = oneHistogram.count();
    DEBUG_LOG( QString( "SMI: one count = %1" ).arg( oneCount ) );
    for ( int i = 0; i < m_nLabels; i++ )
    {
        valueProbabilities[i] = oneHistogram[i] / oneCount;
    }

    DEBUG_LOG( "SMI: segon pas" );
    // Després fem els càlculs finals
    m_smi.resize( m_sliceCount );
    for ( int i = 0; i < m_sliceCount; i++ )    // iterem sobre les llesques
    {
        m_smi[i] = InformationTheory::kullbackLeiblerDivergence( m_slices[i].probabilities, valueProbabilities );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/smi%1.txt" ).arg( m_id ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_sliceCount; i++ )
        {
            DEBUG_LOG( QString( "SMI: SMI[%1] = %2" ).arg( i ).arg( m_smi[i] ) );
            out << "SMI[" << i << "] = " << m_smi[i] << "\n";
        }
        outFile.close();
    }
}


void VolumeReslicer::computeSliceUnstabilities()   /// \todo Fer-ho més eficient!!!
{
    if ( m_slices.isEmpty() ) createSlices();

    m_sliceUnstabilities.resize( m_sliceCount );

    for ( int i = 0; i < m_sliceCount; i++ )    // iterem sobre les llesques
    {
        m_sliceUnstabilities[i] = sliceUnstability( i );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/sliceUnstabilities%1.txt" ).arg( m_id ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_sliceCount; i++ )
        {
            DEBUG_LOG( QString( "SU: U[%1] = %2" ).arg( i ).arg( m_sliceUnstabilities[i] ) );
            out << "U[" << i << "] = " << m_sliceUnstabilities[i] << "\n";
        }
        outFile.close();
    }
}


void VolumeReslicer::computePmi()   /// \todo Fer-ho més eficient!!!
{
    if ( m_properties.isEmpty() ) createSlices();

    DEBUG_LOG( "PMI: primer pas" );
    // Primer hem de calcular la probabilitat de cada llesca -> volume de la llesca dividit pel volum total
    unsigned int totalVolume = 0;
    for ( int i = 0; i < m_sliceCount; i++ ) totalVolume += m_slices[i].volume;
    QVector<double> sliceProbabilities( m_sliceCount ); // vector de probabilitats p(s)
    double dTotalVolume = totalVolume;
    for ( int i = 0; i < m_sliceCount; i++ )
    {
        sliceProbabilities[i] = m_slices[i].volume / dTotalVolume;
    }

    DEBUG_LOG( "PMI: segon pas" );
    // Després fem els càlculs finals
    m_pmi.resize( m_nLabels );
    for ( int i = 0; i < m_nLabels; i++ )   // iterem sobre els valors de propietat
    {
        m_pmi[i] = InformationTheory::kullbackLeiblerDivergence( m_properties[i].sliceProbabilities, sliceProbabilities );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/pmi%1.txt" ).arg( m_id ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_nLabels; i++ )
        {
            DEBUG_LOG( QString( "PMI: PMI[%1] = %2" ).arg( i ).arg( m_pmi[i] ) );
            out << "PMI[" << i << "] = " << m_pmi[i] << "\n";
        }
        outFile.close();
    }
}


void VolumeReslicer::computePropertySaliencies()    /// \todo Fer-ho més eficient!!!
{
    if ( m_properties.isEmpty() ) createSlices();

    m_propertySaliencies.resize( m_nLabels );

    for ( int i = 0; i < m_nLabels; i++ )   // iterem sobre els valors de propietat
    {
        m_propertySaliencies[i] = propertySaliency( i );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/propertySaliencies%1.txt" ).arg( m_id ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_nLabels; i++ )
        {
            DEBUG_LOG( QString( "PS: S[%1] = %2" ).arg( i ).arg( m_propertySaliencies[i] ) );
            out << "S[" << i << "] = " << m_propertySaliencies[i] << "\n";
        }
        outFile.close();
    }
}


void VolumeReslicer::findExtent( const unsigned short *data, int dim0, int dim1, int dim2, int inc0, int inc1, int inc2, int &min0, int &max0 )
{
    int i0, i1, i2;
    bool found;

    found = false;
    i0 = 0;
    while ( i0 < dim0 && !found )
    {
        i1 = 0;
        while ( i1 < dim1 && !found )
        {
            i2 = 0;
            while ( i2 < dim2 && !found )
            {
                const unsigned short *pointer = data + i0 * inc0 + i1 * inc1 + i2 * inc2;
                found = *pointer != 0;
                i2++;
            }
            i1++;
        }
        if ( !found ) i0++;
    }
    if ( found ) min0 = i0;

    found = false;
    i0 = dim0 - 1;
    while ( i0 >= 0 && !found )
    {
        i1 = dim1 - 1;
        while ( i1 >= 0 && !found )
        {
            i2 = dim2 - 1;
            while ( i2 >= 0 && !found )
            {
                const unsigned short *pointer = data + i0 * inc0 + i1 * inc1 + i2 * inc2;
                found = *pointer != 0;
                i2--;
            }
            i1--;
        }
        if ( !found ) i0--;
    }
    if ( found ) max0 = i0;
}


void VolumeReslicer::createSlices()
{
    m_slices.resize( m_sliceCount );
    m_properties.resize( m_nLabels );

    QVector<Histogram> propertyHistograms( m_nLabels );
    for ( int i = 0; i < m_nLabels; i++ ) propertyHistograms[i].setSize( m_sliceCount );

    for ( int i = 0; i < m_sliceCount; i++ )
    {
        m_slices[i].data = m_reslicedData + i * m_sliceSize;

        Histogram histogram( m_nLabels );
        for ( int j = 0; j < m_sliceSize; j++ )
        {
            if ( m_slices[i].data[j] > 0 )
            {
                unsigned short value = m_slices[i].data[j];
                histogram.add( value );
                propertyHistograms[value].add( i );
            }
        }

        m_slices[i].volume = histogram.count();
        m_slices[i].probabilities.resize( m_nLabels );  // això inicialitza les probabilitats a 0.0

        if ( histogram.count() > 0 )
        {
            double count = histogram.count();
            for ( int j = 0; j < m_nLabels; j++ ) m_slices[i].probabilities[j] = histogram[j] / count;
        }
    }

    for ( int i = 0; i < m_nLabels; i++ )
    {
        m_properties[i].volume = propertyHistograms[i].count();
        m_properties[i].sliceProbabilities.resize( m_sliceCount );  // això inicialitza les probabilitats a 0.0

        if ( propertyHistograms[i].count() > 0 )
        {
            double count = propertyHistograms[i].count();
            for ( int j = 0; j < m_sliceCount; j++ ) m_properties[i].sliceProbabilities[j] = propertyHistograms[i][j] / count;
        }
    }
}


// D(si,sj) = JSD(p(si)/p(ŝ), p(sj)/p(ŝ); p(O|si), p(O|sj))
double VolumeReslicer::sliceDissimilarity( int slice1, int slice2 ) const
{
    Q_ASSERT( slice1 >= 0 && slice1 < m_sliceCount );
    Q_ASSERT( slice2 >= 0 && slice2 < m_sliceCount );
    Q_ASSERT( !m_slices.isEmpty() );

    const Slice &s1 = m_slices[slice1];
    const Slice &s2 = m_slices[slice2];
    double volume1 = s1.volume;
    double volume2 = s2.volume;
    double totalVolume = volume1 + volume2;
    double pi1, pi2;

    if ( totalVolume > 0.0 )
    {
        pi1 = volume1 / totalVolume;
        pi2 = volume2 / totalVolume;
    }
    else
    {
        pi1 = pi2 = 0.0;
    }

    return InformationTheory::jensenShannonDivergence( pi1, pi2, s1.probabilities, s2.probabilities );
}


// U(s_i) = ( D(s_i, s_i-1) + D(s_i, s_i+1) ) / 2
double VolumeReslicer::sliceUnstability( int slice ) const
{
    Q_ASSERT( slice >= 0 && slice < m_sliceCount );
    Q_ASSERT( !m_slices.isEmpty() );

    if ( slice == 0 )   // un extrem
        return sliceDissimilarity( slice, slice + 1 );
    else if ( slice == m_sliceCount - 1 )   // l'altre extrem
        return sliceDissimilarity( slice, slice - 1 );
    else    // general
        return ( sliceDissimilarity( slice, slice - 1 ) + sliceDissimilarity( slice, slice + 1 ) ) / 2.0;
}


// D(oi,oj) = JSD(p(oi)/p(ô), p(oj)/p(ô); p(S|oi), p(S|oj))
double VolumeReslicer::propertyDissimilarity( int property1, int property2 ) const
{
    Q_ASSERT( property1 >= 0 && property1 < m_nLabels );
    Q_ASSERT( property2 >= 0 && property2 < m_nLabels );
    Q_ASSERT( !m_properties.isEmpty() );

    const Property &p1 = m_properties[property1];
    const Property &p2 = m_properties[property2];
    double volume1 = p1.volume;
    double volume2 = p2.volume;
    double totalVolume = volume1 + volume2;
    double pi1, pi2;

    if ( totalVolume > 0.0 )
    {
        pi1 = volume1 / totalVolume;
        pi2 = volume2 / totalVolume;
    }
    else
    {
        pi1 = pi2 = 0.0;
    }

    return InformationTheory::jensenShannonDivergence( pi1, pi2, p1.sliceProbabilities, p2.sliceProbabilities );
}


// S(o_i) = ( D(o_i, o_i-1) + D(o_i, o_i+1) ) / 2
double VolumeReslicer::propertySaliency( int property ) const
{
    Q_ASSERT( property >= 0 && property < m_nLabels );
    Q_ASSERT( !m_properties.isEmpty() );

    if ( property == 0 )    // un extrem
        return propertyDissimilarity( property, property + 1 );
    else if ( property == m_nLabels - 1 )   // l'altre extrem
        return propertyDissimilarity( property, property - 1 );
    else    // general
        return ( propertyDissimilarity( property, property - 1 ) + propertyDissimilarity( property, property + 1 ) ) / 2.0;
}


}
