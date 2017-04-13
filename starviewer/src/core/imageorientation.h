/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UGDIMAGEORIENTATION_H
#define UGDIMAGEORIENTATION_H

#include "vector3.h"

namespace udg {

/**
    Aquesta classe encapsula l'atribut DICOM Image Orientation (Patient) (0020,0037) que defineix les direccions dels vectors de la primera fila i columna
    de la imatge respecte al pacient. Per més informació consultar PS 3.3, secció C.7.6.2.1.1.

    En format DICOM aquest atribut consta de 6 valors separats per '\', essent els 3 primers el vector de la fila i els 3 últims el vector de la columna.

    Aquesta classe, a més a més guarda la normal del pla que formen aquests dos vectors.
  */
class ImageOrientation {

public:

    /// Constructs an image orientation with null vectors.
    ImageOrientation();
    /// Constructs an image orientation with the given row and column vectors.
    ImageOrientation(Vector3 rowVector, Vector3 columnVector);

    /// Returns true if the row and column vectors are not null, and false otherwise.
    bool isValid() const;
    
    /// Assigna la orientació proporcionada en el format estipulat pel DICOM: 6 valors numèrics separats per '\' o una cadena buida. 
    /// Si la cadena no està en el format esperat, es re-inicialitzen els valors dels vectors i es retorna fals, cert altrament.
    bool setDICOMFormattedImageOrientation(const QString &imageOrientation);

    /// Ens retorna la orientació en el format estipulat pel DICOM: 6 valors numèrics separats per '\'
    /// En cas que no s'hagi introduit cap valor anteriorment, es retornarà una cadena buida
    QString getDICOMFormattedImageOrientation() const;

    /// Assigna la orientació a través dels 2 vectors 3D corresponents a les direccions de la fila i de la columna respectivament
    void setRowAndColumnVectors(Vector3 rowVector, Vector3 columnVector);
    
    /// Returns the row vector.
    const Vector3& getRowVector() const;
    /// Returns the column vector.
    const Vector3& getColumnVector() const;
    /// Returns the normal vector, computed as the cross product of the row and column vectors.
    const Vector3& getNormalVector() const;

    /// Operador igualtat
    bool operator==(const ImageOrientation &imageOrientation) const;

private:

    /// The row vector.
    Vector3 m_rowVector;
    /// The column vector.
    Vector3 m_columnVector;
    /// The normal vector, computed as the cross product of the row and column vectors.
    Vector3 m_normalVector;

};

} // End namespace udg

#endif
