/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICER_H
#define UDGSLICER_H

#include <vector>

class vtkImageData;
class vtkMatrix4x4;

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Slicer{
public:
    Slicer();

    ~Slicer();

    void setInput( vtkImageData * input );
    void setMatrix( vtkMatrix4x4 * matrix );
    void setSpacing( double xSpacing, double ySpacing, double zSpacing );
    void reslice();
    void compute();

private:

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

};

}

#endif
