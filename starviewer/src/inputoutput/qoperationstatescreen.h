/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQOPERATIONSTATESCREEN_H
#define UDGQOPERATIONSTATESCREEN_H

#include "ui_qoperationstatescreenbase.h"

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class Status;
class Operation;
class PacsManager;
class PACSJob;
class RetrieveDICOMFilesFromPACSJob;
class SendDICOMFilesToPACSJob;
class Study;

/// Interfície que implementa la llista d'operacions realitzades cap a un PACS
class QOperationStateScreen : public QDialog , private Ui::QOperationStateScreenBase{
Q_OBJECT
public:
    QOperationStateScreen( QWidget *parent = 0 );

    /// Retorna el núm. d'operacions que s'estan executant
    unsigned int getActiveOperationsCount();

    ///Estableix instància de PacsManager que s'encarrega de fer les peticions als PACS
    void setPacsManager(PacsManager *pacsManager);


protected :
    /** Event que s'activa al tancar al rebren un event de tancament
     * @param event de tancament
     */
    void closeEvent( QCloseEvent* ce );

private:
    /// Crea les connexions pels signals i slots
    void createConnections();

    /// Indica si una operació es pot considerar com a finalitzada a partir del seu missatge d'estat
    bool isOperationFinalized(const QString &message);

//nou
private slots:
    
    ///Slot que s'activa quan s'ha encuat un nou PACSJob insereix al QTreeWidget la informació del nou job i la posa com a Pedent de realitzar
    void newPACSJobEnqueued(PACSJob *);

    ///Slot que s'activa quan job comença, al QTreeWidget es marca aquell job com començat
    void PACSJobStarted(PACSJob *);

    ///Slot que s'activa quabn el jo ha acabat, es marca aquell Job al QTreeWidget amb l'estatus en el que ha finalitzat
    void PACSJobFinished(PACSJob *);

    ///Slot que s'activa quan job ha fet una acció amb una imatge, s'augmenta pel job al QTreeWidget el número de d'imatges
    void DICOMFileCommit(PACSJob *pacsJob, int numberOfImages);

    ///Slot que s'activa quan job ha fer una acció amb una sèrie completa, s'augmenta pel job al QTreeWidget el número de sèries
    void DICOMSeriesCommit(PACSJob *pacsJob, int numberOfSeries);

    /// Neteja la llista d'estudis excepte dels que s'estant descarregant en aquells moments
    void clearList();

private:
    QString m_currentProcessingStudyUID;
    PacsManager *m_pacsManager;

private:

    ///Afegeix al QTreeWidget el nou job encuat, i el mostra
    void insertNewPACSJob(PACSJob *pacsJob);
 
    ///Retorna l'objecte Study relatiu a PACSJob, només funciona amb PACSJob de tipus RetrieveDICOMFilesFromPACSJob i SendDICOMFilesToPACSJob
    Study *getStudyFromPACSJob(PACSJob* pacsJob);

    /**Retorna un QString per mostrar-lo per la QOperationStateScreen indicant com ha finalitzat un PACSJob
      *Per RetrieveDICOMFilesFromPACS pot retornar : RETRIEVED, CANCELLED, ERROR
      *Per SendDICOMFilesToPACS pot retornar : SENT, CANCELLED, ERROR*/
    QString getPACSJobStatusResume(PACSJob *pacsJob);
};

};

#endif

