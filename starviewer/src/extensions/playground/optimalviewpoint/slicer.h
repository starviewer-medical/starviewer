/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGSLICER_H
#define UDGSLICER_H


#include <QList>
#include <QVector>

#include "vector3.h"
#include "histogram.h"


class vtkImageData;
class vtkMatrix4x4;


namespace udg {


/**
 * Aquesta classe treballa rep una imatge de VTK, li aplica un reslice amb una
 * determinada transformació i pot fer diversos càlculs sobre la nova imatge.
 *
 * La classe rep com a entrada una imatge de VTK, una matriu de transformació i
 * unes dades d'espaiat i genera una nova imatge de VTK sobre la qual aplicarà
 * els càlculs. Quan genera la nova imatge la guarda al directori temporal
 * creant un fitxer reslicedID.mhd i un reslicedID.raw, on ID correspon a
 * l'identificador de l'objecte Slicer. La nova imatge té la mida mínima perquè
 * hi càpiguen tots els vòxels que no són background. El background afegit en la
 * transformació té el valor 255, i a la imatge d'entrada ha de ser el 0.
 *
 * Pot calcular el grau de representativitat d'una llesca respecte el volum
 * sencer i també té dos mètodes diferents per agrupar llesques segons la
 * semblança entre elles.
 *
 * \todo Aquí també es podrien implementar els càlculs d'entropy rate i excess
 * entropy d'un punt de vista, sense necessitat de fer ray casting.
 *
 * \todo Potser quedaria bé posar tots els mètodes de càlcul en una altra classe
 * (estàtica?).
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class Slicer {

public:

    /// Creates a Slicer object with identifier \a id.
    Slicer( unsigned char id );
    ~Slicer();

    /// Sets input image.
    void setInput( vtkImageData * input );
    void setVector( const Vector3 & vector );
    /// Sets transformation matrix (for orientation).
    void setMatrix( vtkMatrix4x4 * matrix );
    /// Sets spacing of output resliced image.
    void setSpacing( double xSpacing, double ySpacing, double zSpacing );
    /**
     * Reslices the input image to obtain the resliced image.
     * If \a saveMhd is true the resliced image is saved to the temporary directory in MHD format.
     * If \a doClip is true the resliced image is clipped to remove slices of only background.
     */
    void reslice( bool saveMhd = true, bool doClip = true );

    /// Estableix si s'ha de llegir l'extent des d'un fitxer (sinó, es calcula sobre les dades)
    void setReadExtentFromFile( bool readExtentFromFile );

    /**
     * Calcula la mesura SMI (Slice Mutual Information). Escriu els resultats
     * per pantalla i en un fitxer anomenat smiID.txt al directori temporal.
     */
    void computeSmi();

    /**
     * Ajunta les llesques començant per la primera i ajuntant mentre la
     * semblança entre (i) i (i0) sigui més gran que un llindar. Després comença
     * un nou grup i va fent el mateix, i així fins que acaba.
     */
    void method1A( double threshold );
    /// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
    void method1B( double threshold );
    /// Works with fusioned slices.
    void groupingMethodC( double threshold );
    /// Works with fusioned slices (Jensen-Shannon)
    void groupingMethodC_JS( double threshold );
    /// Works with fusioned slices
    void splittingMethodC( double threshold );

    vtkImageData * getReslicedImage() const { return m_reslicedImage; }

    // New methods to find most structured view
    /// "Sum of similarities"
    double newMethod2( int step, bool normalized );

private:

    struct Slice
    {
        unsigned short id, group;
        const unsigned char * data;
    };

    struct Group
    {
        unsigned short id;
        QList< Slice > slices;
        double similarityToNext;    // similarity to next group (optimization for groupingMethodC)
        Group * previous, * next;
    };

    struct Partition
    {
        Group g1, g2;
        Histogram g1Histogram, g2Histogram;
        Histogram jointHistogram;
    };

    /// Finds minimum extent in direction 0 and stores results in min0 and max0.
    void findExtent( const unsigned char * data,
                     int dim0, int dim1, int dim2,
                     int inc0, int inc1, int inc2,
                     int & min0, int & max0 ) const;

    /// Returns mutual information between two slices. \todo Fer-ho passant els índexs.
    double mutualInformation( const unsigned char * sliceX, const unsigned char * sliceY ) const;
    /// Returns the similarity between two slices. \todo Fer-ho passant els índexs.
    double similarity( const unsigned char * sliceX, const unsigned char * sliceY, bool background = true ) const;
    /// Computes similarities between all pairs of consecutive slices.
    void computeSimilarities();
    /// Sets group \a group to \a slice and all her neighbours with higher id and same group.
    void setGroup( QVector< unsigned short > & groups, unsigned short slice, unsigned short group ) const;
    /// Sets group \a group to \a slice and all her neighbours with lower id and same group.
    void setRightGroup( QVector< unsigned short > & rightGroups, unsigned short slice, unsigned short group ) const;
    /// Returns similarity between two groups.
    double similarity( const Group & groupX, const Group & groupY ) const;
    /// Integrates \a groupY into \a groupX.
    void join( Group & groupX, Group & groupY ) const;
    /// Returns Jensen-Shannon divergence between two groups.
    double jensenShannonDivergence( const Group & groupX, const Group & groupY ) const;
    /// Returns the first partition of a group.
    Partition firstPartition( const Group & group ) const;
    /// Converts \a partition in the next partition if that exists, and returns true if successful.
    bool nextPartition( Partition & partition ) const;

    /// Object identifier.
    unsigned char m_id;

    /// Input image.
    vtkImageData * m_input;
    /// Number of labels of input image.
    unsigned short m_nLabels;
    Vector3 m_vector;
    /// Transformation matrix.
    vtkMatrix4x4 * m_matrix;
    /// Spacing of resliced image.
    double m_xSpacing, m_ySpacing, m_zSpacing;
    /// Label for background added in the reslice process.
    unsigned char m_newBackground;

    /// Indica si s'ha de llegir l'extent des d'un fitxer (sinó, es calcula sobre les dades)
    bool m_readExtentFromFile;

    /// Resliced image.
    vtkImageData * m_reslicedImage;
    /// Raw data of resliced image.
    unsigned char * m_reslicedData;
    /// Size of data of resliced image.
    unsigned int m_reslicedDataSize;
    /// Size of data of a slice in the resliced image.
    unsigned int m_sliceSize;
    /// Number of slices of the resliced image.
    unsigned short m_sliceCount;

    /// SMI per cada llesca.
    QVector< double > m_smi;    // Slice Mutual Information B-)
    /// Similarities between pairs of consecutive slices. Every position stores similarity between slices (i) and (i+1).
    QVector< double > m_similarities; // IM(X,Y) / H(X,Y)

};


}


#endif
