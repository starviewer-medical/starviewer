#ifndef UDGDICOMPRINT_H
#define UDGDICOMPRINT_H

#include "createdicomprintspool.h"
#include "printdicomspool.h"

/**
* Imprimeix en una impressora dicom un DicomPrintJob
*/

namespace udg{

class DicomPrinter;
class DicomPrintJob;

class DicomPrint
{
public:
    
    enum DicomPrintError {CanNotConnectToDicomPrinter, ErrorSendingDicomPrintJob,  ErrorLoadingImagesToPrint, ErrorCreatingPrintSpool, UnknowError, Ok, 
                           NotRespondedAsExpected};

    /*retorna el número de pàgines impreses, com ara tenim el hack de que només poden enviar cada vegada filmbox a imprimir podria ser que s'hagués
     enviat a imprimir correctament alguna de les pàgines, per tant per saber si és aquest el cas indiquem el número de pàgines que s'han imprés*/
    int print(DicomPrinter printer, DicomPrintJob printJob);

    ///Comprova si amb la configuració passada per paràmetre una impressora respón
    bool echoPrinter(DicomPrinter printer);

	DicomPrint::DicomPrintError getLastError();

private:

    DicomPrintError m_lastError;

    QStringList createDicomPrintSpool(DicomPrinter, DicomPrintJob printJob);

    ///Transforma l'error de CreateDicomPrintSpool a un error de la classe DicomPrint
    DicomPrint::DicomPrintError createDicomPrintSpoolErrorToDicomPrintError(CreateDicomPrintSpool::CreateDicomPrintSpoolError error);
    
    ///Transforma l'error de CreateDicomPrintSpool a un error de la classe DicomPrint
    DicomPrint::DicomPrintError printDicomSpoolErrorToDicomPrintError(PrintDicomSpool::PrintDicomSpoolError error);

    ///Retorna el directori on genera l'Spool per imprimir les imatges DICOM
    QString getSpoolDirectory();

};
}; 
#endif