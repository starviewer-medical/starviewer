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

#ifndef UDG_DICOMFORMATTEDVALUESCONVERTER_H
#define UDG_DICOMFORMATTEDVALUESCONVERTER_H

#include <QList>
#include <QString>

namespace udg {

class VoiLut;
class WindowLevel;

/** 
    Classe helper per convertir dades en format DICOM a estructures de dades i classes pròpies
 */
class DICOMFormattedValuesConverter {
public:
    DICOMFormattedValuesConverter();
    ~DICOMFormattedValuesConverter();

    /// Donats els valors en string de window width/center i explanation construeix la llista d'objectes Window Level pertinent
    /// Si els valors de les cadenes windowWidthString i windowCenterString no són Decimal Strings es retornarà una llista buida
    /// Si el nombre d'elements de windowWidthString i windowCenterString no és el mateix es crearà una llista que tingui tants
    /// elements com la llista amb menys elements. Els elements de la llista més gran seran descartats
    /// explanationString no cal que tingui el mateix nombre d'elements que windowWidthString i windowCenterString, ja que es considera opcional.
    /// En cas que el nombre d'elements d'explanationString difereixi, només s'afegiran aquells que estiguin dins del rang del nombre de window levels
    static QList<WindowLevel> parseWindowLevelValues(const QString &windowWidthString, const QString &windowCenterString, const QString &explanationString = QString());

    /// Given the values of the DICOM tags LUTDescriptor, LUTExplanation and LUTData, constructs and returns a VoiLut object matching these values.
    static VoiLut parseVoiLut(const QString &lutDescriptor, const QString &lutExplanation, const QString &lutData);
};

}

#endif
