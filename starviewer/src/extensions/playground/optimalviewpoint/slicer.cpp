/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
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

    m_input = 0;
    m_matrix = vtkMatrix4x4::New();

    m_readExtentFromFile = false;

    m_reslicedImage = 0;
}


Slicer::~Slicer()
{
    if ( m_input ) m_input->Delete();
    m_matrix->Delete();

    if ( m_reslicedImage ) m_reslicedImage->Delete();
}


void Slicer::setInput( vtkImageData * input )
{
    m_input = input; m_input->Register( 0 );
    double range[2];
    m_input->GetScalarRange( range );
    DEBUG_LOG( QString( "scalar range = %1 %2" ).arg( range[0] ).arg( range[1] ) );
    m_nLabels = static_cast< unsigned short >( round( range[1] ) ) + 1;
    m_newBackground = m_nLabels < 256 ? 255 : 0;
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


void Slicer::reslice( bool saveMhd, bool doClip )
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

    // Extract a slice in the desired orientation
    vtkImageReslice * reslice = vtkImageReslice::New();
    reslice->SetInput( m_input );
    reslice->SetOutputDimensionality( 3 );
    reslice->SetResliceAxes( resliceAxes );
    reslice->AutoCropOutputOn();
//     reslice->SetBackgroundLevel( 255.0 );
    reslice->SetBackgroundLevel( m_newBackground );
    reslice->SetOutputSpacing( m_xSpacing, m_ySpacing, m_zSpacing );
    reslice->Update();
//     reslice->Print( std::cout );

    // Trobar l'extent mínim
    vtkImageData * resliced = reslice->GetOutput();
    unsigned char * data = reinterpret_cast< unsigned char * >( resliced->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    int increments[3];
    resliced->GetIncrements( increments );
    int dims[3];
    resliced->GetDimensions( dims );
    int minX, maxX, minY, maxY, minZ, maxZ;
    bool hasExtent = false;

    if ( m_readExtentFromFile )
    {
        // llegim l'extent d'un fitxer
        QFile inFileExtent( QDir::tempPath().append( QString( "/extent%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
        if ( inFileExtent.open( QFile::ReadOnly ) )
        {
            QTextStream in( &inFileExtent );
            in >> minX >> maxX >> minY >> maxY >> minZ >> maxZ;
            inFileExtent.close();
            hasExtent = true;
        }
    }

    if ( !hasExtent)
    {
        findExtent( data, dims[0], dims[1], dims[2], increments[0], increments[1], increments[2], minX, maxX );
        findExtent( data, dims[1], dims[0], dims[2], increments[1], increments[0], increments[2], minY, maxY );
        findExtent( data, dims[2], dims[0], dims[1], increments[2], increments[0], increments[1], minZ, maxZ );

        // guardem l'extent en un fitxer
        QFile outFileExtent( QDir::tempPath().append( QString( "/extent%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
        if ( outFileExtent.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFileExtent );
            out << minX << " " << maxX << " " << minY << " " << maxY << " " << minZ << " " << maxZ;
            outFileExtent.close();
        }
    }
    DEBUG_LOG( QString( "minX = %1, maxX = %2" ).arg( minX ).arg( maxX ) );
    DEBUG_LOG( QString( "minY = %1, maxY = %2" ).arg( minY ).arg( maxY ) );
    DEBUG_LOG( QString( "minZ = %1, maxZ = %2" ).arg( minZ ).arg( maxZ ) );

    // Retallar la imatge fins al mínim extent
    vtkImageClip * clip = vtkImageClip::New();
    clip->SetInput( reslice->GetOutput() );
    clip->SetOutputWholeExtent( minX, maxX, minY, maxY, minZ, maxZ );
    if ( doClip ) clip->ClipDataOn();
    clip->Update();

    if ( saveMhd )
    {
        // Guardar la nova imatge en un fitxer
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
    }

    // Informació sobre la nova imatge
//     DEBUG_LOG( "------------------------------------------------------------" );
//     clipped->Print( std::cout );
//     DEBUG_LOG( "============================================================" );

    // Guardar la nova imatge als atributs de l'objecte
    m_reslicedImage = clip->GetOutput(); m_reslicedImage->Register( 0 );
    m_reslicedData = reinterpret_cast< unsigned char * >( m_reslicedImage->GetPointData()->GetScalars()->GetVoidPointer( 0 ) );
    m_reslicedDataSize = m_reslicedImage->GetPointData()->GetScalars()->GetSize();
    DEBUG_LOG( QString( "[Slicer] resliced data size = %1" ).arg( m_reslicedDataSize ) );
    int dimensions[3];
    m_reslicedImage->GetDimensions( dimensions );
    m_sliceSize = dimensions[0] * dimensions[1];
    DEBUG_LOG( QString( "[Slicer] slice size = %1" ).arg( m_sliceSize ) );
    m_sliceCount = dimensions[2];
    DEBUG_LOG( QString( "[Slicer] slice count = %1" ).arg( m_sliceCount ) );
    DEBUG_LOG( QString( "[Slicer] n labels = %1" ).arg( m_nLabels ) );
    DEBUG_LOG( QString( "[Slicer] new background = %1" ).arg( static_cast< short >( m_newBackground ) ) );

    // Destruir els objectes creats
    reslice->Delete();
    clip->Delete();
}


void Slicer::computeSmi()   /// \todo Fer-ho més eficient!!!
{
    DEBUG_LOG( "[*SMI*] ------------------ computeSmi 0 ------------------ " );
    // Primer una passada per tenir un histograma independent de les llesques
    Histogram oneHistogram( m_nLabels ); // to rule them all
    for ( unsigned int i = 0; i < m_reslicedDataSize; i++ )
    {
        unsigned char value = m_reslicedData[i];
        if ( value < m_nLabels && value > 0 )    // no comptem cap background
            oneHistogram.add( value );
    }
    QVector< double > p_o_;
    double oneCount = oneHistogram.count();
    DEBUG_LOG( QString( "[*SMI*] one count = %1" ).arg( oneCount ) );
    QVectorIterator< unsigned long > * itOneHistogram = oneHistogram.getIterator();
    while ( itOneHistogram->hasNext() )
    {
        double d = itOneHistogram->next() / oneCount;
        p_o_.append( d );
        DEBUG_LOG( QString( "[*SMI*] d = %1" ).arg( d ) );
    }
    delete itOneHistogram;

    DEBUG_LOG( "[*SMI*] ------------------ computeSmi 1 ------------------ " );
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
//             DEBUG_LOG( QString( "[*SMI*] p(o|s) = %1" ).arg( p_o_s_ ) );
            if ( p_o_s_ > 0.0 ) I_s_O_ += p_o_s_ * log( p_o_s_ / p_o_[o] );
            o++;
        }
        I_s_O_ /= log( 2.0 );
        DEBUG_LOG( QString( "[*SMI*] I(s,O) = %1" ).arg( I_s_O_ ) );
        delete itHistogram;
        m_smi.append( I_s_O_ );
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/smi%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        for ( int i = 0; i < m_smi.size(); i++ )
        {
            DEBUG_LOG( QString( "[SMI] SMI[%1] = %2" ).arg( i ).arg( m_smi[i] ) );
            out << "SMI[" << i << "] = " << m_smi[i] << "\n";
        }
        outFile.close();
    }
}


// Ajunta les llesques començant per la primera i ajuntant mentre la semblança entre (i) i (i0) sigui més gran que un llindar.
// Després comença un nou grup i va fent el mateix, i així fins que acaba.
void Slicer::method1A( double threshold )   /// \todo Fer-ho més eficient!!!
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
            DEBUG_LOG( QString( "[*M1A*] x = %1, y = %2; similarity = %3" ).arg( x ).arg( y ).arg( similarity ) );

            if ( similarity > threshold )
            {
                DEBUG_LOG( "[*M1A*] agrupem!!" );
                groups << g;
                y++;
            }
            else grouped = false;
        }

        x = y;
        g++;
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/m1a%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        DEBUG_LOG( QString( "[M1A] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        for ( int i = 0; i < groups.size(); i++ )
        {
            DEBUG_LOG( QString( "[M1A] group %1 : slice %2" ).arg( groups[i] ).arg( i ) );
            out << "group " << groups[i] << " : slice " << i << "\n";
        }
        outFile.close();
    }
}


// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
void Slicer::method1B_0( double threshold )   /// \todo Fer-ho més eficient!!!
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

    DEBUG_LOG( "[*M1B*] while" );

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
    for ( int i = 0; i < groups.size(); i++ )
    {
        if ( groups[i] == lastGroup ) groups[i] = g;
        else
        {
            lastGroup = groups[i];
            groups[i] = ++g;
        }
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/m1b%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        DEBUG_LOG( QString( "[M1B] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        for ( int i = 0; i < groups.size(); i++ )
        {
            DEBUG_LOG( QString( "[M1B] group %1 : slice %2" ).arg( groups[i] ).arg( i ) );
            out << "group " << groups[i] << " : slice " << i << "\n";
        }
        outFile.close();
    }
}


// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
void Slicer::method1B( double threshold )   /// \todo Fer-ho més eficient!!!
{
    if ( m_similarities.isEmpty() ) computeSimilarities();

    QVector< QPair< double, unsigned short > > sortedSimilarities;

    for ( unsigned short i = 0; i < m_similarities.size(); i++ )
        sortedSimilarities << qMakePair( m_similarities[i], i );

    qSort( sortedSimilarities );    // ordenació creixent

    QVector< unsigned short > groups;   // cada posició correspon a una llesca i conté l'id del grup al qual pertany la llesca
    QVector< unsigned short > rightGroups;   // cada posició correspon a una llesca i conté l'id de l'última llesca del grup al qual pertany la llesca
    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        groups << i;    // inicialment cada llesca té un grup propi
        rightGroups << i;
    }

    unsigned short i = sortedSimilarities.size() - 1;
    bool belowThreshold = true;

    // hauria de ser i >= 0, però com que és unsigned si es passa serà més gran que sortedSimilarities.size()
    // quedaria millor amb un iterador
    while ( i < sortedSimilarities.size() && belowThreshold )
    {
        if ( sortedSimilarities[i].first > threshold )
        {
            unsigned short sx = sortedSimilarities[i].second;
            unsigned short sy = sortedSimilarities[i].second + 1;
            unsigned short gx = groups[sx];
            unsigned short gy = rightGroups[sy];
            unsigned char * sliceX = m_reslicedData + gx * m_sliceSize;  // començament de la llesca X
            unsigned char * sliceY = m_reslicedData + gy * m_sliceSize;  // començament de la llesca Y
            double sim = similarity( sliceX, sliceY );
            DEBUG_LOG( QString( "[*M1B*] provem %1(g%2) i %3(rg%4) --> sim = %5" ).arg( sx ).arg( gx ).arg( sy ).arg( gy ).arg( sim ) );
            if ( sim > threshold )
            {
                DEBUG_LOG( QString( "[*M1B*] agrupem %1(g%2)(rg%3) i %4(g%5)(rg%6)" ).arg( sx ).arg( gx ).arg( rightGroups[sx] ).arg( sy ).arg( groups[sy] ).arg( gy ) );
                setGroup( groups, sy, gx );
                setRightGroup( rightGroups, sx, gy );
                DEBUG_LOG( QString( "[*M1B*] agrupats %1(g%2)(rg%3) i %4(g%5)(rg%6)" ).arg( sx ).arg( groups[sx] ).arg( rightGroups[sx] ).arg( sy ).arg( groups[sy] ).arg( rightGroups[sy] ) );
            }
        }
        else
        {
            belowThreshold = false;
        }

        i--;
    }

    // assignem ids consecutius als groups
    unsigned short g = 0, lastGroup = groups[0];
    for ( int i = 0; i < groups.size(); i++ )
    {
        if ( groups[i] == lastGroup ) groups[i] = g;
        else
        {
            lastGroup = groups[i];
            groups[i] = ++g;
        }
    }
    unsigned short rg = groups.last(), lastRightGroup = rightGroups.last();
    for ( int i = groups.size() - 1; i >= 0; i-- )
    {
        if ( rightGroups[i] == lastRightGroup ) rightGroups[i] = rg;
        else
        {
            lastRightGroup = rightGroups[i];
            rightGroups[i] = --rg;
        }
    }

    // Printar resultats i guardar-los en un fitxer
    QFile outFile( QDir::tempPath().append( QString( "/m1b%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        DEBUG_LOG( QString( "[M1B] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        for ( int i = 0; i < groups.size(); i++ )
        {
            DEBUG_LOG( QString( "[M1B] group %1 : slice %2 : right group %3" ).arg( groups[i] ).arg( i ).arg( rightGroups[i] ) );
            out << "group " << groups[i] << " : slice " << i << "\n";
        }
        outFile.close();
    }
}


void Slicer::findExtent( const unsigned char * data,
                         int dim0, int dim1, int dim2,
                         int inc0, int inc1, int inc2,
                         int & min0, int & max0 ) const
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
                found = *pointer != 0 && *pointer != m_newBackground;
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
                found = *pointer != 0 && *pointer != m_newBackground;
                i2--;
            }
            i1--;
        }
        if ( !found ) i0--;
    }
    if ( found ) max0 = i0;
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
        unsigned char valueX = sliceX[j]; if ( valueX == m_newBackground ) valueX = 0;
        unsigned char valueY = sliceY[j]; if ( valueY == m_newBackground ) valueY = 0;
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

    DEBUG_LOG( QString( "[*similarity*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
    DEBUG_LOG( QString( "[*similarity*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
    DEBUG_LOG( QString( "[*similarity*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );
    DEBUG_LOG( QString( "[*similarity*][semblança] I/H = %1" ).arg( similarity ) );

    return similarity;
}


void Slicer::computeSimilarities()  /// \todo Fer-ho més eficient!!!
{
    for ( int i = 0; i < m_sliceCount - 1; i++ )  // iterem sobre les llesques
    {
        unsigned char * sliceX = m_reslicedData + i * m_sliceSize;          // començament de la llesca X
        unsigned char * sliceY = m_reslicedData + ( i + 1 ) * m_sliceSize;  // començament de la llesca Y
        DEBUG_LOG( QString( "[*CS*] X = %1, Y = %2" ).arg( i ).arg( i + 1 ) );
        m_similarities << similarity( sliceX, sliceY );
    }
}


void Slicer::setGroup( QVector< unsigned short > & groups, unsigned short slice, unsigned short group ) const
{
    if ( slice < m_sliceCount - 1 && groups[slice+1] == groups[slice] )
        setGroup( groups, slice + 1, group );
    groups[slice] = group;
}


void Slicer::setRightGroup( QVector< unsigned short > & rightGroups, unsigned short slice, unsigned short group ) const
{
    if ( slice > 0 && rightGroups[slice-1] == rightGroups[slice] )
        setRightGroup( rightGroups, slice - 1, group );
    rightGroups[slice] = group;
}


void Slicer::setReadExtentFromFile( bool readExtentFromFile )
{
    m_readExtentFromFile = readExtentFromFile;
}


}
