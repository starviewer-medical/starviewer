#ifndef UDGFAKEOBJECTS_H
#define UDGFAKEOBJECTS_H

#include <QString>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "dicomprintpage.h"

///Representació d'una impressora DICOM amb les eines DCMTK.

namespace udg
{   

class FakeObjects
{
public:

    ///Retorna una impressora 
    DicomPrinter getDicomPrinterFake();

    ///Retorna un DicomPrintjob amb imatges de l'estudi per imprimir, numberOfImages indica el número d'imatges de l'estudi
    DicomPrintJob getDicomPrintJobFake(QString studyUID, QString seriesUID, int numberOfImages);

    ///Retorna un DicomPrintjob Per DicomScope amb imatges de l'estudi per imprimir, numberOfImages indica el número d'imatges de l'estudi
    DicomPrintJob getDicomPrintJobFakeDicomScope(QString studyUID, QString seriesUID, int numberOfImages);

    ///Retorna un DicomPrintPage amb imatges de l'estudi per imprimir, numberOfImages indica el número d'imatges de l'estudi
    DicomPrintPage getDicomPrintPageFake(QString studyUID, QString seriesUID, int numberOfImages);

private:		
	
};
}; 
#endif
