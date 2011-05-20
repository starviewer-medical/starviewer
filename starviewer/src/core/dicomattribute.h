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
    virtual QString toString() = 0;

private:
    /// Guarda el Tag associat a l'atribut
    DICOMTag *m_tag;

};

}

#endif
