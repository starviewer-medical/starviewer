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

#ifndef UDGDICOMVALUEREPRESENTATIONCONVERTER_H
#define UDGDICOMVALUEREPRESENTATIONCONVERTER_H

#include <QString>
#include <QVector>
#include <QVector2D>
#include <QTime>

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

    /// Given a decimal string, converts it to a 2D vector. If the decimal has less or more than 2 decimal values it is considered invalid
    /// ok will be false and a 0 initialized vector will be returned. It's an specific merthod for decimal strings with two values
    static QVector2D decimalStringTo2DDoubleVector(const QString &decimalString, bool *ok = 0);

    /// Maximum resolution conversion are milliseconds (DICOM Time can have a resolution of a millionth of a second (6 digits).
    static QTime timeToQTime(const QString &timeString);
};

} // End namespace udg

#endif
