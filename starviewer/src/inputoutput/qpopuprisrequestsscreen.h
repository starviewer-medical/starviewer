#ifndef UDGQPOPURISREQUESTSCREEN_H
#define UDGQPOPURISREQUESTSCREEN_H

#include <QDialog>
#include <QPropertyAnimation>
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

class QPopUpRISRequestsScreen : public QDialog, private Ui::QPopUpRISRequestsScreenBase{
Q_OBJECT
public:

    ///Constructor
    QPopUpRISRequestsScreen(QWidget *parent = 0);

    ///Destructor
    ~QPopUpRISRequestsScreen();

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

    ///Si es rep un event de fer click sobre el PopUp aquest s'amaga
    bool eventFilter(QObject *obj, QEvent *event);

private slots :

    ///Slot que amaga el PopUp sempre i quan no s'estigui processant cap petició del RIS
    void hidePopUp();
    void hidePopUpSmoothly();

    /// Mou el PopUp a la cantonada dreta de la pantalla a on es troba en aquest moment.
    void moveToBottomRight();

    ///S'indica que la descàrrega dels estudis degut a una petició del RIS ha finalitzat
    void retrieveDICOMFilesFromPACSJobFinished(PACSJob *pacsJob);

    ///S'indica que ha fallat o s'ha cancel·lat la descàrrega dels estudi demanat pel RIS
    void retrieveDICOMFilesFromPACSJobCancelledOrFailed(PACSJob *pacsJob);

private:

    ///Mostra per pantalla el nom del pacient pel qual s'ha rebut la petició de descarregar estudis.
    void showPatientNameOfRetrievingStudies(Patient *patient);

    ///Actualitza per pantalla l'estat de la descàrrege
    void refreshScreenRetrieveStatus();

private:

    QTimer *m_qTimerToHidePopUp;
    QTimer *m_qTimerToMovePopUpToBottomRight;
    static const int msTimeOutToHidePopUp;
    static const int msTimeOutToMovePopUpToBottomRight;

    QList<int> m_pacsJobIDOfStudiesToRetrieve;
    int m_numberOfStudiesRetrieved;

    /// Animació utilitzada per desplaçar el pop up fin a la cantonada dreta
    QPropertyAnimation m_moveToBottomAnimation;
    QPropertyAnimation m_hidePopUpAnimation;
};

};

#endif

