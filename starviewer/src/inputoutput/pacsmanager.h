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

    ///Fa una consulta d'estudis assíncrona als PACS passats per paràemetre
    void queryStudy(DicomMask mask, QList<PacsDevice> pacsToQuery);

    ///Fa una consulta assíncrona de les sèries que compleixen la màscara de cerca al PACS passat per paràmetre
    void querySeries(DicomMask mask, PacsDevice pacsToQuery);

    ///Fa una consulta assíncrona de les imatges que compleixen la màscara de cerca al PACS passat per paràmetre
    void queryImage(DicomMask mask, PacsDevice pacsToQuery);

    ///Cancel·la les consultes actuals que s'estan executant, i cancel·la les consultes encuades per executar
    void cancelCurrentQueries();

    ///Indica si s'executen queries en aquest moment
    bool isExecutingQueries();

    ///Encua un PACSJob per a que es processi
    void enqueuePACSJob(PACSJob *pacsJob);

    ///Indica si s'executen peticions del tipus de PACSJob indicat
    bool isExecutingPACSJob(PACSJob::PACSJobType pacsJobType);

signals:

    ///Signal que s'emet quan s'han rebut resultats d'un PACS de la cerca d'estudis
    void queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString> hashPacsIDOfStudyInstanceUID);

    ///Signal que s'emet quan s'han rebut resultats d'un PACS de la cerca de series
    void querySeriesResultsReceived(QString studyInstanceUID, QList<Series*>);

    ///Signal que s'emet quan s'han rebut resultats d'un PACS de la cerca d'imatges
    void queryImageResultsReceived(QString studyInstanceUID, QString seriesInstanceUID, QList<Image*>);

    ///Signal que s'emet per indicar que la query ha finalitzat
    void queryFinished();

    ///Signal que s'emet per indicar que s'ha produït un error a la consulta d'estudis d'un PACS
    void errorQueryingStudy(PacsDevice pacs);

    ///Signal que s'emet per indicar que s'ha produït un error a la consulta de series d'un PACS
    void errorQueryingSeries(QString studyInstanceUID, PacsDevice pacs);

    ///Signal que s'emet per indicar que s'ha produït un error a la consulta d'imatges d'un PACS
    void errorQueryingImage(QString studyInstanceUID, QString seriesInstanceUID, PacsDevice pacs);

    ///Signal que s'emet per indicar que s'ha encuat un nou PACSJob
    void newPACSJobEnqueued(PACSJob *pacsJob);

private slots:

    ///Slot que s'activa quan un job ha finalitzat
    void queryJobFinished ( ThreadWeaver::Job* );

private:

    ThreadWeaver::Weaver* m_queryWeaver;
    ThreadWeaver::Weaver *m_sendDICOMFilesToPACSWeaver;
    ThreadWeaver::Weaver *m_retrieveDICOMFilesFromPACSWeaver;

    int m_numberOfQueryPacsJobsPending;

};

};  //  end  namespace udg

#endif
