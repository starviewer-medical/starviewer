/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�fics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPACSMANAGER_H
#define UDGPACSMANAGER_H

#include <QList>
#include <QHash>
#include <ThreadWeaver/Weaver>

#include "patient.h"
#include "pacsdevice.h"
#include "pacsjob.h"

namespace udg {

class DicomMask;

/** Classe manager que ens permet comunicar-nos amb el PACS
*/
class PacsManager: public QObject{
Q_OBJECT
public:

    ///Constructor de la classe
    PacsManager();

    ///Encua un PACSJob per a que es processi
    void enqueuePACSJob(PACSJob *pacsJob);

    ///Indica si s'executen peticions del tipus de PACSJob indicat
    bool isExecutingPACSJob(PACSJob::PACSJobType pacsJobType);

    ///Demana que es cancel·li l'execució del PACSJob
    void requestCancelPACSJob(PACSJob *pacsJob);

signals:

    ///Signal que s'emet per indicar que s'ha encuat un nou PACSJob
    void newPACSJobEnqueued(PACSJob *pacsJob);

    ///Signal que indica que ens han demanat cancel·lar un PACSJob
    void requestedCancelPACSJob(PACSJob *pacsJob);

private:

    ThreadWeaver::Weaver* m_queryWeaver;
    ThreadWeaver::Weaver *m_sendDICOMFilesToPACSWeaver;
    ThreadWeaver::Weaver *m_retrieveDICOMFilesFromPACSWeaver;
};

};  //  end  namespace udg

#endif
