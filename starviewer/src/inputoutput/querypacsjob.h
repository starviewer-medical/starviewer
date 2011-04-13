#ifndef UDGQQUERYPACSJOB_H
#define UDGQQUERYPACSJOB_H

#include <QList>
#include <QHash>

#include "dicommask.h"
#include "pacsjob.h"
#include "pacsrequeststatus.h"

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

    ///Retorna la llista d'estudis trobats que compleixen el criteri de cerca
    QList<Patient*> getPatientStudyList();

    ///Retorna la llista de series trobades que compleixen els criteris de cerca
    QList<Series*> getSeriesList();

    ///Retorna la llista d'imatges trobades que compleixen els criteris de cerca
    QList<Image*> getImageList();

    ///Retorna un Hashtable que indica per l'UID de l'estudi a quin PACS pertany l'estudi
    QHash<QString,QString> getHashTablePacsIDOfStudyInstanceUID();

    ///Retorna l'estat de la consulta
    PACSRequestStatus::QueryRequestStatus getStatus();

    ///Retorna una descripció de l'estat retornat per la consulta al PACS
    QString getStatusDescription();

private :

    ///Demana que es cancel·li la consulta del job
    void requestCancelJob();

    ///Retorna el Query Level com a QString per poder generar els missatges d'error
    QString getQueryLevelAsQString();

private :

    DicomMask m_mask;
    QueryPacs *m_queryPacs;
    QueryLevel m_queryLevel;

    PACSRequestStatus::QueryRequestStatus m_queryRequestStatus;
};

}  //end namespace UdG

#endif
