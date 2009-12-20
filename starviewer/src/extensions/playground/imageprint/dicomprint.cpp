#include "dicomprint.h"

#include <QDir>
#include <QStringList>
#include <QDesktopServices>

#include "dicomprinter.h"
#include "dicomprintjob.h"
#include "createdicomprintspool.h"
#include "printdicomspool.h"
#include "../inputoutput/pacsdevice.h"
#include "../inputoutput/pacsserver.h"
#include "../inputoutput/status.h"
#include "logging.h"
#include "deletedirectory.h" 

namespace udg
{
int DicomPrint::print(DicomPrinter printer, DicomPrintJob printJob)
{
    PrintDicomSpool printDicomSpool;

    QStringList dcmtkStoredPrintPathFileList;
    int indexNumberOfCopies = 0, numberOfCopies, numberOfFilmSessionPrinted = 0;
    DeleteDirectory deleteDirectory;

	m_lastError = Ok;

    INFO_LOG("Han demanat imprimir imatges DICOM a la impresssora " + printer.getAETitle() + ", IP: " + printer.getHostname() + ", port: " + QString().setNum(printer.getPort()));

    dcmtkStoredPrintPathFileList = createDicomPrintSpool(printer, printJob);

    if (getLastError() == Ok)
    {
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
            INFO_LOG("Envio a imprimir la copia " + QString().setNum(indexNumberOfCopies + 1) + "/" + QString().setNum(numberOfCopies));
            //Enviem a imprimir cada pàgina    
            foreach(QString dcmtkStoredPrintPathFile, dcmtkStoredPrintPathFileList)
            {
                INFO_LOG("Envio FilmSession a imprimir");
                printDicomSpool.printBasicGrayscale(printer, printJob, dcmtkStoredPrintPathFile, getSpoolDirectory());
				if (printDicomSpool.getLastError() != PrintDicomSpool::Ok)
				{
					//Si hi ha error parem
					break;
				}
				
				numberOfFilmSessionPrinted++;
            }

			if (printDicomSpool.getLastError() != PrintDicomSpool::Ok)
			{
				//Si hi ha error parem
				break;
			}

            indexNumberOfCopies++;
        }

		if (printDicomSpool.getLastError()==PrintDicomSpool::Ok)
		{
			INFO_LOG("S'ha imprés correctament.");
		}

		m_lastError = printDicomSpoolErrorToDicomPrintError(printDicomSpool.getLastError());
    }

    INFO_LOG("Esborro directori spool");
    deleteDirectory.deleteDirectory(getSpoolDirectory(), true);

    return numberOfFilmSessionPrinted;
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
    pacs.setPort(printer.getPort());
    pacs.setAddress(printer.getHostname());
    pacsServer.setPacs(pacs);

    state = pacsServer.connect(PacsServer::echoPacs , PacsServer::studyLevel);

    //TODO: Si no reconeix el nostre aetitle no retornem l'error correcte, indique association rejected
    if (!state.good())
    {
        m_lastError = DicomPrint::CanNotConnectToDicomPrinter;
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
            m_lastError = DicomPrint::NotRespondedAsExpected;
            ERROR_LOG("Doing echo to printer " + pacs.getAETitle() + " doesn't responds correctly. Error description : " + state.text());
        }

        pacsServer.disconnect();
    }

    return resultTest;
}

QStringList DicomPrint::createDicomPrintSpool(DicomPrinter printer, DicomPrintJob printJob)
{
    CreateDicomPrintSpool dicomPrintSpool;
    QString storedDcmtkFilePath;
    QStringList dcmtkStoredPrintPathFileList;

    //Per cada pàgina que tenim generem el fitxer storedPrint de dcmtk, cada fitxer és un FilmBox (una placa)
    foreach(DicomPrintPage dicomPrintPage, printJob.getDicomPrintPages())
    {
        QString storedPrintPathFile;
        INFO_LOG("Creo les " + QString().setNum(dicomPrintPage.getImagesToPrint().count()) + " images de la pagina " + QString().setNum(dicomPrintPage.getPageNumber()));
        
        storedPrintPathFile = dicomPrintSpool.createPrintSpool(printer, dicomPrintPage, getSpoolDirectory());

        if (dicomPrintSpool.getLastError() == CreateDicomPrintSpool::Ok)
        {
            dcmtkStoredPrintPathFileList.append(storedPrintPathFile);
        }
        else
        {
            break;
        }
    }

    if (dicomPrintSpool.getLastError() != CreateDicomPrintSpool::Ok)
    {
		//Si hi ha error no enviem a imprimir cap imatge, netegem la llista de fitxer StoredPrint
        dcmtkStoredPrintPathFileList.clear();
    }

	m_lastError = createDicomPrintSpoolErrorToDicomPrintError(dicomPrintSpool.getLastError());

    return dcmtkStoredPrintPathFileList;
}

DicomPrint::DicomPrintError DicomPrint::getLastError()
{
    return m_lastError;
}

DicomPrint::DicomPrintError DicomPrint::createDicomPrintSpoolErrorToDicomPrintError(CreateDicomPrintSpool::CreateDicomPrintSpoolError createDicomPrintSpoolError)
{
    DicomPrint::DicomPrintError error;

    switch(createDicomPrintSpoolError)
    {
        case CreateDicomPrintSpool::ErrorCreatingImageSpool:
            error = DicomPrint::ErrorCreatingPrintSpool;
            break;
        case CreateDicomPrintSpool::ErrorLoadingImageToPrint:
            error = DicomPrint::ErrorLoadingImagesToPrint;
            break;
        case CreateDicomPrintSpool::Ok:
            error = DicomPrint::Ok;
            break;
        default:
            error = DicomPrint::UnknowError;
            break;
    }

    return error;
}

DicomPrint::DicomPrintError DicomPrint::printDicomSpoolErrorToDicomPrintError(PrintDicomSpool::PrintDicomSpoolError printDicomSpoolError)
{
    DicomPrint::DicomPrintError error;

    switch(printDicomSpoolError)
    {
        case PrintDicomSpool::CanNotConnectToDICOMPrinter :
            error = DicomPrint::CanNotConnectToDicomPrinter;
            break;
        case PrintDicomSpool::ErrorCreatingFilmbox:
            error = DicomPrint::ErrorSendingDicomPrintJob;
            break;
        case PrintDicomSpool::ErrorCreatingFilmSession:
            error = DicomPrint::ErrorSendingDicomPrintJob;
            break;
        case PrintDicomSpool::ErrorCreatingImageBox:
            error = DicomPrint::ErrorSendingDicomPrintJob;
            break;
        case PrintDicomSpool::ErrorLoadingImageToPrint:
            error = DicomPrint::ErrorLoadingImagesToPrint;
            break;
		case PrintDicomSpool::Ok:
			error = DicomPrint::Ok;
			break;
        default:
            error = DicomPrint::UnknowError;
            break;
    }

    return error;
}

QString DicomPrint::getSpoolDirectory()
{
    //Creem Spool al directori tempora del S.O.
    return QDesktopServices::storageLocation( QDesktopServices::TempLocation ) + QDir::separator() + "DICOMSpool"; 
}

}