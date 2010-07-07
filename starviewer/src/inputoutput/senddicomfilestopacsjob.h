/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSENDDICOMFILESTOPACSJOB_H
#define UDGSENDDICOMFILESTOPACSJOB_H

#include <QObject>

#include "pacsjob.h"
#include "pacsdevice.h"
#include "pacsrequeststatus.h"

namespace udg {

/**
    Job que s'encarrega d'enviar fitxers del PACS.

   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Study;
class Image;
class SendDICOMFilesToPACS;

class SendDICOMFilesToPACSJob: public PACSJob
{
Q_OBJECT
public:

    ///Atenció, La llista d'imatges ha de contenir l'estructura Pacient, Estudi, Series, Imatges
    SendDICOMFilesToPACSJob(PacsDevice pacsDevice, QList<Image*>);
    ~SendDICOMFilesToPACSJob();

    ///Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    ///Codi que executarà el job
    void run();

    ///Retorna l'Status del Job
    PACSRequestStatus::SendRequestStatus getStatus();

    /**Retorna l'Status descrit en un QString , aquest QString està pensat per ser mostrat en QMessageBox per informar a l'usuari de l'estat que ha retornat
        el job en el mateixa descripció s'indica de quin és l'estudi afectat*/
    QString getStatusDescription();

    ///Retorna la llista d'imatges que s'han indicat que s'havien de guardar
    QList<Image*> getFilesToSend();

    ///Retorna l'estudi de les imatges que s'havia de guardar
    Study *getStudyOfImagesToSend();

    ///Retorna el número d'imatges enviades correctament
    int getNumberOfImagesSentSuccesfully();

    ///Retorna el número d'imatges que l'enviament ha fallat
    int getNumberOfImagesSentFailed();

    ///Retorna el número d'imatges que s'ha enviat però han donat warning
    int getNumberOfImagesSentWarning();

signals:

    ///Signal que s'emet quan s'enviat una imatge al PACS
    void DICOMFileSent(PACSJob *, int numberOfImagesSent);

    ///Signal que s'emet quan s'ha enviat un serie completa al PACS
    void DICOMSeriesSent(PACSJob*, int numberOfSeriesSent);

private:

    ///Sol·licita que ens cancel·li el job
    void requestCancelJob();

private slots:

    ///Slot que respón al signal de SendDICOMFilesToPACS DICOMFileSent
    void DICOMFileSent(Image *imageSent, int numberOfImagesSent);

private:

    QList<Image*> m_imagesToSend;
    PACSRequestStatus::SendRequestStatus m_sendRequestStatus;
    SendDICOMFilesToPACS *m_sendDICOMFilesToPACS;
    bool m_abortRequested;
    int m_numberOfSeriesSent;
    QString m_lastImageSeriesInstanceUID;
};

};

#endif
