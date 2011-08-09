#ifndef UDGPREVIOUSSTUDIESMANAGER_H
#define UDGPREVIOUSSTUDIESMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QDate>

#include "pacsdevice.h"

namespace udg {

class Patient;
class Study;
class DicomMask;
class PacsManager;
class PACSJob;
class QueryPacsJob;

/**
    Aquesta classe donat un Study demana els estudis previs en els PACS configurats per defecte, degut a que
    ara actualment en el PACS tenim el mateix pacients amb PatientID diferents, també a part de cercar estudis
    que coincideixin amb el PatientID també es farà una altre cerca per Patient Name.
  */
/* TODO: En teoria amb la implantació del SAP els problemes de que un Pacient té diversos Patient ID o que té el nom
   escrit de maneres diferents haurien de desapareixer, per tant d'aquí un temps quan la majoria d'estudis del PACS
   ja s'hagin fet a través del SAP i constatem que el Patient ID pel mateix pacient sempre és el mateix,
   la cerca per nom de pacient podria desapareixer
 */
class PreviousStudiesManager : public QObject {
Q_OBJECT
public:
    PreviousStudiesManager();
    ~PreviousStudiesManager();

    /// Fa una consulta d'estudis previs assíncrona als PACS que estiguin marcats per defecte, si dos del PACS retornen el mateix estudi només es tindrà en compte
    /// el del primer PACS que ha respós
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryMergedPreviousStudies(Study *study);

    /// Fa una consulta d'estudis del pacient assíncrona als PACS que estiguin marcats per defecte, si dos del PACS retornen el mateix estudi només es tindrà en compte
    /// el del primer PACS que ha respós
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryMergedStudies(Patient *patient);

    /// Cancel·la les consultes actuals que s'estan executant, i cancel·la les consultes encuades per executar
    void cancelCurrentQuery();

    /// Indica si s'executen queries en aquest moment
    bool isExecutingQueries();

    /// Mètode per demanar que es descarregui un estudi
    void downloadStudy(Study *study, QString pacs);

signals:
    /// Signal que s'emet quan ha finalitzat la consulta d'estudis previs. La llista amb els resultats s'esborrarà quan es demani una altra cerca.
    void queryStudiesFinished(QList<Study*>);

    /// Signal que s'emet per indicar que s'ha produït un error a la consulta d'estudis d'un PACS
    void errorQueryingStudies(PacsDevice pacs);

    /// Signal que s'emet per indicar que s'ha produït un error durant la descarrega d'un estudi (pot ser previ o no)
    void errorDownloadingPreviousStudy(QString studyUID);

private:
    /// Realitza una consulta dels estudis del pacient "patient" als PACS marcats per defecte.
    /// Si s'especifica una data "until" només cercarà els estudis fins la data especificada (aquesta inclosa).
    /// Si no es passa cap data per paràmetre cercarà tots els estudis, independentment de la data.
    void makeAsynchronousStudiesQuery(Patient *patient, QDate untilDate = QDate());

    /// Retorna una màscara de cerca base a partir de les quals es generan les DicomMask per cercar previs
    DicomMask getBasicDicomMask();

    /// Ens indica si aquell estudi està a la llista d'estudis ja rebuts de previs, per evitar duplicats
    /// Hem de tenir en compte que com fem la cerca per ID i un altre per Patient Name per obtenir més resultats
    /// potser que en les dos consultes ens retornin el mateix estudi, per tant hem d'evitar duplicats.
    bool isStudyInMergedStudyList(Study *study);

    /// Ens indica si aquest estudi és el mateix pel qual ens han demanat els previs, per evitar incloure'l a la llista
    bool isStudyToFindPrevious(Study *study);

    /// Inicialitza les variables per realitzar una nova consulta
    void initializeQuery();

    /// Ens encua el QueryPACSJob al PACSManager i ens connecta amb els seus signals per poder processar els resultats. També afegeix el Job en una taula
    /// de hash on es guarden tots els QueryPACSJobs demanats per aquesta classe que estant pendents d'executar-se o s'estan executant
    void enqueueQueryPACSJobToPACSManagerAndConnectSignals(QueryPacsJob *queryPACSJob);

    /// Ens afegeix els estudis trobats en una llista, si algun dels estudis ja existeix a la llista perquè s'ha trobat en algun altre PACS no
    /// se li afegeix
    void mergeFoundStudiesInQuery(QueryPacsJob *queryPACSJob);

    /// Emet signal indicant que la consulta a un PACS ha fallat
    void errorQueringPACS(QueryPacsJob *queryPACSJob);

    /// Emet signal indicant la la consulta ha acabat
    void queryFinished();

    /// Esborra els resultats de la cerca
    void deleteQueryResults();

    /// Retorna una llista indicant de quins PACS s'han descarregat els estudis que conté el pacient, sempre que continguin aquesta informació al DICOMSource
    QList<PacsDevice> getPACSRetrievedStudiesOfPatient(Patient *patient);

private slots:
    /// Slot que s'activa quan finalitza un job de consulta al PACS
    void queryPACSJobFinished(PACSJob *pacsJob);

    /// Slot que s'activa quan un job de consulta al PACS és cancel·lat
    void queryPACSJobCancelled(PACSJob *pacsJob);

private:
    PacsManager *m_pacsManager;
    QList<Study*> m_mergedStudyList;
    QString m_studyInstanceUIDToFindPrevious;
    /// Com fem una consulta dos consultes al mateix PACS si falla una segurament també fallarà la segona per això
    /// en aquesta llista registrarem l'ID dels Pacs pel quals hem emés el signal d'error i si rebem un segon error
    /// com ja el tindrem aquesta llista ja no en farem signal
    QStringList m_pacsDeviceIDErrorEmited;
    /// Hash que ens guarda tots els QueryPACSJob pendent d'executar o que s'estan executant llançats des d'aquesta classe
    QHash<int, QueryPacsJob*> m_queryPACSJobPendingExecuteOrExecuting;
    /// Boolea per saber si s'ha de cercar previes a partir del nom del pacient.
    bool m_searchRelatedStudiesByName;
};

}

#endif // UDGPREVIOUSSTUDIESMANAGER_H
