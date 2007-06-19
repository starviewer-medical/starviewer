/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICER_H
#define UDGSLICER_H


#include <vector>

#include <QVector>


class vtkImageData;
class vtkMatrix4x4;

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Slicer{
public:
    Slicer( unsigned char id );

    ~Slicer();

    void setInput( vtkImageData * input );
    void setMatrix( vtkMatrix4x4 * matrix );
    void setSpacing( double xSpacing, double ySpacing, double zSpacing );
    void reslice();
    void compute();

    // mètodes per ajuntar llesques
    void computeSimilarities();
    /// Ajunta les llesques començant per la primera i ajuntant mentre la semblança entre (i) i (i0) sigui més gran que un llindar.
    /// Després comença un nou grup i va fent el mateix, i així fins que acaba.
    void method1A( double threshold );
    /// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
    void method1B( double threshold );

private:

    static void findExtent( const unsigned char * data,
                            int dim0, int dim1, int dim2,
                            int inc0, int inc1, int inc2,
                            int & min0, int & max0 );
    void setGroup( QVector< unsigned short > & groups, unsigned short slice, unsigned short group ) const;
    double similarity( const unsigned char * sliceX, const unsigned char * sliceY ) const;



    unsigned char m_id;

    vtkImageData * m_input;
    vtkMatrix4x4 * m_matrix;
    double m_xSpacing, m_ySpacing, m_zSpacing;

    unsigned char m_nLabels;

    vtkImageData * m_reslicedImage;
    unsigned char * m_reslicedData;
    unsigned int m_reslicedDataSize;
    unsigned int m_sliceSize;
    unsigned short m_sliceCount;

    std::vector< double > m_smi;    // Slice Mutual Information B-)


    // coses pels mètodes per ajuntar llesques
    /// Guarda la semblança entre la llesca (i) i la (i+1)
    QVector< double > m_similarities; // IM(X,Y) / H(X,Y)

};

}

#endif
