/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGSLICER_H
#define UDGSLICER_H


#include <QVector>


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

    /// Crea un objecte Slicer amb l'identificador \a id.
    Slicer( unsigned char id );
    ~Slicer();

    /// Assigna la imatge d'entrada.
    void setInput( vtkImageData * input );
    /// Assigna la matriu de transformació.
    void setMatrix( vtkMatrix4x4 * matrix );
    /// Assigna l'espaiat de la imatge de sortida.
    void setSpacing( double xSpacing, double ySpacing, double zSpacing );
    /// Aplica el reslice per generar la nova imatge.
    void reslice();

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
    void method1B_0( double threshold );
    /// Ajunta llesques consecutives amb semblança per sobre d'un llindar.
    void method1B( double threshold );

private:

    /// Troba l'extent mínim en la direcció 0 i guarda els resultats a min0 i max0.
    void findExtent( const unsigned char * data,
                     int dim0, int dim1, int dim2,
                     int inc0, int inc1, int inc2,
                     int & min0, int & max0 ) const;

    /// Retorna la similaritat entre dues llesques. \todo Fer-ho passant els índexs.
    double similarity( const unsigned char * sliceX, const unsigned char * sliceY ) const;
    /// Calcula la semblança entre tots els parells de llesques consecutives.
    void computeSimilarities();
    /// Assigna el grup \a group a \a slice i totes les seves seguidores del mateix grup.
    void setGroup( QVector< unsigned short > & groups, unsigned short slice, unsigned short group ) const;
    /// Assigna el grup \a group a \a slice i totes les seves seguidores del mateix grup.
    void setRightGroup( QVector< unsigned short > & rightGroups, unsigned short slice, unsigned short group ) const;

    /// Identificador de l'objecte.
    unsigned char m_id;

    /// Imatge d'entrada.
    vtkImageData * m_input;
    /// Nombre de valors de la imatge d'entrada.
    unsigned short m_nLabels;
    /// Matriu de transformació.
    vtkMatrix4x4 * m_matrix;
    /// Espaiat de la imatge de sortida.
    double m_xSpacing, m_ySpacing, m_zSpacing;
    /// Valor de propietat del background afegit pel reslice.
    unsigned char m_newBackground;

    /// Nova imatge.
    vtkImageData * m_reslicedImage;
    /// Dades de la nova imatge.
    unsigned char * m_reslicedData;
    /// Mida de les dades de la nova imatge.
    unsigned int m_reslicedDataSize;
    /// Mida de les dades de cada llesca de la nova imatge.
    unsigned int m_sliceSize;
    /// Nombre de llesques de la nova imatge.
    unsigned short m_sliceCount;

    /// SMI per cada llesca.
    QVector< double > m_smi;    // Slice Mutual Information B-)
    /// A cada posició guarda la similaritat entre la llesca (i) i la (i+1)
    QVector< double > m_similarities; // IM(X,Y) / H(X,Y)

};


}


#endif
