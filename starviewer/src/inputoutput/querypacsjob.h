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

/**
    Classe que cercar estudis en un dispositiu pacs, creant un nou job utilitzant les threadweaver
  */
class QueryPacsJob : public PACSJob {
Q_OBJECT
public:

    /// Constructor/Desctructor de la classe
    ///A la DICOMMask per cada camp que volem que el PACS ens retorni li hem d'haver fet el set amb un string empty per strings o Null per dates i hores, sinó la consulta el PACS
    ///no retornarà la informació d'aquest camp per l'estudi/sèrie/imatge, ja que al PACS no retorna tots els camps sinó només els que se li sol·liciten
    QueryPacsJob(PacsDevice parameters, DicomMask mask);
    ~QueryPacsJob();

    /// El codi d'aquest mètode es el que s'executa en un nou thread
    void run();

    /// Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    /// Retorna la màscara sobre la que es fa la consulta
    DicomMask getDicomMask();

    /// Indica a quin nivell es fa la consulta study, series, image
    DicomMask::QueryLevel getQueryLevel();

    /// Retorna els resultats de cerca. Només retornarà els resulats de cerca pel nivell de cerca indicat a la DicomMask, per exemple en una DicomMask
    /// per cerca sèries només getQueryResultAsSeriesList retornarà resultats, els altres dos mètodes retornaran llistes buides
    QList<Patient*> getPatientStudyList();
    QList<Series*> getSeriesList();
    QList<Image*> getImageList();

    /// Retorna l'estat de la consulta
    PACSRequestStatus::QueryRequestStatus getStatus();

    /// Retorna una descripció de l'estat retornat per la consulta al PACS
    QString getStatusDescription();

private:
    /// Demana que es cancel·li la consulta del job
    void requestCancelJob();

    /// Retorna el Query Level com a QString per poder generar els missatges d'error
    QString getQueryLevelFromDICOMMaskAsQString();

private:
    DicomMask m_mask;
    QueryPacs *m_queryPacs;

    PACSRequestStatus::QueryRequestStatus m_queryRequestStatus;
};

}  // End namespace UdG

#endif
