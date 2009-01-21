#include "volumereslicer.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

#include <vtkCamera.h>
#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include "../optimalviewpoint/oldhistogram.h"
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


void VolumeReslicer::reslice( bool saveMhd, bool doClip )
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
    int increments[3];
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

    if ( saveMhd )
    {
        // Save resliced image to file
        vtkImageData *clipped = clip->GetOutput();
        unsigned short *clippedData = reinterpret_cast<unsigned short*>( clipped->GetScalarPointer() );
        int size = clipped->GetNumberOfPoints();
        QFile outFile( QDir::tempPath().append( QString( "/resliced%1.raw" ).arg( m_id ) ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QDataStream out( &outFile );
            for ( int i = 0; i < size; i++ ) out << clippedData[i];
            outFile.close();
        }
        QFile outFileMhd( QDir::tempPath().append( QString( "/resliced%1.mhd" ).arg( m_id ) ) );
        if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFileMhd );
            out << "NDims = 3\n";
            int dimSize[3];
            clipped->GetDimensions( dimSize );
            out << "DimSize = " << dimSize[0] << " " << dimSize[1] << " " << dimSize[2] << "\n";
            out << "ElementSpacing = " << m_xSpacing << " " << m_ySpacing << " " << m_zSpacing << "\n";
            out << "ElementType = MET_USHORT\n";
            out << "ElementByteOrderMSB = True\n";
            out << "ElementDataFile = resliced" << m_id << ".raw";
            outFileMhd.close();
        }
    }

    // Get some data about resliced image
    m_reslicedImage = clip->GetOutput(); m_reslicedImage->Register( 0 );
    m_reslicedData = reinterpret_cast<unsigned short*>( m_reslicedImage->GetScalarPointer() );
    m_reslicedDataSize = m_reslicedImage->GetNumberOfPoints();
    m_reslicedImage->GetDimensions( dimensions );
    m_sliceSize = dimensions[0] * dimensions[1];
    m_sliceCount = dimensions[2];
    double *range = m_reslicedImage->GetScalarRange();
    unsigned short min = static_cast<unsigned short>( qRound( range[0] ) );
    unsigned short max = static_cast<unsigned short>( qRound( range[1] ) );
    m_nLabels = max - min + 1;

    // Destroy created objects
    reslice->Delete();
    clip->Delete();

    DEBUG_LOG( "end reslice" );
}


void VolumeReslicer::computeSmi()   /// \todo Fer-ho més eficient!!!
{
    DEBUG_LOG( "SMI: primer pas" );
    // Primer una passada per tenir un histograma independent de les llesques
    OldHistogram oneHistogram( m_nLabels ); // to rule them all
    for ( int i = 0; i < m_reslicedDataSize; i++ )
    {
        unsigned short value = m_reslicedData[i];
        if ( value > 0 )    // no comptem cap background
            oneHistogram.add( value );
    }
    QVector<double> p_o_;
    double oneCount = oneHistogram.count();
    DEBUG_LOG( QString( "SMI: one count = %1" ).arg( oneCount ) );
    QVectorIterator<quint64> *itOneHistogram = oneHistogram.getIterator();
    while ( itOneHistogram->hasNext() )
    {
        double d = itOneHistogram->next() / oneCount;
        p_o_.append( d );
    }
    delete itOneHistogram;

    DEBUG_LOG( "SMI: segon pas" );
    // Després les passades per tenir els histogrames per llesca i fer els càlculs finals
    m_smi.clear();
    for ( int i = 0; i < m_sliceCount; i++ )    // iterem sobre les llesques
    {
        OldHistogram histogram( m_nLabels );
        unsigned short *slice = m_reslicedData + i * m_sliceSize;   // començament de la llesca
        for ( int j = 0; j < m_sliceSize; j++ ) // iterem sobre la llesca actual
        {
            unsigned short value = slice[j];
            if ( value > 0 )    // no comptem cap background
                histogram.add( value );
        }

        double I_s_O_ = 0.0;
        double count = histogram.count();
        QVectorIterator<quint64> *itHistogram = histogram.getIterator();
        unsigned short o = 0;
        while ( itHistogram->hasNext() )
        {
            double p_o_s_ = itHistogram->next() / count;
            if ( p_o_s_ > 0.0 ) I_s_O_ += p_o_s_ * log( p_o_s_ / p_o_[o] );
            o++;
        }
        I_s_O_ /= log( 2.0 );
        delete itHistogram;
        m_smi.append( I_s_O_ );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/smi%1.txt" ).arg( m_id ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_smi.size(); i++ )
        {
            DEBUG_LOG( QString( "SMI: SMI[%1] = %2" ).arg( i ).arg( m_smi[i] ) );
            out << "SMI[" << i << "] = " << m_smi[i] << "\n";
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


}
