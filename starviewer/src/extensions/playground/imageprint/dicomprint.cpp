#include "dicomprint.h"

#include <QDir>
#include <QStringList>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"
#include "printdicomspool.h"
#include "../core/settings.h"
#include "imageprintsettings.h"
#include "../inputoutput/pacsdevice.h"
#include "../inputoutput/pacsserver.h"
#include "../inputoutput/status.h"
#include "logging.h"

namespace udg
{
void DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;
    PrintDicomSpool printDicomSpool;
    QString storedDcmtkFilePath;
    ImagePrintSettings imagePrintSettings;
    QStringList dcmtkStoredPrintPathFileList;
    int indexNumberOfCopies = 0, numberOfCopies;

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

    numberOfCopies = printJob.getNumberOfCopies();
    printJob.setNumberOfCopies(1);//Indiquem que només en volem una còpia
    
    while (indexNumberOfCopies < numberOfCopies)
    {
        //Enviem a imprimir cada pàgina    
        foreach(QString dcmtkStoredPrintPathFile, dcmtkStoredPrintPathFileList)
        {
            printDicomSpool.printBasicGrayscale(printer, printJob, dcmtkStoredPrintPathFile, Settings().getValue(ImagePrintSettings::SpoolDirectory).toString());
        }

        indexNumberOfCopies++;
    }
    //TODO: falta esborra el contingut del directori spool
}

bool DicomPrint::echoPrinter(DicomPrinter printer)
{
    Status state;
    PacsDevice pacs;
    PacsServer pacsServer;
    bool resultTest = false;;

    /*HACK el codi de fer echoSCU espera que li passem un PACS, com aquest codi està a PacsServer una classe orientada completament a PACS, el que implica
      que és difícil adaptar el codi perquè accepti altres objectes, a part de que les responsabilitat de la classe és sobre objectes PACS, per això
      transformem l'objecte printer a PACS per poder fer l'echo i utilitzem les classes de PACS */ 
    pacs.setAETitle(printer.getAETitle());
    pacs.setPort(QString().setNum(printer.getPort()));
    pacs.setAddress(printer.getHostname());
    pacsServer.setPacs(pacs);

    state = pacsServer.connect(PacsServer::echoPacs , PacsServer::studyLevel);

    if (!state.good())
    {
        m_lastPrinterError = DicomPrint::ErrorConnecting;
        ERROR_LOG("Can't connect to printer " + pacs.getAETitle() + ". Error description : " + state.text());
    }
    else
    {
        state = pacsServer.echo();

        if (state.good())
        {
            resultTest = true;
            INFO_LOG("Test of printer " + pacs.getAETitle() + "is correct.");
        }
        else
        {
            m_lastPrinterError = DicomPrint::NotRespondedAsExpected;
            ERROR_LOG("Doing echo to printer " + pacs.getAETitle() + " doesn't responds correctly. Error description : " + state.text());
        }

        pacsServer.disconnect();
    }

    return resultTest;
}

DicomPrint::PrinterError DicomPrint::getLastError()
{
    return m_lastPrinterError;
}

}