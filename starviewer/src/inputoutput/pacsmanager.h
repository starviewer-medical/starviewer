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


#ifndef UDGPACSMANAGER_H
#define UDGPACSMANAGER_H

#include <QList>
#include <QHash>
#include <ThreadWeaver/Queue>

#include "patient.h"
#include "pacsdevice.h"
#include "pacsjob.h"

namespace udg {

class DicomMask;

/**
    Classe manager que ens permet comunicar-nos amb el PACS
  */
class PacsManager : public QObject {
Q_OBJECT
public:
    /// Constructor de la classe
    PacsManager();

    /// La PacsManager no té constructor implementat degut a que fer un delete dels ThreadWeaver implica que l'objecte ThreadWeaver no es destruirà fins que
    /// hagi acabat l'execució dels jobs que està executant en aquell moment, i fins i tot sol·licitant un requestAbort dels jobs que s'executen podríem tenir
    /// problemes. Imaginem el cas en que fem una consulta al PACS i ens indiquen que volen tancar Starviewer, tot i que es faci un requestAbort pot ser que el
    /// job tardi uns segons a cancel·lar-se pq el PACS envia els resultats que tenia a punt a per enviar en el moment de la cancel·lació, o un cas pitjor que
    /// el PACS no té implementada la cancel·lació, això vol dir que Starviewer no es tancaria fins que s'acabés la consulta el PACS, si és una consulta llarga
    /// pot durar segons. Un altre cas és el bug de dcmtk que fa que si el PACS deixa de respondre mentre ens envia fitxers, dcmtk mai saltarà per timeout
    /// això obliga a l'usuari quan passa a haver de tancar i tornar a obrir Starviewer, però si fèssim delete del thread de descàrregues com que aquest
    /// thread no acabaria mai Starviewer no es tancaria i pel QtSingleApplication l'usuari no podria tornar a obrir l'aplicació.
    //~PacsManager();

    /// Encua un PACSJob per a que es processi
    void enqueuePACSJob(PACSJobPointer pacsJob);

    /// Indica si s'estan executant PACSJob
    bool isExecutingPACSJob();

    /// Indica si s'executen peticions del tipus de PACSJob indicat
    bool isExecutingPACSJob(PACSJob::PACSJobType pacsJobType);

    /// Demana que es cancel·li l'execució dels PACSJob d'un tipus. Al invocar aquests mètodes es desencua els jobs pendents d'executar i
    /// i demana abortar els jobs que s'estan executant. Els jobs que s'estan executant no s'aborten immeditament, el mètode és assíncron.
    void requestCancelPACSJob(PACSJobPointer pacsJob);

    /// Demana cancel·lar tots els jobs. Els PACSJob pendents d'executar no s'executen i els que s'estan executant s'aborten. Els PACSJob
    /// que s'estan executant no s'aborten immeditament, el mètode és assíncron.
    void requestCancelAllPACSJobs();

    /// Indica si s'està executant algun PACSJob
    bool isIdle();

    /// Espera a que hagin acabat tots els job
    bool waitForAllPACSJobsFinished(int msec = INT_MAX);

signals:
    /// Signal que s'emet per indicar que s'ha encuat un nou PACSJob
    void newPACSJobEnqueued(PACSJobPointer pacsJob);

    /// Signal que indica que ens han demanat cancel·lar un PACSJob
    void requestedCancelPACSJob(PACSJobPointer pacsJob);

private:
    ThreadWeaver::Queue *m_queryQueue;
    ThreadWeaver::Queue *m_sendDICOMFilesToPACSQueue;
    ThreadWeaver::Queue *m_retrieveDICOMFilesFromPACSQueue;
};

};  //  end  namespace udg

#endif
