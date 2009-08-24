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
    
    ///Especifica/retorna el número de pàgines que s'han d'implementar del JOB
    void print(DicomPrinter printer, DicomPrintJob printJob );

};
}; 
#endif