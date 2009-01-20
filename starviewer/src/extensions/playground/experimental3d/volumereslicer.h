#ifndef UDGVOLUMERESLICER_H
#define UDGVOLUMERESLICER_H


#include <QVector>

#include "vector3.h"


class vtkImageData;
class vtkMatrix4x4;


namespace udg {


/**
 * Remake de la classe Slicer d'optimal viewpoint.
 */
class VolumeReslicer {

public:

    VolumeReslicer( unsigned short id = 0 );
    ~VolumeReslicer();

    void setInput( vtkImageData *input );
    void setViewpoint( const Vector3 &viewpoint, const Vector3 &up );
    void setSpacing( double xSpacing, double ySpacing, double zSpacing );
    // saveMhd -> guardar el nou volum en mhd; doClip -> retallar les llesques que només són fons
    void reslice( bool saveMhd = true, bool doClip = true );

    /**
     * Calcula la mesura SMI (Slice Mutual Information). Escriu els resultats
     * per pantalla i en un fitxer anomenat smiID.txt al directori temporal.
     */
    void computeSmi();

private:

    /// Finds minimum extent in direction 0 and stores results in min0 and max0.
    static void findExtent( const unsigned short *data, int dim0, int dim1, int dim2, int inc0, int inc1, int inc2, int &min0, int &max0 );

private:

    unsigned short m_id;
    vtkImageData *m_input;
    vtkMatrix4x4 *m_resliceAxes;
    double m_xSpacing, m_ySpacing, m_zSpacing;

    vtkImageData *m_reslicedImage;
    unsigned short *m_reslicedData;
    int m_reslicedDataSize;
    int m_sliceSize;
    int m_sliceCount;
    int m_nLabels;

    /// SMI per cada llesca.
    QVector<double> m_smi;  // Slice Mutual Information

};


}


#endif
