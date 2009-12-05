/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "previousstudiesmanager.h"

#include "study.h"
#include "dicommask.h"
#include "patient.h"
#include "pacsmanager.h"
#include "pacsdevicemanager.h"
#include "logging.h"

namespace udg {

PreviousStudiesManager::PreviousStudiesManager()
{
    m_pacsManager = new PacsManager();
    createConnections();
}

PreviousStudiesManager::~PreviousStudiesManager()
{
    m_pacsManager->cancelCurrentQueries();
}

void PreviousStudiesManager::createConnections()
{
    connect(m_pacsManager, SIGNAL(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)), SLOT(queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>)));

    connect(m_pacsManager, SIGNAL(queryFinished()), SLOT(queryFinished()));

    connect(m_pacsManager, SIGNAL(errorQueryingStudy(PacsDevice)), SLOT(errorQueryingStudy(PacsDevice)));
}

void PreviousStudiesManager::queryPreviousStudies(Study *study)
{
    PacsDeviceManager pacsDeviceManager;
    QList<PacsDevice> pacsDeviceListToQuery = pacsDeviceManager.getPACSList(true);

    INFO_LOG("Es buscaran els estudis previs del pacient " + study->getParentPatient()->getFullName() + " amb ID " + study->getParentPatient()->getID() + 
    " de l'estudi " + study->getInstanceUID() + " fet a la data " + study->getDate().toString());

    m_pacsManager->cancelCurrentQueries();//Per si hi hagués una consulta executant-se

    ///Fem neteja de consultes anteriors
    m_mergedHashPacsIDOfStudyInstanceUID.clear();
    m_mergedStudyList.clear();
    m_pacsDeviceIDErrorEmited.clear();

    m_studyToFindPrevious = study;

    //Preguntem al PACS per estudis
    if (pacsDeviceListToQuery.count() > 0)
    {
        m_pacsManager->queryStudy(getPreviousStudyDicomMaskPatientID(study), pacsDeviceListToQuery);
        m_pacsManager->queryStudy(getPreviousStudyDicomMaskPatientName(study), pacsDeviceListToQuery);
    }
    else 
    {
        //Sinó hi ha cap PACS pel qual cercar per defecte fem l'emit del queryFinished
        queryFinished();
    }
}

void PreviousStudiesManager::cancelCurrentQuery()
{
    m_pacsManager->cancelCurrentQueries();
}

bool PreviousStudiesManager::isExecutingQueries()
{
    return m_pacsManager->isExecutingQueries();
}

void PreviousStudiesManager::queryFinished()
{
    /*Quan totes les query han acabat és quant fem l'emit amb els estudis previs trobats. A diferència de la 
      PacsManager no podem emetre els resultats que anem rebent, perquè hem de fer un merge del resultats rebuts,
      per no tenir duplicats*/
    emit queryPreviousStudiesFinished(m_mergedStudyList, m_mergedHashPacsIDOfStudyInstanceUID);
}

void PreviousStudiesManager::queryStudyResultsReceived(QList<Patient*> patientListResults, QHash<QString, QString> hashTablePacsIDOfStudyInstanceUID)
{
    foreach(Patient *patient, patientListResults)
    {
        foreach(Study *study, patient->getStudies())
        {
            if (!isStudyInMergedStudyList(study) && !isStudyToFindPrevious(study))
            {
                /*Si l'estudi no està a llista ja d'estudis afegits i no és el mateix estudi pel qua ens han demanat el 
                 *previ l'afegim*/
                m_mergedStudyList.append(study);

                m_mergedHashPacsIDOfStudyInstanceUID[study->getInstanceUID()] = hashTablePacsIDOfStudyInstanceUID[study->getInstanceUID()];
            }
        }
    }
}

void PreviousStudiesManager::errorQueryingStudy(PacsDevice pacs)
{
    /*Com que fem dos cerques al mateix pacs si una falla, l'altra segurament també fallarà per evitar enviar
      dos signals d'error si les dos fallen, ja que per des de fora ha de ser transparent el número de consultes
      que es fa al PACS, i han de rebre un sol error comprovem si tenim l'ID del PACS a la llista de signals 
      d'errors en PACS emesos*/
    if (!m_pacsDeviceIDErrorEmited.contains(pacs.getID()))
    {
	    m_pacsDeviceIDErrorEmited.append(pacs.getID());
	    emit errorQueryingPreviousStudies(pacs);
    }
}

bool PreviousStudiesManager::isStudyInMergedStudyList(Study *study)
{
    bool studyFoundInMergedList = false;

    foreach(Study *studyMerged, m_mergedStudyList)
    {
        if (study->getInstanceUID() == studyMerged->getInstanceUID())
        {
            studyFoundInMergedList = true;
            break;
        }
    }

    return studyFoundInMergedList;
}

bool PreviousStudiesManager::isStudyToFindPrevious(Study *study)
{
    return study->getInstanceUID() == m_studyToFindPrevious->getInstanceUID();
}

DicomMask PreviousStudiesManager::getBasicDicomMask()
{
    DicomMask dicomMask;

    ///Definim els camps que la consulta ha de retornar
    dicomMask.setPatientName("");
    dicomMask.setPatientId("");
    dicomMask.setStudyId("");
    dicomMask.setStudyDescription("");
    dicomMask.setStudyModality("");
    dicomMask.setStudyDate("");
    dicomMask.setStudyTime("");
    dicomMask.setStudyUID("");

    return dicomMask;
}

DicomMask PreviousStudiesManager::getPreviousStudyDicomMaskPatientID(Study *study)
{
    DicomMask dicomMask = getBasicDicomMask();

    ///Indiquem que volem buscar estudis igual o menors d'aquella data
    dicomMask.setStudyDate(getPreviousStudyDateMask(study->getDate()));
    dicomMask.setPatientId(study->getParentPatient()->getID());

    return dicomMask;
}

DicomMask PreviousStudiesManager::getPreviousStudyDicomMaskPatientName(Study *study)
{
    DicomMask dicomMask = getBasicDicomMask();

    ///Indiquem que volem buscar estudis igual o menors d'aquella data
    dicomMask.setStudyDate(getPreviousStudyDateMask(study->getDate()));
    dicomMask.setPatientName(study->getParentPatient()->getFullName());

    return dicomMask;
}

QString PreviousStudiesManager::getPreviousStudyDateMask(QDate studyDate)
{
    return "-" + studyDate.toString("yyyyMMdd");
}
}
