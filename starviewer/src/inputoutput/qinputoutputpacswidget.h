/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGQINPUTOUTPUTPACSWIDGET_H
#define UDGQINPUTOUTPUCPACSWIDGET_H

#include "ui_qinputoutputpacswidgetbase.h"

#include <QMenu>
#include <QHash>

#include "qexecuteoperationthread.h"

// fordward declarations
class QString;

namespace udg {

// fordward declarations
class DicomMask;
class Patient;
class StatsWatcher;
class Status;
class PacsDevice;
class Study;
class QOperationStateScreen;
class ProcessImageSingleton;
class PacsManager;

/** 
 * Widget en el que controla les operacions d'entrada/sortida del PACS
 */
class QInputOutputPacsWidget : public QWidget, private Ui::QInputOutputPacsWidgetBase
{
Q_OBJECT
public:
    /// Constructor de la classe
    QInputOutputPacsWidget(QWidget *parent = 0);
    ~QInputOutputPacsWidget();

    ///Consulta els estudis al dicomdir obert que compleixin la màscara de cerca
    void queryStudy(DicomMask queryMask, QList<PacsDevice> pacsToQuery);

    //TODO: per implementar
    void storeStudiesToPacs(PacsDevice pacs, QList<Study*> studiesToStore);

    ///Neteja els resultats de la última cerca
    void clear();

    ///Especifica quina interfície s'ha d'utilitzar per comunicar l'estat de les operacions
    void setQOperationStateScreen(QOperationStateScreen *qoperationStateScreen);

    /** Descarrega una estudi del pacs
     * @param indica si l'estudi s'ha de visualitzar
     * @param pacsIdToRetrieve indica l'id del Pacs del qual s'ha de descarregar l'estudi
     * @param maskStudyToRetrieve la màscara dels objectes a descarregar
     * @param studyToRetrieve L'estudi al qual pertanyen els objectes ad escarregar
     */
    void retrieve(bool view, QString pacsIdToRetrieve, DicomMask mask, Study *studyToRetrieve);

signals:
    ///Signal que s'emet per indicar que es netegin els camps de cerca
    void clearSearchTexts();

    ///Signal que s'emet per indicar que s'ha demanat visualitzar un estudi
    void viewRetrievedStudy(QString studyInstanceUID);

    ///Signal que s'emet per indica que un estudi ha estat descarregat
    void studyRetrieved(QString studyInstanceUID);

    ///Signal que s'emet per indicar que hi hagut algun canvi en l'estat de les operacions, (s'ha cancel·lat, ha acabat, s'ha creat un de nova, ha fallat)
    void operationStateChange();

    ///Indica que un estudi serà esborrat de la base de dades local, això es produeix perquè al intentar descarregar un estudi es comprovar que no hi ha suficient espai al disc i s'esborren els estudis que fa més temps que no han estat visualitzats
    void studyWillBeDeletedFromDatabase(QString studyInstanceUID);

private:

    ///Crea les connexions entre signals i slots
    void createConnections();

    ///Genera el menú contextual que apareix quan clickem amb el botó dret a sobre d'un item del StudyTreeWidget
    void createContextMenuQStudyTreeWidget();

    /**Comprova que els paràmetres per la cerca siguin correctes, que no es tractir d'un consulta pesada i que ens hagin seleccionat
     * algun PACS per consultar 
     */
    bool AreValidQueryParameters(DicomMask *maskToQuery, QList<PacsDevice> pacsToQuery);

    ///Retorna l'ID del pacs al que pertany l'estudi passat per paràmetre, només té en compte els estudis que s'han consultat a la última query
    QString getPacsIDFromQueriedStudies(QString studyInstanceUID);

    ///Construeix la màscara de cerca per cercar les sèries d'un estudi
    DicomMask buildSeriesDicomMask(QString studyInstanceUID);

    ///Construeix la màscara de cerca per cercar les imatges d'una sèrie
    DicomMask buildImageDicomMask(QString studyInstanceUID, QString seriesInstanceUID);

private slots:
    ///Mostra les sèries d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandSeriesOfStudy(QString seriesInstanceUID);

    ///Mostra les imatges d'un estudi, les consulta al dicomdir i les mostra al tree widget
    void expandImagesOfSeries(QString studyIntanceUID, QString seriesInstanceUID);

    ///Importa cap a la base de dades locals els estudis seleccionats
    void retrieveSelectedStudies(bool view = false);

    ///Emet signal selectedPatients indicant que s'han seleccionat estudis per ser visualitzats
    void view();

    ///Ens Mostra un missatge indicant l'error produït a la QExecuteOperationThread, i com es pot solucionar
    void showQExecuteOperationThreadError(QString studyInstanceUID, QString pacsID, QExecuteOperationThread::OperationError error);

    ///Slot que s'activa quan s'han rebut d'un PACS resultats d'una cerca d'estudis
    void queryStudyResultsReceived(QList<Patient*> patients, QHash<QString, QString> hashTablePacsIDOfStudyInstanceUID);

    ///Slot que s'activa quan s'han rebut d'un PACS resultats d'una cerca de series
    void querySeriesResultsReceived(QString studyInstanceUID, QList<Series*> series);

    ///Slot que s'activa quan s'han rebut d'un PACS resultats d'una cerca d'imatges
    void queryImageResultsReceived(QString studyInstanceUID, QString seriesInstanceUID, QList<Image*> image);

    ///Slot que s'activa quan les query finalitzen crida el mètode setQueryInProgress en false
    void queryFinished();

    ///Slot que s'activa quan s'ha produït un error al consultar els estudis d'un PACS
    void errorQueryingStudy(PacsDevice pacsDevice);

    ///Slot que s'activa quan s'ha produït un error al consultar les series d'un PACS
    void errorQueryingSeries(QString studyInstanceUID, PacsDevice pacsDevice);

    ///Slot que s'activa quan s'ha produït un error al consultar les imatges d'un PACS
    void errorQueryingImage(QString studyInstanceUID, QString seriesInstanceUID, PacsDevice pacsDevice);

    ///Cancel·la les consultes que s'estan executant en aquell moment
    void cancelCurrentQueries();

private:
    QMenu m_contextMenuQStudyTreeWidget;
    QHash<QString, QString> m_hashPacsIDOfStudyInstanceUID;
    QExecuteOperationThread m_qexecuteOperationThread;
    QOperationStateScreen *m_qoperationStateScreen;
    ProcessImageSingleton *m_processImageSingleton;
    PacsManager *m_pacsManager;

    StatsWatcher *m_statsWatcher;

    ///Amaga/mostra que hi ha una query en progress i habilitat/deshabilitat el botó de cancel·lar la query actual
    void setQueryInProgress(bool queryInProgress);
};

};// end namespace udg

#endif
