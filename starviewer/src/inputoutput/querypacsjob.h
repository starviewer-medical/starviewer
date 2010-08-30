/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQQUERYPACSJOB_H
#define UDGQQUERYPACSJOB_H

#include <QThread>
#include <QList>
#include <QHash>
#include <ThreadWeaver/Job>
#include <ThreadWeaver/Thread>

#include "dicommask.h"
#include "pacsdevice.h"
#include "status.h"

class QString;

namespace udg {

class Patient;
class Study;
class Series;
class Image;
class QueryPacs;

/** Classe que cercar estudis en un dispositiu pacs, creant un nou job utilitzant les threadweaver
	@author Grup de Gràfics de Girona  (GGG) <vismed@ima.udg.es>
*/
using namespace ThreadWeaver;

class QueryPacsJob :public Job
{
    Q_OBJECT
public:

    ///Indica a quin nivell fem la query
    enum QueryLevel { study, series, image};

    /// Constructor/Desctructor de la classe
    QueryPacsJob(PacsDevice parameters, DicomMask mask, QueryLevel queryLevel, QObject *parent = 0);
    ~QueryPacsJob();

    /// el codi d'aquest mètode es el que s'executa en un nou thread
    void run();

    ///sol·licita que es s'aborti el jo actual
    void requestAbort();

    ///Retorna la màscara sobre la que es fa la consulta
    DicomMask getDicomMask();

    ///Indica a quin nivell es fa la consulta study, series, image
    QueryLevel getQueryLevel();

    ///Retorna l'estat de la consulta
    Status getStatus();

    ///Retorna el Pacs sobre el qual es fa la consulta
    PacsDevice getPacsDevice();

    ///Indica si s'ha demanaat abortar el job
    bool isAbortRequested();

    ///Retorna la llista d'estudis trobats que compleixen el criteri de cerca
    QList<Patient*> getPatientStudyList();

    ///Retorna la llista de series trobades que compleixen els criteris de cerca
    QList<Series*> getSeriesList();

    ///Retorna la llista d'imatges trobades que compleixen els criteris de cerca
    QList<Image*> getImageList();

    ///Retorna un Hashtable que indica per l'UID de l'estudi a quin PACS pertany l'estudi
    QHash<QString,QString> getHashTablePacsIDOfStudyInstanceUID();

private :

    PacsDevice m_pacsDevice;
    DicomMask m_mask;
    QueryPacs *m_queryPacs;
    QueryLevel m_queryLevel;
    bool m_isAbortRequested; //Indica si s'ha demanat abortar el job

    Status m_queryStatus;
};

}  //end namespace UdG

#endif
