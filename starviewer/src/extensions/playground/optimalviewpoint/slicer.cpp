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
#include <QMultiMap>
#include <QPair>
#include <QTextStream>

#include "doublehistogram.h"
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
    DEBUG_LOG( "setInput()" );
    m_input = input; m_input->Register( 0 );
    double range[2];
    DEBUG_LOG( "m_input->GetScalarRange" );
    m_input->GetScalarRange( range );
    DEBUG_LOG( QString( "scalar range = %1 %2" ).arg( range[0] ).arg( range[1] ) );
    m_nLabels = static_cast< unsigned short >( round( range[1] ) ) + 1;
    m_newBackground = m_nLabels < 256 ? 255 : 0;
}


void Slicer::setVector( const Vector3 & vector )
{
    m_vector = vector;
    m_vector.normalize();
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

    // Reslice the image in the desired orientation
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

    // Find minimum extent
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
        // read extent from file
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
        // Compute extent from data
        findExtent( data, dims[0], dims[1], dims[2], increments[0], increments[1], increments[2], minX, maxX );
        findExtent( data, dims[1], dims[0], dims[2], increments[1], increments[0], increments[2], minY, maxY );
        findExtent( data, dims[2], dims[0], dims[1], increments[2], increments[0], increments[1], minZ, maxZ );

        // store extent to file
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

    // Clip image to minimum extent
    vtkImageClip * clip = vtkImageClip::New();
    clip->SetInput( reslice->GetOutput() );
    clip->SetOutputWholeExtent( minX, maxX, minY, maxY, minZ, maxZ );
    if ( doClip ) clip->ClipDataOn();
    clip->Update();

    if ( saveMhd )
    {
        // Save resliced image to file
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

        QFile outFileXml( QDir::tempPath().append( QString( "/gmc%1.xml" ).arg( static_cast< short >( m_id ) ) ) );
        if ( outFileXml.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream outXml( &outFileXml );
            outXml << "<volume>\n";
            outXml << "<property name=\"Slicing Axis\" type=\"vector\">\n";
            outXml << "<![CDATA[(" << m_vector.x << ";" << m_vector.y << ";" << m_vector.z << ")]]>\n";
            outXml << "</property>\n";
            outXml << "<property name=\"Viewing Transformation\" type=\"matrix\">\n";
            outXml << "<![CDATA[("
                   << "(" << resliceAxes->Element[0][0] << ";" << resliceAxes->Element[0][1] << ";"
                          << resliceAxes->Element[0][2] << ";" << resliceAxes->Element[0][3] << ");"
                   << "(" << resliceAxes->Element[1][0] << ";" << resliceAxes->Element[1][1] << ";"
                          << resliceAxes->Element[1][2] << ";" << resliceAxes->Element[1][3] << ");"
                   << "(" << resliceAxes->Element[2][0] << ";" << resliceAxes->Element[2][1] << ";"
                          << resliceAxes->Element[2][2] << ";" << resliceAxes->Element[2][3] << ");"
                   << "(" << resliceAxes->Element[3][0] << ";" << resliceAxes->Element[3][1] << ";"
                          << resliceAxes->Element[3][2] << ";" << resliceAxes->Element[3][3] << ")"
                   << ")]]>\n";
            outXml << "</property>\n";
            outXml << "<similarities>\n\n</similarities>\n";
            outXml << "</volume>\n";
            outFileXml.close();
        }

        QFile outFileViola( QDir::tempPath().append( QString( "/viola_gmc%1.txt" ).arg( static_cast< short >( m_id ) ) ) );
        if ( outFileViola.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream outViola( &outFileViola );
            outViola << "(" << resliceAxes->Element[0][0] << ";" << resliceAxes->Element[0][1] << ";"
                            << resliceAxes->Element[0][2] << ";" << resliceAxes->Element[0][3] << ");"
                     << "(" << resliceAxes->Element[1][0] << ";" << resliceAxes->Element[1][1] << ";"
                            << resliceAxes->Element[1][2] << ";" << resliceAxes->Element[1][3] << ");"
                     << "(" << resliceAxes->Element[2][0] << ";" << resliceAxes->Element[2][1] << ";"
                            << resliceAxes->Element[2][2] << ";" << resliceAxes->Element[2][3] << ");"
                     << "(" << resliceAxes->Element[3][0] << ";" << resliceAxes->Element[3][1] << ";"
                            << resliceAxes->Element[3][2] << ";" << resliceAxes->Element[3][3] << ")"
                    << "\n";
            outViola << "(" << m_vector.x << ";" << m_vector.y << ";" << m_vector.z << ")\n";
            outFileViola.close();
        }
    }

    // Informació sobre la nova imatge
//     DEBUG_LOG( "------------------------------------------------------------" );
//     clipped->Print( std::cout );
//     DEBUG_LOG( "============================================================" );

    // Get some data about resliced image
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

    // Destroy created objects
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
    QVectorIterator<quint64> * itOneHistogram = oneHistogram.getIterator();
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
        QVectorIterator<quint64> * itHistogram = histogram.getIterator();
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
    // Start of the algorithm
    QVector< unsigned short > groups;   // every position corresponds to a slice and stores id of the group to which the slice belongs
    unsigned short g = 0;   // group id
    unsigned int x = 0;     // slice index

    while ( x < m_sliceCount )
    {
        groups << g;    // start new group
        unsigned int y = x + 1;
        bool grouped = true;

        while ( y < m_sliceCount && grouped )
        {
            unsigned char * sliceX = m_reslicedData + x * m_sliceSize;  // start of slice X
            unsigned char * sliceY = m_reslicedData + y * m_sliceSize;  // start of slice Y
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
    // End of the algorithm
    // The results are in the vector "groups"

    // Print results and save them in a file
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
void Slicer::method1B( double threshold )   /// \todo Fer-ho més eficient!!!
{
    // Start of the algorithm
    if ( m_similarities.isEmpty() ) computeSimilarities();  // compute similarities between pairs of consecutive slices

    // Order by similarity
    QVector< QPair< double, unsigned short > > sortedSimilarities;  // each position stores a pair ("similarity between (i) and (i+1)", i)

    for ( unsigned short i = 0; i < m_similarities.size(); i++ )
        sortedSimilarities << qMakePair( m_similarities[i], i );

    qSort( sortedSimilarities );    // sort in ascending order

    QVector< unsigned short > groups;   // every position corresponds to a slice s and stores id of the group to which the slice s belongs
    QVector< unsigned short > rightGroups;// every position corresponds to a slice s and stores id of the last slice of the group to which the slice s belongs
    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        groups << i;    // at the start every slice has an own group
        rightGroups << i;
    }

    unsigned short i = sortedSimilarities.size() - 1;
    bool belowThreshold = true; // it's name should be "aboveThreshold"

    // should be while i >= 0, but since i is unsigned, if it goes below 0 it will overflow to a number bigger than sortedSimilarities.size()
    // would be better with an iterator
    while ( i < sortedSimilarities.size() && belowThreshold )
    {
        if ( sortedSimilarities[i].first > threshold )
        {
            unsigned short sx = sortedSimilarities[i].second;
            unsigned short sy = sortedSimilarities[i].second + 1;
            unsigned short gx = groups[sx];
            unsigned short gy = rightGroups[sy];
            unsigned char * sliceX = m_reslicedData + gx * m_sliceSize;  // start of slice X
            unsigned char * sliceY = m_reslicedData + gy * m_sliceSize;  // start of slice Y
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

    // Assign consecutive ids to the groups
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
    // End of the algorithm
    // The results are in the vector "groups" (rightGroups was only temporary)

    // Print results and save them in a file
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


double Slicer::mutualInformation( const unsigned char * sliceX, const unsigned char * sliceY ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)
    Histogram histogramX( m_nLabels ), histogramY( m_nLabels ); // histogrames individuals
    Histogram jointHistogram( m_nLabels * m_nLabels );  // joint histogram (2d)
                                                        // indexation is valueX + m_nLabels * valueY

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
    QVectorIterator<quint64> * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;

    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator<quint64> * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;

    double H_X_Y_ = 0.0;
    double count = jointHistogram.count();
    QVectorIterator<quint64> * itJointHistogram = jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    DEBUG_LOG( QString( "[*mutualInformation*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
    DEBUG_LOG( QString( "[*mutualInformation*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
    DEBUG_LOG( QString( "[*mutualInformation*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );

    return I_X_Y_;
}


double Slicer::similarity( const unsigned char * sliceX, const unsigned char * sliceY, bool background ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)
    Histogram histogramX( m_nLabels ), histogramY( m_nLabels ); // individual histograms
    Histogram jointHistogram( m_nLabels * m_nLabels );  // joint histogram (2d)
                                                        // indexation is valueX + m_nLabels * valueY

    for ( unsigned int j = 0; j < m_sliceSize; j++ )
    {
        unsigned char valueX = sliceX[j]; if ( valueX == m_newBackground ) valueX = 0;
        unsigned char valueY = sliceY[j]; if ( valueY == m_newBackground ) valueY = 0;
        if ( background
             || ( valueX < m_nLabels && valueX > 0
             && valueY < m_nLabels && valueY > 0 ) )    // no comptem cap background si el paràmetre background és fals
        {
            histogramX.add( valueX ); histogramY.add( valueY );
            jointHistogram.add( valueX + m_nLabels * valueY );
        }
    }

    double H_X_ = 0.0;
    double countX = histogramX.count();
    QVectorIterator<quint64> * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;

    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator<quint64> * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;

    double H_X_Y_ = 0.0;
    double count = jointHistogram.count();
    QVectorIterator<quint64> * itJointHistogram = jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    // H(X,Y) = 0 when they are homogenious slices; then we check if they are equal or different to set similarity value
    double similarity = ( ( H_X_Y_ > 0.0 ) ? ( I_X_Y_ / H_X_Y_ ) : ( sliceX[0] == sliceY[0] ? 1.0 : 0.0 ) );

//     DEBUG_LOG( QString( "[*similarity*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
//     DEBUG_LOG( QString( "[*similarity*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarity*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarity*][semblança] I/H = %1" ).arg( similarity ) );

    return similarity;
}


void Slicer::computeSimilarities()  /// \todo Fer-ho més eficient!!!
{
    for ( int i = 0; i < m_sliceCount - 1; i++ )  // iterate over slices
    {
        unsigned char * sliceX = m_reslicedData + i * m_sliceSize;          // start of slice X
        unsigned char * sliceY = m_reslicedData + ( i + 1 ) * m_sliceSize;  // start of slice Y
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


// method to find most structured view
double Slicer::newMethod2( int step, bool normalized )
{
    double sum = 0.0;

    for ( int i = 0; i < m_sliceCount - step; i++ ) // iterarate over slices
    {
        unsigned char * sliceX = m_reslicedData + i * m_sliceSize;              // start of slice X
        unsigned char * sliceY = m_reslicedData + ( i + step ) * m_sliceSize;   // start of slice Y
        DEBUG_LOG( QString( "[*NM2*] X = %1, Y = %2" ).arg( i ).arg( i + step ) );
        sum += normalized ? similarity( sliceX, sliceY ) : mutualInformation( sliceX, sliceY );
    }

    // normalize sum
    sum /= m_sliceCount;

    DEBUG_LOG( QString( "[*NM2*] sum = %1" ).arg( sum ) );

    return sum;
}


// llesques fusionades
void Slicer::groupingMethodC( double threshold )    /// \todo Fer-ho més eficient!!!
{
    // Start of the algorithm
    /// \warning Agafem adreces d'elements d'un vector de qt que podrien canviar. S'hauria de fer d'una manera mes neta.
    // Create the vector directly with m_sliceCount elements to ensure addresses of individual elements will not change.
    // Should be done in a cleaner way.
    QVector< Group > groups( m_sliceCount );   // l'accés de lectura a un qvector o qlist pot ser més ràpid amb at(i) que [i] (mirar doc de qt)

    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        Slice s = { i, i, m_reslicedData + i * m_sliceSize };
        Group g;
        g.id = i;
        g.slices << s;
        g.similarityToNext = 0.0;
        g.previous = i > 0 ? &(groups[i - 1]) : 0;
        groups[i] = g;
        if ( g.previous ) groups[i].previous->next = &(groups[i]);
    }
    groups.last().next = 0;

    if ( m_similarities.isEmpty() ) computeSimilarities();

    QMultiMap< double, unsigned short > sortedSimilarities; // similarities sorted in ascending order

    for ( unsigned short i = 0; i < m_similarities.size(); i++ )
    {
        sortedSimilarities.insert( m_similarities[i], i );
        groups[i].similarityToNext = m_similarities[i];
    }

    bool aboveThreshold = true;
    unsigned short nGroups = m_sliceCount;

    while ( !sortedSimilarities.isEmpty() && aboveThreshold )
    {
        QMultiMap< double, unsigned short >::iterator it = --sortedSimilarities.end();  // last element

//         if ( it.key() > threshold )
        if ( nGroups > 5 )
        {
            Group & groupX = groups[it.value()];    // group X
            Group & groupY = *groupX.next;          // group Y (following X)
            DEBUG_LOG( QString( "[*GMC*] agrupem g%1(%2-%3) i g%4(%5-%6) (sim = %7)" ).arg( groupX.id ).arg( groupX.slices.first().id ).arg( groupX.slices.last().id ).arg( groupY.id ).arg( groupY.slices.first().id ).arg( groupY.slices.last().id ).arg( it.key() ) );
            join( groupX, groupY );
            DEBUG_LOG( "[*GMC*] agrupats :)" );

            // remove obsolete entries form sortedSimilarities
            sortedSimilarities.erase( it );
            if ( groupX.previous )
            {
                Group & groupW = *groupX.previous;
                QMultiMap< double, unsigned short >::iterator itW = sortedSimilarities.find( groupW.similarityToNext );
                while ( itW != sortedSimilarities.end() && itW.value() != groupW.id ) ++itW;
                if ( itW != sortedSimilarities.end() ) sortedSimilarities.erase( itW );
                else DEBUG_LOG( "[*GMC*] problema esborrant groupW!!!" );
            }
            if ( groupY.next )
            {
                QMultiMap< double, unsigned short >::iterator itY = sortedSimilarities.find( groupY.similarityToNext );
                while ( itY != sortedSimilarities.end() && itY.value() != groupY.id ) ++itY;
                if ( itY != sortedSimilarities.end() ) sortedSimilarities.erase( itY );
                else DEBUG_LOG( "[*GMC*] problema esborrant groupY!!!" );
            }

            // compute new similarities
            if ( groupX.previous )
            {
                Group & groupW = *groupX.previous;
                groupW.similarityToNext = similarity( groupW, groupX );
//                 DEBUG_LOG( QString( "[**GMC**] sim = %1" ).arg( groupW.similarityToNext ) );
//                 double sim2 = similarity2( groupW, groupX );
//                 DEBUG_LOG( QString( "[**GMC**] sim2 = %2" ).arg( sim2 ) );
                sortedSimilarities.insert( groupW.similarityToNext, groupW.id );
            }
            if ( groupX.next )
            {
                Group & groupZ = *groupX.next;
                groupX.similarityToNext = similarity( groupX, groupZ );
//                 DEBUG_LOG( QString( "[**GMC**] sim = %1" ).arg( groupX.similarityToNext ) );
//                 double sim2 = similarity2( groupX, groupZ );
//                 DEBUG_LOG( QString( "[**GMC**] sim2 = %2" ).arg( sim2 ) );
                sortedSimilarities.insert( groupX.similarityToNext, groupX.id );
            }
            else groupX.similarityToNext = 0.0;

            // invalidate groupY (to later assign consecutive ids)
            groupY.id = groupX.id;

            nGroups--;
        }
        else
        {
            aboveThreshold = false;
        }
    }

    // assign consecutive ids to groups
    unsigned short g = 0;
    QList< Group > finalGroups;
    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        if ( i == groups[i].id )    // the group is valid
        {
            groups[i].id = g++;
            finalGroups << groups[i];
        }
    }
    // End of the algorithm
    // The results are in the vector "finalGroups"

    // Print results i save them to a file
    QFile outFile( QDir::tempPath().append( QString( "/gmc%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileXml( QDir::tempPath().append( QString( "/gmc%1_%2.xml" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileViola( QDir::tempPath().append( QString( "/viola_gmc%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate )
        && outFileXml.open( QFile::WriteOnly | QFile::Truncate )
        && outFileViola.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        QTextStream outXml( &outFileXml );
        QTextStream outViola( &outFileViola );

        outXml << "<similarity>\n";

        DEBUG_LOG( QString( "[GMC] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        outXml << "<value type=\"float\"><![CDATA[" << threshold << "]]></value>\n";
        outXml << "<slabs>\n";
        for ( int i = 0; i < finalGroups.size(); i++ )
        {
            const Group & gr = finalGroups[i];
            for ( int j = 0; j < gr.slices.size(); j++ )
            {
                DEBUG_LOG( QString( "[GMC] group %1 : slice %2" ).arg( gr.id ).arg( gr.slices[j].id ) );
                out << "group " << gr.id << " : slice " << gr.slices[j].id << "\n";
            }
            outXml << "<position type=\"float\"><![CDATA[" << static_cast<double>(gr.slices[0].id) / m_sliceCount << "]]></position>\n";
            outViola << "(" << threshold << ";" << static_cast<double>(gr.slices[0].id) / m_sliceCount << ")\n";
        }

        outXml << "</slabs>\n</similarity>\n";

        outFile.close();
        outFileXml.close();
        outFileViola.close();
    }
}


double Slicer::similarity( const Group & groupX, const Group & groupY ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)
    DoubleHistogram histogramX( m_nLabels ), histogramY( m_nLabels );   // individual histograms
    DoubleHistogram jointHistogram( m_nLabels * m_nLabels );            // joint histogram (2d)
                                                                        // indexation is valueX + m_nLabels * valueY
    unsigned short nx = groupX.slices.size(), ny = groupY.slices.size();
    double amountX = 1.0 / nx, amountY = 1.0 / ny, amountXY = amountX * amountY;

    for ( unsigned int i = 0; i < m_sliceSize; i++ )
    {
        // iterate over all slices of every group
        for ( unsigned short sx = 0; sx < nx; sx++ )
        {
            for ( unsigned short sy = 0; sy < ny; sy++ )
            {
                unsigned char valueX = groupX.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
                unsigned char valueY = groupY.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;
                //if ( valueX < m_nLabels && valueX > 0
                //     && valueY < m_nLabels && valueY > 0 )  // no comptem cap background
                //{
                histogramX.add( valueX, amountX ); histogramY.add( valueY, amountY );
                jointHistogram.add( valueX + m_nLabels * valueY, amountXY );
                //}
            }
        }
    }

    double H_X_ = 0.0;
    double countX = histogramX.count();
    QVectorIterator< double > * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;

    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator< double > * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;

    double H_X_Y_ = 0.0;
    double count = jointHistogram.count();
    QVectorIterator< double > * itJointHistogram = jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    // H(X,Y) = 0 when they are homogenious groups; then check wether they are equal or different to set the similarity value
    double similarity = ( ( H_X_Y_ > 0.0 ) ? ( I_X_Y_ / H_X_Y_ ) : ( groupX.slices[0].data[0] == groupY.slices[0].data[0] ? 1.0 : 0.0 ) );


//     double min = H_X_ < H_Y_ ? H_X_ : H_Y_;
//     double max = H_X_ > H_Y_ ? H_X_ : H_Y_;
//     double similarity = ( ( max > 0.0 ) ? ( I_X_Y_ / max ) : ( groupX.slices[0].data[0] == groupY.slices[0].data[0] ? 1.0 : 0.0 ) );

//     DEBUG_LOG( QString( "[*similarityG*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][semblança] I/H = %1" ).arg( similarity ) );

    return similarity;
}


double Slicer::similarity2( const Group & groupX, const Group & groupY ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)
    DoubleHistogram histogramX( m_nLabels ), histogramY( m_nLabels );   // individual histograms
    DoubleHistogram jointHistogram( m_nLabels * m_nLabels );            // joint histogram (2d)
                                                                        // indexation is valueX + m_nLabels * valueY
    unsigned short nx = groupX.slices.size(), ny = groupY.slices.size();
    double amountX = 1.0 / nx, amountY = 1.0 / ny, amountXY = amountX * amountY;

    for ( unsigned int i = 0; i < m_sliceSize; i++ )
    {
        // iterate over all slices of every group
        for ( unsigned short sx = 0; sx < nx; sx++ )
        {
            for ( unsigned short sy = 0; sy < ny; sy++ )
            {
                unsigned char valueX = groupX.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
                unsigned char valueY = groupY.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;
                //if ( valueX < m_nLabels && valueX > 0
                //     && valueY < m_nLabels && valueY > 0 )  // no comptem cap background
                //{
                if ( sy == 0 ) histogramX.add( valueX, amountX );
                if ( sx == 0 ) histogramY.add( valueY, amountY );
                jointHistogram.add( valueX + m_nLabels * valueY, amountXY );
                //}
            }
        }
    }

    double H_X_ = 0.0;
    double countX = histogramX.count();
    QVectorIterator< double > * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;

    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator< double > * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;

    double H_X_Y_ = 0.0;
    double count = jointHistogram.count();
    QVectorIterator< double > * itJointHistogram = jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    // H(X,Y) = 0 when they are homogenious groups; then check wether they are equal or different to set the similarity value
    double similarity = ( ( H_X_Y_ > 0.0 ) ? ( I_X_Y_ / H_X_Y_ ) : ( groupX.slices[0].data[0] == groupY.slices[0].data[0] ? 1.0 : 0.0 ) );


//     double min = H_X_ < H_Y_ ? H_X_ : H_Y_;
//     double max = H_X_ > H_Y_ ? H_X_ : H_Y_;
//     double similarity = ( ( max > 0.0 ) ? ( I_X_Y_ / max ) : ( groupX.slices[0].data[0] == groupY.slices[0].data[0] ? 1.0 : 0.0 ) );

//     DEBUG_LOG( QString( "[*similarityG*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );
//     DEBUG_LOG( QString( "[*similarityG*][semblança] I/H = %1" ).arg( similarity ) );

    return similarity;
}


void Slicer::join( Group & groupX, Group & groupY ) const
{
    for ( unsigned short i = 0; i < groupY.slices.size(); i++ )
    {
        groupY.slices[i].group = groupX.id;
    }

    groupX.slices << groupY.slices;
    groupX.next = groupY.next;
    if ( groupX.next ) groupX.next->previous = &groupX;
}


double Slicer::jensenShannonDivergence( const Group & groupX, const Group & groupY ) const
{
    DoubleHistogram histogramX( m_nLabels ), histogramY( m_nLabels );   // individual histograms
    DoubleHistogram histogramXY( m_nLabels );                           // merge histogram
    unsigned short nx = groupX.slices.size(), ny = groupY.slices.size();
    double amountX = 1.0 / nx, amountY = 1.0 / ny, amountXY = 1.0 / ( nx + ny );

    /*
    // amb background
    for ( unsigned int i = 0; i < m_sliceSize; i++ )
    {
        // iterate over all slices of every group
        for ( unsigned short sx = 0; sx < nx; sx++ )
        {
            unsigned char valueX = groupX.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
            //if ( valueX < m_nLabels && valueX > 0 ) // no comptem cap background
            //{
            histogramX.add( valueX, amountX );
            histogramXY.add( valueX, amountXY );
            //}
        }
        for ( unsigned short sy = 0; sy < ny; sy++ )
        {
            unsigned char valueY = groupY.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;
            //if ( valueY < m_nLabels && valueY > 0 ) // no comptem cap background
            //{
            histogramY.add( valueY, amountY );
            histogramXY.add( valueY, amountXY );
            //}
        }
    }
    */

    /*
    // sense background
    for ( unsigned int i = 0; i < m_sliceSize; i++ )
    {
        // iterate over all slices of every group
        for ( unsigned short sx = 0; sx < nx; sx++ )
        {
            unsigned char valueX = groupX.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
            if ( valueX < m_nLabels && valueX > 0 ) // no comptem cap background
            {
            histogramX.add( valueX );
            histogramXY.add( valueX );
            }
        }
        for ( unsigned short sy = 0; sy < ny; sy++ )
        {
            unsigned char valueY = groupY.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;
            if ( valueY < m_nLabels && valueY > 0 ) // no comptem cap background
            {
            histogramY.add( valueY );
            histogramXY.add( valueY );
            }
        }
    }
    */

    // sense background i amb altres pesos
    for ( unsigned int i = 0; i < m_sliceSize; i++ )
    {
        // iterate over all slices of every group
        for ( unsigned short sx = 0; sx < nx; sx++ )
        {
            unsigned char valueX = groupX.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
            if ( valueX < m_nLabels && valueX > 0 ) // no comptem cap background
            {
            histogramX.add( valueX );
            }
        }
        for ( unsigned short sy = 0; sy < ny; sy++ )
        {
            unsigned char valueY = groupY.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;
            if ( valueY < m_nLabels && valueY > 0 ) // no comptem cap background
            {
            histogramY.add( valueY );
            }
        }
    }
    double pointsPerSliceX = histogramX.count() / nx, pointsPerSliceY = histogramY.count() / ny;
    double pointsPerSliceXY = pointsPerSliceX + pointsPerSliceY;
    double piX = pointsPerSliceX / pointsPerSliceXY, piY = pointsPerSliceY / pointsPerSliceXY;
//     DEBUG_LOG( QString( "piX = %1, piY = %2, piX + piY = %3" ).arg( piX ).arg( piY ).arg( piX + piY ) );
    QVectorIterator< double > * _itHistogramX = histogramX.getIterator();
    QVectorIterator< double > * _itHistogramY = histogramY.getIterator();
    double cx = histogramX.count(), cy = histogramY.count();
    int i = 0;
    while ( _itHistogramX->hasNext() )
    {
        double x = _itHistogramX->next() / cx;
        double y = _itHistogramY->next() / cy;
        double xy = x * piX + y * piY;
        histogramXY.add( i, xy );
        i++;
    }

//     double sum = 0.0;
    double H_X_ = 0.0;
    double countX = histogramX.count();
    QVectorIterator< double > * itHistogramX = histogramX.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
//         sum += p_x_;
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;
//     DEBUG_LOG( QString( "sum p(x) = %1" ).arg( sum ) );

//     sum = 0.0;
    double H_Y_ = 0.0;
    double countY = histogramY.count();
    QVectorIterator< double > * itHistogramY = histogramY.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
//         sum += p_y_;
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;
//     DEBUG_LOG( QString( "sum p(y) = %1" ).arg( sum ) );

//     sum = 0.0;
    double H_XY_ = 0.0;
    double countXY = histogramXY.count();
    QVectorIterator< double > * itHistogramXY = histogramXY.getIterator();
    while ( itHistogramXY->hasNext() )
    {
        double p_xy_ = itHistogramXY->next() / countXY;
        if ( p_xy_ > 0.0 ) H_XY_ -= p_xy_ * log( p_xy_ );
//         sum += p_xy_;
    }
    H_XY_ /= log( 2.0 );
    delete itHistogramXY;
//     DEBUG_LOG( QString( "sum p(xy) = %1" ).arg( sum ) );

    //double jsd = H_XY_ - nx * amountXY * H_X_ - ny * amountXY * H_Y_; // versió amb background
//     double jsd = H_XY_ - (countX / countXY) * H_X_ - (countY / countXY) * H_Y_;   // versió sense background
    double jsd = H_XY_ - piX * H_X_ - piY * H_Y_;   // versió sense background i amb altres pesos

//     DEBUG_LOG( QString( "[*JSD*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
//     DEBUG_LOG( QString( "[*JSD*][CE] H(XY) = %1" ).arg( H_XY_ ) );
//     DEBUG_LOG( QString( "[*JSD*][JSD] JSD = %1" ).arg( jsd ) );

    return jsd;
}


// llesques fusionades (Jensen-Shannon)
void Slicer::groupingMethodC_JS( double threshold )    /// \todo Fer-ho més eficient!!!
{
    // Start of the algorithm
    /// \warning Agafem adreces d'elements d'un vector de qt que podrien canviar. S'hauria de fer d'una manera mes neta.
    // Create the vector directly with m_sliceCount elements to ensure addresses of individual elements will not change.
    // Should be done in a cleaner way.
    QVector< Group > groups( m_sliceCount );   // l'accés de lectura a un qvector o qlist pot ser més ràpid amb at(i) que [i] (mirar doc de qt)

    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        Slice s = { i, i, m_reslicedData + i * m_sliceSize };
        Group g;
        g.id = i;
        g.slices << s;
        g.similarityToNext = 0.0;
        g.previous = i > 0 ? &(groups[i - 1]) : 0;
        groups[i] = g;
        if ( g.previous ) groups[i].previous->next = &(groups[i]);
    }
    groups.last().next = 0;

    QVector<double> divergences;
    for ( int i = 0; i < m_sliceCount - 1; i++ )  // iterate over slices
    {
        divergences << jensenShannonDivergence( groups[i], groups[i+1] );
    }

    QMultiMap< double, unsigned short > sortedDivergences;  // divergences sorted in ascending order

    for ( unsigned short i = 0; i < divergences.size(); i++ )
    {
        sortedDivergences.insert( divergences[i], i );
        groups[i].similarityToNext = divergences[i];    // divergence to next
    }

    bool belowThreshold = true;
    unsigned short nGroups = m_sliceCount;

    while ( !sortedDivergences.isEmpty() && belowThreshold )
    {
        QMultiMap< double, unsigned short >::iterator it = sortedDivergences.begin();   // first element

//         if ( it.key() < threshold )
        if ( nGroups > 5 )
        {
            Group & groupX = groups[it.value()];    // group X
            Group & groupY = *groupX.next;          // group Y (following X)
            DEBUG_LOG( QString( "[*GMC_JS*] agrupem g%1(%2-%3) i g%4(%5-%6) (sim = %7)" ).arg( groupX.id ).arg( groupX.slices.first().id ).arg( groupX.slices.last().id ).arg( groupY.id ).arg( groupY.slices.first().id ).arg( groupY.slices.last().id ).arg( it.key() ) );
            join( groupX, groupY );
            DEBUG_LOG( "[*GMC_JS*] agrupats :)" );

            // remove obsolete entries form sortedDivergences
            sortedDivergences.erase( it );
            if ( groupX.previous )
            {
                Group & groupW = *groupX.previous;
                QMultiMap< double, unsigned short >::iterator itW = sortedDivergences.find( groupW.similarityToNext );
                while ( itW != sortedDivergences.end() && itW.value() != groupW.id ) ++itW;
                if ( itW != sortedDivergences.end() ) sortedDivergences.erase( itW );
                else DEBUG_LOG( "[*GMC_JS*] problema esborrant groupW!!!" );
            }
            if ( groupY.next )
            {
                QMultiMap< double, unsigned short >::iterator itY = sortedDivergences.find( groupY.similarityToNext );
                while ( itY != sortedDivergences.end() && itY.value() != groupY.id ) ++itY;
                if ( itY != sortedDivergences.end() ) sortedDivergences.erase( itY );
                else DEBUG_LOG( "[*GMC_JS*] problema esborrant groupY!!!" );
            }

            // compute new divergences
            if ( groupX.previous )
            {
                Group & groupW = *groupX.previous;
                groupW.similarityToNext = jensenShannonDivergence( groupW, groupX );
                sortedDivergences.insert( groupW.similarityToNext, groupW.id );
            }
            if ( groupX.next )
            {
                Group & groupZ = *groupX.next;
                groupX.similarityToNext = jensenShannonDivergence( groupX, groupZ );
                sortedDivergences.insert( groupX.similarityToNext, groupX.id );
            }
            else groupX.similarityToNext = 0.0;

            // invalidate groupY (to later assign consecutive ids)
            groupY.id = groupX.id;

            nGroups--;
        }
        else
        {
            belowThreshold = false;
        }
    }

    // assign consecutive ids to groups
    unsigned short g = 0;
    QList< Group > finalGroups;
    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        if ( i == groups[i].id )    // the group is valid
        {
            groups[i].id = g++;
            finalGroups << groups[i];
        }
    }
    // End of the algorithm
    // The results are in the vector "finalGroups"

    // Print results i save them to a file
    QFile outFile( QDir::tempPath().append( QString( "/gmc_js%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileXml( QDir::tempPath().append( QString( "/gmc_js%1_%2.xml" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileViola( QDir::tempPath().append( QString( "/viola_gmc_js%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate )
        && outFileXml.open( QFile::WriteOnly | QFile::Truncate )
        && outFileViola.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        QTextStream outXml( &outFileXml );
        QTextStream outViola( &outFileViola );

        outXml << "<similarity>\n";

        DEBUG_LOG( QString( "[GMC_JS] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        outXml << "<value type=\"float\"><![CDATA[" << threshold << "]]></value>\n";
        outXml << "<slabs>\n";
        for ( int i = 0; i < finalGroups.size(); i++ )
        {
            const Group & gr = finalGroups[i];
            for ( int j = 0; j < gr.slices.size(); j++ )
            {
                DEBUG_LOG( QString( "[GMC_JS] group %1 : slice %2" ).arg( gr.id ).arg( gr.slices[j].id ) );
                out << "group " << gr.id << " : slice " << gr.slices[j].id << "\n";
            }
            outXml << "<position type=\"float\"><![CDATA[" << static_cast<double>(gr.slices[0].id) / m_sliceCount << "]]></position>\n";
            outViola << "(" << threshold << ";" << static_cast<double>(gr.slices[0].id) / m_sliceCount << ")\n";
        }

        outXml << "</slabs>\n</similarity>\n";

        outFile.close();
        outFileXml.close();
        outFileViola.close();
    }
}


// llesques fusionades
void Slicer::splittingMethodC( double threshold )   /// \todo Fer-ho més eficient!!!
{
    // Start of the algorithm
    QList<Group> groups;    // l'accés de lectura a un qvector o qlist pot ser més ràpid amb at(i) que [i] (mirar doc de qt)
    Group firstGroup;
    firstGroup.id = 0;

    for ( unsigned short i = 0; i < m_sliceCount; i++ )
    {
        Slice s = { i, 0, m_reslicedData + i * m_sliceSize };
        firstGroup.slices << s;
    }

    groups << firstGroup;

    bool belowThreshold = true;

    while ( belowThreshold )
    {
        double minSimilarity = 1.0;
        Partition minPartition;
        unsigned short minGroup;

        for ( unsigned short i = 0; i < groups.size(); i++ )
        {
            const Group & group = groups[i];

            if ( group.slices.size() == 1 ) continue;

            Partition partition = firstPartition( group );

            do
            {
                double sim = similarity( partition );
                if ( sim < minSimilarity )
                {
                    minSimilarity = sim;
                    minPartition = partition;
                    minGroup = i;
                    DEBUG_LOG( QString( "[*SMC*] min: %1 | %2 || sim = %3" ).arg( partition.g1.slices.first().id ).arg( partition.g2.slices.first().id ).arg( sim ) );
                }
            }
            while ( nextPartition( partition ) );
        }

        //if ( minSimilarity < threshold )
        if ( minPartition.g1.id != 0 )
        {
            split( minPartition );  // en realitat no caldria, perquè en aquest mètode no fem servir els ids dels grups

            groups.removeAt( minGroup );
            groups.insert( minGroup, minPartition.g2 );
            groups.insert( minGroup, minPartition.g1 );

            DEBUG_LOG( QString( "[*SMC*] Split: %1 | %2 || sim = %3" ).arg( minPartition.g1.id ).arg( minPartition.g2.id ).arg( minSimilarity ) );

            if ( groups.size() == 5 ) belowThreshold = false;
        }
        else
        {
            belowThreshold = false;
        }
    }

    // assign consecutive ids to groups
    for ( unsigned short i = 0; i < groups.size(); i++ )
    {
        groups[i].id = i;
    }
    // End of the algorithm
    // The results are in the list "groups"

    // Print results i save them to a file
    QFile outFile( QDir::tempPath().append( QString( "/smc%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileXml( QDir::tempPath().append( QString( "/smc%1_%2.xml" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    QFile outFileViola( QDir::tempPath().append( QString( "/viola_smc%1_%2.txt" ).arg( static_cast< short >( m_id ) ).arg( threshold ) ) );
    if ( outFile.open( QFile::WriteOnly | QFile::Truncate )
        && outFileXml.open( QFile::WriteOnly | QFile::Truncate )
        && outFileViola.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QTextStream out( &outFile );
        QTextStream outXml( &outFileXml );
        QTextStream outViola( &outFileViola );

        outXml << "<similarity>\n";

        DEBUG_LOG( QString( "[SMC] threshold = %1" ).arg( threshold ) );
        out << "threshold = " << threshold << "\n";
        outXml << "<value type=\"float\"><![CDATA[" << threshold << "]]></value>\n";
        outXml << "<slabs>\n";
        for ( int i = 0; i < groups.size(); i++ )
        {
            const Group & gr = groups[i];
            for ( int j = 0; j < gr.slices.size(); j++ )
            {
                DEBUG_LOG( QString( "[SMC] group %1 : slice %2" ).arg( gr.id ).arg( gr.slices[j].id ) );
                out << "group " << gr.id << " : slice " << gr.slices[j].id << "\n";
            }
            outXml << "<position type=\"float\"><![CDATA[" << static_cast<double>(gr.slices[0].id) / m_sliceCount << "]]></position>\n";
            outViola << "(" << threshold << ";" << static_cast<double>(gr.slices[0].id) / m_sliceCount << ")\n";
        }

        outXml << "</slabs>\n</similarity>\n";

        outFile.close();
        outFileXml.close();
        outFileViola.close();
    }
}


Slicer::Partition Slicer::firstPartition( const Group & group ) const
{
    Partition partition;

    if ( group.slices.size() > 1 )
    {
        Group g1, g2;
        g2.id = 2; g2.slices << group.slices;
        g1.id = 1; g1.slices << g2.slices.takeFirst();

        partition.g1 = g1; partition.g2 = g2;

        fillHistograms( partition );
    }

    return partition;
}


bool Slicer::nextPartition( Partition & partition ) const
{
    if ( partition.g2.slices.size() == 1 ) return false;

    partition.g1.slices << partition.g2.slices.takeFirst();

    fillHistograms( partition );

    return true;
}


void Slicer::fillHistograms( Partition & partition, bool background ) const
{
    if ( partition.g1.id == 0 ) return; // invalid partition

    unsigned short nx = partition.g1.slices.size(), ny = partition.g2.slices.size();

    if ( partition.g1Histogram.size() == 0 )    // first call with this partition
    {
        DEBUG_LOG( "fillHistograms first call" );
        partition.g1Histogram.setSize( m_nLabels );
        partition.g2Histogram.setSize( m_nLabels );
        partition.jointHistogram.setSize( m_nLabels * m_nLabels );

        for ( unsigned int i = 0; i < m_sliceSize; i++ )
        {
            // iterate over all slices of every group
            for ( unsigned short sx = 0; sx < nx; sx++ )
            {
                for ( unsigned short sy = 0; sy < ny; sy++ )
                {
                    unsigned char valueX = partition.g1.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;
                    unsigned char valueY = partition.g2.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;

                    if ( background
                         || ( valueX < m_nLabels && valueX > 0
                              && valueY < m_nLabels && valueY > 0 ) )   // don't count any background
                    {
                        if ( sy == 0 ) partition.g1Histogram.add( valueX );
                        if ( sx == 0 ) partition.g2Histogram.add( valueY );
                        partition.jointHistogram.add( valueX + m_nLabels * valueY );
                    }
                }
            }
        }
    }
    else    // just adjust histograms
    {
        DEBUG_LOG( "fillHistograms adjust" );
        for ( unsigned int i = 0; i < m_sliceSize; i++ )
        {
            unsigned char value = partition.g1.slices.last().data[i]; if ( value == m_newBackground ) value = 0;

            if ( background
                 || value < m_nLabels && value > 0 )  // don't count any background
            {
                partition.g1Histogram.add( value ); partition.g2Histogram.substract( value );
            }

            // iterate over all slices of both groups
            for ( unsigned short sx = 0; sx < nx - 1; sx++ )
            {
                unsigned char valueX = partition.g1.slices[sx].data[i]; if ( valueX == m_newBackground ) valueX = 0;

                if ( background
                     || ( valueX < m_nLabels && valueX > 0
                          && value < m_nLabels && value > 0 ) ) // don't count any background
                {
                    partition.jointHistogram.substract( valueX + m_nLabels * value );
                }
            }
            for ( unsigned short sy = 0; sy < ny; sy++ )
            {
                unsigned char valueY = partition.g2.slices[sy].data[i]; if ( valueY == m_newBackground ) valueY = 0;

                if ( background
                     || ( value < m_nLabels && value > 0
                          && valueY < m_nLabels && valueY > 0 ) )   // don't count any background
                {
                    partition.jointHistogram.add( value + m_nLabels * valueY );
                }
            }
        }
    }

    DEBUG_LOG( QString( "[*FH*] H(g1).count() = %1" ).arg( partition.g1Histogram.count() ) );
    DEBUG_LOG( QString( "[*FH*] H(g2).count() = %1" ).arg( partition.g2Histogram.count() ) );
    DEBUG_LOG( QString( "[*FH*] H(g1,g2).count() = %1" ).arg( partition.jointHistogram.count() ) );
}


double Slicer::similarity( const Partition & partition ) const
{
    // H(X) = -\sum_x p_x \log_2(p_x)
    // H(X,Y) = -\sum_{x,y} p_{x,y} \log_2(p_{x,y})
    // I(X;Y) = \sum_{y \in Y} \sum_{x \in X} p(x,y) \log{ \left( \frac{p(x,y)}{p(x)\,p(y)} \right) }
    // I(X;Y) = H(X) + H(Y) - H(X,Y)

    double sum = 0.0;
    double H_X_ = 0.0;
    double countX = partition.g1Histogram.count();
    QVectorIterator<quint64> * itHistogramX = partition.g1Histogram.getIterator();
    while ( itHistogramX->hasNext() )
    {
        double p_x_ = itHistogramX->next() / countX;
        if ( p_x_ > 0.0 ) H_X_ -= p_x_ * log( p_x_ );
        sum += p_x_;
    }
    H_X_ /= log( 2.0 );
    delete itHistogramX;
    DEBUG_LOG( QString( "sum p(x) = %1" ).arg( sum ) );

    sum = 0.0;
    double H_Y_ = 0.0;
    double countY = partition.g2Histogram.count();
    QVectorIterator<quint64> * itHistogramY = partition.g2Histogram.getIterator();
    while ( itHistogramY->hasNext() )
    {
        double p_y_ = itHistogramY->next() / countY;
        if ( p_y_ > 0.0 ) H_Y_ -= p_y_ * log( p_y_ );
        sum += p_y_;
    }
    H_Y_ /= log( 2.0 );
    delete itHistogramY;
    DEBUG_LOG( QString( "sum p(y) = %1" ).arg( sum ) );

    sum = 0.0;
    double H_X_Y_ = 0.0;
    double count = partition.jointHistogram.count();
    QVectorIterator<quint64> * itJointHistogram = partition.jointHistogram.getIterator();
    while ( itJointHistogram->hasNext() )
    {
        double p_x_y_ = itJointHistogram->next() / count;
        if ( p_x_y_ > 0.0 ) H_X_Y_ -= p_x_y_ * log( p_x_y_ );
        sum += p_x_y_;
    }
    H_X_Y_ /= log( 2.0 );
    delete itJointHistogram;
    DEBUG_LOG( QString( "sum p(x,y) = %1" ).arg( sum ) );

    double I_X_Y_ = H_X_ + H_Y_ - H_X_Y_;

    // H(X,Y) = 0 when they are homogenious groups; then check wether they are equal or different to set the similarity value
    double similarity = ( ( H_X_Y_ > 0.0 ) ? ( I_X_Y_ / H_X_Y_ ) : ( partition.g1.slices[0].data[0] == partition.g2.slices[0].data[0] ? 1.0 : 0.0 ) );

//     double min = H_X_ < H_Y_ ? H_X_ : H_Y_;
//     double max = H_X_ > H_Y_ ? H_X_ : H_Y_;
//     double similarity = ( ( max > 0.0 ) ? ( I_X_Y_ / max ) : ( groupX.slices[0].data[0] == groupY.slices[0].data[0] ? 1.0 : 0.0 ) );

    DEBUG_LOG( QString( "[*similarityP*][entropies] H(X) = %1, H(Y) = %2" ).arg( H_X_ ).arg( H_Y_ ) );
    DEBUG_LOG( QString( "[*similarityP*][JE] H(X,Y) = %1" ).arg( H_X_Y_ ) );
    DEBUG_LOG( QString( "[*similarityP*][IM] I(X;Y) = %1" ).arg( I_X_Y_ ) );
    DEBUG_LOG( QString( "[*similarityP*][semblança] I/H = %1" ).arg( similarity ) );

    return similarity;
}


void Slicer::split( Partition & partition ) const
{
    partition.g1.id = partition.g1.slices.first().id;
    partition.g2.id = partition.g2.slices.first().id;

    for ( unsigned short i = 0; i < partition.g1.slices.size(); i++ )
        partition.g1.slices[i].group = partition.g1.id;

    for ( unsigned short i = 0; i < partition.g2.slices.size(); i++ )
        partition.g2.slices[i].group = partition.g2.id;
}


}
