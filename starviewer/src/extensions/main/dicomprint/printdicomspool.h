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

#ifndef UDGPRINTDICOMSPOOL_H
#define UDGPRINTDICOMSPOOL_H

#include <QString>

#include "dicomprinter.h"
#include "dicomprintjob.h"

/// Representació d'una impressora DICOM amb les eines DCMTK.
class DVPSStoredPrint;
class DVPSPrintMessageHandler;
class DcmDataset;
class OFCondition;

namespace udg {

class PrintDicomSpool {
public:
    enum PrintDicomSpoolError { CanNotConnectToDICOMPrinter, ErrorCreatingFilmSession, ErrorCreatingFilmbox, ErrorCreatingImageBox,
                                ErrorLoadingImageToPrint, Ok };

    void printBasicGrayscale(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintjob, const QString &storedPrintDcmtkFilePath,
                             const QString &spoolDirectoryPath);

    PrintDicomSpool::PrintDicomSpoolError getLastError();

private:
    /// TODO: Potser que daria més entés no guardar les variables com a membre i desdle mètode printSpool passar-la al mètode que
    /// que les necessiti. Per exemple printSCUCreateBasicFilmSession necessita DicomPrintJob
    DicomPrinter m_dicomPrinter;
    DicomPrintJob m_dicomPrintJob;

    DVPSStoredPrint* loadStoredPrintFileDcmtk(const QString &pathStoredPrintDcmtkFile);

    void printStoredPrintDcmtkContent(DVPSPrintMessageHandler &printerConnection, const QString storedPrintDcmtkFilePath, const QString &spoolDirectoryPath);

    // Retorna els atributs del FilmSession en un Dataset
    DcmDataset getAttributesBasicFilmSession();

    /// Una vegada creada la FilmSession i FilmBox aquest mètode ens permet enviar una imatge a imprimir cap a la impressora
    OFCondition createAndSendBasicGrayscaleImageBox(DVPSPrintMessageHandler &printConnection, DVPSStoredPrint *storedPrintDcmtk, size_t imageNumber,
                                                    const QString &spoolDirectoryPath);

    /// Crea i envia les anotacions pel filmBox
    OFCondition createAndSendFilmBoxAnnotations(DVPSPrintMessageHandler &printConnection, DVPSStoredPrint *storedPrintDcmtk);

    PrintDicomSpoolError m_lastError;
};
};
#endif
