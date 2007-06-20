/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "slicer.h"

#include <vtkImageClip.h>
#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QPair>
#include <QTextStream>

#include "histogram.h"
#include "logging.h"


namespace udg {

Slicer::Slicer( unsigned char id )
{
    m_id = id;
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
    reslice->SetResliceAxes( resliceAxes );
    reslice->AutoCropOutputOn();
    reslice->SetBackgroundLevel( 255.0 );
    reslice->SetOutputSpacing( m_xSpacing, m_ySpacing, m_zSpacing );
    reslice->Update();
    reslice->Print( std::cout );

    vtkImageData * resliced = reslice->GetOutput();
    unsigned char * data = reinterpret_cast< unsigned char * >( resliced->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );

    // trobar el millor extent
    int increments[3];
    resliced->GetIncrements( increments );
    int dims[3];
    resliced->GetDimensions( dims );

    int minX, maxX, minY, maxY, minZ, maxZ;
    findExtent( data, dims[0], dims[1], dims[2], increments[0], increments[1], increments[2], minX, maxX );
    findExtent( data, dims[1], dims[0], dims[2], increments[1], increments[0], increments[2], minY, maxY );
    findExtent( data, dims[2], dims[0], dims[1], increments[2], increments[0], increments[1], minZ, maxZ );
    DEBUG_LOG( qPrintable( QString( "minX = %1, maxX = %2" ).arg( minX ).arg( maxX ) ) );
    DEBUG_LOG( qPrintable( QString( "minY = %1, maxY = %2" ).arg( minY ).arg( maxY ) ) );
    DEBUG_LOG( qPrintable( QString( "minZ = %1, maxZ = %2" ).arg( minZ ).arg( maxZ ) ) );

    vtkImageClip * clip = vtkImageClip::New();
    clip->SetInput( reslice->GetOutput() );
    clip->SetOutputWholeExtent( minX, maxX, minY, maxY, minZ, maxZ );
    clip->ClipDataOn();
    clip->Update();

    vtkImageData * clipped = clip->GetOutput();
    unsigned char * clippedData = reinterpret_cast< unsigned char * >( clipped->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    int size = clipped->GetPointData()->GetScalars()->GetSize();




    QFile outFile( QDir::tempPath().append( QString( "/resliced%1.raw" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QDataStream out( &outFile );
        for ( int i = 0; i < size; i++ )
            out << clippedData[i];
        outFile.close();
    }
    QFile outFileMhd( QDir::tempPath().append( QString( "/resliced%1.mhd" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFileMhd.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFileMhd );
        out << "NDims = 3\n";
        int dimensions[3];
        clipped->GetDimensions( dimensions );
        out << "DimSize = " << dimensions[0] << " " << dimensions[1] << " " << dimensions[2] << "\n";
        out << "ElementSpacing = " << m_xSpacing << " " << m_ySpacing << " " << m_zSpacing << "\n";
        out << "ElementType = MET_UCHAR\n";
        out << "ElementDataFile = resliced" << static_cast< short >( m_id ) << ".raw";
        outFileMhd.close();
    }


    std::cout << "-------------------------------------------------------------" << std::endl;
    clipped->Print( std::cout );
    std::cout << "=============================================================" << std::endl;



    m_reslicedImage = clip->GetOutput(); m_reslicedImage->Register( 0 );
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
    clip->Delete();
}


void Slicer::findExtent( const unsigned char * data,
                         int dim0, int dim1, int dim2,
                         int inc0, int inc1, int inc2,
                         int & min0, int & max0 )
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
                const unsigned char * pointer = data + i0 * inc0 + i1 * inc1 + i2 * inc2;
                found = *pointer != 0 && *pointer != 255;
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
                const unsigned char * pointer = data + i0 * inc0 + i1 * inc1 + i2 * inc2;
                found = *pointer != 0 && *pointer != 255;
                i2--;
            }
            i1--;
        }
        if ( !found ) i0--;
    }
    if ( found ) max0 = i0;
}


void Slicer::compute()  /// \todo Fer-ho més eficient!!!
{
    std::cout << "[Slicer] ----------------- compute0 -------------- " << std::endl;
    // Primer una passada per tenir un histograma independent de les llesques
    Histogram oneHistogram( m_nLabels ); // to rule them all
    for ( unsigned int i = 0; i < m_reslicedDataSize; i++ )
    {
        unsigned char value = m_reslicedData[i];
        if ( value < m_nLabels && value > 0 )    // no comptem cap background
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
            if ( value < m_nLabels && value > 0 )    // no comptem cap background
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
    QFile outFile( QDir::tempPath().append( QString( "/smi%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
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


void Slicer::computeSimilarities()  /// \todo Fer-ho més eficient!!!
{
    // calcular la similaritat entre cada parell de llesques consecutives
    for ( unsigned int i = 0; i < m_sliceCount - 1; i++ )  // iterem sobre les llesques
    {
        unsigned char * sliceX = m_reslicedData + i * m_sliceSize;          // començament de la llesca X
        unsigned char * sliceY = m_reslicedData + ( i + 1 ) * m_sliceSize;  // començament de la llesca Y
        DEBUG_LOG( qPrintable( QString( "[CS] X = %1, Y = %2" ).arg( i ).arg( i + 1 ) ) );
        m_similarities << similarity( sliceX, sliceY );
    }
}


double Slicer::similarity( const unsigned char * sliceX, const unsigned char * sliceY ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)
    Histogram histogramX( m_nLabels ), histogramY( m_nLabels ); // histogrames individuals
    Histogram jointHistogram( m_nLabels * m_nLabels );  // histograma conjunt (2d)
    // la indexació serà valueX + m_nLabels * valueY

    for ( unsigned int j = 0; j < m_sliceSize; j++ )
    {
        unsigned char valueX = sliceX[j]; if ( valueX == 255 ) valueX = 0;
        unsigned char valueY = sliceY[j]; if ( valueY == 255 ) valueY = 0;
        //if ( valueX < m_nLabels && valueX > 0
        //     && valueY < m_nLabels && valueY > 0 )  // no comptem cap background
        //{
        histogramX.add( valueX ); histogramY.add( valueY );
        jointHistogram.add( valueX + m_nLabels * valueY );
        //}
    }

    double H_X_ = 0.0;
    double countX = histogramX.count();
    QVectorIterator< unsigned long > * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;

    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator< unsigned long > * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;

    double H_X_Y_ = 0.0;
    double count = jointHistogram.count();
    QVectorIterator< unsigned long > * itJointHistogram = jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    // H(X,Y) = 0 quan són llesques homogènies; llavors comprovem si són iguals o diferents per assingar el valor de semblança
    double similarity = ( ( H_X_Y_ > 0.0 ) ? ( I_X_Y_ / H_X_Y_ ) : ( sliceX[0] == sliceY[0] ? 1.0 : 0.0 ) );

    DEBUG_LOG( qPrintable( QString( "[entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) ) );
    DEBUG_LOG( qPrintable( QString( "[JE] H(X,Y) = %1" ).arg( H_X_Y_ ) ) );
    DEBUG_LOG( qPrintable( QString( "[IM] I(X;Y) = %1" ).arg( I_X_Y_ ) ) );
    DEBUG_LOG( qPrintable( QString( "[semblança] I/H = %1" ).arg( similarity ) ) );

    return similarity;
}


// Ajunta les llesques començant per la primera i ajuntant mentre la semblança entre (i) i (i0) sigui més gran que un llindar.
// Després comença un nou grup i va fent el mateix, i així fins que acaba.
void Slicer::method1A( double threshold )
{
    QVector< unsigned short > groups;   // cada posició correspon a una llesca i conté l'id del grup al qual pertany la llesca
    unsigned short g = 0;   // id del grup
    unsigned int x = 0;

    while ( x < m_sliceCount )
    {
        groups << g;    // comencem el nou grup
        unsigned int y = x + 1;
        bool grouped = true;

        while ( y < m_sliceCount && grouped )
        {
            unsigned char * sliceX = m_reslicedData + x * m_sliceSize;  // començament de la llesca X
            unsigned char * sliceY = m_reslicedData + y * m_sliceSize;  // començament de la llesca Y
            double similarity = this->similarity( sliceX, sliceY );
            DEBUG_LOG( qPrintable( QString( "[M1A] x = %1, y = %2; similarity = %3" ).arg( x ).arg( y ).arg( similarity ) ) );

            if ( similarity > threshold )
            {
                DEBUG_LOG( "[M1A] agrupem!!" );
                groups << g;
                y++;
            }
            else grouped = false;
        }

        x = y;
        g++;
    }

    DEBUG_LOG( qPrintable( QString( "[M1A] threshold = %1" ).arg( threshold ) ) );
    for ( unsigned int i = 0; i < groups.size(); i++ )
        DEBUG_LOG( qPrintable( QString( "[M1A] group %1 : slice %2" ).arg( groups[i] ).arg( i ) ) );

    QFile outFile( QDir::tempPath().append( QString( "/m1a%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        out << "threshold = " << threshold << "\n";
        for ( unsigned int i = 0; i < groups.size(); i++ )
            out << "group " << groups[i] << " : slice " << i << "\n";
        outFile.close();
    }
}


void Slicer::setGroup( QVector< unsigned short > & groups, unsigned short slice, unsigned short group ) const
{
    if ( slice < m_sliceCount - 1 &&  groups[slice+1] == groups[slice] )
        setGroup( groups, slice + 1, group );
    groups[slice] = group;
}


// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
void Slicer::method1B( double threshold )
{
    if ( m_similarities.isEmpty() ) computeSimilarities();

    QVector< QPair< double, unsigned short > > sortedSimilarities;

    for ( unsigned short i = 0; i < m_similarities.size(); i++ )
        sortedSimilarities << qMakePair( m_similarities[i], i );

    qSort( sortedSimilarities );    // ordenació creixent

    QVector< unsigned short > groups;   // cada posició correspon a una llesca i conté l'id del grup al qual pertany la llesca
    for ( unsigned short i = 0; i < m_sliceCount; i++ ) groups << i;    // inicialment cada llesca té un grup propi

    unsigned short i = sortedSimilarities.size() - 1;
    bool grouped = true;

    DEBUG_LOG( "[M1B] while" );

    // hauria de ser i >= 0, però com que és unsigned si es passa serà més gran que sortedSimilarities.size()
    // quedaria millor amb un iterador
    while ( i < sortedSimilarities.size() && grouped )
    {
        if ( sortedSimilarities[i].first > threshold )
        {
            //groups[sortedSimilarities[i].second+1] = groups[sortedSimilarities[i].second];
            setGroup( groups, sortedSimilarities[i].second + 1, groups[sortedSimilarities[i].second] );
        }
        else
        {
            grouped = false;
        }

        i--;
    }

    // assignem ids consecutius als groups
    unsigned short g = 0, lastGroup = groups[0];
    for ( unsigned int i = 0; i < groups.size(); i++ )
    {
        if ( groups[i] == lastGroup ) groups[i] = g;
        else
        {
            lastGroup = groups[i];
            groups[i] = ++g;
        }
    }

    DEBUG_LOG( qPrintable( QString( "[M1B] threshold = %1" ).arg( threshold ) ) );
    for ( unsigned int i = 0; i < groups.size(); i++ )
        DEBUG_LOG( qPrintable( QString( "[M1B] group %1 : slice %2" ).arg( groups[i] ).arg( i ) ) );

    QFile outFile( QDir::tempPath().append( QString( "/m1b%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        out << "threshold = " << threshold << "\n";
        for ( unsigned int i = 0; i < groups.size(); i++ )
            out << "group " << groups[i] << " : slice " << i << "\n";
        outFile.close();
    }
}


}
