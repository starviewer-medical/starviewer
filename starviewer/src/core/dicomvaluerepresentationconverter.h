#ifndef UDGDICOMVALUEREPRESENTATIONCONVERTER_H
#define UDGDICOMVALUEREPRESENTATIONCONVERTER_H

#include <QString>
#include <QVector>

namespace udg {

/**
    Classe encarregada de validar i transformar formats DICOM de Value Representation a formats interns
    Per la definició dels diferents Value Representation (VR) veure PS 3.5 - Table 6.2-1 DICOM VALUE REPRESENTATIONS
  */
class DICOMValueRepresentationConverter {
public:
    DICOMValueRepresentationConverter();
    ~DICOMValueRepresentationConverter();

    /// Declaració de la constant per definir el caràcter separador per atributs DICOM multivaluats (Value Multiplicity(VM) > 1)
    static const QString ValuesSeparator;
    
    /// Donada una cadena de text amb valors decimals (Decimal String), ens la converteix a un vector de doubles
    /// Si la cadena no compleix amb la definició de Decimal String del DICOM, el vector estarà buit i el valor del paràmetre ok, serà false
    /// Si la cadena és buida, el vector retornat serà buit i ok serà true
    /// Si la cadena està en el format correcte, ens retornarà un vector de doubles amb els corresponents valors i ok serà true
    static QVector<double> decimalStringToDoubleVector(const QString &decimalString, bool *ok = 0);
};

} // End namespace udg

#endif
