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

#include "dicommask.h"
#include "status.h"
#include "pacsjob.h"

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

class QueryPacsJob :public PACSJob
{
    Q_OBJECT
public:

    ///Indica a quin nivell fem la query
    enum QueryLevel { study, series, image};

    /// Constructor/Desctructor de la classe
    QueryPacsJob(PacsDevice parameters, DicomMask mask, QueryLevel queryLevel);
    ~QueryPacsJob();

    /// el codi d'aquest mètode es el que s'executa en un nou thread
    void run();

    ///Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    ///Retorna la màscara sobre la que es fa la consulta
    DicomMask getDicomMask();

    ///Indica a quin nivell es fa la consulta study, series, image
    QueryLevel getQueryLevel();

    ///Retorna l'estat de la consulta
    Status getStatus();

    ///Retorna la llista d'estudis trobats que compleixen el criteri de cerca
    QList<Patient*> getPatientStudyList();

    ///Retorna la llista de series trobades que compleixen els criteris de cerca
    QList<Series*> getSeriesList();

    ///Retorna la llista d'imatges trobades que compleixen els criteris de cerca
    QList<Image*> getImageList();

    ///Retorna un Hashtable que indica per l'UID de l'estudi a quin PACS pertany l'estudi
    QHash<QString,QString> getHashTablePacsIDOfStudyInstanceUID();

private :

    ///Demana que es cancel·li la consulta del job
    void requestCancelJob();

private :

    DicomMask m_mask;
    QueryPacs *m_queryPacs;
    QueryLevel m_queryLevel;

    Status m_queryStatus;
};

}  //end namespace UdG

#endif
