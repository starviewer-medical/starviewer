#include "printdicomspool.h"

//TODO Fer neteja d'includes
#include "dcmtk/dcmpstat/dvpsdef.h"    /* for constants */
#include "dcmtk/dcmpstat/dviface.h"    /* for DVInterface */
#include "dcmtk/ofstd/ofstring.h"   /* for OFString */
#include "dcmtk/ofstd/ofbmanip.h"   /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"   /* for OFDateTime */
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/cmdlnarg.h"   /* for prepareCmdLineArgs */
#include "dcmtk/ofstd/ofconapp.h"   /* for OFConsoleApplication */
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/dcmpstat/dvpspr.h"
#include "dcmtk/dcmpstat/dvpssp.h"
#include "dcmtk/dcmpstat/dvpshlp.h"     /* for class DVPSHelper */
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmdata/dcdebug.h"

#include <QDir>

#include "logging.h"
#include "../inputoutput/pacsdevice.h"

namespace udg{

void PrintDicomSpool::printSpool(DicomPrinter dicomPrinter, DicomPrintJob dicomPrintJob, const QString &storedPrintDcmtkFilePath, const QString &spoolDirectoryPath)
{
    m_dicomPrinter = dicomPrinter;
    m_dicomPrintJob = dicomPrintJob;

    m_storedPrintDcmtk = loadStoredPrintFileDcmtk(storedPrintDcmtkFilePath);

    print(spoolDirectoryPath);

    delete m_storedPrintDcmtk;
}

void PrintDicomSpool::print(const QString &spoolDirectoryPath)
{
    DVPSPrintMessageHandler printConnection;
    OFCondition result;

    //TODO: podem utilitzar el del DVSStoredPrint
    //Connectem amb la impressora
    result = printConnection.negotiateAssociation(NULL/*tlayer només s'utilitza per ssl*/, qPrintable(PacsDevice::getLocalAETitle()),
                                               qPrintable(m_dicomPrinter.getAETitle()), qPrintable(m_dicomPrinter.getHostname()), m_dicomPrinter.getPort(),
                                               ASC_DEFAULTMAXPDU, false /*targetSupportsPLUT*/, true /*targetSupportsAnnotation*/, true /*targetImplicitOnly*/);
    
    if (result.bad())
    {
        ERROR_LOG(QString("No s'ha pogut connectar amb la impressora amb AETitle: %1, IP: %2, port: %3, descripció error:%4 ").arg(m_dicomPrinter.getAETitle(), 
                           m_dicomPrinter.getHostname(), QString().setNum(m_dicomPrinter.getPort()), result.text()));

        return;
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUgetPrinterInstance(printConnection)))
    {
        //TODO: Request printer settings els podem obtenir ?
        DEBUG_LOG(QString("spooler: printer communication failed, unable to request printer settings. %1").arg(result.text()));
    }

    if (EC_Normal==result) if (EC_Normal != (result = m_storedPrintDcmtk->printSCUpreparePresentationLUT(
    printConnection, true /*targetRequiresMatchingLUT*/, true/*targetPreferSCPLUTRendering*/, true/*targetSupports12bit*/)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create presentation LUT.").arg(result.text()));
    }
    if (!printSCUcreateBasicFilmSession(printConnection).good())
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film session. %1").arg(result.text()));
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUcreateBasicFilmBox(printConnection, true /*targetPLUTinFilmSession*/)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to create basic film box. %1").arg(result.text()));
    }

    for (size_t imageNumber = 0; imageNumber < m_storedPrintDcmtk->getNumberOfImages(); imageNumber++)
    {
        sendImageToPrint(printConnection, imageNumber, spoolDirectoryPath);
    }

    if (EC_Normal != (result = m_storedPrintDcmtk->printSCUprintBasicFilmSession(printConnection)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to print (at film session level). %1").arg(result.text()));
    } 
    else 
    {
        if (EC_Normal != (result = m_storedPrintDcmtk->printSCUprintBasicFilmBox(printConnection)))
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to print. %1").arg(result.text()));
        }
    }

    if (EC_Normal==result) if (EC_Normal != (result = m_storedPrintDcmtk->printSCUdelete(printConnection)))
    {
        DEBUG_LOG(QString("spooler: printer communication failed, unable to delete print objects %1.").arg(result.text()));
    }

    result = printConnection.releaseAssociation();
    if (result.bad())
    {
        DEBUG_LOG(QString("spooler: release of connection to printer failed. %1").arg(result.text()));
    }
}

