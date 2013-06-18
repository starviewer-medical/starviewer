#ifndef UDGQPOPURISREQUESTSCREEN_H
#define UDGQPOPURISREQUESTSCREEN_H

#include "qnotificationpopup.h"

namespace udg {

class PACSJob;
class RetrieveDICOMFilesFromPACSJob;
class Patient;
class Study;

/**
    Classe que mostra un PopUp indicant que s'ha rebut una petició del RIS per descarregar un estudi
  */
class QPopUpRISRequestsScreen : public QNotificationPopup {
Q_OBJECT
public:
    /// Constructor
    QPopUpRISRequestsScreen(QWidget *parent = 0);

    /// Destructor
    ~QPopUpRISRequestsScreen();

    /// S'indica que s'està cercant als PACS els estudis amb un determinat accessionnumber
    void queryStudiesByAccessionNumberStarted();

    /// Afegim un estudi per descarregar que s'ha demanat a través del rIS
    void addStudyToRetrieveFromPACSByAccessionNumber(RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob);

    /// Afegim un estudi que s'ha obtingut de la base de dades del PACS demana't en una petició del RIS
    //HACK: Com ara mateix nom tenim cap manera de fer el seguiment de les peticions d'estudis a la base de dades, afegim aquest mètode que mostarà
    //aquest estudi ja com a descarregat i el sumarà al total d'estudis
    void addStudyRetrievedFromDatabaseByAccessionNumber(Study *study);

    /// Mostra en el PopUp que no s'ha trobat cap estudi que compleixi els criteris de cerca
    void showNotStudiesFoundMessage();

    /// S'especifica els segons que ha de tardar el PopUp a amagar-se després que s'hagin acabat de descarregar totes els estudis
    /// Sinó s'especifica per defecte són 5000 ms
    void setTimeOutToHidePopUpAfterStudiesHaveBeenRetrieved(int timeOutms);

private slots:
    /// S'indica que la descàrrega dels estudis degut a una petició del RIS ha finalitzat
    void retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob);

    /// S'indica que ha fallat o s'ha cancel·lat la descàrrega dels estudi demanat pel RIS
    void retrieveDICOMFilesFromPACSJobCancelled(PACSJob *pacsJob);

private:
    /// Actualitza per pantalla l'estat de la descàrrege
    void refreshScreenRetrieveStatus(Study *study);

    /// Indica que la descàrrega ha finalitzat, indica el número d'estudis descarregats i el cap de 5 segons amaga el PopUp
    void showRetrieveFinished();

private:
    QList<int> m_pacsJobIDOfStudiesToRetrieve;
    int m_numberOfStudiesRetrieved;
    int m_numberOfStudiesFailedToRetrieve;

    /// Guarda el número d'estudis que s'han de descarregar
    int m_numberOfStudiesToRetrieve;
};

};

#endif
