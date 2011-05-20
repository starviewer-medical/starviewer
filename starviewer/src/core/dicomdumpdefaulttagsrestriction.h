#ifndef UDGDICOMDUMPDEFAULTTAGSRESTRICTION_H
#define UDGDICOMDUMPDEFAULTTAGSRESTRICTION_H

#include "dicomtag.h"
#include <QString>

namespace udg {

/**
    Classe que ens implementa una restricció dels DefaultTags utilitzat en el DICOMDump
  */
class DICOMDumpDefaultTagsRestriction {

public:
    DICOMDumpDefaultTagsRestriction();

    ~DICOMDumpDefaultTagsRestriction();

    /// Mètode per definir el DicomTag que conté la Restricció
    void setDICOMTag(const DICOMTag &dicomTag);

    /// Mètode que retorna el DicomTag que conté la Restricció
    DICOMTag getDICOMTag() const;

    /// Mètode que defineix el Valor de la Restricció
    void setValue(const QString &restrictionValue);

    /// Mètode que retorna el Valor de la Restricció
    QString getValue() const;

    /// Mètode que defineix el valor de l'operador de la Restricció
    void setOperator(const QString &restrictionOperator);

    /// Mètode que retorna el valor de l'operador de la Restricció
    QString getOperator() const;

    /// Mètode que aplica l'operació definida a la restricció amb els valor definit a la restricció i el valor passat per paràmetre.
    /// Retorna True si és compleix i False altrament
    bool isValidValue(const QString &value) const;

private:
    DICOMTag m_dicomTag;
    QString m_value;
    QString m_operator;
};
}

#endif