Status PrintDicomSpool::printSCUcreateBasicFilmSession(DVPSPrintMessageHandler& printConnection)
{
    OFCondition result;
    DcmDataset datasetBasicFilmSession;
    DcmElement *attributeBasicFilmSession = NULL;
    Status state;

    attributeBasicFilmSession = new DcmCodeString(DCM_MediumType);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getMediumType()));
    result = datasetBasicFilmSession.insert(attributeBasicFilmSession);

    attributeBasicFilmSession = new DcmCodeString(DCM_FilmDestination);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getFilmDestination())); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    //TODO:Comprovar si l'utilitzarem  
    /*attributeBasicFilmSession = new DcmLongString(DCM_FilmSessionLabel);
    attributeBasicFilmSession->putString(printerFilmSessionLabel.c_str()); else result=EC_IllegalCall;
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
   */

    attributeBasicFilmSession = new DcmCodeString(DCM_PrintPriority);
    attributeBasicFilmSession->putString(qPrintable(m_dicomPrintJob.getPrintPriority()));
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    /*TODO:Si no m'equivoco només serveix per quan s'esborra el treball imprés del servidor d'impressió, si no l'especifiquem 
     el servidor d'impressió l'emplena automàticament*/
    /*attributeBasicFilmSession = new DcmShortString(DCM_OwnerID);
    attributeBasicFilmSession->putString(printerOwnerID.c_str());
    datasetBasicFilmSession.insert(attributeBasicFilmSession, OFTrue replaceOld);
    */

    attributeBasicFilmSession = new DcmIntegerString(DCM_NumberOfCopies);
    attributeBasicFilmSession->putString(qPrintable(QString().setNum(m_dicomPrintJob.getNumberOfCopies()))); 
    datasetBasicFilmSession.insert(attributeBasicFilmSession);

    result = m_storedPrintDcmtk->printSCUcreateBasicFilmSession(printConnection, datasetBasicFilmSession, true /*plutInSession*/);
    
    return state.setStatus(result);
}

Status PrintDicomSpool::sendImageToPrint(DVPSPrintMessageHandler& printConnection, size_t imageNumber, const QString &spoolDirectoryPath)
{
    OFCondition result;
    const char *studyUID = NULL, *seriesUID = NULL, *instanceUID = NULL;
    QString imageToPrintPath;

    result = m_storedPrintDcmtk->getImageReference(imageNumber, studyUID, seriesUID, instanceUID);

    if (result != EC_Normal || !studyUID || !seriesUID || !instanceUID)
    {
        ERROR_LOG(QString("No s'ha trobat la imatge número %1 per imprimir").arg(QString().setNum(imageNumber)));
        return Status().setStatus(result);
    }

    //TODO:S'hauria de fer a un altre lloc el càlcul del path de la imatge perquè també s'utilitza a CreateDicomPrintSpool
    imageToPrintPath = QDir::toNativeSeparators(spoolDirectoryPath + QDir::separator() + instanceUID + ".dcm");

    DicomImage *dcmimage = new DicomImage(qPrintable(imageToPrintPath));

    if (dcmimage && EIS_Normal == dcmimage->getStatus())
    {
        //Enviem la imatge
        result = m_storedPrintDcmtk->printSCUsetBasicImageBox(printConnection, imageNumber, *dcmimage, true/*opt_Monochrome1*/);
        if (EC_Normal != result)
        {
            DEBUG_LOG(QString("spooler: printer communication failed, unable to transmit basic grayscale image box. %1").arg(result.text()));
        }
    } 
    else 
    {
        DEBUG_LOG("spooler: unable to load image file " + imageToPrintPath );
    }
    
    delete dcmimage;
    //No s'ha de fer el delete del studyUID, seriesUID i instanceUID perquè són un punter a informació del storedPrint

    return Status().setStatus(result);
}

DVPSStoredPrint* PrintDicomSpool::loadStoredPrintFileDcmtk(const QString &pathStoredPrintDcmtkFile)
{
    DcmFileFormat *storedPrintDcmtkFile = NULL;
    DcmDataset *datasetStoredPrintDcmtkFile = NULL;
    DVPSStoredPrint *storedPrint = new DVPSStoredPrint(2000 /*Valor per defecte a dcpstat.cfg getDefaultIlluminaton() */,10 /*Valor per defecte a dcpstat.cfg getDefaultReflection() */, qPrintable(m_dicomPrinter.getAETitle()));

    OFCondition result = DVPSHelper::loadFileFormat(qPrintable(QDir::toNativeSeparators(pathStoredPrintDcmtkFile)), storedPrintDcmtkFile);
    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut obrir el fitxer StoredPrint de dcmtk, path:" + pathStoredPrintDcmtkFile);
        return NULL;
    }
    
    datasetStoredPrintDcmtkFile  = storedPrintDcmtkFile->getDataset();

    if (!datasetStoredPrintDcmtkFile)
    {
        ERROR_LOG("No s'ha pogut obtenir el dataset del fitxer StoredPrint de dcmtk");
        return NULL;
    }

    result = storedPrint->read(*datasetStoredPrintDcmtkFile); 

    if (EC_Normal != result)
    {
        ERROR_LOG("No s'ha pogut carregar les dades del StoredPrint per imprimir");
        return NULL;
    }

    delete storedPrintDcmtkFile;

    return storedPrint;
}
}