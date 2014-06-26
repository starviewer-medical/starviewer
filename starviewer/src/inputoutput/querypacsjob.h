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
    /// Indica a quin nivell fem la query
    enum QueryLevel { study, series, image };

    /// Constructor/Desctructor de la classe
    ///A la DICOMMask per cada camp que volem que el PACS ens retorni li hem d'haver fet el set amb un string empty per strings o Null per dates i hores, sinó la consulta el PACS
    ///no retornarà la informació d'aquest camp per l'estudi/sèrie/imatge, ja que al PACS no retorna tots els camps sinó només els que se li sol·liciten
    QueryPacsJob(PacsDevice parameters, DicomMask mask, QueryLevel queryLevel);
    ~QueryPacsJob();

    /// El codi d'aquest mètode es el que s'executa en un nou thread
    virtual void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread);

    /// Retorna el tipus de PACSJob que és l'objecte
    PACSJob::PACSJobType getPACSJobType();

    /// Retorna la màscara sobre la que es fa la consulta
    DicomMask getDicomMask();

    /// Indica a quin nivell es fa la consulta study, series, image
    QueryLevel getQueryLevel();

    /// Retorna la llista d'estudis trobats que compleixen el criteri de cerca. La classe que demani els resultats de cerca d'estudis, és responsable 
    /// d'eliminar els objects retornats aquest mètode
    QList<Patient*> getPatientStudyList();

    /// Retorna la llista de series trobades que compleixen els criteris de cerca. La classe que demani els resultats de cerca de sèries és responsable 
    /// d'eliminar els objects retornats aquest mètode
    QList<Series*> getSeriesList();

    /// Retorna la llista d'imatges trobades que compleixen els criteris de cerca. LLa classe que demani els resultats de cerca d'imatges, és responsable d'eliminar 
    /// els objects retornats aquest mètode
    QList<Image*> getImageList();

    /// Retorna l'estat de la consulta
    PACSRequestStatus::QueryRequestStatus getStatus();

    /// Retorna una descripció de l'estat retornat per la consulta al PACS
    QString getStatusDescription();

private:
    /// Demana que es cancel·li la consulta del job
    void requestCancelJob();

    /// Retorna el Query Level com a QString per poder generar els missatges d'error
    QString getQueryLevelAsQString();

private:
    DicomMask m_mask;
    QueryPacs *m_queryPacs;
    QueryLevel m_queryLevel;

    PACSRequestStatus::QueryRequestStatus m_queryRequestStatus;
};

}  // End namespace UdG

#endif
