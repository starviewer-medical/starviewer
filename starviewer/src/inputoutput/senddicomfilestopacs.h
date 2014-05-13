/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGSENDDICOMFILESTOPACS_H
#define UDGSENDDICOMFILESTOPACS_H

#include <QList>
#include <QObject>
#include <ofcond.h>

#include "pacsdevice.h"
#include "pacsrequeststatus.h"
#include "dimsecservice.h"

class DcmDataset;

struct T_DIMSE_C_StoreRSP;
struct T_ASC_Association;

namespace udg {

class Image;
class PACSConnection;

class SendDICOMFilesToPACS : public QObject, public DIMSECService {
Q_OBJECT
public:
    SendDICOMFilesToPACS(PacsDevice pacsDevice);

    /// Retorna el PACS que s'ha passat al constructor i amb el qual es fa el send de fitxers DICOM
    PacsDevice getPacs();

    /// Guarda les imatges que s'especifiquen a la llista en el pacs establert per la connexió
    /// @param ImageListStore de les imatges a enviar al PACS
    /// @return indica estat del mètode
    PACSRequestStatus::SendRequestStatus send(QList<Image*> imageListToSend);

    /// Demanem cancel·lar l'enviament d'imatges. La cancel·lació de les imatges és assíncrona no es duu a terme fins que ha finalitzat l'enviament de la
    /// imatge que s'estava enviant al moment de demananr la cancel·lació
    void requestCancel();

    /// Retorna el número d'imatges enviades correctament
    int getNumberOfDICOMFilesSentSuccesfully();

    /// Retorna el número d'imatges que l'enviament ha fallat
    int getNumberOfDICOMFilesSentFailed();

    /// Retorna el número d'imatges que s'ha enviat però han donat warning, pot donar warning per exemple en el cas que el PACS modifiqui alguna dada de
    /// la imatge
    int getNumberOfDICOMFilesSentWarning();

signals:
   /// Sinal que indica que s'ha fet l'enviament de la imatge passada per paràmetre al PACS, i el número d'imatges que es porten enviades
    void DICOMFileSent(Image *image, int numberOfDICOMFilesSent);

protected:

    /// Number of files that have been sent successfully.
    int m_numberOfDICOMFilesSentSuccessfully;

private:

    /// Creates and returns a PACS connection to the given PACS device.
    virtual PACSConnection* createPACSConnection(const PacsDevice &pacsDevice) const;

    /// Removes images from the list when multiple images point to the same file, so that at the end each file is present only once.
    void removeDuplicateFiles(QList<Image*> &imageList) const;

    /// Inicialitze els comptadors d'imatges per controlar quantes han fallat/s'han enviat....
    void initialitzeDICOMFilesCounters(int numberOfDICOMFilesToSend);

    /// Processa un resposta del Store SCP que no ha tingut l'Status Successfull
    void processResponseFromStoreSCP(unsigned int dimseStatusCode, QString filePathDicomObjectStoredFailed);

    /// Envia una image al PACS amb l'associació passada per paràmetre, retorna si la imatge s'ha enviat correctament
    virtual bool storeSCU(T_ASC_Association *association, QString filePathToStore);

    /// Retorna un Status indicant com ha finalitzat l'operació C-Store
    PACSRequestStatus::SendRequestStatus getStatusStoreSCU();

private:

    /// Number of files that have been sent but with a warning.
    int m_numberOfDICOMFilesSentWithWarning;
    /// Total number of files that had to be sent.
    int m_numberOfDICOMFilesToSend;
    PacsDevice m_pacs;
    bool m_abortIsRequested;
    OFCondition m_lastOFCondition;

};

}

#endif
