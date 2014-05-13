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

#ifndef UDGCONVERTDICOMTOLITTLEENDIAN_H
#define UDGCONVERTDICOMTOLITTLEENDIAN_H

class QString;

namespace udg {

class Status;

/**
    Converteix les imatges guardades al format littleEndian, necessari per crear els DicomDir
  */
class ConvertDicomToLittleEndian {
public:
    ConvertDicomToLittleEndian();

    /// Converteix el fitxer d'entrada dicom a format little endian i el guarda, amb el nom i directori que s'indiqui a outputfile
    /// @param inputFile  ruta completa del fitxer a convertir
    /// @param outputFile ruta completa indicant on s'ha de desar el fitxer convertit, s'hi ha d'incloure el nom del fitxer
    /// @return
    Status convert(QString inputFile, QString outputFile);

    ~ConvertDicomToLittleEndian();
};

}

#endif
