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

#ifndef UDGDICOMATTRIBUTE_H
#define UDGDICOMATTRIBUTE_H

class QString;

namespace udg {

class DICOMTag;

/**
    Classe pare encarregada de representar atributs DICOM.
  */
class DICOMAttribute {

public:
    DICOMAttribute();

    virtual ~DICOMAttribute();

    /// Assigna/Obtenir el Tag associat a l'atribut
    void setTag(const DICOMTag &tag);
    DICOMTag* getTag();

    /// Mètode abstracte per saber si l'atribut és de tipus valor
    virtual bool isValueAttribute() = 0;

    /// Mètode abstracte per saber si l'atribut és de tipus seqüència
    virtual bool isSequenceAttribute() = 0;

    /// Mètode abstracte que retorna el contingut de l'atribut formatat
    virtual QString toString(bool verbose = false) = 0;

private:
    /// Guarda el Tag associat a l'atribut
    DICOMTag *m_tag;

};

}

#endif
