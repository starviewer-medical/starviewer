/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef RETRIEVEIMAGES
#define RETRIEVEIMAGES

#include <QString>
#include <QObject>
#include <ofcond.h>
#include <assoc.h>

#include "pacsdevice.h"

struct T_DIMSE_C_MoveRQ;
struct T_DIMSE_C_MoveRSP;
struct T_DIMSE_C_StoreRQ;
struct T_DIMSE_StoreProgress;
struct T_DIMSE_C_StoreRSP;
struct T_DIMSE_Message;

class DcmDataset;
class DcmFileFormat;

namespace udg{

class Status;
class PacsConnection;
class DicomMask;
class DICOMTagReader;

/** 
    This class helps to interactive with the pacs, retrieve images that match with the mask
 */
class RetrieveImages: public QObject
{
Q_OBJECT
public:
    RetrieveImages(PacsDevice pacs);

    ///Starts the download
    Status retrieve(DicomMask dicomMask);

signals:

    ///Signal que indica que s'ha descarregat un fitxer
    void DICOMFileRetrieved(DICOMTagReader *dicomTagReader);

private:

    /// En aquesta funció acceptem la connexió que se'ns sol·licita per transmetre'ns imatges, i indiquem quins transfer syntax suportem
    OFCondition acceptSubAssociation(T_ASC_Network * associationNetwork, T_ASC_Association ** association);

    /// Responem a una petició d'echo
    OFCondition echoSCP(T_ASC_Association * association, T_DIMSE_Message * dimseMessage, T_ASC_PresentationContextID presentationContextID);

    ///Responem a una petició per guardar una imatge
    OFCondition storeSCP(T_ASC_Association *association, T_DIMSE_Message *messagge, T_ASC_PresentationContextID presentationContextID);

    /// Accepta la connexió que ens fa el PACS, per convertir-nos en un scp
    OFCondition subOperationSCP(T_ASC_Association **subAssociation);

    /// Guarda una composite instance descarregada
    OFCondition save(DcmFileFormat *fileRetrieved, QString dicomFileAbsolutePath);

    /// Retorna el nom del fitxer amb que s'ha de guardar l'objecte descarregat, composa el path on s'ha de guardar + el nom del fitxer, si el path on s'ha de guardar la imatge no existeix el crea
    QString getAbsoluteFilePathCompositeInstance(DcmDataset *imageDataset, QString fileName);

    ///Configura l'objecte MoveRequest per la descàrrega de fitxers DICOM
    T_DIMSE_C_MoveRQ getConfiguredMoveRequest(T_ASC_Association *association);

    /// En cas d'error processa la resposta rebuda per part del SCP, grava l'error el log i el retorna en forma d'objecte Status
    Status processErrorResponseFromMoveSCP(T_DIMSE_C_MoveRSP *moveResponse, DcmDataset *statusDetail);

    ///Callback de move, semblaria que s'executa cada vegada que s'ha descarregat una imatge
    static void moveCallback(void *callbackData, T_DIMSE_C_MoveRQ *moveRequest, int responseCount, T_DIMSE_C_MoveRSP *moveResponse);

    ///Aquesta funció s'encarrega de guardar cada trama DICOM que rebem
    static void storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *storeRequest, char *imageFileName, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *storeResponse, DcmDataset **statusDetail);

    ///Callbac que s'ha executa quan s'inicia un suboperació de descàrrega d'imatges
    static void subOperationCallback(void * /*subOperationCallbackData*/, T_ASC_Network *associationNetwork, T_ASC_Association **subAssociation);

private:
    /// Request DICOM association;
    PacsDevice m_pacs;

    struct StoreSCPCallbackData
    {
        DcmFileFormat *dcmFileFormat;
        RetrieveImages *retrieveImages;
        QString fileName;
    };

};

};
#endif
