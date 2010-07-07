/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSENDDICOMFILESTOPACS_H
#define UDGSENDDICOMFILESTOPACS_H

#include <QList>
#include <QObject>
#include "pacsdevice.h"
#include "pacsrequeststatus.h"
struct T_DIMSE_C_StoreRSP;

class DcmDataset;

namespace udg {

class PacsConnection;
class Image;

/**
	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
class SendDICOMFilesToPACS: public QObject{
Q_OBJECT
public:
    SendDICOMFilesToPACS(PacsDevice pacsDevice);

    ///Retorna el PACS que s'ha passat al constructor i amb el qual es fa el send de fitxers DICOM
    PacsDevice getPacs();

   /** Guarda les imatges que s'especifiquen a la llista en el pacs establert per la connexió
    * @param ImageListStore de les imatges a enviar al PACS
    * @return indica estat del mètode
    */
    PACSRequestStatus::SendRequestStatus send(QList<Image*> imageListToSend);

    ///Demanem cancel·lar l'enviament d'imatges. La cancel·lació de les imatges és assíncrona no es duu a terme fins que ha finalitzat l'enviament de la imatge que
    ///s'estava enviant al moment de demananr la cancel·lació
    void requestCancel();

    ///Retorna el número d'imatges enviades correctament
    int getNumberOfImagesSentSuccesfully();

    ///Retorna el número d'imatges que l'enviament ha fallat
    int getNumberOfImagesSentFailed();

    ///Retorna el número d'imatges que s'ha enviat però han donat warning, pot donar warning per exemple en el cas que el PACS modifiqui alguna dada de la imatge
    int getNumberOfImagesSentWarning();

signals:
   ///Sinal que indica que s'ha fet l'enviament de la imatge passada per paràmetre al PACS, i el número d'imatges que es porten enviades
   void DICOMFileSent(Image *image, int numberOfImagesSent);

private :

    ///Inicialitze els comptadors d'imatges per controlar quantes han fallat/s'han enviat....
    void initialitzeImagesCounters(int numberOfImagesToStore);

    ///Processa un resposta del Store SCP que no ha tingut l'Status Successfull
    void processResponseFromStoreSCP(T_DIMSE_C_StoreRSP *response, DcmDataset *statusDetail, QString filePathDicomObjectStoredFailed);

    ///Envia una image al PACS amb l'associació passada per paràmetre, retorna si la imatge s'ha enviat correctament
    bool storeSCU(PacsConnection pacsConnection, QString filePathToStore);

    ///Retorna un Status indicant com ha finalitzat l'operació C-Store
    PACSRequestStatus::SendRequestStatus getStatusStoreSCU();

private: 
    //Indica números d'imatges enviades correctament/Imatges enviades però que ha retorna warning/Total d'imatges que s'ha enviat
    int m_numberOfSendImagesSuccessful, m_numberOfSendImagesWithWarning, m_numberOfImagesToSend;
    PacsDevice m_pacs;
    bool m_abortIsRequested;
    

};

}

#endif
