/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPREVIOUSSTUDIESMANAGER_H
#define UDGPREVIOUSSTUDIESMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include "pacsdevice.h"

class QDate;

namespace udg {

class Patient;
class Study;
class DicomMask;
class PacsManager;

/**Aquesta classe donat un Study demana els estudis previs en els PACS configurats per defecte, degut a que 
  *ara actualment en el PACS tenim el mateix pacients amb PatientID diferents, també a part de cercar estudis
  *que coincideixin amb el PatientID també es farà una altre cerca per Patient Name. 
  */
/*TODO: En teoria amb la implantació del SAP els problemes de que un Pacient té diversos Patient ID o que té el nom
  *escrit de maneres diferents haurien de desapareixer, per tant d'aquí un temps quan la majoria d'estudis del PACS
  *ja s'hagin fet a través del SAP i constatem que el Patient ID pel mateix pacient sempre és el mateix, 
  *la cerca per nom de pacient podria desapareixer*/
class PreviousStudiesManager : public QObject {
Q_OBJECT
public:

    PreviousStudiesManager();
    ~PreviousStudiesManager();

    /// Fa una consulta d'estudis previs assíncrona als PACS passats per paràmetre
    /// Si ja s'estigués executant una consulta la cancel·laria i faria la nova consulta
    void queryPreviousStudies(Study *study);

    /// Cancel·la les consultes actuals que s'estan executant, i cancel·la les consultes encuades per executar
    void cancelCurrentQuery();

    /// Indica si s'executen queries en aquest moment
    bool isExecutingQueries();

	/// Mètode per demanar que es descarregui un estudi
    void downloadStudy( Study * study, QString pacs );

signals:

    /// Signal que s'emet quan ha finalitzat la consulta d'estudis previs
    void queryPreviousStudiesFinished(QList<Study*>, QHash<QString, QString> hashPacsIDOfStudyInstanceUID);

    /// Signal que s'emet per indicar que s'ha produït un error a la consulta d'estudis d'un PACS
    void errorQueryingPreviousStudies(PacsDevice pacs);

	/// Signal que s'emet quan s'ha descarregat un estudi.
	void previousStudyRetrieved(QString downloadedStudyUID);

private slots:

    ///Slot que s'executa quan s'ha acabat la consulta d'estudis previs a PacsManager
    void queryFinished();

    ///Slot que s'executa quan rebem els resultats d'una cerca a un PACS
    void queryStudyResultsReceived(QList<Patient*>, QHash<QString, QString>);

    ///Slot que s'executa quan la consulta a un PACS a de PacsManager ha fallat
    void errorQueryingStudy(PacsDevice);

private:

    PacsManager *m_pacsManager;
    QList<Study*> m_mergedStudyList;
    QHash<QString,QString> m_mergedHashPacsIDOfStudyInstanceUID;
    Study *m_studyToFindPrevious;
    /*Com fem una consulta dos consultes al mateix PACS si falla una segurament també fallarà la segona per això
    *en aquesta llista registrarem l'ID dels Pacs pel quals hem emés el signal d'error i si rebem un segon error
    *com ja el tindrem aquesta llista ja no en farem signal*/
    QStringList m_pacsDeviceIDErrorEmited;

    ///Crea les connexions
    void createConnections();

    ///Retorna una màscara de cerca base a partir de les quals es generan les DicomMask per cercar previs
    DicomMask getBasicDicomMask();

    ///Retorna una màscara per buscar estudis previs que coincideixin amb ID del pacient del Study
    DicomMask getPreviousStudyDicomMaskPatientID(Study *study);

    ///Retorna una màscara per buscar estudis previs que coincideixin amb Nom del pacient del Study
    DicomMask getPreviousStudyDicomMaskPatientName(Study *study);	

    ///Comprova si l'estudi ja l'hem rebut d'un altre PACS si és així no l'afegim, si no l'hem trobat
    void mergeStudyWithReceivedResults(Study *study, QString pacsID);

    ///Ens indica si aquell estudi està a la llista d'estudis ja rebuts de previs, per evitar duplicats
    /* Hem de tenir en compte que com fem la cerca per ID i un altre per Patient Name per obtenir més resultats
    * potser que en les dos consultes ens retornin el mateix estudi, per tant hem d'evitar duplicats.
    */
    bool isStudyInMergedStudyList(Study *study);

    ///Ens indica si aquest estudi és el mateix pel qual ens han demanat els previs, per evitar incloure'l a la llista 
    bool isStudyToFindPrevious(Study *study);

    ///Donada una data la transforma a una màscara per cercar estudis previs a aquell
    QString getPreviousStudyDateMask(QDate studyDate);

};

}

#endif // UDGPREVIOUSSTUDIESMANAGER_H
