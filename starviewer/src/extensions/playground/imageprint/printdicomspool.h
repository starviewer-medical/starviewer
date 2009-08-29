#ifndef UDGFAKEOBJECTS_H
#define UDGFAKEOBJECTS_H

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

    PrintDicomSpool();

    void printSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintjob, QString pathStoredPrintDcmtkFile);

private:		
	
    DVPSStoredPrint	*m_storedPrintDcmtk;
    /*TODO: Potser que daria més entés no guardar les variables com a membre i desdle mètode printSpool passar-la al mètode que 
     *que les necessiti. Per exemple printSCUCreateBasicFilmSession necessita DicomPrintJob*/
    DicomPrinter m_dicomPrinter;
    DicomPrintJob m_dicomPrintJob;
    QString m_spoolPath;

    DVPSStoredPrint* loadStoredPrintFileDcmtk(QString pathStoredPrintDcmtkFile);

    void print();

    //Crea un Film session
    Status printSCUcreateBasicFilmSession(DVPSPrintMessageHandler& printConnection);
};
}; 
#endif
