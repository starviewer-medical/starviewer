#include "dicomprint.h"

#include <QDir>
#include <QStringList>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"
#include "printdicomspool.h"
#include "../core/settings.h"
#include "imageprintsettings.h"

namespace udg
{
void DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;
    PrintDicomSpool printDicomSpool;
    QString storedDcmtkFilePath;
    ImagePrintSettings imagePrintSettings;
    QStringList dcmtkStoredPrintPathFileList;
    int indexNumberOfCopies = 0;

    imagePrintSettings.init();//TODO:A on s'ha de fer l'ini ? 

    //Per cada pàgina que tenim generem el fitxer storedPrint de dcmtk, cada fitxer és un FilmBox (una placa)
    foreach(DicomPrintPage dicomPrintPage, printJob.getDicomPrintPages())
    {
        dcmtkStoredPrintPathFileList.append(dicomPrintSpool.createPrintSpool(printer, dicomPrintPage, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString()));
    }

    /*Workaround per poder imprimir més d'una còpia d'un printjob. Degut a que ara no podem imprimir més d'una pàgina per connexió amb la impressora, per la 
      limitació de les classes utilitzades de dcmtk que només ens deixen associar un filmBox per filmeSsion, provoca que en el cas que d'un printjob en volem
      fer més d'una còpia, per exemple amb un tenim un DicomPrintJob amb dos pàgines del qual en volem 3 còpies, primer s'imprimirà 3 vegades la primera pàgina 
      i després sortirà 3 vegades la segona pàgina, per evitar que sortin ordenades així fem aquest workaround, en el qual es repeteix el procés d'enviar cada 
      printjob tantes còpies com ens n'hagin sol·licitat, d'aquesta manera les pàgines sortiran correctament ordenades.
      */
    while (indexNumberOfCopies < printJob.getNumberOfCopies())
    {
        DicomPrintJob dicomPrintJobToPrint = printJob;

        dicomPrintJobToPrint.setNumberOfCopies(1);//Indiquem que només en volem una còpia
        //Enviem a imprimir cada pàgina    
        foreach(QString dcmtkStoredPrintPathFile, dcmtkStoredPrintPathFileList)
        {
            printDicomSpool.printBasicGrayscale(printer, printJob, dcmtkStoredPrintPathFile, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString());
        }

        indexNumberOfCopies++;
    }
    //TODO: falta esborra el contingut del directori spool
}

}