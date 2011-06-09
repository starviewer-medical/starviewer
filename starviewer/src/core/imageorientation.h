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
    
    /// Constructor a partir de dos vectors 3D
    ImageOrientation(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Assigna la orientació proporcionada en el format estipulat pel DICOM: 6 valors numèrics separats per '\' o una cadena buida. 
    /// Si la cadena no està en el format esperat, es re-inicialitzen els valors dels vectors i es retorna fals, cert altrament.
    bool setDICOMFormattedImageOrientation(const QString &imageOrientation);

    /// Ens retorna la orientació en el format estipulat pel DICOM: 6 valors numèrics separats per '\'
    /// En cas que no s'hagi introduit cap valor anteriorment, es retornarà una cadena buida
    QString getDICOMFormattedImageOrientation() const;

    /// Assigna la orientació a través dels 2 vectors 3D corresponents a les direccions de la fila i de la columna respectivament
    void setRowAndColumnVectors(const QVector3D &rowVector, const QVector3D &columnVector);
    
    /// Ens retorna els vectors fila, columna i normal respectivament.
    /// En cas que no s'hagi assignat cap orientació, els vectors no tindran cap element
    QVector3D getRowVector() const;
    QVector3D getColumnVector() const;
    QVector3D getNormalVector() const;

    /// Operador igualtat
    bool operator==(const ImageOrientation &imageOrientation) const;

private:
    /// Ens retorna la orientació dels dos vectors en una cadena en el format estipulat pel DICOM
    QString convertToDICOMFormatString(const QVector3D &rowVector, const QVector3D &columnVector) const;

    /// Inicialitza els vectors a (0,0,0)
    void setVectorValuesToDefault();

private:
    /// Els 3 vectors de la orientació de la imatge.
    /// Els vectors fila i columna els obtindrem directament a partir de la cadena assignada amb setDICOMFormattedImageOrientation()
    /// El vector normal de la orientació de la imatge es calcularà fent el producte vectorial dels vectors fila i colummna
    QVector3D m_rowVector;
    QVector3D m_columnVector;
    QVector3D m_normalVector;
};

} // End namespace udg

#endif
