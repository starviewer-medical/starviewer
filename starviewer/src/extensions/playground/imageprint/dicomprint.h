#ifndef UDGDICOMPRINT_H
#define UDGDICOMPRINT_H

/**
* Imprimeix en una impressora dicom un DicomPrintJob
*/

namespace udg{

class DicomPrinter;
class DicomPrintJob;

class DicomPrint
{
public:
    
    enum PrinterError { ErrorConnecting, NotRespondedAsExpected,  };

    ///Especifica/retorna el número de pàgines que s'han d'implementar del JOB
    void print(DicomPrinter printer, DicomPrintJob printJob );

    ///Comprova si amb la configuració passada per paràmetre una impressora respón
    bool echoPrinter(DicomPrinter printer);

    PrinterError getLastError();

private:

    PrinterError m_lastPrinterError;

};
}; 
#endif