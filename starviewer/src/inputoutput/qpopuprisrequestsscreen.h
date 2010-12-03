/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQPOPURISREQUESTSCREEN_H
#define UDGQPOPURISREQUESTSCREEN_H

#include <QDialog>
#include "ui_qpopuprisrequestsscreenbase.h"

class QTimer;

/** Classe que mostra un PopUp indicant que s'ha rebut una petició del RIS per descarregar un estudi
 *
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

namespace udg {

class PACSJob;
class RetrieveDICOMFilesFromPACSJob;
class Patient;

class QPopUpRisRequestsScreen : public QDialog, private Ui::QPopUpRisRequestsScreenBase{
Q_OBJECT
public:

    ///Constructor
    QPopUpRisRequestsScreen( QWidget *parent = 0 );

    ///Destructor
    ~QPopUpRisRequestsScreen();

    ///S'indica que s'està cercant als PACS els estudis amb un determinat accessionnumber
    void queryStudiesByAccessionNumberStarted();

    ///Afegim un estudi per descarregar que s'ha demanat a través del rIS
    void addStudyToRetrieveByAccessionNumber(RetrieveDICOMFilesFromPACSJob *retrieveDICOMFilesFromPACSJob);

    ///Mostra en el PopUp que no s'ha trobat cap estudi que compleixi els criteris de cerca
    void showNotStudiesFoundMessage();

    ///Indica que la descàrrega ha finalitzat, indica el número d'estudis descarregats i el cap de 5 segons amaga el PopUp
    void showRetrieveFinished();

protected:

    ///Quan es fa un show al popup s'activa un timer, que passat un temps amaga automàticament el PopUp
    void showEvent(QShowEvent *);

private slots :

    ///Slot que s'activa quan acaba el timer per amagar el popup
    void timeoutTimer();

    ///S'indica que la descàrrega dels estudis degut a una petició del RIS ha finalitzat
    void retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob);

    ///S'indica que ha fallat o s'ha cancel·lat la descàrrega dels estudi demanat pel RIS
    void retrieveDICOMFilesFromPACSJobCancelledOrFailed(PACSJob *pacsJob);

private:

    ///Refresquem el label que indica quants estudis portem descarregats del total d'estudis a descarregar
    void refreshLabelStudyCounter();

    ///Mostra per pantalla el nom del pacient pel qual s'ha rebut la petició de descarregar estudis.
    void showPatientNameOfRetrievingStudies(Patient *patient);

private:

    QTimer *m_qTimer;
    static const int msTimeOutToHidePopUp;
    int m_numberOfStudiesToRetrieve;
    int m_numberOfStudiesRetrieved;
};

};

#endif

