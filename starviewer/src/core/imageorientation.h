#ifndef UGDIMAGEORIENTATION_H
#define UGDIMAGEORIENTATION_H

#include <QString>
#include <QVector3D>

namespace udg {

/**
    Aquesta classe encapsula l’atribut DICOM Image Orientation (Patient) (0020,0037) que defineix les direccions dels vectors de la primera fila i columna 
    de la imatge respecte al pacient. Per més informació consultar PS 3.3, secció C.7.6.2.1.1.

    En format DICOM aquest atribut consta de 6 valors separats per '\', essent els 3 primers el vector de la fila i els 3 últims el vector de la columna.

    Aquesta classe, a més a més guarda la normal del pla que formen aquests dos vectors.
  */
class ImageOrientation {
public:
    ImageOrientation();
    ~ImageOrientation();
    
    /// Constructor a partir d'un string d'orientació en format DICOM
    ImageOrientation(const QString &dicomFormattedImageOrientationString);
    
    /// Constructor a partir de dos vectors 3D
    ImageOrientation(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Assigna la orientació proporcionada en el format estipulat pel DICOM: 6 valors separats per '\'
    /// Si la cadena no està en el format esperat, es manté el valor que tenia fins aquell moment i es retorna fals, cert altrament.
    bool setDICOMFormattedImageOrientation(const QString &imageOrientation);

    /// Ens retorna la orientació en el format estipulat pel DICOM: 6 valors separats per '\'
    /// En cas que el valor no s'hagi assignat la cadena serà buida.
    QString getDICOMFormattedImageOrientation() const;

    /// Assigna la orientació a través dels 2 vectors 3D corresponents a les direccions de la fila i de la columna respectivament
    void setRowAndColumnVectors(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Ens retorna els vectors fila, columna i normal respectivament.
    /// En cas que no s'hagi assignat cap orientació, els vectors no tindran cap element
    QVector3D getRowVector() const;
    QVector3D getColumnVector() const;
    QVector3D getNormalVector() const;

    /// Ens diu si conté algun valor o no
    bool isEmpty() const;

    /// Operador còpia
    ImageOrientation& operator=(const ImageOrientation &imageOrientation);

    /// Operador igualtat
    bool operator==(const ImageOrientation &imageOrientation) const;

private:
    /// Crea els vectors de doubles (row, column, normal) a partir del valor de l'string d'orientació
    /// Si l'string és buit, els vectors també ho seran
    void buildVectorsFromOrientationString();

    /// Inicialitza correctament els vectors de doubles que contenen les orientacions
    void initializeVectors();

private:
    /// Guarda la orientació en el format estipulat pel DICOM: 6 valors separats per '\'
    QString m_imageOrientationString;

    /// Els 3 vectors de la orientació de la imatge.
    /// Els vectors fila i columna els obtindrem directament a partir de la cadena assignada amb setDICOMFormattedImageOrientation()
    /// El vector normal de la orientació de la imatge es calcularà fent el producte vectorial dels vectors fila i colummna
    QVector3D m_rowVector;
    QVector3D m_columnVector;
    QVector3D m_normalVector;
};

} // End namespace udg

#endif
