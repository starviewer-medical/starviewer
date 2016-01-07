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

#ifndef UDGSENDDICOMFILESTOPACSJOB_H
#define UDGSENDDICOMFILESTOPACSJOB_H

#include <QObject>

#include "pacsjob.h"
#include "pacsdevice.h"
#include "pacsrequeststatus.h"

namespace udg {

class Study;
class Image;
class SendDICOMFilesToPACS;

/**
    Job que s'encarrega d'enviar fitxers del PACS.
  */
class SendDICOMFilesToPACSJob : public PACSJob {
Q_OBJECT
public:
    /// Atenció, La llista d'imatges ha de contenir l'estructura Pacient, Estudi, Series, Imatges
    SendDICOMFilesToPACSJob(PacsDevice pacsDevice, QList<Image*>);
    ~SendDICOMFilesToPACSJob();

    /// Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    /// Codi que executarà el job
    virtual void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread);

    /// Retorna l'Status del Job
    PACSRequestStatus::SendRequestStatus getStatus();

    /// Retorna l'Status descrit en un QString , aquest QString està pensat per ser mostrat en QMessageBox per informar a l'usuari de l'estat que ha retornat
    /// el job en el mateixa descripció s'indica de quin és l'estudi afectat
    QString getStatusDescription();

    /// Retorna la llista d'imatges que s'han indicat que s'havien de guardar
    QList<Image*> getFilesToSend();

    /// Retorna l'estudi de les imatges que s'havia de guardar
    Study* getStudyOfDICOMFilesToSend();

signals:
    /// Signal que s'emet quan s'enviat una imatge al PACS
    void DICOMFileSent(PACSJobPointer pacsJob, int numberOfDICOMFilesSent);

    /// Signal que s'emet quan s'ha enviat un serie completa al PACS
    void DICOMSeriesSent(PACSJobPointer pacsJob, int numberOfSeriesSent);

private:
    /// Sol·licita que ens cancel·li el job
    void requestCancelJob();

private slots:
    /// Slot que respón al signal de SendDICOMFilesToPACS DICOMFileSent
    void DICOMFileSent(Image *imageSent, int numberOfDICOMFilesSent);

private:
    QList<Image*> m_imagesToSend;
    PACSRequestStatus::SendRequestStatus m_sendRequestStatus;
    SendDICOMFilesToPACS *m_sendDICOMFilesToPACS;
    int m_numberOfSeriesSent;
    QString m_lastDICOMFileSeriesInstanceUID;
};

};

#endif
