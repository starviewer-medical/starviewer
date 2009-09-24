#ifndef UDGPRINTDICOMSPOOL_H
#define UDGPRINTDICOMSPOOL_H

#include <QString>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "dicomprintpage.h"

#include "../inputoutput/status.h"

///Representació d'una impressora DICOM amb les eines DCMTK.
class DVPSStoredPrint;
class DVPSPrintMessageHandler;

namespace udg
{   

class PrintDicomSpool
{
public:

    void printBasicGrayscale(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintjob, const QString &storedPrintDcmtkFilePath, const QString &spoolDirectoryPath);

private:		
	
    DVPSStoredPrint	*m_storedPrintDcmtk;
    /*TODO: Potser que daria més entés no guardar les variables com a membre i desdle mètode printSpool passar-la al mètode que 
     *que les necessiti. Per exemple printSCUCreateBasicFilmSession necessita DicomPrintJob*/
    DicomPrinter m_dicomPrinter;
    DicomPrintJob m_dicomPrintJob;

    DVPSStoredPrint* loadStoredPrintFileDcmtk(const QString &pathStoredPrintDcmtkFile);

    void printBasicGrayscaleSpool(const QString &spoolDirectoryPath);

    //Crea un Film session
    Status createAndSendBasicFilmSession(DVPSPrintMessageHandler& printConnection);

    ///Una vegada creada la FilmSession i FilmBox aquest mètode ens permet enviar una imatge a imprimir cap a la impressora
    Status createAndSendBasicGrayscaleImageBox(DVPSPrintMessageHandler& printConnection, size_t imageNumber, const QString &spoolDirectoryPath);
};
}; 
#endif
