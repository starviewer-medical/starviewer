/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "slicer.h"

#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkPointData.h>

#include <QFile>
#include <QDataStream>
#include <QTextStream>

#include "histogram.h"

namespace udg {

Slicer::Slicer()
{
    m_matrix = vtkMatrix4x4::New();
}


Slicer::~Slicer()
{
}


void Slicer::setInput( vtkImageData * input )
{
    m_input = input;
    double range[2];
    m_input->GetScalarRange( range );
    m_nLabels = static_cast< unsigned char >( round( range[1] ) ) + 1;
}


void Slicer::setMatrix( vtkMatrix4x4 * matrix )
{
    m_matrix->DeepCopy( matrix );
}


void Slicer::setSpacing( double xSpacing, double ySpacing, double zSpacing )
{
    m_xSpacing = xSpacing;
    m_ySpacing = ySpacing;
    m_zSpacing = zSpacing;
}


void Slicer::reslice()
{
    m_input->UpdateInformation();

    // Set the slice orientation
    vtkMatrix4x4 * resliceAxes = m_matrix;
    resliceAxes->SetElement( 3, 0, 0.0 );
    resliceAxes->SetElement( 3, 1, 0.0 );
    resliceAxes->SetElement( 3, 2, 0.0 );
    resliceAxes->SetElement( 3, 3, 1.0 );
    resliceAxes->SetElement( 0, 3, 0.0 );
    resliceAxes->SetElement( 1, 3, 0.0 );
    resliceAxes->SetElement( 2, 3, 0.0 );
    resliceAxes->Print( std::cout );

    // Extract a slice in the desired orientation
    vtkImageReslice *reslice = vtkImageReslice::New();
    reslice->SetInput( m_input );
    reslice->SetOutputDimensionality( 3 );
    reslice->SetResliceAxes(resliceAxes);
    reslice->AutoCropOutputOn();
    reslice->SetBackgroundLevel( 255.0 );
    reslice->SetOutputSpacing( m_xSpacing, m_ySpacing, m_zSpacing );
    reslice->Update();
    reslice->Print( std::cout );

    vtkImageData * result = reslice->GetOutput();
    unsigned char * data = reinterpret_cast<unsigned char *>( result->GetPointData()->GetScalars()->GetVoidPointer(0) );
    int size = result->GetPointData()->GetScalars()->GetSize();

    QFile outFile( "/scratch/result.bin" );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QDataStream out( &outFile );
        for ( int i = 0; i < size; i++ )
            out << data[i];
        outFile.close();
    }

    std::cout << "-------------------------------------------------------------" << std::endl;
    result->Print( std::cout );
    std::cout << "=============================================================" << std::endl;



    m_reslicedImage = reslice->GetOutput(); m_reslicedImage->Register( 0 );
    m_reslicedData = reinterpret_cast<unsigned char *>( m_reslicedImage->GetPointData()->GetScalars()->GetVoidPointer(0) );
    m_reslicedDataSize = m_reslicedImage->GetPointData()->GetScalars()->GetSize();
    std::cout << "[Slicer] resliced data size = " << m_reslicedDataSize << std::endl;
    int dimensions[3];
    m_reslicedImage->GetDimensions( dimensions );
    m_sliceSize = dimensions[0] * dimensions[1];
    std::cout << "[Slicer] slice size = " << m_sliceSize << std::endl;
    m_sliceCount = dimensions[2];
    std::cout << "[Slicer] slice count = " << m_sliceCount << std::endl;
    std::cout << "[Slicer] n labels = " << (short) m_nLabels << std::endl;


    reslice->Delete();
}


void Slicer::compute()  /// \todo Fer-ho més eficient!!!
{
    std::cout << "[Slicer] ----------------- compute0 -------------- " << std::endl;
    // Primer una passada per tenir un histograma independent de les llesques
    Histogram oneHistogram( m_nLabels ); // to rule them all
    for ( unsigned int i = 0; i < m_reslicedDataSize; i++ )
    {
        unsigned char value = m_reslicedData[i];
        if ( value < m_nLabels )    // no comptem el background afegit pel reslicer
            oneHistogram.add( value );
    }
    std::vector< double > p_o_;
    double oneCount = oneHistogram.count();
    std::cout << "[Slicer] one count = " << oneCount << std::endl;
    QVectorIterator< unsigned long > * itOneHistogram = oneHistogram.getIterator();
    while ( itOneHistogram->hasNext() )
    {
        double d = itOneHistogram->next() / oneCount;
        p_o_.push_back( d );
        std::cout << "[Slicer] d = " << d << std::endl;
    }
    delete itOneHistogram;

    std::cout << "[Slicer] ----------------- compute1 -------------- " << std::endl;
    // Després les passades per tenir els histogrames per llesca i fer els càlculs finals
    m_smi.clear();
    for ( unsigned int i = 0; i < m_sliceCount; i++ )  // iterem sobre les llesques
    {
        Histogram histogram( m_nLabels );
        unsigned char * slice = m_reslicedData + i * m_sliceSize;   // començament de la llesca
        for ( unsigned int j = 0; j < m_sliceSize; j++ )    // iterem sobre la llesca actual
        {
            unsigned char value = slice[j];
            if ( value < m_nLabels )    // no comptem el background afegit pel reslicer
                histogram.add( value );
        }

        double I_s_O_ = 0.0;
        double count = histogram.count();
        QVectorIterator< unsigned long > * itHistogram = histogram.getIterator();
        unsigned char o = 0;
        while ( itHistogram->hasNext() )
        {
            double p_o_s_ = itHistogram->next() / count;
            std::cout << "[Slicer] p(o|s) = " << p_o_s_ << std::endl;
            if ( p_o_s_ > 0.0 ) I_s_O_ += p_o_s_ * log( p_o_s_ / p_o_[o] );
            o++;
        }
        I_s_O_ /= log( 2.0 );
        std::cout << "[Slicer] I(s,O) = " << I_s_O_ << std::endl;
        delete itHistogram;
        m_smi.push_back( I_s_O_ );
    }

    // Printar resultats
    QFile outFile( "/scratch/smi.txt" );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        std::cout << "[SR] Slicer results:" << std::endl;
        for ( unsigned int i = 0; i < m_smi.size(); i++ )
        {
            std::cout << "[SR] SMI[" << i << "] = " << m_smi[i] << std::endl;
            out << "SMI[" << i << "] = " << m_smi[i] << "\n";
        }
        std::cout << std::endl;
        outFile.close();
    }
}


}
